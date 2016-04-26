#pragma once

#include <envire_core/serialization/SerializationHandle.hpp>
#include <envire_core/serialization/ItemHeader.hpp>
#include <envire_core/items/ItemBase.hpp>

#include <map>

namespace envire { namespace core
{

/**
 * @brief This class contains static helper methods which allow the correct
 * serialization of inherited classes from the abstract class ItemBase.
 * For this to work it is mandatory to call the ENVIRE_REGISTER_SERIALIZATION( classname )
 * macro in the source file of the inherited class.
 * ENVIRE_REGISTER_PLUGIN( classname ) will also call ENVIRE_REGISTER_SERIALIZATION( classname ).
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
    static bool save(ArchiveOutType& ar, const ItemBase::Ptr& item);

    /**
     * @brief Unserializes an abstract item from a boost archive
     *
     * @param ar boost polymorphic iarchive
     * @param item pointer to the ItemBase class
     * @return true if successful
     */
    static bool load(ArchiveInType& ar, ItemBase::Ptr& item);

    /**
     * @brief Unserializes an abstract item from a boost archive
     *
     * @param ar boost polymorphic iarchive
     * @param item pointer to the ItemBase class
     * @return true if successful
     */
    template<typename T>
    static bool load(ArchiveInType& in, typename T::Ptr& item)
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
    static bool save(std::vector<uint8_t>& binary, const ItemBase::Ptr& item);

    /**
     * @brief Unserializes an abstract item from a binary blob
     *
     * @param binary data
     * @param item pointer to the ItemBase class
     * @return true if successful
     */
    static bool load(const std::vector<uint8_t>& binary, ItemBase::Ptr& item);

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
};

}}