#include <boost/uuid/uuid_generators.hpp>
#include <boost/test/unit_test.hpp>

#include <envire_core/TransformTree.hpp>
#include <envire_core/Item.hpp>
#include <envire_core/GraphViz.hpp>


BOOST_AUTO_TEST_CASE(add_and_remove_vertex_test)
{
    BOOST_TEST_MESSAGE("ADD VERTEX TEST...\n");
    envire::core::TransformTree tree;

    register unsigned int i = 0;
    for (i = 0; i<1000; ++i)
    {
        envire::core::Frame frame("frame_"+std::to_string(i));
        envire::core::TransformTree::vertex_descriptor v1 = tree.add_vertex(frame);
    }

    BOOST_CHECK(tree.num_vertices() == i);

    BOOST_TEST_MESSAGE("REMOVE VERTEX TEST...\n");
    std::pair<envire::core::TransformTree::vertex_iterator, envire::core::TransformTree::vertex_iterator> vp;
    envire::core::TransformTree::vertex_iterator vi, vi_end, next;
    boost::tie(vi, vi_end) = tree.vertices();
    for (next = vi; vi != vi_end; vi = next)
    {
        ++next;
        tree.remove_vertex(*vi);
    }

    BOOST_CHECK(tree.num_vertices() == 0);
}

BOOST_AUTO_TEST_CASE(add_and_remove_edge_test)
{

}


