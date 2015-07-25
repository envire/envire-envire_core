#include <boost/uuid/uuid_generators.hpp>
#include <boost/test/unit_test.hpp>

#include <envire_core/TransformTree.hpp>
#include <envire_core/Item.hpp>
#include <envire_core/GraphViz.hpp>


BOOST_AUTO_TEST_CASE(add_and_remove_vertex_test)
{
    unsigned int max_vertices = 100;

    BOOST_TEST_MESSAGE("ADD VERTEX TEST...");
    envire::core::TransformTree tree;

    register unsigned int i = 0;
    for (i = 0; i<max_vertices; ++i)
    {
        envire::core::Frame frame("frame_"+std::to_string(i));
        envire::core::TransformTree::vertex_descriptor v1 = tree.add_vertex(frame);
    }

    BOOST_CHECK(tree.num_vertices() == i);
    BOOST_TEST_MESSAGE("DONE\n");

    BOOST_TEST_MESSAGE("REMOVE VERTEX TEST...");
    envire::core::TransformTree::vertex_iterator vi, vi_end, next;
    boost::tie(vi, vi_end) = tree.vertices();
    for (next = vi; vi != vi_end; vi = next)
    {
        ++next;
        tree.remove_vertex(*vi);
    }

    BOOST_CHECK(tree.num_vertices() == 0);
    BOOST_TEST_MESSAGE("DONE\n");
}

BOOST_AUTO_TEST_CASE(add_and_remove_edge_test)
{

    unsigned int max_vertices = 100;

    BOOST_TEST_MESSAGE("ADD EDGES TEST...");
    envire::core::TransformTree tree;

    register unsigned int i = 0;
    for (i = 0; i<max_vertices; ++i)
    {
        envire::core::Frame frame("frame_"+std::to_string(i));
        envire::core::TransformTree::vertex_descriptor v1 = tree.add_vertex(frame);
    }

    /** get root node **/
    envire::core::TransformTree::vertex_descriptor root = tree.vertex(0);
    for (envire::core::TransformTree::vertices_size_type iv = 1; iv < tree.num_vertices(); ++iv)
    {
        envire::core::TransformTree::edge_descriptor edge; bool b;
        envire::core::TransformTree::vertex_descriptor node_to = tree.vertex(iv);
        try
        {
            boost::tie(edge, b) = tree.add_edge(root, node_to);
            BOOST_CHECK(b == true);
        }catch (std::logic_error e)
        {
            std::cerr<<e.what();
        }
    }

    BOOST_CHECK(tree.num_edges() == max_vertices - 1);
    BOOST_TEST_MESSAGE("DONE\n");

    BOOST_TEST_MESSAGE("REMOVE EDGES TEST...");
    envire::core::TransformTree::edge_iterator ei, ei_end, next;
    boost::tie(ei, ei_end) = tree.edges();
    for (next = ei; ei != ei_end; ei = next)
    {
        ++next;
        tree.remove_edge(*ei);
    }

    BOOST_CHECK(tree.num_edges() == 0);
    BOOST_TEST_MESSAGE("DONE\n");
}

BOOST_AUTO_TEST_CASE(property_and_grahviz_test)
{

    unsigned int max_vertices = 100;

    class Vector: public envire::core::Item<Eigen::Vector3d>
    {
    };

    envire::core::TransformTree tree;
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

    register unsigned int i = 0;
    for (i = 0; i<max_vertices; ++i)
    {
        envire::core::Frame frame("frame_"+std::to_string(i));
        frame.items = items_vector;
        envire::core::TransformTree::vertex_descriptor v1 = tree.add_vertex(frame);
    }

    BOOST_TEST_MESSAGE("FRAME PROPERTY TEST...");
    for (i = 0; i<max_vertices; ++i)
    {
        envire::core::Frame frame = tree.getFrame(tree.vertex(i));
        BOOST_CHECK(frame.name == "frame_"+std::to_string(i));
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

    /** get root node and create edges **/
    base::Time now = base::Time::now();
    envire::core::TransformTree::vertex_descriptor root = tree.vertex(0);
    for (envire::core::TransformTree::vertices_size_type iv = 1; iv < tree.num_vertices(); ++iv)
    {
        envire::core::TransformTree::edge_descriptor edge; bool b;
        envire::core::Transform tf_prop(now);
        base::TransformWithCovariance tf(Eigen::AngleAxisd::Identity(), static_cast<base::Position>(my_vector->getData()));
        tf_prop.setTransform(tf);
        envire::core::TransformTree::vertex_descriptor node_to = tree.vertex(iv);
        boost::tie(edge, b) = tree.add_edge(root, node_to, tf_prop);
        BOOST_CHECK(b == true);
    }

    BOOST_TEST_MESSAGE("TRANSFORM PROPERTY TEST...");
    envire::core::TransformTree::edge_iterator it, end;
    for(boost::tie(it, end) = tree.edges(); it != end; ++it)
    {
        envire::core::Transform transform = boost::get(&envire::core::TransformProperty::transform, tree, *it);
        BOOST_CHECK(transform.time == now);
        BOOST_CHECK(transform.transform.translation == my_vector->getData());
    }
    BOOST_TEST_MESSAGE("DONE\n");

    BOOST_TEST_MESSAGE("GRAPHVIZ TEST...");
    envire::core::GraphViz gviz;

    gviz.write(tree, "graphviz_boost_test_transform_tree.dot");

    //tree.clear();
    BOOST_TEST_MESSAGE("DONE\n");
}

