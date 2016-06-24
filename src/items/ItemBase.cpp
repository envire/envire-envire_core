#include "ItemBase.hpp"
#include "RandomGenerator.hpp"
#define BOOST_SERIALIZATION_DYN_LINK 1

using namespace envire::core;

ItemBase::ItemBase() : time(base::Time::now()), uuid(RandomGenerator::getRandomGenerator()())
{
}

ItemBase::ItemBase(const ItemBase& item) : time(item.time), uuid(item.uuid),
                                           frame_name(item.frame_name)
{
}

ItemBase::ItemBase(ItemBase&& item) : time(std::move(item.time)), uuid(std::move(item.uuid)),
                                      frame_name(std::move(item.frame_name))
{
}

ItemBase& ItemBase::operator=(const ItemBase& item)
{
    time = item.time;
    uuid = item.uuid;
    frame_name = item.frame_name;
    return *this;
}

ItemBase& ItemBase::operator=(ItemBase&& item)
{
    time = std::move(item.time);
    uuid = std::move(item.uuid);
    frame_name = std::move(item.frame_name);
    return *this;
}

BOOST_CLASS_EXPORT(envire::core::ItemBase)