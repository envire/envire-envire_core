//
// Copyright (c) 2015, Deutsches Forschungszentrum für Künstliche Intelligenz GmbH.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

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

