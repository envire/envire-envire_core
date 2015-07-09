#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/graph/graphviz.hpp>
#include <envire_core/Item.hpp>
#include <envire_core/Tree.hpp>

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

BOOST_AUTO_TEST_CASE(envire_tree_test)
{

    envire::core::Tree envire_tree;

    /** Create a list of items **/
    std::vector< boost::shared_ptr<envire::core::ItemBase> > items_vector(100);
    boost::shared_ptr<Integer> my_int(new Integer(10));

    /** Fill the list of nodes **/
    for (std::vector<boost::shared_ptr<envire::core::ItemBase> >::iterator it = items_vector.begin() ; it != items_vector.end(); ++it)
    {
        /** Pointer to the same object **/
        *it = my_int;
        //std::cout<<"pointer count: "<<(*it).use_count()<<"\n";
    }
    my_int.reset();
    std::cout<<"pointer count: "<<items_vector[0].use_count()<<"\n";

    /** Add root node **/
    envire::core::Tree::Graph::vertex_descriptor root = boost::add_vertex(envire_tree.tree);
    envire_tree.tree[root].setFrame("root");

    /** Create 100 nodes with its edges **/
    for (register int i=0; i<10; ++i)
    {
        envire::core::Tree::Graph::vertex_descriptor node = boost::add_vertex(envire_tree.tree);
        envire::core::Tree::Graph::edge_descriptor edge; bool b;
        boost::tie(edge, b) = boost::add_edge(root, node, envire_tree.tree);
        base::TransformWithCovariance tf;
        envire_tree.tree[node].setFrame("child_"+std::to_string(i));
        envire_tree.tree[edge].setTransform(tf);

        for (register int j=0; j<10; ++j)
        {
            envire::core::Tree::Graph::vertex_descriptor another_node = boost::add_vertex(envire_tree.tree);
            envire::core::Tree::Graph::edge_descriptor edge; bool b;
            boost::tie(edge, b) = boost::add_edge(node, another_node, envire_tree.tree);
            envire_tree.tree[another_node].setFrame("grand_child_"+std::to_string(j));
            envire_tree.tree[edge].setTransform(tf);


        }
    }

//    std::vector<double> distances(num_vertices(envire_tree.tree));
//    boost::dijkstra_shortest_paths(envire_tree.tree, root,
//      boost::weight_map(boost::get(&envire::core::Edge::idx, envire_tree.tree))
//      .distance_map(boost::make_iterator_property_map(distances.begin(),
//                                               boost::get(boost::vertex_index, envire_tree.tree))));

    //property accessors
//    boost::property_map<envire::core::Tree::Graph, int>::type edgeIdx = boost::get(&envire::core::Edge::idx, envire_tree);

   // printDependencies(std::cout, envire_tree, boost::get(boost::vertex_index, envire_tree));

    /** Write the dot file */
    std::ofstream dotfile ("envire_tree_test.dot");
    boost::write_graphviz (dotfile, envire_tree.tree,
            boost::make_label_writer(boost::get(&envire::core::Node::frame_name, envire_tree.tree)));
}

