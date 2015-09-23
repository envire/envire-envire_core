#include <boost/uuid/uuid_generators.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/lexical_cast.hpp> /** to string conversion when using < C++11 */

#include <envire_core/LabeledTransformTree.hpp>
#include <envire_core/Item.hpp>
#include <envire_core/GraphViz.hpp>

using namespace envire::core;

BOOST_AUTO_TEST_CASE(add_and_remove_vertex_labeled_tree_test)
{

    unsigned int max_vertices = 100;

    BOOST_TEST_MESSAGE("ADD VERTEX TEST...");
    envire::core::LabeledTransformTree labeled_tree;

    /** Add root node **/
    envire::core::Frame root_prop("root");
    envire::core::LabeledTransformTree::vertex_descriptor root = labeled_tree.add_vertex(root_prop);

    register unsigned int i = 0;
    for (i = 0; i<max_vertices-1; ++i)
    {
        envire::core::Frame frame("frame_"+boost::lexical_cast<std::string>(i));
        envire::core::LabeledTransformTree::vertex_descriptor v1 = labeled_tree.add_vertex(frame);
    }

    BOOST_CHECK(labeled_tree.num_vertices() == max_vertices);
    BOOST_TEST_MESSAGE("DONE\n");

    BOOST_TEST_MESSAGE("REMOVE VERTEX TEST...");
    envire::core::LabeledTransformTree::vertex_iterator vi, vi_end, next;
    boost::tie(vi, vi_end) = labeled_tree.vertices();
    for (next = vi; vi != vi_end; vi = next)
    {
        ++next;
        labeled_tree.remove_vertex(labeled_tree.getFrame(*vi).name);
    }

    BOOST_CHECK(labeled_tree.num_vertices() == 0);
    BOOST_TEST_MESSAGE("DONE\n");
}

 class Vector: public envire::core::Item<Eigen::Vector3d>
{
};

BOOST_AUTO_TEST_CASE(add_and_remove_edge_labeled_tree_test)
{

    unsigned int max_vertices = 10;

    envire::core::LabeledTransformTree labeled_tree;
    unsigned int vector_size = 100;
    unsigned int magic_number = 42;

    /** Create a list of items **/
    std::vector< boost::intrusive_ptr<envire::core::ItemBase> > items_vector(vector_size);
    boost::intrusive_ptr<Vector> my_vector(new Vector());
    my_vector->setData(magic_number * Eigen::Vector3d::Ones());

    for (std::vector<boost::intrusive_ptr<envire::core::ItemBase> >::iterator it = items_vector.begin() ; it != items_vector.end(); ++it)
    {
        /** Pointer to the same object **/
        *it = my_vector;
    }

    BOOST_TEST_MESSAGE("ADD EDGE TEST...");

    /** Add root node **/
    envire::core::Frame root_prop("root");
    envire::core::LabeledTransformTree::vertex_descriptor root = labeled_tree.add_vertex(root_prop);

    /** Create max_vertices nodes with its edges **/
    for (register unsigned int i=0; i<max_vertices; ++i)
    {
        envire::core::Frame node_prop("child_"+boost::lexical_cast<std::string>(i));
        node_prop.items = items_vector;
        envire::core::LabeledTransformTree::vertex_descriptor node = labeled_tree.add_vertex(node_prop);
        envire::core::Transform tf_prop;
        base::TransformWithCovariance tf;
        tf_prop.setTransform(tf);
        envire::core::LabeledTransformTree::edge_descriptor edge; bool b;
        boost::tie(edge, b) = labeled_tree.add_edge(node, root, tf_prop);
        //std::cout<<edge<<"b("<<boost::edge(node, root, labeled_tree.labeled_tree).second<<")\n";

        /** Create max_vertices nodes with its edges **/
        for (register unsigned int j=0; j<max_vertices; ++j)
        {
            envire::core::Frame node_prop("grand_child_"+boost::lexical_cast<std::string>(i)+boost::lexical_cast<std::string>(j));
            node_prop.items = items_vector;
            envire::core::LabeledTransformTree::vertex_descriptor another_node = labeled_tree.add_vertex(node_prop);
            envire::core::Transform tf_prop;
            base::TransformWithCovariance tf;
            tf_prop.setTransform(tf);
            envire::core::LabeledTransformTree::edge_descriptor edge; bool b;
            boost::tie(edge, b) = labeled_tree.add_edge(another_node, node, tf_prop);
        }

    }
    //std::cout<<"labeled tree.num_vertices(): "<<labeled_tree.num_vertices()<<"\n";
    //std::cout<<"labeled tree.num_edges(): "<<labeled_tree.num_edges()<<"\n";
    BOOST_CHECK(labeled_tree.num_vertices() == (max_vertices*max_vertices) + max_vertices+1);
    BOOST_CHECK(labeled_tree.num_edges() == (max_vertices*max_vertices) + max_vertices);

    BOOST_TEST_MESSAGE("DONE\n");
    BOOST_TEST_MESSAGE("GRAPHVIZ TEST...");

    BOOST_TEST_MESSAGE("REMOVE EDGES TEST...");
    envire::core::LabeledTransformTree::edge_iterator ei, ei_end, next;
    boost::tie(ei, ei_end) = labeled_tree.edges();
    bool destructive = true;
    for (next = ei; ei != ei_end; ei = next)
    {
        ++next;
        labeled_tree.remove_edge(*ei, destructive);
    }
    BOOST_CHECK(labeled_tree.num_edges() == 0);
    if (destructive)
        BOOST_CHECK(labeled_tree.num_vertices() == 0);
    else
        BOOST_CHECK(labeled_tree.num_vertices() == (max_vertices*max_vertices) + max_vertices+1);

    BOOST_TEST_MESSAGE("DONE\n");
}

