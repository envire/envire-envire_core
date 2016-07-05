#include "ItemBase.hpp"
#include "RandomGenerator.hpp"
#define BOOST_SERIALIZATION_DYN_LINK 1

using namespace envire::core;

ItemBase::ItemBase()
{
}

ItemBase::ItemBase(const ItemBase& item)
{
}

ItemBase::ItemBase(ItemBase&& item)
{
}

ItemBase& ItemBase::operator=(const ItemBase& item)
{
    return *this;
}

ItemBase& ItemBase::operator=(ItemBase&& item)
{
    return *this;
}

BOOST_CLASS_EXPORT(envire::core::ItemBase)