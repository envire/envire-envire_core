#include "ItemBase.hpp"
#include "RandomGenerator.hpp"

using namespace envire::core;

const std::string ItemBase::class_name = "envire::core::ItemBase";

ItemBase::ItemBase() : time(base::Time::now()), uuid(RandomGenerator::getRandomGenerator()()), user_data_ptr(NULL)
{
}
