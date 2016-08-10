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
