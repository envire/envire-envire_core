#include <iostream>
#include <vector>

#include <boost/test/unit_test.hpp>
#include <boost/graph/adjacency_list.hpp> // for customizable graphs
#include <boost/uuid/uuid.hpp> /** uuid class */
#include <boost/uuid/uuid_generators.hpp>

#include <base/Time.hpp>
#include <base/TransformWithCovariance.hpp>

//vertex property:-->
class NodeInfo 
{
    public:
        std::string name; /** Frame name */
        boost::uuids::uuid uuid; /** Unique Identifier */
};   //actual bundled property

class EdgeInfo
{
    public:
        base::Time time; /** Timestamp */
        base::TransformWithCovariance transform; /** the transformation */
};   //actual bundled property


struct NodeInfoPropertyTag {               // tag and kind  (as in boost documentation)
  typedef boost::vertex_property_tag kind;
  static  std::size_t const num;
};

std::size_t const NodeInfoPropertyTag::num = (std::size_t) &NodeInfoPropertyTag::num;

struct EdgeInfoPropertyTag {               // tag and kind  (as in boost documentation)
  typedef boost::edge_property_tag kind;
  static  std::size_t const num;
};

std::size_t const EdgeInfoPropertyTag::num = (std::size_t) &EdgeInfoPropertyTag::num;


BOOST_AUTO_TEST_CASE(property_test)
{

    //typedef the Vertex Property
    typedef boost::property <NodeInfoPropertyTag, NodeInfo>  NodeProperty;

    //Similar fashion for Edge Property --> some property for each edge of graph.
    typedef boost::property <EdgeInfoPropertyTag, EdgeInfo>  EdgeProperty;

    typedef boost::adjacency_list <boost::vecS, boost::vecS, boost::undirectedS, NodeProperty, EdgeProperty, boost::no_property, boost::listS> Graph_t;

    Graph_t  g;
    typedef boost::graph_traits<Graph_t>::vertex_descriptor   vd_t;
    typedef boost::graph_traits<Graph_t>::edge_descriptor ed_t;

    NodeInfo   ninfo1, ninfo2;   //put some values in ninfo
    ninfo1.name = "node_v";
    ninfo2.name = "node_u";
    vd_t  v = boost::add_vertex (ninfo1, g);   //add a vertex in G with property ninfo1
    vd_t  u = boost::add_vertex (ninfo2, g);   //add a vertex in G with property ninfo2

    //Edge Map and Node Map
    typedef typename boost::property_map <Graph_t, EdgeInfoPropertyTag>::type  EdgeMap;
    typedef typename boost::property_map <Graph_t, NodeInfoPropertyTag>::type  NodeMap;

    //edge e --> get
    NodeInfo  ninfo_v = boost::get(NodeInfoPropertyTag(), g,  v);
    NodeInfo  ninfo_u = boost::get(NodeInfoPropertyTag(), g,  u);
    std::cout<<"ninfo_v.name: "<<ninfo_v.name<<"\n";
    BOOST_CHECK(ninfo_v.name == ninfo1.name);
    BOOST_CHECK(ninfo_u.name == ninfo2.name);
    ninfo_v.name = "4321";
    boost::put(NodeInfoPropertyTag(), g, u, ninfo_v);
    ninfo_u = boost::get (NodeInfoPropertyTag(), g,  u);
    std::cout<<"ninfo_u.name: "<<ninfo_u.name<<"\n";
    BOOST_CHECK(ninfo_u.name == ninfo_v.name);
}

struct NodeOtherProperty
{
    NodeInfo frame;
};

struct EdgeOtherProperty
{
    EdgeInfo transform;
};


BOOST_AUTO_TEST_CASE(other_property_test)
{

    typedef boost::adjacency_list <boost::vecS, boost::vecS, boost::undirectedS, NodeOtherProperty, EdgeOtherProperty, boost::no_property, boost::listS> Graph_t;

    Graph_t  g;
    typedef boost::graph_traits<Graph_t>::vertex_descriptor   vd_t;
    typedef boost::graph_traits<Graph_t>::edge_descriptor ed_t;

    NodeOtherProperty   ninfo1, ninfo2;   //put some values in ninfo
    ninfo1.frame.name = "node_v";
    ninfo2.frame.name = "node_u";
    vd_t  v = boost::add_vertex (ninfo1, g);   //add a vertex in G with property ninfo1
    vd_t  u = boost::add_vertex (ninfo2, g);   //add a vertex in G with property ninfo2

    //edge e --> get
    NodeInfo *ninfo_v = &boost::get(&NodeOtherProperty::frame, g,  v);
    NodeInfo *ninfo_u = &boost::get(&NodeOtherProperty::frame, g,  u);
    std::cout<<"ninfo_v.name: "<<ninfo_v->name<<"\n";
    BOOST_CHECK(ninfo_v->name == ninfo1.frame.name);
    BOOST_CHECK(ninfo_u->name == ninfo2.frame.name);
}

