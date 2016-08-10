#include "ItemMetadata.hpp"
#include <glog/logging.h>
#include <iostream>
using namespace std;

namespace envire { namespace core
{

ItemMetadataMapping::TypeMap ItemMetadataMapping::mapping;
mutex ItemMetadataMapping::mappingMutex;

void ItemMetadataMapping::addMapping(const std::type_info& id, const ItemMetadata& data)
{
  DLOG(INFO) << "Adding meta-data for " << id.name() << ". Content: className=" << data.className
            <<  ", embeddedTypename=" << data.embeddedTypename;
  
  lock_guard<mutex> guard(mappingMutex);
  mapping[type_index(id)] = data;
}

const ItemMetadata& ItemMetadataMapping::getMetadata(const std::type_info& type) 
{  
  lock_guard<mutex> guard(mappingMutex);
  const type_index id(type);
  if(mapping.find(id) == mapping.end())
    LOG(ERROR) << "No mapping for type: " << type.name();
  return mapping.at(id);
}

bool ItemMetadataMapping::containsMetadata(const std::type_info& type)
{
  lock_guard<mutex> guard(mappingMutex);
  const type_index id(type);
  return mapping.find(id) != mapping.end();
}

}}
