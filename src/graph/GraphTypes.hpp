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

#include <envire_core/items/Environment.hpp>
#include <envire_core/items/ItemBase.hpp>
#include <envire_core/serialization/SerializableConcept.hpp>


namespace envire { namespace core
{
    
    /**Concept checking class for frame properties in the Graph.
     * 
     * A FrameProperty should:
     *  * store a FrameId.
     * 
     *  * provide getId() const method that returns a const reference to the id.
     * 
     *  * provide a setId(const FrameId&) method.
     * 
     *  * provide a toString() const method that returns a const reference to a
     *    string representation of this frame. 
     * 
     *  * be default constructible
     */
    template <class T>
    class FramePropertyConcept
    {
        BOOST_CONCEPT_ASSERT((boost::DefaultConstructible<T>));
        BOOST_CONCEPT_ASSERT((SerializableConcept<T>));
    public:
        BOOST_CONCEPT_USAGE(FramePropertyConcept)
        {
          T prop;
          static_assert(std::is_same<decltype(t.getId()), const FrameId&>::value, "getId() must return a const FrameId&");
          const FrameId& id = t.getId();
          prop.setId(id);
          const std::string graphviz_text = t.toString(); //check for toString() method
        }
      
    private:
        const T t;
    };
    
    /**A concept checking class for edge properties in the Graph.
     * 
     * An EdgeProperty should:
     *  * provide an inverse() method that creates an inverted copy.
     * 
     *  * provide a toString() const method that returns a const reference to a
     *    string representation of this edge. 
     */
    template <class T>
    class EdgePropertyConcept
    {
        BOOST_CONCEPT_ASSERT((SerializableConcept<T>));
    public:
        BOOST_CONCEPT_USAGE(EdgePropertyConcept)
        {
            T u = t.inverse();//check for inverse() method that returns a T
            u.inverse();//suppress "u not used" warning
            const std::string graphviz_text = t.toString(); //check for toString() method
        }
    private:
      const T t; //inverse() has to be const
    };
    
                                                       
    
     /* vertex_descriptor and edge_descriptor are don't depend on the
     * graph properties. They only depend on the type of the underlying
     * datastructure (they are different for random access and pointer based
     * structures).
     * The datastructure is already set in the directed_graph. Thus
     * vertex_descriptor and edge_descriptor will be the same type for all
     * template instanziations of GraphBase.
     *
     * null_vertex() also depends on the datastructure. however there is no 
     * easy way to access the implementation of null_vertex from the traits.
     * 
     * @warning This works as long as the directed_graph is an adjacency_list
     *          based on boost::listS.
     */
    struct GraphTraits : public boost::adjacency_list_traits<boost::listS, boost::listS,
                                                             boost::bidirectionalS,
                                                             boost::listS>
    {
        using Base = boost::adjacency_list_traits<boost::listS, boost::listS,
                                                  boost::bidirectionalS,
                                                  boost::listS>;
        using vertex_descriptor = Base::vertex_descriptor;
        using edge_descriptor = Base::edge_descriptor;
        using vertices_size_type = Base::vertices_size_type;
        using edges_size_type = Base::edges_size_type;
        
        static vertex_descriptor null_vertex()
        {
            //the null_vertex definiton is different depending on the data type
            //of the vertex_descriptor. For lists the descriptor is a pointer
            //and nullptr is the correct null_vertex. 
            //However, for vectors the correct null_vertex is 
            //std::numeric_limits<vertex_descriptor>::max.
            //if the underlying datatype changes, you have to modify this method
            //to reflect the change.
            //
            //FIXME A better implementation might check for Base::is_rand_access
            //      and return either a nullptr or limits::max.
            static_assert(std::is_same<Base::vertex_descriptor, Base::vertex_ptr>::value,
                          "vertex_descriptor should be a vertex_ptr.");
            return nullptr;
        }
                                                                
    };
                                                     
                                              
    template <class FRAME_PROP, class EDGE_PROP>
    using GraphBase = boost::labeled_graph<boost::directed_graph<FRAME_PROP, EDGE_PROP, envire::core::Environment>, FrameId>;
    
    
    /**A hash function for the edge_descriptor.
     * @see http://stackoverflow.com/questions/15902134/removing-edges-temporarily-from-a-boost-graph*/
    template <class GRAPH>
    struct EdgeHash : std::unary_function<typename GRAPH::edge_descriptor, std::size_t> 
    {
        EdgeHash(const std::shared_ptr<GRAPH> graph) : graph(graph){}

        std::size_t operator()(typename GRAPH::edge_descriptor const& e) const {
        std::size_t hash = 1337; //just some random number :)
        //Each edge is uniquely defined by it's source and target.
        //Thus we can use those vertex_descriptors to create the hash
        //value of the edge
        boost::hash_combine(hash, boost::source(e, **graph));
        boost::hash_combine(hash, boost::target(e, **graph));
        return hash;
      }
      const std::shared_ptr<GRAPH> graph;
    };
    
    // A hash function for vertices.
    struct VertexHash : std::unary_function<GraphTraits::vertex_descriptor, std::size_t> {
      std::size_t operator()(GraphTraits::vertex_descriptor const& u) const {
        std::size_t seed = 0;
        boost::hash_combine(seed, (long)u);
        return seed;
      }
    };

}}

namespace std {

  template <>
  struct hash<std::pair<envire::core::FrameId, envire::core::FrameId>>
  {
    std::size_t operator()(const std::pair<envire::core::FrameId, envire::core::FrameId>& k) const
    {
      using std::size_t;
      using std::hash;
      using std::string;

      // Compute individual hash values for first,
      // second and third and combine them using XOR
      // and bit shifting:
      std::size_t hashVal = 0;
      boost::hash_combine(hashVal, std::hash<std::string>()(k.first));
      boost::hash_combine(hashVal, std::hash<std::string>()(k.second));
      return hashVal;
    }
  };

}
