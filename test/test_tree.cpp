#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/test/unit_test.hpp>
#include <envire_core/Item.hpp>
#include <envire_core/TransformTree.hpp>
#include <envire_core/GraphViz.hpp>


using namespace envire::core;

class Integer: public envire::core::ItemBase
{
    public:
        int number;

    public:
        Integer(const int n):number(n){};
};

template < typename _Graph, typename _VertexNameMap > void
printDependencies(std::ostream & out, const _Graph & g,
                   _VertexNameMap name_map)
{
  typename boost::graph_traits < _Graph >::edge_iterator ei, ei_end;
  for (boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
    out << boost::get(name_map, boost::source(*ei, g)) << " -$>$ "
      << boost::get(name_map, boost::target(*ei, g)) << std::endl;
}

/** Property writer for Nodes **/
template <class _ID, class _FrameName, class _Items>
class NodeWriter
{
    public:
        NodeWriter(_ID id, _FrameName name, _Items it) : id(id), name(name), it(it){}
        template <class _Node>
        void operator()(std::ostream &out, const _Node& n) const
        {
            out << "[shape=record, label=\"<f0> " << name[n] <<
                "|<f1>" << it[n].size()<<"\"]";
        }

    private:
        _ID id;
        _FrameName name;
        _Items it;
};

template <class _ID, class _FrameName, class _Items>
inline NodeWriter<_ID, _FrameName, _Items>
make_node_writer(_ID id, _FrameName name, _Items it)
{
    return NodeWriter<_ID, _FrameName, _Items>(id, name, it);
}

/** Property writer for Edges **/
template <class _Time, class _Transform>
class EdgeWriter {
    public:
        EdgeWriter(_Time time, _Transform tf) : time(time), tf(tf){}
        template <class _Edge>
        void operator()(std::ostream &out, const _Edge& e) const
        {
            out << "[label=\"" << time[e].toString(base::Time::Resolution::Seconds) <<
                boost::format("\\nt: (%.1f %.1f %.1f)\\nr: (%.1f %.1f %.1f)") % tf[e].translation.x() % tf[e].translation.y() % tf[e].translation.z()
                % tf[e].orientation.axis()[0] % tf[e].orientation.axis()[1] % tf[e].orientation.axis()[2]
		        << "\""
                << ",shape=ellipse,style=filled,fillcolor=lightblue]";
        }
    private:
        _Time time;
        _Transform tf;
};
template <class _Time, class _Transform>
inline EdgeWriter<_Time, _Transform>
make_edge_writer(_Time time, _Transform tf)
{
    return EdgeWriter<_Time, _Transform>(time, tf);
}

BOOST_AUTO_TEST_CASE(envire_tree_test)
{

    envire::core::TransformTree envire_tree;

    /** Create a list of items **/
    std::vector< boost::shared_ptr<envire::core::ItemBase> > items_vector(100);
    boost::shared_ptr<Integer> my_int(new Integer(10));

    /** Fill the list of items **/
    for (std::vector<boost::shared_ptr<envire::core::ItemBase> >::iterator it = items_vector.begin() ; it != items_vector.end(); ++it)
    {
        /** Pointer to the same object **/
        *it = my_int;
        //std::cout<<"pointer count: "<<(*it).use_count()<<"\n";
    }
    my_int.reset();
    std::cout<<"pointer count: "<<items_vector[0].use_count()<<"\n";

    /** Add root node **/
    envire::core::Frame root_prop("root");
    envire::core::TransformTree::vertex_descriptor root = envire_tree.addVertex(root_prop);
    //std::cout<<boost::vertex(root, envire_tree.tree);
    //boost::get(&envire::core::Node::frame_name,envire_tree.tree);

    /** Create 10 nodes with its edges **/
    for (register int i=0; i<10; ++i)
    {
        envire::core::Frame node_prop("child_"+std::to_string(i));
        node_prop.items = items_vector;
        envire::core::TransformTree::vertex_descriptor node = envire_tree.addVertex(node_prop);
        envire::core::Transform tf_prop;
        base::TransformWithCovariance tf;
        tf_prop.setTransform(tf);
        envire::core::TransformTree::edge_descriptor edge; bool b;
        boost::tie(edge, b) = boost::add_edge(node, root, tf_prop, envire_tree);
        //std::cout<<edge<<"b("<<boost::edge(node, root, envire_tree.tree).second<<")\n";

        /** Create 10 nodes with its edges **/
        for (register int j=0; j<10; ++j)
        {
            envire::core::Frame node_prop("grand_child_"+std::to_string(i)+std::to_string(j));
            node_prop.items = items_vector;
            envire::core::TransformTree::vertex_descriptor another_node = envire_tree.addVertex(node_prop);
            envire::core::Transform tf_prop;
            base::TransformWithCovariance tf;
            tf_prop.setTransform(tf);
            envire::core::TransformTree::edge_descriptor edge; bool b;
            boost::tie(edge, b) = boost::add_edge(another_node, node, tf_prop, envire_tree);
        }

    }

    envire::core::TransformTree::vertex_descriptor node = boost::vertex_by_label("child_0", envire_tree);

    if (node != envire::core::TransformTree::null_vertex())
    {
        std::cout << "There are " << boost::num_vertices(envire_tree) << " vertices." << std::endl;
        std::cout << "There are " << boost::num_edges(envire_tree) << " edges." << std::endl;

        boost::clear_vertex(node, envire_tree);

        std::cout << "There are " << boost::num_vertices(envire_tree) << " vertices." << std::endl;
        std::cout << "There are " << boost::num_edges(envire_tree) << " edges." << std::endl;
    }


//    std::vector<double> distances(num_vertices(envire_tree.tree));
//    boost::dijkstra_shortest_paths(envire_tree.tree, root,
//      boost::weight_map(boost::get(&envire::core::Edge::idx, envire_tree.tree))
//      .distance_map(boost::make_iterator_property_map(distances.begin(),
//                                               boost::get(boost::vertex_index, envire_tree.tree))));

    //property accessors
//    boost::property_map<envire::core::Tree::Graph, int>::type edgeIdx = boost::get(&envire::core::Edge::idx, envire_tree);

   // printDependencies(std::cout, envire_tree, boost::get(boost::vertex_index, envire_tree));


    /** Print graph by edges **/
    envire::core::TransformTree::edge_iterator it, end;
    for(boost::tie(it, end) = boost::edges(envire_tree); it != end; ++it)
    {
        std::cout << boost::get(FramePropertyTag(), envire_tree)[boost::source(*it, envire_tree)].name << " -> "
            << boost::get(FramePropertyTag(), envire_tree)[boost::target(*it, envire_tree)].name << '\n';
    }

    /** Print graph by vertex **/
    envire::core::TransformTree::vertex_iterator vertexIt, vertexEnd;

    std::cout << "vertices(g) = ";
    boost::tie(vertexIt, vertexEnd) = boost::vertices(envire_tree);
    for (; vertexIt != vertexEnd; ++vertexIt)
    {
        std::cout << boost::get(FramePropertyTag(), envire_tree)[*vertexIt].name <<" ";
    }
    std::cout << std::endl;


    /** Add edges by label **/
    for (register int j=0; j<10; ++j)
    {
        envire::core::Transform tf_prop;
        envire_tree.addEdge("grand_child_0"+std::to_string(j), "root", tf_prop);
    }

    envire::core::Transform tf_prop;
    envire_tree.addEdge("child_0", "root", tf_prop);

    std::pair<envire::core::TransformTree::edge_descriptor, bool> edge_pair = boost::edge_by_label("child_0", "root", envire_tree);
    std::cout<<"get edge return: "<<edge_pair.second<<"\n";

    /** update the edge child_0 -> root **/
    tf_prop.time = base::Time::now();
    base::TransformWithCovariance tf;
    tf_prop.setTransform(tf);

    envire_tree.addEdge("child_0", "root", tf_prop);

    std::cout << "There are " << boost::num_vertices(envire_tree) << " vertices." << std::endl;
    std::cout << "There are " << boost::num_edges(envire_tree) << " edges." << std::endl;

    envire_tree.removeVertex("grand_child_00");

    std::cout << "There are " << boost::num_vertices(envire_tree) << " vertices." << std::endl;
    std::cout << "There are " << boost::num_edges(envire_tree) << " edges." << std::endl;

    std::cout<<"****************\n";

    std::cout << "There are " << boost::num_vertices(envire_tree) << " vertices." << std::endl;
    std::cout << "There are " << boost::num_edges(envire_tree) << " edges." << std::endl;

    envire_tree.removeEdge("grand_child_01", "root", true);

    std::cout << "There are " << boost::num_vertices(envire_tree) << " vertices." << std::endl;
    std::cout << "There are " << boost::num_edges(envire_tree) << " edges." << std::endl;


    /** Graph Viz export **/
    GraphViz gviz;

    gviz.write(envire_tree.graph(), "envire_tree_test.dot");

    envire_tree.clear();
}

//vertex property:-->
class NodeInfo 
{
    public:
        std::string name; /** Frame name */
        boost::uuids::uuid uuid; /** Unique Identifier */
        std::vector< boost::shared_ptr<ItemBase> > items; /** List of items in the node */
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
    vd_t  v = boost::add_vertex (ninfo1, g);   //add a vertex in G with property ninfo1
    vd_t  u = boost::add_vertex (ninfo2, g);   //add a vertex in G with property ninfo2

    //Edge Map and Node Map
    typedef typename boost::property_map <Graph_t, EdgeInfoPropertyTag>::type  EdgeMap;
    typedef typename boost::property_map <Graph_t, NodeInfoPropertyTag>::type  NodeMap;

    //edge e --> get
    NodeInfo  ninfo_v = boost::get (NodeInfoPropertyTag(), g,  v);
    std::cout<<"ninfo_v.name: "<<ninfo_v.name<<"\n";
    ninfo_v.name = "4321";
    boost::put(NodeInfoPropertyTag(), g, u, ninfo_v);
    NodeInfo  ninfo_u = boost::get (NodeInfoPropertyTag(), g,  u);
    std::cout<<"ninfo_u.name: "<<ninfo_u.name<<"\n";
    ninfo_u.name = "hola";
}
