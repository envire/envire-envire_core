#pragma once

#include "SerializationHandle.hpp"
#include "Serialization.hpp"
#include "Eigen.hpp"
#include <envire_core/items/ItemBase.hpp>

#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>

namespace envire { namespace core
{

#define ENVIRE_SERIALIZATION_HEADER( _datatype ) \
friend class boost::serialization::access; \
template <typename Archive> \
void serialize(Archive &ar, const unsigned int version) \
{ \
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(envire::core::Item<_datatype>); \
}

#define ENVIRE_REGISTER_SERIALIZATION( _classname, _datatype ) \
BOOST_CLASS_EXPORT(envire::core::ItemBase) \
BOOST_CLASS_EXPORT(envire::core::Item< _datatype >) \
BOOST_CLASS_EXPORT(_classname) \
class _classname ## SerializationHandle : public envire::core::SerializationHandle \
{ \
private: \
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