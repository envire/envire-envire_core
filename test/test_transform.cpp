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

#include <boost/test/unit_test.hpp>
#include <envire_core/items/Transform.hpp>

using namespace envire::core;



BOOST_AUTO_TEST_CASE(transform_set_identity)
{
    Transform tf;
    tf.transform.translation << 1, 2, 3;
    tf.transform.orientation = Eigen::AngleAxisd(1.0, Eigen::Vector3d::UnitX());
    
    tf.setIdentity();
    BOOST_CHECK(tf.transform.translation.x() == 0);
    BOOST_CHECK(tf.transform.translation.y() == 0);
    BOOST_CHECK(tf.transform.translation.z() == 0);
}

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