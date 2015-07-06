#include "ItemBase.hpp"

#include <boost/uuid/uuid_generators.hpp>

using namespace envire::core;

const std::string ItemBase::class_name = "envire::core::ItemBase";

ItemBase::ItemBase() : time(base::Time::now()), uuid(boost::uuids::random_generator()()), user_data_ptr(NULL)
{
}