BOOST_AUTO_TEST_CASE(property_and_grahviz_labeled_tree_test)
{
    unsigned int max_vertices = 10;

    envire::core::LabeledTransformTree labeled_tree;
    unsigned int vector_size = 100;
    unsigned int magic_number = 42;

    /** Create a list of items **/
    std::vector< boost::intrusive_ptr<envire::core::ItemBase> > items_vector(vector_size);
    boost::intrusive_ptr<Vector> my_vector(new Vector());
    my_vector->setData(magic_number * Eigen::Vector3d::Ones());

    for (std::vector<boost::intrusive_ptr<envire::core::ItemBase> >::iterator it = items_vector.begin() ; it != items_vector.end(); ++it)
    {
        /** Pointer to the same object **/
        *it = my_vector;
    }

    /** Add root node **/
    envire::core::Frame root_prop("root");
    envire::core::LabeledTransformTree::vertex_descriptor root = labeled_tree.add_vertex(root_prop);

    base::Time now = base::Time::now();
    /** Create max_vertices nodes with its edges **/
    for (register unsigned int i=0; i<max_vertices; ++i)
    {
        envire::core::Frame node_prop("child_"+boost::lexical_cast<std::string>(i));
        node_prop.items = items_vector;
        envire::core::LabeledTransformTree::vertex_descriptor node = labeled_tree.add_vertex(node_prop);
        envire::core::Transform tf_prop(now);
        base::TransformWithCovariance tf(static_cast<base::Position>(my_vector->getData()), Eigen::Quaterniond::Identity());
        tf_prop.setTransform(tf);
        envire::core::LabeledTransformTree::edge_descriptor edge; bool b;
        boost::tie(edge, b) = labeled_tree.add_edge(node, root, tf_prop);
        //std::cout<<edge<<"b("<<boost::edge(node, root, labeled_tree.labeled_tree).second<<")\n";

        /** Create max_vertices nodes with its edges **/
        for (register unsigned int j=0; j<max_vertices; ++j)
        {
            envire::core::Frame node_prop("grand_child_"+boost::lexical_cast<std::string>(i)+boost::lexical_cast<std::string>(j));
            node_prop.items = items_vector;
            envire::core::LabeledTransformTree::vertex_descriptor another_node = labeled_tree.add_vertex(node_prop);
            envire::core::LabeledTransformTree::edge_descriptor edge; bool b;
            boost::tie(edge, b) = labeled_tree.add_edge(another_node, node, tf_prop);
        }
    }

    BOOST_TEST_MESSAGE("FRAME PROPERTY TEST...");
    for (register unsigned int i = 0; i<max_vertices; ++i)
    {
        envire::core::LabeledTransformTree::vertex_descriptor vd = labeled_tree.vertex("child_"+boost::lexical_cast<std::string>(i));
        envire::core::Frame frame = labeled_tree.getFrame(vd);
        BOOST_CHECK(frame.name == "child_"+boost::lexical_cast<std::string>(i));
        BOOST_CHECK(frame.items.size() == vector_size);
        for (std::vector< boost::intrusive_ptr<envire::core::ItemBase> >::const_iterator it = frame.items.begin();
                it != frame.items.end(); ++it)
        {
            BOOST_CHECK(*it == my_vector);
            Vector* pvector = dynamic_cast< Vector* >(it->get());
            BOOST_CHECK(pvector->getData() == magic_number * Eigen::Vector3d::Ones());
        }
    }
    BOOST_TEST_MESSAGE("DONE\n");

    BOOST_TEST_MESSAGE("TRANSFORM PROPERTY TEST...");
    envire::core::LabeledTransformTree::edge_iterator it, end;
    for(boost::tie(it, end) = labeled_tree.edges(); it != end; ++it)
    {
        envire::core::Transform transform = boost::get(&TransformProperty::transform, labeled_tree, *it);
        BOOST_CHECK(transform.time == now);
        BOOST_CHECK(transform.transform.translation == my_vector->getData());
    }
    BOOST_TEST_MESSAGE("DONE\n");

    BOOST_TEST_MESSAGE("GRAPHVIZ TEST...");
    envire::core::GraphViz gviz;

    gviz.write(labeled_tree.graph(), "graphviz_boost_test_labeled_transform_tree.dot");

    labeled_tree.clear();
    BOOST_TEST_MESSAGE("DONE\n");
}


