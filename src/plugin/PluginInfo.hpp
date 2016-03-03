#pragma once

#include <string>
#include <vector>

namespace envire { namespace core
{

/**
 * Stores the information of a class that can be
 * loaded by class loader.
 */
struct PluginInfo
{
    /** Name of the class, without namespace if the class has one */
    std::string class_name;

    /** Full name of the class with namespace.
     *  If the class has no namespace this is equal to class_name */
    std::string full_class_name;

    /** Full name of the base class, this class is inherited from */
    std::string base_class_name;

    /** Names of classes that are associated to this one. This fields are optional.
     * E.g. This is a visualization plugin for a certain class */
    std::vector<std::string> associated_classes;

    /** Path to the library relative to the install folder */
    std::string library_path;

    /** Description of this plugin class. This field is optional. */
    std::string description;

    /** Marks the plugin as singleton. This field is optional and false per default. */
    bool singleton;
};

}}