#include "ClassLoader.hpp"

using namespace envire;

const std::string ClassLoader::plugin_path = std::string(std::getenv("LD_LIBRARY_PATH"));
constexpr char ClassLoader::envire_item_suffix[];
constexpr char ClassLoader::envire_collision_suffix[];


ClassLoader::ClassLoader()
{

}

ClassLoader::~ClassLoader()
{
    try
    {
        loaders.clear();
    }
    catch(class_loader::LibraryUnloadException e)
    {
         std::cerr << "got exception: " << e.what() << std::endl;
    }
}

ClassLoader* ClassLoader::getInstance()
{
    return base::Singleton<ClassLoader>::getInstance();
}

bool ClassLoader::hasValidPluginPath()
{
    return !plugin_path.empty();
}

bool ClassLoader::hasItem(const std::string& class_name)
{
    LoaderMap::iterator it = loaders.find(class_name);
    if(it != loaders.end())
    {
        return it->second->isClassAvailable<ItemBaseClass>(class_name);
    }
    return false;
}

ClassLoader::ItemBaseClassPtr ClassLoader::createItem(const std::string& class_name)
{
    return ItemBaseClassPtr(createItemIntern(class_name));
}


ClassLoader::ItemBaseClass* ClassLoader::createItemIntern(const std::string& class_name)
{
    LoaderMap::iterator it = loaders.find(class_name);

    if(it == loaders.end())
    {
        loadLibrary(class_name);
        it = loaders.find(class_name);
    }

    if(it == loaders.end())
    {
        std::string error_msg = "Could not fing plugin library ";
        error_msg += class_name;
        throw std::runtime_error(error_msg);
    }
    else
        return it->second->createUnmanagedInstance<ItemBaseClass>(class_name);
}

void ClassLoader::loadLibrary(const std::string& class_name)
{
    if(hasValidPluginPath())
    {
        std::string path = plugin_path;
        path += "/lib";
        path += class_name;
        path += envire_item_suffix;
        path += ".so";
        boost::shared_ptr<class_loader::ClassLoader> loader(new class_loader::ClassLoader(path, false));
        if(loader->isLibraryLoaded() && loader->isClassAvailable<ItemBaseClass>(class_name))
            loaders.insert( std::make_pair(class_name, loader));
        else
        {
            std::string error_msg = "Failed to load plugin library ";
            error_msg += path;
            throw std::runtime_error(error_msg);
        }
    }
    else
        throw std::runtime_error("Have no valid path in environment variable, called LD_LIBRARY_PATH");
}