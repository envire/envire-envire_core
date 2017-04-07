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

#pragma once
#include <typeindex>
#include <typeinfo>
#include <string>
#include <unordered_map>
#include <mutex>
/** Provides meta information such as type names of items */

namespace envire { namespace core
{
    
struct ItemMetadata
{
    /** Name of the plugin class. e.g. "envire::core::Item<int>"
     */
    std::string className;

    /**Name of the embedded typed. e.g. for Item<int> the name would be
    * "int".*/
    std::string embeddedTypename;
};

/**Provides a mapping between std::type_info and the metadata of the item.*/
class ItemMetadataMapping 
{
public:
   
    /**Return the meta-data corresponding to @p type.
     * The returned reference has static storage duration.
     * This method is thread-safe.
     * @throw std::out_of_range if type is not part of the mapping */
    static const ItemMetadata& getMetadata(const std::type_info& type);
    
    /** Returns true if metadata for the given type is known */
    static bool containsMetadata(const std::type_info& type);

private:
    /**Add meta data for a new type.
     * Is thread-safe.*/
    static void addMapping(const std::type_info& id, const ItemMetadata& data);

    using TypeMap = std::unordered_map<std::type_index, ItemMetadata>;
    static TypeMap mapping;
    static std::mutex mappingMutex; 
    
    friend class MetadataInitializer;//needs access to addMapping()
};

/**Used to add meta data at program startup */
struct MetadataInitializer
{
  MetadataInitializer(const std::type_info& type, const std::string& embeddedTypename, const std::string& className)
  {
    ItemMetadata data;
    data.embeddedTypename = embeddedTypename;
    data.className = className;
    ItemMetadataMapping::addMapping(type, data);
  }
};


}}
