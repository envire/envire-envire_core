#pragma once

#include <class_loader/class_loader.h>

/**
* ** Envire marcos **
* It is either possible to use ENVIRE_PLUGIN_HEADER and ENVIRE_REGISTER_PLUGIN to
* define a new envire plugin or to use ENVIRE_PLUGIN alone.
*/
/**
* The ENVIRE_PLUGIN_HEADER macro needs to be added inside of the class definition
*
* Example usage in *.hpp:
*      class VectorPlugin : public Item<Eigen::Vector3d>
*      {
*          ENVIRE_PLUGIN_HEADER(VectorPlugin)
*
*          [...]
*      }
*/
#define ENVIRE_PLUGIN_HEADER( _classname ) \
    protected: \
    static const std::string class_name; \
    public:\
    const std::string& getClassName() const { return class_name; } \
    typedef boost::shared_ptr<_classname> Ptr; \
    private: \
    ENVIRE_SERIALIZATION_HEADER(TemplateType)

/**
* The ENVIRE_REGISTER_PLUGIN macro exports the given class as plugin.
* The class must be inherited from envire::core::Item.
*
* Example usage in *.cpp:
*      ENVIRE_REGISTER_PLUGIN(ClassName)
*/
#define ENVIRE_REGISTER_PLUGIN( _classname ) \
const std::string _classname::class_name = #_classname; \
CLASS_LOADER_REGISTER_CLASS(_classname, envire::core::ItemBase); \
ENVIRE_REGISTER_SERIALIZATION(_classname, _classname::TemplateType)


/**
* The ENVIRE_PLUGIN macro defines the complete plugin class.
* When using this macro the plugin is defined in the source file
* and can therefore not directly instantiated.
*
* Example usage in *.cpp:
*      ENVIRE_PLUGIN(VectorPlugin, Eigen::Vector3d)
*/
#define ENVIRE_PLUGIN( _classname, _datatype ) \
class _classname : public envire::core::Item<_datatype> \
{ \
    ENVIRE_PLUGIN_HEADER( _classname ) \
}; \
ENVIRE_REGISTER_PLUGIN( _classname )

