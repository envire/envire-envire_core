#ifndef __ENVIRE_CORE_FRAME__
#define __ENVIRE_CORE_FRAME__

#include <boost/serialization/string.hpp>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <typeindex>
    
#include "ItemBase.hpp"
#include "RandomGenerator.hpp"
#include <boost_serialization/BoostTypes.hpp>
#include <envire_core/serialization/Serialization.hpp>
#include <envire_core/util/Demangle.hpp>

namespace envire { namespace core
{
    /**A Frame is attached to each vertex in the TransformGraph.
     * It holds a lists of arbitrary items organized by item type. 
     * The Frame itself is a POD. The logic for manipulating Frames is part of
     * the TransformGraph. */
    class Frame
    {
    public:
        FrameId id; /** Frame name */

        using ItemList = std::vector<ItemBase::Ptr>;
        using ItemMap = std::unordered_map<std::type_index, ItemList>;
        //contains all items that have been added to the frame sorted by type
        ItemMap items;

    public:
        
        Frame() : id("envire::core::default_frame_id"){}
      
        Frame(const FrameId& id): id(id) {}

        ~Frame(){ this->items.clear(); }

        /**@brief setFrame
        *
        * Sets the frame name of the item
        */
        void setId(const FrameId& _id) { id = _id; }

        /**@brief getFrame
        *
        * Returns the frame name of the item
        */
        const FrameId& getId() const { return id; }
        
        /**Returns the total number of items in this frame */
        std::size_t calculateTotalItemCount() const 
        {
            std::size_t count = 0;
            for(const auto& itemPair : items)
            {
              count += itemPair.second.size();
            }
            return count;
        }
        
        const std::string toGraphviz() const 
        {
            std::stringstream out;
            out << "[shape=record, label=\"{{" << id <<
                   "|" << calculateTotalItemCount() << "}";
                
            for(const auto& itemPair : items)
            {
                std::string typeName = demangleTypeName(itemPair.first);
                typeName = escapeAngleBraces(typeName);
                out << "| {" << typeName  << "|" << itemPair.second.size() << "}";
            }
            out << "}\"" << ",style=filled,fillcolor=lightblue]";
            return out.str();
        }
        
        /**Visits all items in this frame.
         * @param func should have an operator(const ItemBase::Ptr)*/
        template <class T>
        void visitItems(T func) const
        {
            for(ItemMap::const_iterator it = items.begin(); it != items.end(); ++it)
            {
                const ItemList& list = it->second;
                for(const ItemBase::Ptr& item : list)
                {
                    func(item);
                }
            }
        }

    private:
        /**Grants access to boost serialization */
        friend class boost::serialization::access;

        /**Serializes the members of this class*/
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & BOOST_SERIALIZATION_NVP(id);
            ar & BOOST_SERIALIZATION_NVP(items);
        }

    };
}}

namespace boost { namespace serialization
{

    /**Serializes envire::core::Frame::ItemMap. Usses envire::core::Serialization
     * to correctly serialize ItemBase types. The type_index is not serialized, since
     * it is provided by each Item */
    template<class Archive>
    inline void save(Archive & ar,
                    const envire::core::Frame::ItemMap& item_map,
                    const unsigned int version)
    {
        // Store map size
        std::size_t map_size = item_map.size();
        ar << boost::serialization::make_nvp("map_size", map_size);
        for(envire::core::Frame::ItemMap::const_iterator it = item_map.begin(); it != item_map.end(); it++)
        {
            // Store list size
            const envire::core::Frame::ItemList& item_list = it->second;
            std::size_t list_size = item_list.size();
            ar << boost::serialization::make_nvp("list_size", list_size);
            for(envire::core::Frame::ItemList::const_iterator it = item_list.begin(); it != item_list.end(); it++)
            {
                // Serialize item
                envire::core::Serialization::save(ar, *it);
            }
        }
    }

    /**Unserializes envire::core::Frame::ItemMap. Uses envire::core::Serialization
     * to correctly unserialize ItemBase types. The type_index is retrieve from the
     * first Item in each ItemList */
    template<class Archive>
    inline void load(Archive & ar,
                     envire::core::Frame::ItemMap& item_map,
                     const unsigned int version)
    {
        // Recover map size
        std::size_t map_size;
        ar >> boost::serialization::make_nvp("map_size", map_size);
        for(std::size_t i = 0; i < map_size; i++)
        {
            // Recover list size
            envire::core::Frame::ItemList item_list;
            std::size_t list_size;
            ar >> boost::serialization::make_nvp("list_size", list_size);
            if(list_size > 0)
            {
                item_list.resize(list_size);
                for(std::size_t j = 0; j < list_size; j++)
                {
                    // Unserializes item
                    envire::core::ItemBase::Ptr item;
                    envire::core::Serialization::load(ar, item);
                    item_list[j] = item;
                }
            }

            if(!item_list.empty())
            {
                // Insert ItemList in ItemMap by using the type_index of the first element
                std::pair<std::type_index, envire::core::Frame::ItemList> new_entry(item_list.front()->getTypeIndex(), std::move(item_list));
                item_map.insert(std::move(new_entry));
            }
        }
    }

    /**Splits serialization of envire::core::Frame::ItemMap
     * into a load and save method */
    template<class Archive>
    inline void serialize(Archive & ar,
                          envire::core::Frame::ItemMap& item_map,
                          const unsigned int version)
    {
        boost::serialization::split_free(ar, item_map, version);
    }

}}
#endif
