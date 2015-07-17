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

void AlignedBoundingBox::extend(const boost::intrusive_ptr< BoundingVolume >& bv)
{
    boost::intrusive_ptr<AlignedBoundingBox> aligned_box;
    if(cast<AlignedBoundingBox>(bv, aligned_box))
    {
        box.extend(aligned_box->box);
    }
    else
        throw std::runtime_error("Failed to cast BoundingVolume to AlignedBoundingBox!");
}

void AlignedBoundingBox::extend(const Eigen::Vector3d& point)
{
    box.extend(point);
}

bool AlignedBoundingBox::contains(const boost::intrusive_ptr< BoundingVolume >& bv) const
{
    boost::intrusive_ptr<AlignedBoundingBox> aligned_box;
    if(cast<AlignedBoundingBox>(bv, aligned_box))
    {
        return box.contains(aligned_box->box);
    }
    else
        throw std::runtime_error("Failed to cast BoundingVolume to AlignedBoundingBox!");
}

bool AlignedBoundingBox::contains(const Eigen::Vector3d& point) const
{
    return box.contains(point);
}

double AlignedBoundingBox::exteriorDistance(const boost::intrusive_ptr< BoundingVolume >& bv) const
{
    boost::intrusive_ptr<AlignedBoundingBox> aligned_box;
    if(cast<AlignedBoundingBox>(bv, aligned_box))
    {
        return box.exteriorDistance(aligned_box->box);
    }
    else
        throw std::runtime_error("Failed to cast BoundingVolume to AlignedBoundingBox!");
}

double AlignedBoundingBox::exteriorDistance(const Eigen::Vector3d& point) const
{
    return box.exteriorDistance(point);
}

boost::intrusive_ptr< BoundingVolume > AlignedBoundingBox::intersection(const boost::intrusive_ptr< BoundingVolume >& bv) const
{
    boost::intrusive_ptr<AlignedBoundingBox> aligned_box;
    if(cast<AlignedBoundingBox>(bv, aligned_box))
    {
        Eigen::AlignedBox<double,3> intersection = box.intersection(aligned_box->box);
        return boost::intrusive_ptr<BoundingVolume>(new AlignedBoundingBox(intersection));
    }
    else
        throw std::runtime_error("Failed to cast BoundingVolume to AlignedBoundingBox!");
}

bool AlignedBoundingBox::intersects(const boost::intrusive_ptr< BoundingVolume >& bv) const
{
    boost::intrusive_ptr< BoundingVolume > intersection = this->intersection(bv);
    boost::intrusive_ptr<AlignedBoundingBox> aligned_box;
    if(cast<AlignedBoundingBox>(intersection, aligned_box))
    {
        return !aligned_box->box.isEmpty();
    }
    else
        throw std::runtime_error("Failed to cast BoundingVolume to AlignedBoundingBox!");
}

Eigen::Vector3d AlignedBoundingBox::center() const
{
    return box.center();
}

void envire::core::AlignedBoundingBox::clear()
{
    box.setEmpty();
}
