#include <boost/test/unit_test.hpp>

#include <iostream>
#include <boost/graph/directed_graph.hpp> /** Boost directed graph **/
#include <boost/graph/labeled_graph.hpp> /** Boost directed graph **/
#include <boost/uuid/uuid.hpp> /** uuid class */
#include <boost/uuid/uuid_generators.hpp>

BOOST_AUTO_TEST_CASE(dummy_graph_test)
{

    typedef boost::directed_graph<boost::no_property> Graph;

    // Create a graph object
    Graph g;

    // Add vertices
    Graph::vertex_descriptor v0 = g.add_vertex();
    Graph::vertex_descriptor v1 = g.add_vertex();
    Graph::vertex_descriptor v2 = g.add_vertex();
    std::cout<<"v0: "<<v0<<"\n";
    std::cout<<"v1: "<<v1<<"\n";
    std::cout<<"v2: "<<v2<<"\n";

    std::cout << "There are " << g.num_vertices() << " vertices." << std::endl;
    Graph::vertex_descriptor u0 = boost::vertex(0, g);
    std::cout<<"u0: "<<u0<<"\n";
    g.remove_vertex(v0);
    std::cout << "There are " << g.num_vertices() << " vertices." << std::endl;
    u0 = boost::vertex(0, g);
    std::cout<<"u0: "<<u0<<"\n";
    Graph::vertex_descriptor u1 = boost::vertex(1, g);
    std::cout<<"u1: "<<u1<<"\n";
    Graph::vertex_descriptor u2 = boost::vertex(2, g);
    std::cout<<"u2: "<<u2<<"\n";

    typedef boost::labeled_graph< Graph, boost::uuids::uuid > LabeledGraph;

    LabeledGraph lg;

    // Add vertices
    boost::uuids::uuid id = boost::uuids::random_generator()();
    LabeledGraph::vertex_descriptor lv0 = boost::add_vertex(id, lg);
    LabeledGraph::vertex_descriptor lv1 = boost::add_vertex(boost::uuids::random_generator()(), lg);
    LabeledGraph::vertex_descriptor lv2 = boost::add_vertex(boost::uuids::random_generator()(), lg);
    std::cout<<"\nlv0: "<<lv0<<"\n";
    std::cout<<"lv1: "<<lv1<<"\n";
    std::cout<<"lv2: "<<lv2<<"\n";

    std::cout << "There are " << boost::num_vertices(lg) << " vertices." << std::endl;
    LabeledGraph::vertex_descriptor lu0 = boost::vertex_by_label(id, lg);
    std::cout<<"lu0: "<<lu0<<"\n";

}
