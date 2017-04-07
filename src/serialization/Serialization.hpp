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

#pragma once

#include <envire_core/serialization/SerializationHandle.hpp>
#include <envire_core/serialization/ItemHeader.hpp>
#include <envire_core/items/ItemBase.hpp>

#include <boost/serialization/nvp.hpp>
#include <glog/logging.h>
#include <map>

namespace envire { namespace core
{

/**
 * @brief This class contains static helper methods which allow the correct
 * serialization of inherited classes from the abstract class ItemBase.
 * For this to work it is mandatory to call the ENVIRE_REGISTER_SERIALIZATION( classname )
 * macro in the source file of the inherited class.
 * ENVIRE_REGISTER_ITEM( classname ) will also call ENVIRE_REGISTER_SERIALIZATION( classname ).
 */
class Serialization
{
    typedef boost::shared_ptr<SerializationHandle> HandlePtr;
    typedef std::map< std::string, HandlePtr > HandleMap;

public:

    /**
     * @brief Serializes an abstract item to a boost archive.
     *
     * @param ar boost polymorphic oarchive
     * @param item pointer to the ItemBase class
     * @return true if successful
     */
    template <typename Archive>
    static bool save(Archive& ar, const ItemBase::Ptr& item)
    {
        if(isSerializable(item))
        {
            std::string class_name;
            HandlePtr handle;
            try
            {
                if (item->getClassName(class_name) && getHandle(class_name, handle) && handle)
                {
                    ItemHeader header(class_name);
                    ar << BOOST_SERIALIZATION_NVP(header);
                    return handle->save(ar, item);
                }
            }
            catch(const std::runtime_error& e)
            {
                LOG(ERROR) << "Caught exception while trying to save an item of type " << class_name;
            }
        }

        LOG(ERROR) << "Failed to serialize an item with embedded type " << item->getEmbeddedTypeInfo()->name() << "."
                   << "Make sure that the item is registered using the ENVIRE_REGISTER_ITEM macro and a xml file "
                   << "with the plugin meta information is exported.";
        return false;
    }

    /**
     * @brief Unserializes an abstract item from a boost archive
     *
     * @param ar boost polymorphic iarchive
     * @param item pointer to the ItemBase class
     * @return true if successful
     */
    template <typename Archive>
    static bool load(Archive& ar, ItemBase::Ptr& item)
    {
        try
        {
            ItemHeader header;
            ar >> BOOST_SERIALIZATION_NVP(header);
            // try to get handle
            if(!hasHandle(header.class_name))
            {
                // load plugin lib
                if(loadPluginLibrary(header.class_name))
                {
                    // try to get handle
                    LOG(INFO) << "Successfully loaded plugin library for item " << header.class_name;
                    if(!hasHandle(header.class_name))
                    {
                        LOG(ERROR) << "Library has been loaded but can't find a serialization handle for " << header.class_name << "."
                                << "Did you forget to register the Item with the ENVIRE_REGISTER_ITEM macro?";
                        return false;
                    }
                }
                else
                {
                    LOG(ERROR) << "Failed to load plugin library for item " << header.class_name;
                    return false;
                }
            }
            HandlePtr handle;
            if(getHandle(header.class_name, handle) && handle)
                return handle->load(ar, item);
            return false;
        }
        catch(const std::runtime_error& e)
        {
            LOG(ERROR) << "Caught exception while trying to load item";
        }
        return false;
    }

    /**
     * @brief Unserializes an abstract item from a boost archive
     *
     * @param ar boost polymorphic iarchive
     * @param item pointer to the ItemBase class
     * @return true if successful
     */
    template<typename Archive, typename T>
    static bool load(Archive& in, typename T::Ptr& item)
    {
        ItemBase::Ptr base_item;
        bool result = load(in, base_item);
        item = boost::dynamic_pointer_cast<T>(base_item);
        return result;
    }

    /**
     * @brief Serializes an abstract item to a binary blob
     *
     * @param binary data
     * @param item pointer to the ItemBase class
     * @return true if successful
     */
    static bool saveToBinary(std::vector< uint8_t >& binary, const ItemBase::Ptr& item);

    /**
     * @brief Unserializes an abstract item from a binary blob
     *
     * @param binary data
     * @param item pointer to the ItemBase class
     * @return true if successful
     */
    static bool loadFromBinary(const std::vector< uint8_t >& binary, ItemBase::Ptr& item);

    /**
     * @brief Returns true if a serialization handle is registered for the given item.
     *
     * @param item pointer to the ItemBase class
     * @return true if a serialization handle is registered
     */
    static bool isSerializable(const ItemBase::Ptr& item);

    /**
     * @brief Returns a reference to the static HandleMap object.
     *
     * @return envire::core::Serialization::HandleMap&
     */
    static HandleMap& getHandleMap();

    /**
     * @brief Registers a new SerializationHandle of a certain inherited class.
     * This method is called be the serialization registration macro.
     *
     * @param plugin_name name of the inherited class
     * @param handle pointer to the SerializationHandle
     */
    static void registerHandle(const std::string& plugin_name, HandlePtr& handle);

    /**
     * @brief Returns true if a SerializationHandle for a certain inherited class
     * is available.
     *
     * @param plugin_name name of the inherited class
     * @return true if handle is available
     */
    static bool hasHandle(const std::string& plugin_name);

    /**
     * @brief Returns the SerializationHandle for a certain inherited class
     * is available.
     *
     * @param plugin_name name of the inherited class
     * @param handle pointer to the SerializationHandle
     * @return true of successful
     */
    static bool getHandle(const std::string& plugin_name, HandlePtr& handle);

private:
    static bool loadPluginLibrary(const std::string& class_name);

    static bool loadAllPluginLibraries();
};

}}