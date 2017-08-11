//
// Copyright (c) 2015, Deutsches Forschungszentrum für Künstliche Intelligenz GmbH.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#pragma once

#include <class_loader/class_loader_register_macro.h>
#include <envire_core/items/ItemMetadata.hpp>
#include <envire_core/items/Item.hpp>
#include <envire_core/serialization/SerializationRegistration.hpp>

/**
* The macro ENVIRE_REGISTER_ITEM(DataType) exports the class envire::core::Item<DataType> as class_loader plugin.
* It is recommended to use the full namespace of the embedded type.
*
* Example usage in *.cpp:
*      ENVIRE_REGISTER_ITEM(DataType)
*/
#define ENVIRE_REGISTER_ITEM( _datatype ) \
CLASS_LOADER_REGISTER_CLASS(envire::core::Item<_datatype>, envire::core::ItemBase) \
ENVIRE_REGISTER_SERIALIZATION(envire::core::Item<_datatype>, envire::core::Item<_datatype>::TemplateType) \
ENVIRE_REGISTER_METADATA(envire::core::Item<_datatype>, _datatype)

#define ENVIRE_REGISTER_METADATA( _classname, _datatype ) \
static envire::core::MetadataInitializer _metadataInit(typeid(_classname), #_datatype, #_classname);

/**
 * The following macros are @deprecated, use ENVIRE_REGISTER_ITEM instead
 */
/**
* The ENVIRE_PLUGIN_HEADER macro needs to be added inside of the class definition.
* It is not nessesary, but recommended to add the namespace in order to have a more
* unique lookup name.
*
* Example usage in *.hpp:
*      class VectorPlugin : public Item<Eigen::Vector3d>
*      {
*          ENVIRE_PLUGIN_HEADER(Namespace::VectorPlugin)
*
*          [...]
*      }
*
* @deprecated
*/
#define ENVIRE_PLUGIN_HEADER( _classname ) \
    public: \
    typedef boost::shared_ptr<_classname> Ptr; \
    private: \
    ENVIRE_SERIALIZATION_HEADER(TemplateType)


/**
* The ENVIRE_REGISTER_PLUGIN macro exports the given class as plugin.
* The class must be inherited from envire::core::Item.
* It is not nessesary, but recommended to add the namespace in order to have a more
* unique lookup name.
*
* Example usage in *.cpp:
*      ENVIRE_REGISTER_PLUGIN(Namespace::ClassName, DataType)
*
* @deprecated
*/
#define ENVIRE_REGISTER_PLUGIN1( _classname ) \
CLASS_LOADER_REGISTER_CLASS(_classname, envire::core::ItemBase); \
ENVIRE_REGISTER_SERIALIZATION(_classname, _classname::TemplateType)

#define ENVIRE_REGISTER_PLUGIN2(_classname, _datatype) \
CLASS_LOADER_REGISTER_CLASS(_classname, envire::core::ItemBase); \
ENVIRE_REGISTER_SERIALIZATION(_classname, _classname::TemplateType) \
static envire::core::MetadataInitializer _metadataInit(typeid(envire::core::Item<_datatype>), #_datatype, #_classname);

/** Overload selector for ENVIRE_REGISTER_PLUGIN.
 *  FIXME should be removed as soon as the transition to the new macro is done */
#define GET_MACRO(_1,_2,NAME,...) NAME
#define ENVIRE_REGISTER_PLUGIN(...) GET_MACRO(__VA_ARGS__, ENVIRE_REGISTER_PLUGIN2, ENVIRE_REGISTER_PLUGIN1)(__VA_ARGS__)

/**
* The ENVIRE_PLUGIN macro defines the complete plugin class.
* When using this macro the plugin is defined in the source file
* and can therefore not directly instantiated.
*
* Example usage in *.cpp:
*      ENVIRE_PLUGIN(VectorPlugin, Eigen::Vector3d)
*
* @deprecated
*/
#define ENVIRE_PLUGIN( _classname, _datatype ) \
class _classname : public envire::core::Item<_datatype> \
{ \
    ENVIRE_PLUGIN_HEADER( _classname ) \
}; \
ENVIRE_REGISTER_PLUGIN( _classname, _datatype )
