#include "ItemBase.hpp"

#include <boost/uuid/uuid_generators.hpp>

using namespace envire::core;

const std::string ItemBase::class_name = "envire::core::ItemBase";

void envire::core::intrusive_ptr_add_ref( ItemBase* item ) { item->ref_count++; }
void envire::core::intrusive_ptr_release( ItemBase* item ) { if(!--item->ref_count) delete item; }

ItemBase::ItemBase() : ref_count(0), time(base::Time::now()), uuid(boost::uuids::random_generator()()), user_data_ptr(NULL)
{
}