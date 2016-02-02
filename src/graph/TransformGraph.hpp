/**\file TransformTree.hpp
 *
 * This class provided stores and manages the transformation graph
 *
 * @author Javier Hidalgo Carrio et. al (see THANKS  for the full Author list)
 * See LICENSE for the license information
 */
#pragma once

#include <boost/uuid/uuid.hpp>
#include <cassert>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <type_traits>


#include "Graph.hpp"
#include "GraphExceptions.hpp"
#include "TreeView.hpp"

#include <envire_core/events/GraphEventPublisher.hpp>
#define BOOST_RESULT_OF_USE_DECLTYPE //this is important for the transform_iterator
#include <boost/iterator/transform_iterator.hpp>
#include <envire_core/events/ItemAddedEvent.hpp>
#include <envire_core/events/ItemRemovedEvent.hpp>
#include <boost_serialization/BoostTypes.hpp>
#include <envire_core/util/Demangle.hpp>
#include <envire_core/items/Transform.hpp>



namespace envire { namespace core
{
    /**
     * FIXME comment
    */
    template <class FRAME_PROP>
    class TransformGraph :
        public envire::core::Graph<FRAME_PROP, envire::core::Transform>
    {
    public:
        TransformGraph();

        /** @return the transform between a and b. Calculating it if necessary.
         * @throw UnknownTransformException if the transformation doesn't exist
         * @throw UnknownFrameException if the @p origin or @p target does not exist*/
        const Transform getTransform(const FrameId& origin, const FrameId& target) const;
        const Transform getTransform(const vertex_descriptor origin, const vertex_descriptor target) const;

         /** @return the transform between a and b. Calculating it if necessary.
         * @throw UnknownTransformException if the transformation doesn't exist
         * @throw UnknownFrameException if the @p origin or @p target does not exist*/
        const Transform getTransform(const FrameId& origin, const FrameId& target, const TreeView &view) const;
        const Transform getTransform(const vertex_descriptor origin, const vertex_descriptor target, const TreeView &view) const;
        /** @return the transform between source(edge) and target(edge) */
        const Transform getTransform(const edge_descriptor edge) const;
        
        virtual ~TransformGraph();

        
    private:
        /**Grants access to boost serialization */
        friend class boost::serialization::access;

        /**Unserializes this class*/
        template <typename Archive>
        void load(Archive &ar, const unsigned int version);

        /**Serializes this class. Only the directed graph is serialized,
         * subscribers are excluded and the mapping of the labeled graph
         * is regenerated. */
        template <typename Archive>
        void save(Archive &ar, const unsigned int version) const;

        /**Splits boost serialize into a load and save method */
        template <typename Archive>
        void serialize(Archive &ar, const unsigned int version);
    };
    

    
    template <typename Archive>
    void TransformGraph::load(Archive &ar, const unsigned int version)
    {
      //FIXME
//         ar >> boost::serialization::make_nvp("directed_graph", _graph);
// 
//         // regenerate mapping of the labeled graph
//         boost::graph_traits<TransformGraphBase>::vertex_iterator it, end;
//         for (boost::tie( it, end ) = boost::vertices(_graph); it != end; ++it)
//         {
//             _map[_graph[*it].frame.name] = *it;
//         }
    }
    
    template <typename Archive>
    void TransformGraph::save(Archive &ar, const unsigned int version) const
    {
//         ar << boost::serialization::make_nvp("directed_graph", _graph);
    }
    
    template <typename Archive>
    void TransformGraph::serialize(Archive &ar, const unsigned int version)
    {
//         boost::serialization::split_member(ar, *this, version);
    }
  
}}
