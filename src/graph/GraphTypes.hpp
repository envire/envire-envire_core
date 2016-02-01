#pragma once

#include <boost/graph/adjacency_list.hpp>
//HACK to gain access to private members in labeled_graph
//This is necessary because there is a bug in boost and a workaround needs to be
//implemented while we wait for boost to fix it
#define private protected
#include <boost/graph/labeled_graph.hpp>
#undef private

#include <string>
#include <type_traits>
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
    
    /**Concept checking class for frame properties in the Graph.
     * A FrameProperty should:
     * * store a FrameId.
     * * provide getId() method that returns a const reference to the id.
     * * provide a setId(const FrameId&) method.
     * * be default constructible

     */
    template <class T>
    class FramePropertyConcept
    {
      BOOST_CONCEPT_ASSERT((boost::DefaultConstructible<T>));
    public:
      BOOST_CONCEPT_USAGE(FramePropertyConcept)
      {
        T prop;
        static_assert(std::is_same<decltype(t.getId()), const FrameId&>::value, "getId() must return a const FrameId&");
        const FrameId& id = t.getId();
        prop.setId(id);
      }
      
    private:
      const T t;
    };
    
    /**A concept checking class for edge properties in the Graph.
     * An EdgeProperty should:
     * * provide an inverse() method that creates an inverted copy.*/
    template <class T>
    class EdgePropertyConcept
    {
    public:
        BOOST_CONCEPT_USAGE(EdgePropertyConcept)
        {
            T u = t.inverse();//check for inverse() method that returns a T
            u.inverse();//suppress "u not used" warning
        }
    private:
      const T t; //inverse() has to be const
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
