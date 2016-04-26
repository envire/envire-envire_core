#include <envire_core/plugin/ClassLoader.hpp>

using namespace envire::core;

constexpr char ClassLoader::envire_item_base_class[];
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
    std::string associated_class;
    if(getAssociatedClassOfType(embedded_type, envire_item_base_class, associated_class))
    {
        return createEnvireItem(associated_class, base_item);
    }
    return false;
}