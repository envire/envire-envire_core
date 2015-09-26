/*
 * TransformGraphExceptions.hpp
 *
 *  Created on: Sep 22, 2015
 *      Author: jhidalgo
 */

#pragma once
#include <boost/graph/breadth_first_search.hpp>

namespace envire { namespace core
{

    template < typename Vertex >
    class TransformGraphBFSVisitor: public boost::default_bfs_visitor
    {
    public:
        template <typename Graph>
        TransformGraphBFSVisitor(Vertex &v, Graph &g):vertex_to_origin(v)
        {parent.resize(g.num_vertices());}

        template <typename Graph>
        void examine_vertex(Vertex v, const Graph &g)
        {
            std::cout<<"EXAMINE["<<g[v].frame.name<<"]\n";
        }

        template < typename Edge, typename Graph>
        void tree_edge(Edge e, const Graph &g)
        {
            Vertex source_vertex = boost::source(e, g);
            Vertex target_vertex = boost::target(e, g);
            //parent[target_vertex] = source_vertex;

            std::cout<<"EDGE"<<g[source_vertex].frame.name<<"->"<<g[target_vertex].frame.name<<"\n";
        }



    public:
        Vertex vertex_to_origin;
        std::vector<Vertex> parent;
    };
}}

