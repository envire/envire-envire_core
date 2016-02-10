#include "PluginManager.hpp"
#include <tinyxml.h>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <glog/logging.h>

using namespace envire::core;

static const std::string plugin_files_path = "/share/rock/plugin/";
static const std::string plugin_file_extension = ".xml";

PluginManager::PluginManager(const std::vector< std::string >& plugin_xml_paths, bool load_environment_paths)
{
    std::copy(plugin_xml_paths.begin(), plugin_xml_paths.end(), std::back_inserter(this->plugin_xml_paths));
    if(load_environment_paths)
    {
        std::vector<std::string> env_xml_paths = getPluginXmlPathsFromEnv();
        std::copy(env_xml_paths.begin(), env_xml_paths.end(), std::back_inserter(this->plugin_xml_paths));
    }

    reloadXMLPluginFiles();
}

PluginManager::~PluginManager()
{
    clear();
}

std::vector< std::string > PluginManager::getPluginXmlPaths() const
{
    return plugin_xml_paths;
}

std::vector< std::string > PluginManager::getAvailableClasses() const
{
    std::vector<std::string> classes;
    classes.reserve(classes_available.size());
    for(const std::pair<std::string, PluginInfoPtr>& plugin_info : classes_available)
    {
        classes.push_back(plugin_info.second->full_class_name);
    }
    return classes;
}

std::vector< std::string > PluginManager::getAvailableClasses(const std::string& base_class) const
{
    std::vector<std::string> classes;
    classes.reserve(base_classes_available.count(base_class));
    std::pair<std::multimap<std::string, PluginInfoPtr>::const_iterator, std::multimap<std::string, PluginInfoPtr>::const_iterator> range;
    range = base_classes_available.equal_range(base_class);
    for(std::multimap<std::string, PluginInfoPtr>::const_iterator it = range.first; it != range.second; it++)
    {
        classes.push_back(it->second->full_class_name);
    }
    return classes;
}

bool PluginManager::getBaseClass(const std::string& class_name, std::string& base_class) const
{
    std::string full_class_name;
    if(!getFullClassName(class_name, full_class_name))
        return false;

    std::map<std::string, PluginInfoPtr>::const_iterator plugin_info = classes_available.find(full_class_name);
    if(plugin_info != classes_available.end())
    {
        base_class = plugin_info->second->base_class_name;
        return true;
    }
    return false;
}

bool PluginManager::getClassDescription(const std::string& class_name, std::string& class_description) const
{
    std::string full_class_name;
    if(!getFullClassName(class_name, full_class_name))
        return false;

    std::map<std::string, PluginInfoPtr>::const_iterator plugin_info = classes_available.find(full_class_name);
    if(plugin_info != classes_available.end())
    {
        class_description = plugin_info->second->description;
        return true;
    }
    return false;
}

bool PluginManager::getClassLibraryPath(const std::string& class_name, std::string& library_path) const
{
    std::string full_class_name;
    if(!getFullClassName(class_name, full_class_name))
        return false;

    std::map<std::string, PluginInfoPtr>::const_iterator plugin_info = classes_available.find(full_class_name);
    if(plugin_info != classes_available.end())
    {
        library_path = plugin_info->second->library_path;
        return true;
    }
    return false;
}

std::set< std::string > PluginManager::getRegisteredLibraries() const
{
    std::set< std::string > registered_libraries;
    for(const std::pair<std::string, PluginInfoPtr>& plugin_info : classes_available)
    {
        registered_libraries.insert(plugin_info.second->library_path);
    }
    return registered_libraries;
}

bool PluginManager::removeClassInfo(const std::string& class_name)
{
    std::string full_class_name;
    if(!getFullClassName(class_name, full_class_name))
        return false;

    std::map<std::string, PluginInfoPtr>::iterator plugin_info = classes_available.find(full_class_name);
    if(plugin_info != classes_available.end())
    {
        std::pair<std::multimap<std::string, PluginInfoPtr>::iterator, std::multimap<std::string, PluginInfoPtr>::iterator> range;
        range = base_classes_available.equal_range(plugin_info->second->base_class_name);
        for(std::multimap<std::string, PluginInfoPtr>::iterator it = range.first; it != range.second; it++)
        {
            if(it->second == plugin_info->second)
                base_classes_available.erase(it);
        }
        range = classes_no_ns_available.equal_range(plugin_info->second->class_name);
        for(std::multimap<std::string, PluginInfoPtr>::iterator it = range.first; it != range.second; it++)
        {
            if(it->second == plugin_info->second)
                classes_no_ns_available.erase(it);
        }
        classes_available.erase(plugin_info);
        return true;
    }
    return false;
}

void PluginManager::clear()
{
    classes_available.clear();
    base_classes_available.clear();
    classes_no_ns_available.clear();
}

void PluginManager::reloadXMLPluginFiles()
{
    std::set<std::string> plugin_xml_files;
    for(const std::string &folder : plugin_xml_paths)
    {
        determineAvailableXMLPluginFiles(folder, plugin_xml_files);
    }
    for(const std::string &file : plugin_xml_files)
    {
        std::vector< PluginManager::PluginInfoPtr > classes;
        if(processSingleXMLPluginFile(file, classes))
            insertPluginInfos(classes);
    }
}

