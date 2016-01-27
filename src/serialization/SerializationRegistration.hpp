#pragma once

#include "SerializationHandle.hpp"
#include "Serialization.hpp"
#include <envire_core/items/ItemBase.hpp>

#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>

namespace envire { namespace core
{

/**
 * It defines the boost serialize method for this item. If there are more members
 * in the inherited class that shall be serialized, then this macro must not be used.
 * This macro is automaticly called by the macro ENVIRE_PLUGIN_HEADER.
 */
#define ENVIRE_SERIALIZATION_HEADER( _datatype ) \
friend class boost::serialization::access; \
template <typename Archive> \
void serialize(Archive &ar, const unsigned int version) \
{ \
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(envire::core::Item<_datatype>); \
}

/**
 * It does a BOOST_CLASS_EXPORT of the parent classes to allow serialization
 * of polymorphic classes. It defines a serialization handle which is registered
 * in a static map used by the methods in the class envire::core::Serialization.
 */
#define ENVIRE_REGISTER_SERIALIZATION( _classname, _datatype ) \
BOOST_CLASS_EXPORT(envire::core::ItemBase) \
BOOST_CLASS_EXPORT(envire::core::Item< _datatype >) \
BOOST_CLASS_EXPORT(_classname) \
class _classname ## SerializationHandle : public envire::core::SerializationHandle \
{ \
public: \
    virtual bool save(envire::core::ArchiveOutType& ar, const envire::core::ItemBase::Ptr& item) \
    { \
        ar << BOOST_SERIALIZATION_NVP(item); \
        return true; \
    }; \
    virtual bool load(envire::core::ArchiveInType& ar, envire::core::ItemBase::Ptr& item) \
    { \
        ar >> BOOST_SERIALIZATION_NVP(item); \
        return true; \
    }; \
}; \
static envire::core::SerializationRegistration<_classname ## SerializationHandle> reg(#_classname);

/**
 * Helper class which is used to register a handle to the static handle map.
 */
template<typename T>
class SerializationRegistration
{
public:
    SerializationRegistration(const std::string& class_name)
    {
        boost::shared_ptr<SerializationHandle> ptr(new T);
        Serialization::registerHandle(class_name, ptr);
    }
};

}}