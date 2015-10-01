/*
 * TransformGraphExceptions.hpp
 *
 *  Created on: Sep 22, 2015
 *      Author: jhidalgo
 */

#pragma once
#include "TransformGraphExceptions.hpp"
#include "TransformGraphTypes.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/graph/breadth_first_search.hpp>

#include <deque>
#include <unordered_map>

namespace envire { namespace core
{

    class Empty{};

    template < typename Vertex>
    class TransformGraphBFSVisitor: public boost::default_bfs_visitor
    {
    public:
        template <typename Graph>
        TransformGraphBFSVisitor(const Vertex &v,const Graph &g):vertex_to_search(v)
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

                throw FoundFrameException(g[v].frame.name);
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
    
        
    /**Visits every node in bfs order and stores the search tree in the provided map */
    struct TreeBuilderVisitor : public boost::default_bfs_visitor 
    {
        TreeBuilderVisitor(VertexMap& parentToChildren) : parentToChildren(parentToChildren) {}
        
        template <typename Edge, typename Graph>
        void tree_edge(Edge e, const Graph &g)
        {
            parentToChildren[boost::source(e, g)].insert(boost::target(e, g));
        }
        VertexMap& parentToChildren;
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

