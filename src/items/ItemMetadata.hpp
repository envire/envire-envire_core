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
  MetadataInitializer(const std::type_info& type, const std::string& embeddedTypename)
  {
    ItemMetadata data;
    data.embeddedTypename = embeddedTypename;
    ItemMetadataMapping::addMapping(type, data);
  }
};


}}