BOOST_AUTO_TEST_CASE(add_and_updating_edges_labeled_tree_test)
{
    unsigned int max_vertices = 10;

    envire::core::LabeledTransformTree labeled_tree;

    unsigned int vector_size = 100;
    unsigned int magic_number = 42;

    /** Create a list of items **/
    std::vector< boost::intrusive_ptr<envire::core::ItemBase> > items_vector(vector_size);
    boost::intrusive_ptr<Vector> my_vector(new Vector());
    my_vector->setData(magic_number * Eigen::Vector3d::Ones());

    for (std::vector<boost::intrusive_ptr<envire::core::ItemBase> >::iterator it = items_vector.begin() ; it != items_vector.end(); ++it)
    {
        /** Pointer to the same object **/
        *it = my_vector;
    }

    /** Add root node **/
    envire::core::Frame root_prop("root");
    root_prop.items = items_vector;
    envire::core::LabeledTransformTree::vertex_descriptor root = labeled_tree.add_vertex(root_prop);

    /** Create max_vertices nodes with its edges **/
    for (register unsigned int i=0; i<max_vertices; ++i)
    {
        envire::core::Frame node_prop("child_"+boost::lexical_cast<std::string>(i));
        node_prop.items = items_vector;
        envire::core::LabeledTransformTree::vertex_descriptor node = labeled_tree.add_vertex(node_prop);
    }

    /** Add edges by label **/
    base::Time now = base::Time::now();
    for (register unsigned int j=0; j<max_vertices; ++j)
    {
        envire::core::Transform tf_prop(now);
        base::TransformWithCovariance tf(static_cast<base::Position>(my_vector->getData()), Eigen::Quaterniond::Identity());
        tf_prop.setTransform(tf);
        labeled_tree.add_edge("child_"+boost::lexical_cast<std::string>(j), "root", tf_prop);
    }

    BOOST_TEST_MESSAGE("UPDATING AN EXISTING TRANSFORM PROPERTY TEST...");
    std::pair<envire::core::LabeledTransformTree::edge_descriptor, bool> edge_pair = boost::edge_by_label("child_0", "root", labeled_tree);
    BOOST_CHECK(edge_pair.second == true);
    envire::core::Transform got_tf = labeled_tree.getTransform(edge_pair.first);
    BOOST_CHECK(got_tf.time == now);

    /* Modify the property of an existing edge **/
    base::Time after = base::Time::now();
    envire::core::Transform tf_prop(after);
    labeled_tree.add_edge("child_0", "root", tf_prop);

    edge_pair = boost::edge_by_label("child_0", "root", labeled_tree);
    BOOST_CHECK(edge_pair.second == true);
    got_tf = labeled_tree.getTransform(edge_pair.first);
    BOOST_CHECK(got_tf.time == after);
    BOOST_TEST_MESSAGE("DONE\n");
}


//BOOST_AUTO_TEST_CASE(print_labeled_tree_test)
//{
//    /** Print graph by edges **/
//    envire::core::LabeledTransformTree::edge_iterator it, end;
//    for(boost::tie(it, end) = boost::edges(labeled_tree); it != end; ++it)
//    {
//        std::cout << boost::get(FramePropertyTag(), labeled_tree)[boost::source(*it, labeled_tree)].name << " -> "
//            << boost::get(FramePropertyTag(), labeled_tree)[boost::target(*it, labeled_tree)].name << '\n';
//    }
//
//    /** Print graph by vertex **/
//    envire::core::LabeledTransformTree::vertex_iterator vertexIt, vertexEnd;
//
//    std::cout << "vertices(g) = ";
//    boost::tie(vertexIt, vertexEnd) = boost::vertices(labeled_tree);
//    for (; vertexIt != vertexEnd; ++vertexIt)
//    {
//        std::cout << boost::get(FramePropertyTag(), labeled_tree)[*vertexIt].name <<" ";
//    }
//    std::cout << std::endl;
//}
//
