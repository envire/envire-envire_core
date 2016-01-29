#include <boost/test/unit_test.hpp>
#include <envire_core/graph/Graph.hpp>
 #include <vector>
 #include <string>
 
using namespace envire::core;
using namespace envire::core::graph;
using namespace std;


class FrameProp : public envire::core::graph::FramePropertyBase { };
class EdgeProp {};

using Gra = envire::core::graph::Graph<FrameProp, EdgeProp>;

BOOST_AUTO_TEST_CASE(simple_add_remove_edge_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";   
    Gra g;
    EdgeProp ep;
    FrameProp fp;
    
    g.addFrame(a, fp);
    g.addFrame(b, fp);
    
    g.add_edge(a, b, ep);
    BOOST_CHECK(g.num_edges() == 1);
    BOOST_CHECK_THROW(g.add_edge(a, b, ep), EdgeAlreadyExistsException);
    BOOST_CHECK(g.num_edges() == 1);
    g.add_edge(b, a, ep);
    BOOST_CHECK(g.num_edges() == 2);
    
    g.remove_edge(a, b);
    BOOST_CHECK(g.num_edges() == 1);
    BOOST_CHECK_NO_THROW(g.getEdge(b, a));
    BOOST_CHECK_THROW(g.getEdge(a, b), UnknownEdgeException);
    
    g.remove_edge(b, a);
    BOOST_CHECK(g.num_edges() == 0);
    BOOST_CHECK_THROW(g.getEdge(a, b), UnknownEdgeException);
    BOOST_CHECK_THROW(g.getEdge(b, a), UnknownEdgeException);

    BOOST_CHECK(g.num_vertices() == 2); //vertices are still there, just not connected anymore
}