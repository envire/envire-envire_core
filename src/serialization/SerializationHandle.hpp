#pragma once

#include <envire_core/items/ItemBase.hpp>

namespace boost { namespace archive
{
    class binary_oarchive;
    class binary_iarchive;
    class text_oarchive;
    class text_iarchive;
}}

namespace envire { namespace core
{

/**
 * @brief Interface class of the item serialization handle.
 * By using the ENVIRE_REGISTER_SERIALIZATION macro a handle class for an
 * item will be generated.
 */
class SerializationHandle
{
public:
    virtual ~SerializationHandle() {}

    virtual bool save(boost::archive::binary_oarchive& ar, const ItemBase::Ptr& item) = 0;
    virtual bool load(boost::archive::binary_iarchive& ar, ItemBase::Ptr& item) = 0;

    virtual bool save(boost::archive::text_oarchive& ar, const ItemBase::Ptr& item) = 0;
    virtual bool load(boost::archive::text_iarchive& ar, ItemBase::Ptr& item) = 0;
};

}}