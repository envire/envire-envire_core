//=======================================================================
// Copyright 2001 Jeremy G. Siek, Andrew Lumsdaine, Lie-Quan Lee
// Copyright 2009 Trustees of Indiana University.
// Copyright 2001 Jeremy G. Siek, Andrew Lumsdaine, Lie-Quan Lee, 
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// NOTE: Based off of dijkstra-example.cpp
//=======================================================================
#include <boost/config.hpp>
#include <iostream>
#include <fstream>
#include <cstring>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths_no_color_map.hpp>
#include <boost/property_map/property_map.hpp>

using namespace boost;


template < typename Vertex >
struct threshold_visitor: boost::default_dijkstra_visitor {
   double threshold;
   Vertex vertex_to_search;
   char name[6];
   threshold_visitor(double threshold, Vertex v): threshold(threshold), vertex_to_search(v) {std::strcpy(name, "ABCDE");}

   struct found_vertex {};
   template < typename Graph >
   void examine_vertex(Vertex v, const Graph& G)
   {
       std::cout<<"EXAMINE["<<name[v]<<"]\n";
       if (vertex_to_search == v)
       {
           std::cout<<"FOUND!\n";
           throw found_vertex();
       }
   }
};

int
main(int, char *[])
{
  typedef adjacency_list < listS, vecS, directedS,
    no_property, property < edge_weight_t, int > > graph_t;
  typedef graph_traits < graph_t >::vertex_descriptor vertex_descriptor;
  typedef std::pair<int, int> Edge;

  const int num_nodes = 5;
  enum nodes { A, B, C, D, E };
  char name[] = "ABCDE";
  Edge edge_array[] = { Edge(A, C), Edge(B, B), Edge(B, D), Edge(B, E),
    Edge(C, B), Edge(C, D), Edge(D, E), Edge(E, A), Edge(E, B)
  };
  int weights[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  int num_arcs = sizeof(edge_array) / sizeof(Edge);
  graph_t g(edge_array, edge_array + num_arcs, weights, num_nodes);
  property_map<graph_t, edge_weight_t>::type weightmap = get(edge_weight, g);
  std::vector<vertex_descriptor> p(num_vertices(g));
  std::vector<int> d(num_vertices(g));
  vertex_descriptor s = vertex(A, g);

  try
  {
    dijkstra_shortest_paths_no_color_map(g, s,
                                       predecessor_map(boost::make_iterator_property_map(p.begin(), get(boost::vertex_index, g))).
                                       distance_map(boost::make_iterator_property_map(d.begin(), get(boost::vertex_index, g))).
                                       visitor(threshold_visitor<vertex_descriptor>(1, vertex(B, g))));

  }catch(...)
  {
      std::cout<<"YIPUUU!!\n";
  }

  std::cout << "distances and parents:" << std::endl;
  graph_traits < graph_t >::vertex_iterator vi, vend;
  for (boost::tie(vi, vend) = vertices(g); vi != vend; ++vi) {
    std::cout << "distance(" << name[*vi] << ") = " << d[*vi] << ", ";
    std::cout << "parent(" << name[*vi] << ") = " << name[p[*vi]] << std::
      endl;
  }
  std::cout << "Vector:" << std::endl;
  for (std::vector<vertex_descriptor>::iterator it=p.begin(); it != p.end(); it++)
  {
      std::cout<<"name: "<<name[*it]<<"\n";
  }

  std::cout << "Path:" << std::endl;
  vertex_descriptor v_target = vertex(B, g);
  vertex_descriptor v_source = vertex(A, g);
  vertex_descriptor v_current = v_target;
  std::cout<<name[v_current];
  while (v_source != v_current)
  {
      v_current = p[v_current];

      std::cout<<"<-"<<name[v_current];
  }
  std::cout<<"\n";



  std::cout << std::endl;

  std::ofstream dot_file("./dijkstra-no-color-map-eg.dot");

  dot_file << "digraph D {\n"
    << "  rankdir=LR\n"
    << "  size=\"4,3\"\n"
    << "  ratio=\"fill\"\n"
    << "  edge[style=\"bold\"]\n" << "  node[shape=\"circle\"]\n";

  graph_traits < graph_t >::edge_iterator ei, ei_end;
  for (boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei) {
    graph_traits < graph_t >::edge_descriptor e = *ei;
    graph_traits < graph_t >::vertex_descriptor
      u = source(e, g), v = target(e, g);
    dot_file << name[u] << " -> " << name[v]
      << "[label=\"" << get(weightmap, e) << "\"";
    if (p[v] == u)
      dot_file << ", color=\"black\"";
    else
      dot_file << ", color=\"grey\"";
    dot_file << "]";
  }
  dot_file << "}";

  dot_file.close();
  return EXIT_SUCCESS;
}
