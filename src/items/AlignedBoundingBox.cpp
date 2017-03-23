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

#include "AlignedBoundingBox.hpp"
#include <stdexcept>

using namespace envire::core;

AlignedBoundingBox::AlignedBoundingBox() : BoundingVolume()
{
    clear();
}

AlignedBoundingBox::AlignedBoundingBox(const Eigen::AlignedBox< double, 3 >& box) : BoundingVolume()
{
    this->box = box;
}

void AlignedBoundingBox::extend(const boost::shared_ptr< BoundingVolume >& bv)
{
    boost::shared_ptr<AlignedBoundingBox> aligned_box =
        boost::dynamic_pointer_cast< AlignedBoundingBox >(bv);
    box.extend(aligned_box->box);
}

void AlignedBoundingBox::extend(const Eigen::Vector3d& point)
{
    box.extend(point);
}

bool AlignedBoundingBox::contains(const boost::shared_ptr< BoundingVolume >& bv) const
{
    boost::shared_ptr<AlignedBoundingBox> aligned_box =
        boost::dynamic_pointer_cast< AlignedBoundingBox >(bv);
    return box.contains(aligned_box->box);
}

bool AlignedBoundingBox::contains(const Eigen::Vector3d& point) const
{
    return box.contains(point);
}

double AlignedBoundingBox::exteriorDistance(const boost::shared_ptr< BoundingVolume >& bv) const
{
    boost::shared_ptr<AlignedBoundingBox> aligned_box =
        boost::dynamic_pointer_cast< AlignedBoundingBox >(bv);
    return box.exteriorDistance(aligned_box->box);
}

double AlignedBoundingBox::exteriorDistance(const Eigen::Vector3d& point) const
{
    return box.exteriorDistance(point);
}

boost::shared_ptr< BoundingVolume > AlignedBoundingBox::intersection(const boost::shared_ptr< BoundingVolume >& bv) const
{
    boost::shared_ptr<AlignedBoundingBox> aligned_box =
        boost::dynamic_pointer_cast< AlignedBoundingBox >(bv);
    Eigen::AlignedBox<double,3> intersection = box.intersection(aligned_box->box);
    return boost::shared_ptr<BoundingVolume>(new AlignedBoundingBox(intersection));
}

bool AlignedBoundingBox::intersects(const boost::shared_ptr< BoundingVolume >& bv) const
{
    boost::shared_ptr< BoundingVolume > intersection = this->intersection(bv);
    boost::shared_ptr<AlignedBoundingBox> aligned_box =
        boost::dynamic_pointer_cast< AlignedBoundingBox >(bv);
    return !aligned_box->box.isEmpty();
}

Eigen::Vector3d AlignedBoundingBox::center() const
{
    return box.center();
}

void envire::core::AlignedBoundingBox::clear()
{
    box.setEmpty();
}
