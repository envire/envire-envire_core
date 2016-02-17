#include "Serialization.hpp"

#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/nvp.hpp>
#include <envire_core/serialization/BinaryBufferHelper.hpp>
#include <boost/archive/polymorphic_binary_iarchive.hpp>
#include <boost/archive/polymorphic_binary_oarchive.hpp>
#include <glog/logging.h>

#ifdef ENABLE_PLUGINS
    #include <envire_core/plugin/ClassLoader.hpp>
#endif

using namespace envire::core;

bool loadPluginLibrary(const std::string& class_name)
{
    #ifdef ENABLE_PLUGINS
        LOG(INFO) << "Trying to load plugin library for item " << class_name;
        ClassLoader* loader = ClassLoader::getInstance();
        ItemBase::Ptr item;
        return loader->createEnvireItem(class_name, item);
    #else
        LOG(INFO) << "Unable to load plugin library from item " << class_name
                  << " because plugin support is disabled (code has been compiled with ENABLE_PLUGINS=OFF).";
        return false;
    #endif
}

bool Serialization::save(ArchiveOutType& ar, const ItemBase::Ptr& item)
{
    try
    {
        // try to get handle
        if(!hasHandle(item->getClassName()))
        {
            // load plugin lib
            if(loadPluginLibrary(item->getClassName()))
            {
                // try to get handle
                LOG(INFO) << "Successfully loaded plugin library for item " << item->getClassName();
                if(!hasHandle(item->getClassName()))
                {
                    LOG(ERROR) << "Library has been loaded but can't find a serialization handle for " << item->getClassName();
                    return false;
                }
            }
            else
            {
                LOG(ERROR) << "Failed to load plugin library for item " << item->getClassName();
                return false;
            }
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
        LOG(ERROR) << "Caught exception while trying to save item of type " << item->getClassName();
    }
    return false;
}

bool Serialization::load(ArchiveInType& ar, ItemBase::Ptr& item)
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
                    LOG(ERROR) << "Library has been loaded but can't find a serialization handle for " << header.class_name;
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

bool Serialization::save(std::vector< uint8_t >& binary, const ItemBase::Ptr& item)
{
    binary.reserve(1000000);
    BinaryOutputBuffer buffer(&binary);
    std::ostream ostream(&buffer);
    boost::archive::polymorphic_binary_oarchive oa(ostream);
    return save(oa, item);
}

bool Serialization::load(const std::vector< uint8_t >& binary, ItemBase::Ptr& item)
{
    BinaryInputBuffer buffer(binary);
    std::istream istream(&buffer);
    boost::archive::polymorphic_binary_iarchive ia(istream);
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