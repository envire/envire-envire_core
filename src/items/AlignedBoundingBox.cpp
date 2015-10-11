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
