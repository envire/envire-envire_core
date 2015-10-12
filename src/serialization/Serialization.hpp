#pragma once

#include "SerializationHandle.hpp"
#include "ItemHeader.hpp"
#include <envire_core/items/ItemBase.hpp>

#include <map>

namespace envire { namespace core
{

class Serialization
{
    typedef boost::shared_ptr<SerializationHandle> HandlePtr;
    typedef std::map< std::string, HandlePtr > HandleMap;

public:

    static bool save(ArchiveOutType& ar, const ItemBase::Ptr& item);

    static bool load(ArchiveInType& ar, ItemBase::Ptr& item);

    template<typename T>
    static bool load(ArchiveInType& in, typename T::Ptr& item)
    {
        ItemBase::Ptr base_item;
        bool result = load(in, base_item);
        item = boost::dynamic_pointer_cast<T>(base_item);
        return result;
    }

    static HandleMap& getHandleMap();

    static void registerHandle(const std::string& plugin_name, HandlePtr& handle);

    static bool hasHandle(const std::string& plugin_name);

    static bool getHandle(const std::string& plugin_name, HandlePtr& handle);
};

}}