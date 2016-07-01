#include "Serialization.hpp"

#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/nvp.hpp>
#include <envire_core/serialization/BinaryBufferHelper.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

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