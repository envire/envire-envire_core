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

#include "SerializationHandle.hpp"
#include "Serialization.hpp"
#include <envire_core/items/ItemBase.hpp>

#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

namespace envire { namespace core
{

/**
 * It does a BOOST_CLASS_EXPORT of the parent classes to allow serialization
 * of polymorphic classes. It defines a serialization handle which is registered
 * in a static map used by the methods in the class envire::core::Serialization.
 */
#define ENVIRE_REGISTER_SERIALIZATION( _classname, _datatype) \
ENVIRE_REGISTER_SERIALIZATION_INTERNAL( _classname, _datatype, __COUNTER__ )

#define ENVIRE_REGISTER_SERIALIZATION_INTERNAL( _classname, _datatype, _unique_id ) \
BOOST_CLASS_EXPORT(_classname) \
class SerializationHandle ## _unique_id : public envire::core::SerializationHandle \
{ \
public: \
    virtual bool save(boost::archive::binary_oarchive& ar, const envire::core::ItemBase::Ptr& item) \
    { \
        ar << BOOST_SERIALIZATION_NVP(item); \
        return true; \
    }; \
    virtual bool load(boost::archive::binary_iarchive& ar, envire::core::ItemBase::Ptr& item) \
    { \
        ar >> BOOST_SERIALIZATION_NVP(item); \
        return true; \
    }; \
    virtual bool save(boost::archive::text_oarchive& ar, const envire::core::ItemBase::Ptr& item) \
    { \
        ar << BOOST_SERIALIZATION_NVP(item); \
        return true; \
    }; \
    virtual bool load(boost::archive::text_iarchive& ar, envire::core::ItemBase::Ptr& item) \
    { \
        ar >> BOOST_SERIALIZATION_NVP(item); \
        return true; \
    }; \
}; \
static envire::core::SerializationRegistration<SerializationHandle ## _unique_id> reg(#_classname);




/**
 * It defines the boost serialize method for this item. If there are more members
 * in the inherited class that shall be serialized, then this macro must not be used.
 * This macro is automaticly called by the macro ENVIRE_PLUGIN_HEADER.
 * @deprecated
 */
#define ENVIRE_SERIALIZATION_HEADER( _datatype ) \
friend class boost::serialization::access; \
template <typename Archive> \
void serialize(Archive &ar, const unsigned int version) \
{ \
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(envire::core::Item<_datatype>); \
}


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