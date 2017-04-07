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

#include <envire_core/plugin/ClassLoader.hpp>
#include <glog/logging.h>

using namespace envire::core;

constexpr char ClassLoader::envire_item_base_class[];
constexpr char ClassLoader::envire_item_class[];
constexpr char ClassLoader::envire_collision_base_class[];

ClassLoader::ClassLoader() : PluginLoader()
{

}

ClassLoader::~ClassLoader()
{

}

ClassLoader* ClassLoader::getInstance()
{
    return base::Singleton<ClassLoader>::getInstance();
}

bool ClassLoader::hasEnvireItem(const std::string& class_name) const
{
    return hasClassOfType(class_name, envire_item_base_class);
}

bool ClassLoader::hasCollisionObject(const std::string& class_name) const
{
    return hasClassOfType(class_name, envire_collision_base_class);
}

bool ClassLoader::createEnvireItem(const std::string& item_name, envire::core::ItemBase::Ptr& base_item)
{
    return createInstance<envire::core::ItemBase>(item_name, base_item);
}

bool ClassLoader::createEnvireItemFor(const std::string& embedded_type, envire::core::ItemBase::Ptr& base_item)
{
    if(createEnvireItem(std::string(envire_item_class) + "<" + embedded_type + ">", base_item))
        return true;

    // try to get the envire item name using the associated classes
    std::string associated_class;
    if(getAssociatedClassOfType(embedded_type, envire_item_base_class, associated_class))
    {
        return createEnvireItem(associated_class, base_item);
    }
    return false;
}

bool ClassLoader::loadEnvireItemLibrary(const std::string& item_name)
{
    if(hasEnvireItem(item_name))
        return loadLibrary(item_name);
    return false;
}

bool ClassLoader::loadAllEnvireItemLibraries()
{
    std::vector<std::string> item_classes = getAvailableClasses(envire_item_base_class);
    bool load_fails = false;
    for (std::string item_name : item_classes)
    {
        if(!loadLibrary(item_name))
        {
            load_fails = true;
            LOG(ERROR) << "Failed to load plugin library of EnviRe item " << item_name;
        }
    }
    return load_fails;
}
