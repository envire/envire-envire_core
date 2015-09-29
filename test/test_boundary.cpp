#include <Eigen/Geometry>
#include <boost/test/unit_test.hpp>
#include <envire_core/items/SpatialItem.hpp>
#include <envire_core/items/AlignedBoundingBox.hpp>

using namespace envire::core;

BOOST_AUTO_TEST_CASE(boundary_test)
{
    SpatialItem<Eigen::Vector3d> vector_item;
    vector_item.setData(Eigen::Vector3d(0,0,0));
    boost::intrusive_ptr<AlignedBoundingBox> bounding_box(new AlignedBoundingBox);
    bounding_box->extend(Eigen::Vector3d(1.0,1.0,1.0));
    bounding_box->extend(Eigen::Vector3d(-1.0,-1.0,-1.0));
    vector_item.setBoundary(bounding_box);

    BOOST_ASSERT(vector_item.contains(vector_item.getData()));

    SpatialItem<Eigen::Vector3d> vector_item_2;
    vector_item_2.setData(Eigen::Vector3d(1,1,1));
    boost::intrusive_ptr<AlignedBoundingBox> bounding_box_2(new AlignedBoundingBox);
    bounding_box_2->extend(Eigen::Vector3d(2.0,2.0,2.0));
    bounding_box_2->extend(Eigen::Vector3d(0.0,0.0,0.0));
    vector_item_2.setBoundary(bounding_box_2);

    SpatialItem<Eigen::Vector3d> vector_item_3;
    boost::intrusive_ptr<AlignedBoundingBox> bounding_box_3(new AlignedBoundingBox);
    bounding_box_3->extend(Eigen::Vector3d(2.0,2.0,2.0));
    bounding_box_3->extend(Eigen::Vector3d(-2.0,-2.0,-2.0));
    vector_item_3.setBoundary(bounding_box_3);

     BOOST_ASSERT(!vector_item.contains(vector_item_2));
     BOOST_ASSERT(vector_item_3.contains(vector_item));
     BOOST_ASSERT(vector_item_3.contains(vector_item_2));

}
