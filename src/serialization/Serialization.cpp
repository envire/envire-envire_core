//
// Copyright (c) 2015, Deutsches Forschungszentrum für Künstliche Intelligenz GmbH.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#include "Serialization.hpp"

#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/nvp.hpp>
#include <envire_core/serialization/BinaryBufferHelper.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#ifdef CMAKE_ENABLE_PLUGINS
    #include <envire_core/plugin/ClassLoader.hpp>
#endif

using namespace envire::core;

bool Serialization::saveToBinary(std::vector< uint8_t >& binary, const ItemBase::Ptr& item)
{
    binary.reserve(1000000);
    BinaryOutputBuffer buffer(&binary);
    std::ostream ostream(&buffer);
    boost::archive::binary_oarchive oa(ostream);
    return save(oa, item);
}

bool Serialization::loadFromBinary(const std::vector< uint8_t >& binary, ItemBase::Ptr& item)
{
    BinaryInputBuffer buffer(binary);
    std::istream istream(&buffer);
    boost::archive::binary_iarchive ia(istream);
    return load(ia, item);
}

bool Serialization::isSerializable(const ItemBase::Ptr& item)
{
    std::string class_name;
    if (!item->getClassName(class_name))
    {
        /* Load all available plugins of type ItemBase.
         * Note: The shared library of the Item has to be dynamicly loaded or linked
         * in order to get a valid class name. Since this is here not jet the case all
         * plugin libraries have to be loaded. */
        loadAllPluginLibraries();

        if (!item->getClassName(class_name))
        {
            LOG(INFO) << "Can't serialize item with embedded type " << item->getEmbeddedTypeInfo()->name() << ", it provides not class name. "
                      << "Did you forget to register the Item with the ENVIRE_REGISTER_ITEM macro?";
            return false;
        }
    }

    // try to get handle
    if (hasHandle(class_name))
        return true;

    // load plugin lib
    if (!loadPluginLibrary(class_name))
    {
        LOG(INFO) << "Failed to load plugin library for item " << class_name;
        return false;
    }

    // try to get handle
    if (hasHandle(class_name))
        return true;
    else
        LOG(INFO) << "Library has been loaded but can't find a serialization handle for " << class_name << "."
                << "Did you forget to register the Item with the ENVIRE_REGISTER_ITEM macro?";

    return false;
}

Serialization::HandleMap& Serialization::getHandleMap()
{
    static HandleMap handle_map;
    return handle_map;
}

void Serialization::registerHandle(const std::string& plugin_name, Serialization::HandlePtr& handle)
{
    HandleMap& handle_map = getHandleMap();
    handle_map[plugin_name] = handle;
}

bool Serialization::hasHandle(const std::string& plugin_name)
{
    HandleMap& handle_map = getHandleMap();
    return handle_map.find(plugin_name) != handle_map.end();
}

bool Serialization::getHandle(const std::string& plugin_name, Serialization::HandlePtr& handle)
{
    HandleMap& handle_map = getHandleMap();
    HandleMap::iterator it = handle_map.find(plugin_name);
    if(it != handle_map.end())
    {
        handle = it->second;
        return true;
    }
    return false;
}

bool Serialization::loadPluginLibrary(const std::string& class_name)
{
    #ifdef CMAKE_ENABLE_PLUGINS
        LOG(INFO) << "Trying to load plugin library for item " << class_name;
        ClassLoader* loader = ClassLoader::getInstance();
        return loader->loadEnvireItemLibrary(class_name);
    #else
        LOG(ERROR) << "Unable to load plugin library from item " << class_name
                   << " because plugin support is disabled (code has been compiled with ENABLE_PLUGINS=OFF).";
        return false;
    #endif
}

bool Serialization::loadAllPluginLibraries()
{
    #ifdef CMAKE_ENABLE_PLUGINS
        ClassLoader* loader = ClassLoader::getInstance();
        return loader->loadAllEnvireItemLibraries();
    #else
        LOG(ERROR) << "Unable to load plugin library from item " << class_name
                << " because plugin support is disabled (code has been compiled with ENABLE_PLUGINS=OFF).";
        return false;
    #endif
}