std::vector< std::string > PluginManager::getPluginXmlPathsFromEnv() const
{
    const char* install_path = std::getenv("CMAKE_PREFIX_PATH");
    std::vector<std::string> paths;
    if(install_path != NULL)
    {
        const std::string path(install_path);
        //":" is the separator in CMAKE_PREFIX_PATH
        boost::split(paths, path, boost::is_any_of(":"));
        //trim ":" and " " from the beginning and end of the string
        for(std::string& path : paths)
        {
            boost::trim_if(path, boost::is_any_of(": "));
            boost::trim_right_if(path, boost::is_any_of("/"));
            path += plugin_files_path;
        }
    }
    return paths;
}

void PluginManager::determineAvailableXMLPluginFiles(const std::string& plugin_xml_folder, std::set<std::string>& plugin_xml_files) const
{
    boost::filesystem::path folder(plugin_xml_folder);
    if(boost::filesystem::exists(folder))
    {
        if(boost::filesystem::is_directory(folder))
        {
            for(boost::filesystem::directory_iterator it(folder); it != boost::filesystem::directory_iterator(); it++)
            {
                if(boost::filesystem::is_regular_file(*it))
                {
                    std::string extension = it->path().extension().string();
                    boost::algorithm::to_lower(extension);

                    if(extension == plugin_file_extension)
                        plugin_xml_files.insert(it->path().string());
                }
            }
        }
        else if(boost::filesystem::is_regular_file(folder))
        {
            // folder is actualy a file
            std::string extension = folder.extension().string();
            boost::algorithm::to_lower(extension);

            if(extension == plugin_file_extension)
                plugin_xml_files.insert(folder.string());
        }
    }
}

bool PluginManager::processSingleXMLPluginFile(const std::string& xml_file, std::vector< PluginInfoPtr >& class_available) const
{
    TiXmlDocument document;
    document.LoadFile(xml_file);

    TiXmlElement* library = document.RootElement();
    if (library == NULL)
    {
        LOG(ERROR) << "Skipping XML Document " << xml_file << " which had no Root Element. This likely means the XML is malformed or missing.";
        return false;
    }
    if (library->ValueStr() != "library")
    {
        LOG(ERROR) << "The XML document " << xml_file << " must have \"library\" as root tag.";
        return false;
    }

    while (library != NULL)
    {
        // read library path
        std::string library_path = library->Attribute("path");
        if (library_path.empty())
        {
            LOG(ERROR) << "Failed to find path attirbute in library element in " << xml_file;
            continue;
        }

        TiXmlElement* class_element = library->FirstChildElement("class");
        while (class_element != NULL)
        {
            PluginInfoPtr plugin_info(new PluginInfo);
            plugin_info->base_class_name = class_element->Attribute("base_class_name");
            plugin_info->full_class_name = class_element->Attribute("class_name");

            if(!plugin_info->full_class_name.empty() && !plugin_info->base_class_name.empty())
            {
                plugin_info->library_path = library_path;
                plugin_info->class_name = removeNamespace(plugin_info->full_class_name);

                TiXmlElement* description_element = class_element->FirstChildElement("description");
                plugin_info->description = description_element->GetText();

                class_available.push_back(plugin_info);
            }
            else
            {
                LOG(ERROR) << "Couldn't find a valid class_name or base_class_name attribute in class element in " << xml_file;
            }
            class_element = class_element->NextSiblingElement("class");
        }
        library = library->NextSiblingElement("library");
    }

    return true;
}

void PluginManager::insertPluginInfos(const std::vector<PluginInfoPtr>& classes)
{
    for(const PluginInfoPtr &plugin_info : classes)
    {
        if(classes_available.count(plugin_info->full_class_name) == 0)
        {
            classes_available[plugin_info->full_class_name] = plugin_info;
            base_classes_available.insert(std::make_pair(plugin_info->base_class_name, plugin_info));
            classes_no_ns_available.insert(std::make_pair(plugin_info->class_name, plugin_info));
        }
        else
        {
            LOG(WARNING) << "Class " << plugin_info->full_class_name << " already available, cannot add class info twice.";
        }
    }
}

std::string PluginManager::removeNamespace(const std::string& class_name) const
{
    std::vector<std::string> split_names;
    boost::split(split_names, class_name, boost::is_any_of("::"));
    if(!split_names.empty())
    {
        return split_names.back();
    }
    else
    {
        return class_name;
    }
}

bool PluginManager::getFullClassName(const std::string& class_name, std::string& full_class_name) const
{
    if(classes_available.count(class_name) >= 1)
    {
        // even if class_name doesn't have a namespace, this is all information we have
        full_class_name = class_name;
        return true;
    }
    else
    {
        size_t count = classes_no_ns_available.count(class_name);
        if(count == 1)
        {
            full_class_name = classes_no_ns_available.find(class_name)->second->full_class_name;
            return true;
        }
        else if(count == 0)
            LOG(WARNING) << "Class " << class_name << " is unknown.";
        else
            LOG(WARNING) << "Class " << class_name << " is multiple defined in different namespaces. Please use the full class name.";
    }
    return false;
}