#include "ItemMetadata.hpp"
#include <iostream>
using namespace std;

namespace envire { namespace core
{

ItemMetadataMapping::TypeMap ItemMetadataMapping::mapping;
mutex ItemMetadataMapping::mappingMutex;

void ItemMetadataMapping::addMapping(const std::type_info& id, const ItemMetadata& name)
{
  std::cout << "REEEEGISTER " << id.name() << "    " << name.embeddedTypename << std::endl;
  lock_guard<mutex> guard(mappingMutex);
  mapping[type_index(id)] = name;
}

const ItemMetadata& ItemMetadataMapping::getMetadata(const std::type_info& type) 
{
  std::cout << "AAAAAAAaa for " << type.name() << std::endl;
   
  lock_guard<mutex> guard(mappingMutex);
  
  for(TypeMap::iterator it = mapping.begin(); it != mapping.end(); ++it)
  {
    std::cout << it->first.name() << "   " << it->second.embeddedTypename << std::endl;
  }
  
  const type_index id(type);
  return mapping.at(id);
}

}}
