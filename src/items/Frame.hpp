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

#ifndef __ENVIRE_CORE_FRAME__
#define __ENVIRE_CORE_FRAME__

#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <typeindex>
#include <glog/logging.h>
    
#include "ItemBase.hpp"
#include "RandomGenerator.hpp"
#include <boost_serialization/BoostTypes.hpp>
#include <boost_serialization/DynamicSizeSerialization.hpp>
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
        
        const std::string toString() const 
        {
            std::stringstream out;
            out << id <<
                   " | " << calculateTotalItemCount();
                   //FIXME improve output
/*                
            for(const auto& itemPair : items)
            {
                std::string typeName = demangleTypeName(itemPair.first);
                typeName = escapeAngleBraces(typeName);
                out << "| {" << typeName  << "|" << itemPair.second.size() << "}";
            }
            out << "}\"" << ",style=filled,fillcolor=lightblue]";*/
            return out.str();
        }
        
        /** @return A list of all item types used in this frame */
        std::vector<std::type_index> getItemTypes() const
        {
            std::vector<std::type_index> result;
            for(const auto& itemPair : items)
            {
              result.push_back(itemPair.first);
            }
            return result;
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

BOOST_CLASS_VERSION(envire::core::Frame::ItemMap, 1)

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
        std::vector<bool> serializable(item_map.size(), false);
        uint64_t serializable_types = 0;
        for(envire::core::Frame::ItemMap::const_iterator it = item_map.begin(); it != item_map.end(); it++)
        {
            if(!it->second.empty() && envire::core::Serialization::isSerializable(it->second.front()))
            {
                serializable[std::distance(item_map.begin(), it)] = true;
                serializable_types++;
            }
        }

        if(version == 0)
        {
            std::size_t serializable_types_st = serializable_types;
            ar << boost::serialization::make_nvp("map_size", serializable_types_st);
        }
        else
            saveSizeValue(ar, serializable_types);
        for(envire::core::Frame::ItemMap::const_iterator it = item_map.begin(); it != item_map.end(); it++)
        {
            if(serializable[std::distance(item_map.begin(), it)])
            {
                // Store list size
                const envire::core::Frame::ItemList& item_list = it->second;
                uint64_t list_size = item_list.size();
                if(version == 0)
                {
                    std::size_t list_size_st = list_size;
                    ar << boost::serialization::make_nvp("list_size", list_size_st);
                }
                else
                    saveSizeValue(ar, list_size);
                for(envire::core::Frame::ItemList::const_iterator it = item_list.begin(); it != item_list.end(); it++)
                {
                    // Serialize item
                    if(!envire::core::Serialization::save(ar, *it))
                    {
                        std::string error_msg = "Failed to serialize an item of type ";
                        std::string class_name;
                        if((*it)->getClassName(class_name))
                            error_msg += class_name;
                        else
                            class_name += (*it)->getEmbeddedTypeInfo()->name();
                        throw std::runtime_error(error_msg);
                    }
                }
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
        item_map.clear();
        // Recover map size
        uint64_t map_size;
        if(version == 0)
        {
            std::size_t map_size_st;
            ar >> boost::serialization::make_nvp("map_size", map_size_st);
            map_size = map_size_st;
        }
        else
            loadSizeValue(ar, map_size);
        for(std::size_t i = 0; i < map_size; i++)
        {
            // Recover list size
            envire::core::Frame::ItemList item_list;
            uint64_t list_size;
            if(version == 0)
            {
                std::size_t list_size_st;
                ar >> boost::serialization::make_nvp("list_size", list_size_st);
                list_size = list_size_st;
            }
            else
                loadSizeValue(ar, list_size);
            if(list_size > 0)
            {
                item_list.reserve(list_size);
                for(std::size_t j = 0; j < list_size; j++)
                {
                    // Unserializes item
                    envire::core::ItemBase::Ptr item;
                    if(envire::core::Serialization::load(ar, item))
                        item_list.push_back(item);
                }
            }

            if(!item_list.empty())
            {
                // Insert ItemList in ItemMap by using the type_index of the first element
                std::pair<std::type_index, envire::core::Frame::ItemList> new_entry(item_list.front()->getTypeIndex(), std::move(item_list));
                item_map.insert(std::move(new_entry));
            }
        }

        if(item_map.size() < map_size)
            LOG(ERROR) << "Failed to deserialize all items. All items of " << (map_size-item_map.size()) << " types have been dropped.";
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
