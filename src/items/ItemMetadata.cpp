//
// Copyright (c) 2015, Deutsches Forschungszentrum für Künstliche Intelligenz GmbH.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

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
