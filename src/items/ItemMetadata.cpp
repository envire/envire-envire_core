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
  LOG(INFO) << "Adding meta-data for " << id.name() << ". Content: embeddedTypename="
            << data.embeddedTypename;
  
  lock_guard<mutex> guard(mappingMutex);
  mapping[type_index(id)] = data;
}

const ItemMetadata& ItemMetadataMapping::getMetadata(const std::type_info& type) 
{  
  lock_guard<mutex> guard(mappingMutex);
  const type_index id(type);
  return mapping.at(id);
}

}}
