#ifndef __ENVIRE_CORE_ALIGNED_BOUNDING_BOX__
#define __ENVIRE_CORE_ALIGNED_BOUNDING_BOX__

#include <Eigen/Geometry>
#include <boost/intrusive_ptr.hpp>
#include "BoundingVolume.hpp"

namespace envire { namespace core
{

    class AlignedBoundingBox : public BoundingVolume
    {
    protected:
        Eigen::AlignedBox<double,3> box;

    public:
        AlignedBoundingBox();
        AlignedBoundingBox(const Eigen::AlignedBox<double,3>& box);
        void extend(const Eigen::Vector3d& point);
        void extend(const boost::intrusive_ptr<BoundingVolume>& bv);
        bool intersects(const boost::intrusive_ptr<BoundingVolume>& bv) const;
        boost::intrusive_ptr<BoundingVolume> intersection(const boost::intrusive_ptr<BoundingVolume>& bv) const;
        bool contains(const Eigen::Vector3d& point) const;
        bool contains(const boost::intrusive_ptr<BoundingVolume>& bv) const;
        double exteriorDistance(const Eigen::Vector3d& point) const;
        double exteriorDistance(const boost::intrusive_ptr<BoundingVolume>& bv) const;
        Eigen::Vector3d center() const;
        void clear();

    };

}}

#endif
