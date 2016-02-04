#include <boost/test/unit_test.hpp>
#include <envire_core/items/Transform.hpp>

using namespace envire::core;



BOOST_AUTO_TEST_CASE(operations_with_transform)
{
    Transform tf1, tf2;
    tf1.transform.translation << 42, 21, -42;
    tf1.transform.orientation = base::AngleAxisd(0.25, base::Vector3d::UnitX());
    tf2.transform.translation << 42, 21, -42;
    tf2.transform.orientation = base::AngleAxisd(0.25, base::Vector3d::UnitX());

    Transform tf_manual;
    tf_manual.setTransform(tf1.transform * tf2.transform);
    Transform tf_operator(tf1 * tf2);
    
    BOOST_CHECK(tf_operator.transform.translation.x() == tf_manual.transform.translation.x());
    BOOST_CHECK(tf_operator.transform.translation.y() == tf_manual.transform.translation.y());
    BOOST_CHECK(tf_operator.transform.translation.z() == tf_manual.transform.translation.z());
    BOOST_CHECK(tf_operator.transform.orientation.x() == tf_manual.transform.orientation.x());
    BOOST_CHECK(tf_operator.transform.orientation.y() == tf_manual.transform.orientation.y());
    BOOST_CHECK(tf_operator.transform.orientation.z() == tf_manual.transform.orientation.z()); 
    BOOST_CHECK(tf_operator.transform.orientation.w() == tf_manual.transform.orientation.w());
}