/*
 * TransformTreeTypes.hpp
 *
 *  Created on: Sep 16, 2015
 *      Author: aboeckmann
 *
 *  This file is needed to break a circular header dependency between
 *  TransformTree and FrameEventArgs.
 */

#ifndef SRC_TRANSFORMTREETYPES_HPP_
#define SRC_TRANSFORMTREETYPES_HPP_

#include <envire_core/items/Frame.hpp> /** Frames are for the Vertex **/
#include <envire_core/items/Transform.hpp> /** Transform are the Edges **/
#include <envire_core/items/Environment.hpp> /** Environment is the graph property **/
#include <boost_serialization/BaseTypes.hpp>
#include <boost_serialization/BoostTypes.hpp>

//HACK to gain access to private members in labeled_graph
//This is necessary because there is a bug in boost and a workaround needs to be
//implemented while we wait for boost to fix it
#define private protected
#include <boost/graph/labeled_graph.hpp>
#undef private

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <cassert>
#include <boost/signals2.hpp>
#include <boost/functional/hash.hpp> 
#include <functional> //to be able to provide custom hash functions for std maps

namespace envire { namespace core
{
    /**@brief Frame Property
     * boost access tag for the Frame property
     */
    struct FrameProperty
    {
        Frame frame;
        FrameProperty() : frame("envire::core::frame::default_name"){}
        explicit FrameProperty(const FrameId& frameId): frame(frameId){}
    };

    /**@brief Transform Property
     * boost access tag for the Transform property
     */
    struct TransformProperty
    {
        Transform transform;
    };

    /**@brief The Transform Graph type
    *
    * The basic class for the Transform Graph
    *
    */
    typedef boost::directed_graph<FrameProperty, TransformProperty, envire::core::Environment> TransformGraphBase;
    typedef boost::labeled_graph<TransformGraphBase, FrameId> LabeledTransformGraph;
    typedef LabeledTransformGraph::vertex_descriptor vertex_descriptor;
    typedef LabeledTransformGraph::edge_descriptor edge_descriptor;
    typedef LabeledTransformGraph::edge_iterator edge_iterator;
    typedef LabeledTransformGraph::vertex_iterator vertex_iterator;
    typedef LabeledTransformGraph::vertices_size_type vertices_size_type;
    typedef LabeledTransformGraph::edges_size_type edges_size_type;
    typedef LabeledTransformGraph::degree_size_type degree_size_type;
    typedef LabeledTransformGraph::out_edge_iterator out_edge_iterator;
    typedef LabeledTransformGraph::in_edge_iterator in_edge_iterator;
    
    /**A Path between two frames. Index 0 is the origin, index n the target of the path. */
    typedef std::vector<FrameId> Path;
    
    /**A hash function for the edge_descriptor.
     * @see http://stackoverflow.com/questions/15902134/removing-edges-temporarily-from-a-boost-graph*/
    template <class GRAPH>
    struct EdgeHash : std::unary_function<edge_descriptor, std::size_t> 
    {
        EdgeHash(const std::shared_ptr<GRAPH> graph) : graph(graph){}

        std::size_t operator()(edge_descriptor const& e) const {
        std::size_t hash = 1338;
        //Each edge is uniquely defined by it's source and target.
        //Thus we can use those vertex_descriptors to create the hash
        //value of the edge
        boost::hash_combine(hash, boost::source(e, **graph));
        boost::hash_combine(hash, boost::target(e, **graph));
        return hash;
      }
      const std::shared_ptr<GRAPH> graph;
    }; 
}}

namespace boost { namespace serialization
{
    /**Serialization of class FrameProperty */
    template<typename _Archive>
    inline void serialize(
        _Archive & ar,
        envire::core::FrameProperty& frame_property,
        const unsigned int version)
    {
        ar & boost::serialization::make_nvp("frame", frame_property.frame);
    }

    /**Serialization of class TransformProperty */
    template<typename _Archive>
    inline void serialize(
        _Archive & ar,
        envire::core::TransformProperty& transform_property,
        const unsigned int version)
    {
        ar & boost::serialization::make_nvp("transform", transform_property.transform);
    }
}}

#endif /* SRC_TRANSFORMTREETYPES_HPP_ */
