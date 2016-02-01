#pragma once

#include <boost/graph/adjacency_list.hpp>
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

#include <envire_core/items/Frame.hpp>
#include <envire_core/items/Transform.hpp>
#include <envire_core/items/Environment.hpp>

namespace envire { namespace core { namespace graph
{
    /**@brief Frame Property
     * boost access tag for the Frame property
     */
    struct FramePropertyBase
    {
        FrameId id;
        
        FramePropertyBase() : id("envire::core::default_name"){}
        explicit FramePropertyBase(const FrameId& frameId): id(frameId){}
        void setId(const FrameId& _id)
        {
          id = _id;
        }
    };
    
    /**A concept checking class to enforce the existence of an inverse() method */
    template <class T>
    class Invertable
    {
    public:
        BOOST_CONCEPT_USAGE(Invertable)
        {
            T u = t.inverse();//check for inverse() method that returns a T
            u.inverse();//suppress "u not used" warning
        }
    private:
      const T t;
    };
    

    /**@brief Transform Property
     * boost access tag for the Transform property
     */
    struct TransformProperty
    {
        envire::core::Transform transform;
    };

    template <class FRAME_PROP, class EDGE_PROP>
    using GraphBase = boost::labeled_graph<boost::directed_graph<FRAME_PROP, EDGE_PROP, envire::core::Environment>, FrameId>;
    
    
    /**A Path between two frames. Index 0 is the origin, index n the target of the path. */
    typedef std::vector<FrameId> Path;
    
    /**A hash function for the edge_descriptor.
     * @see http://stackoverflow.com/questions/15902134/removing-edges-temporarily-from-a-boost-graph*/
    template <class GRAPH>
    struct EdgeHash : std::unary_function<typename GRAPH::edge_descriptor, std::size_t> 
    {
        EdgeHash(const std::shared_ptr<GRAPH> graph) : graph(graph){}

        std::size_t operator()(typename GRAPH::edge_descriptor const& e) const {
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
}}}
