#include "ClassLoader.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <glog/logging.h>

using namespace envire::core;
using namespace std;

constexpr char ClassLoader::envire_item_base_class[];
constexpr char ClassLoader::envire_collision_base_class[];

ClassLoader::ClassLoader() : PluginManager()
{
    loadLibraryPaths();
}

ClassLoader::~ClassLoader()
{
    loaders.clear();
}

ClassLoader* ClassLoader::getInstance()
{
    return base::Singleton<ClassLoader>::getInstance();
}

bool ClassLoader::hasClass(const string& class_name) const
{
    return isClassInfoAvailable(class_name);
}

bool ClassLoader::hasClassOfType(const string& class_name, const string& base_class_name) const
{
    std::string base_class;
    if(getBaseClass(class_name, base_class) && base_class == base_class_name)
        return true;
    return false;
}

bool ClassLoader::hasEnvireItem(const std::string& class_name) const
{
    return hasClassOfType(class_name, envire_item_base_class);
}

bool ClassLoader::hasCollisionObject(const string& class_name) const
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

void ClassLoader::addLibraryPath(const string& library_path)
{
    string lib_path_trimed = library_path;
    boost::trim_right_if(lib_path_trimed, boost::is_any_of("/"));
    library_paths.insert(lib_path_trimed);
}

bool ClassLoader::getAssociatedClassOfType(const string& embedded_type, const string& base_class_name, string& associated_class) const
{
    std::vector<std::string> envire_item_classes = getAvailableClasses(envire_item_base_class);
    for(const std::string& class_name : envire_item_classes)
    {
        std::vector<std::string> associated_classes;
        if(getAssociatedClasses(class_name, associated_classes) && !associated_classes.empty())
        {
            std::vector<std::string>::const_iterator it = std::find(associated_classes.begin(), associated_classes.end(), embedded_type);
            if(it != associated_classes.end())
            {
                associated_class = class_name;
                return true;
            }
        }
    }
    return false;
}

void ClassLoader::loadLibraryPaths()
{
    const char* lib_path = std::getenv("LD_LIBRARY_PATH");
    vector<string> paths;
    if(lib_path != NULL)
    {
        const string lib_paths(lib_path);
        //":" is the separator in LD_LIBRARY_PATH
        boost::split(paths, lib_paths, boost::is_any_of(":"));
        //trim ":" and " " from the beginning and end of the string
        for(string& path : paths)
        {
            boost::trim_if(path, boost::is_any_of(": "));
            boost::trim_right_if(path, boost::is_any_of("/"));
            library_paths.insert(path);
        }
    }
}

bool ClassLoader::loadLibrary(const std::string& class_name)
{
    if(library_paths.empty())
    {
        LOG(ERROR) << "Have no valid library paths. Please set LD_LIBRARY_PATH or add an library path manually.";
        return false;
    }

    std::string lib_name;
    if(!getClassLibraryPath(class_name, lib_name))
    {
        LOG(ERROR) << "Couldn't find library name for given class " << class_name;
        return false;
    }

    //try to load the plugin from all available paths
    for(const string& lib_path : library_paths)
    {
        string path = lib_path + "/lib" + lib_name + ".so";
        if(boost::filesystem::exists(path))
        {
            boost::shared_ptr<class_loader::ClassLoader> loader;
            loader.reset(new class_loader::ClassLoader(path, false));
            if(loader && loader->isLibraryLoaded())
            {
                loaders.insert(std::make_pair(lib_name, loader));
                return true;
            }
            else
                LOG(WARNING) << "Failed to load library in " << path;
        }
    }

    LOG(ERROR) << "Failed to load a plugin library " << lib_name << " for class " << class_name;
    return false;
}
