#include <boost/test/unit_test.hpp>
#include <envire_core/graph/Graph.hpp>


class FrameProp : public envire::core::graph::FramePropertyBase { };

BOOST_AUTO_TEST_CASE(simple_add_item_test)
{
  envire::core::graph::Graph<FrameProp, int> g;
}