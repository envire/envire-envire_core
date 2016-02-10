#pragma once

#include <string>

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

    /** Path to the library relative to the install folder */
    std::string library_path;

    /** Description of this plugin class */
    std::string description;
};

}}