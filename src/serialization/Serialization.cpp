#include "Serialization.hpp"

#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/nvp.hpp>

using namespace envire::core;

bool Serialization::save(ArchiveOutType& ar, const ItemBase::Ptr& item)
{
    try
    {
        if(!hasHandle(item->getClassName()))
        {
            std::cerr << "Cannot find plugin " << item->getClassName() << std::endl;
            throw std::runtime_error("Automatic lookup of plugins is not implemented yet.");
            //TODO try to load library if handle is not available
        }
        HandlePtr handle;
        if(getHandle(item->getClassName(), handle) && handle)
        {
            ItemHeader header(item);
            ar << BOOST_SERIALIZATION_NVP(header);
            return handle->save(ar, item);
        }
        return false;
    }
    catch(const std::runtime_error& e)
    {
        return false;
    }
    return false;
};

bool Serialization::load(ArchiveInType& ar, ItemBase::Ptr& item)
{
    try
    {
        ItemHeader header;
        ar >> BOOST_SERIALIZATION_NVP(header);
        if(!hasHandle(header.class_name))
        {
            std::cerr << "Cannot find plugin " << header.class_name << std::endl;
            throw std::runtime_error("Automatic lookup of plugins is not implemented yet.");
            //TODO try to load library if handle is not available
        }
        HandlePtr handle;
        if(getHandle(header.class_name, handle) && handle)
            return handle->load(ar, item);
        return false;
    }
    catch(const std::runtime_error& e)
    {
        return false;
    }
    return false;
};

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