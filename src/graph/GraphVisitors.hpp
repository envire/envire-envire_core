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

/*
 * GraphExceptions.hpp
 *
 *  Created on: Sep 22, 2015
 *      Author: jhidalgo
 */

#pragma once
#include <envire_core/graph/GraphExceptions.hpp>
#include <envire_core/graph/GraphTypes.hpp>
#include <envire_core/graph/TreeView.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/graph/breadth_first_search.hpp>

#include <deque>
#include <unordered_map>

namespace envire { namespace core
{

    class Empty{};

    /** Searches a graph in bfs order for a certain vertex.
     *  Interrupts the search thrown an exception of the vertex is found. */
    template < typename Vertex>
    class GraphBFSVisitor: public boost::default_bfs_visitor
    {
    public:
        template <typename Graph>
        GraphBFSVisitor(const Vertex &v,const Graph &g):vertex_to_search(v)
        {
            parent.reset(new std::unordered_map<Vertex,Vertex>());
            tree.reset(new std::deque<Vertex>());
        }

        template <typename Graph>
        void discover_vertex(Vertex v, const Graph &g)
        {
            if (vertex_to_search == v)
            {
                Vertex parent_vertex = v;

                /** Get its direct parent **/
                while (parent_vertex != Graph::null_vertex())
                {
                    tree->push_front(parent_vertex);
                    try
                    {
                        parent_vertex = parent->at(parent_vertex);
                    }catch (const std::out_of_range& oor)
                    {
                        parent_vertex = Graph::null_vertex();
                    }
                }

                throw FoundFrameException(g[v].getId());
            }
        }


        template < typename Edge, typename Graph>
        void tree_edge(Edge e, const Graph &g)
        {
            Vertex source_vertex = boost::source(e, g);
            Vertex target_vertex = boost::target(e, g);
            parent->insert(std::make_pair<Vertex,Vertex>(static_cast<Vertex>(target_vertex), static_cast<Vertex>(source_vertex)));
        }

    public:
        Vertex vertex_to_search;
        boost::shared_ptr< std::unordered_map<Vertex, Vertex> > parent;
        boost::shared_ptr< std::deque<Vertex> > tree; /** tree from origin tree[0] to target tree[n] */
    };


    /**Visits every node in bfs order and stores the search tree in the provided map 
     * @param GRAPH should be a boost graph of some kind*/
    template <class GRAPH>
    struct TreeBuilderVisitor : public boost::default_bfs_visitor
    {
        /** @param view The TreeView that should be filled
         *  @param graph The graph that is visited. The reference to the graph
         *               is needed to get the source and target of an edge.*/
        TreeBuilderVisitor(TreeView& view, const GRAPH& graph) :
            view(view), graph(graph) {}

        /**This is invoked on each edge as it becomes a member of 
         * the edges that form the search tree. */
        template <typename Edge, typename Graph>
        void tree_edge(Edge e, const Graph &g)
        {
            //note: graph is used in here instead of g because otherwise
            //      the visitor wouldn't work with boost::filtered_graph.
            //      Because the signature for source() and target() is different
            //      on filtered_graphs. Don't know why exactly that happens but using
            //      the original graph works fine :-)
            const typename GRAPH::vertex_descriptor source = boost::source(e, graph);
            const typename GRAPH::vertex_descriptor target = boost::target(e, graph);

            view.addEdge(source, target);
        }
        
        /**This is only invoked on cross edges, not on back edges
         * @see http://www.boost.org/doc/libs/1_59_0/libs/graph/doc/BFSVisitor.html
         * @see http://www.boost.org/doc/libs/1_59_0/libs/graph/doc/breadth_first_search.html
         */
        template <typename Edge, typename Graph>
        void gray_target(Edge e, Graph& g)
        {
            view.addCrossEdge(boost::source(e, graph), boost::target(e, graph), e);
        }
        
        TreeView& view;
        const GRAPH& graph;
    };

    
    /**A predicate that can be used to remove two edges from a graph.
     * Should be used with boost::filtered_grapg*/
    class EdgeFilter
    {
    public:
      EdgeFilter() {}//needed by boost, dunno why
      
      bool operator()(const GraphTraits::edge_descriptor& e) const 
      {
          return e != edge1 && e != edge2;
      }
      //the edges that should be removed
      GraphTraits::edge_descriptor edge1;
      GraphTraits::edge_descriptor edge2;
  };


    /** TESTING BOOST CODE **/
    template < class Visitor=boost::null_visitor>
    class TransformGraphBFSRecorderVisitor: public boost::bfs_visitor<Visitor>
    {
    public:
        TransformGraphBFSRecorderVisitor():boost::bfs_visitor<>(){ }
        TransformGraphBFSRecorderVisitor(Visitor record_vis):boost::bfs_visitor<Visitor>(record_vis){ }

    };
    
    

}}

