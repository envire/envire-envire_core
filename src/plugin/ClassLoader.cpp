#include "ClassLoader.hpp"
#include <boost/algorithm/string.hpp>

using namespace envire;
using namespace std;

const std::vector<std::string> ClassLoader::plugin_paths = ClassLoader::loadLibraryPath();
constexpr char ClassLoader::envire_item_suffix[];
constexpr char ClassLoader::envire_collision_suffix[];


vector<string> ClassLoader::loadLibraryPath()
{
    const string path(std::getenv("LD_LIBRARY_PATH"));
    vector<string> paths;
    //":" is the separator in LD_LIBRARY_PATH
    boost::split(paths, path, boost::is_any_of(":"));
    //trim ":" and " " from the beginning and end of the string
    for(string& path : paths)
    {
        boost::trim_if(path, boost::is_any_of(": "));
    }
    return paths;
}
    

ClassLoader::ClassLoader()
{

}

ClassLoader::~ClassLoader()
{
    try
    {
        loaders.clear();
    }
    catch(class_loader::LibraryUnloadException& e)
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
    return !plugin_paths.empty();
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
    return createItemIntern(class_name);
}


ClassLoader::ItemBaseClassPtr ClassLoader::createItemIntern(const std::string& class_name)
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
        return it->second->createInstance<ItemBaseClass>(class_name);
}

void ClassLoader::loadLibrary(const std::string& class_name)
{
    if(hasValidPluginPath())
    {
        const string lib_name = "lib" + class_name + envire_item_suffix + ".so";
        //try to load the plugin from all available paths
        bool loaded = false;
        for(string path : plugin_paths)
        {
            path += "/" + lib_name;
            boost::shared_ptr<class_loader::ClassLoader> loader(new class_loader::ClassLoader(path, false));
            
            if(loader->isLibraryLoaded() && loader->isClassAvailable<ItemBaseClass>(class_name))
            {
                loaders.insert(std::make_pair(class_name, loader));
                loaded = true;
                break;
            }
        }
        if(!loaded)
        {
            std::string error_msg = "Failed to load plugin library " + lib_name;
            throw std::runtime_error(error_msg);
        }
    }
    else
        throw std::runtime_error("Have no valid path in environment variable. Please set LD_LIBRARY_PATH");
}
