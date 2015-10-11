#include <Eigen/Geometry>
#include <boost/test/unit_test.hpp>
#include <envire_core/items/SpatialItem.hpp>
#include <envire_core/items/AlignedBoundingBox.hpp>

using namespace envire::core;

BOOST_AUTO_TEST_CASE(boundary_test)
{
    BOOST_TEST_MESSAGE( "BOUNDARY TEST" );
    SpatialItem<Eigen::Vector3d> vector_item;
    vector_item.setData(Eigen::Vector3d(0,0,0));
    boost::shared_ptr<AlignedBoundingBox> bounding_box(new AlignedBoundingBox);
    bounding_box->extend(Eigen::Vector3d(1.0,1.0,1.0));
    bounding_box->extend(Eigen::Vector3d(-1.0,-1.0,-1.0));
    vector_item.setBoundary(bounding_box);

    BOOST_CHECK(vector_item.contains(vector_item.getData()));

    SpatialItem<Eigen::Vector3d> vector_item_2;
    vector_item_2.setData(Eigen::Vector3d(1,1,1));
    boost::shared_ptr<AlignedBoundingBox> bounding_box_2(new AlignedBoundingBox);
    bounding_box_2->extend(Eigen::Vector3d(2.0,2.0,2.0));
    bounding_box_2->extend(Eigen::Vector3d(0.0,0.0,0.0));
    vector_item_2.setBoundary(bounding_box_2);

    SpatialItem<Eigen::Vector3d> vector_item_3;
    boost::shared_ptr<AlignedBoundingBox> bounding_box_3(new AlignedBoundingBox);
    bounding_box_3->extend(Eigen::Vector3d(2.0,2.0,2.0));
    bounding_box_3->extend(Eigen::Vector3d(-2.0,-2.0,-2.0));
    vector_item_3.setBoundary(bounding_box_3);

    BOOST_CHECK(!vector_item.contains(vector_item_2));
    BOOST_CHECK(vector_item_3.contains(vector_item));
    BOOST_CHECK(vector_item_3.contains(vector_item_2));
}

BOOST_AUTO_TEST_CASE(boundary_test_with_points)
{
    BOOST_TEST_MESSAGE( "BOUNDARY TEST WITH POINTS" );

    SpatialItem<Eigen::Vector3d> vector_item;
    vector_item.setData(Eigen::Vector3d(0,0,0));
    boost::shared_ptr<AlignedBoundingBox> bounding_box(new AlignedBoundingBox);
    bounding_box->extend(Eigen::Vector3d(1.0,1.0,1.0));
    bounding_box->extend(Eigen::Vector3d(0.0,0.0,0.0));
    vector_item.setBoundary(bounding_box);


    Eigen::Vector3d point_one (0.4, 0.3, 0.5);
    Eigen::Vector3d point_two (4, 3, 9);
    Eigen::Vector3d point_three (1, 1, 1);

    BOOST_CHECK(vector_item.contains(point_one));
    BOOST_CHECK(!vector_item.contains(point_two));
    BOOST_CHECK(vector_item.contains(point_three));
}

