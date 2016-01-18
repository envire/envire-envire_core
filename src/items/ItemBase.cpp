#include "ItemBase.hpp"
#include "RandomGenerator.hpp"

using namespace envire::core;

const std::string ItemBase::class_name = "envire::core::ItemBase";

ItemBase::ItemBase() : time(base::Time::now()), uuid(RandomGenerator::getRandomGenerator()()), user_data_ptr(NULL)
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