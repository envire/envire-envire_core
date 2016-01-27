#pragma once

#include <envire_core/items/ItemBase.hpp>

#include <boost/archive/polymorphic_iarchive.hpp>
#include <boost/archive/polymorphic_oarchive.hpp>

namespace envire { namespace core
{

typedef boost::archive::polymorphic_iarchive ArchiveInType;
typedef boost::archive::polymorphic_oarchive ArchiveOutType;

/**
 * @brief Interface class of the item serialization handle.
 * By using the ENVIRE_REGISTER_SERIALIZATION macro a handle class for an
 * item will be generated.
 */
class SerializationHandle
{
public:
    virtual ~SerializationHandle() {}

    virtual bool save(ArchiveOutType& ar, const ItemBase::Ptr& item) = 0;

    virtual bool load(ArchiveInType& ar, ItemBase::Ptr& item) = 0;
};

}}