#ifndef __ENVIRE_CORE_BOUNDING_VOLUME__
#define __ENVIRE_CORE_BOUNDING_VOLUME__

#include <Eigen/Geometry>
#include <boost/shared_ptr.hpp>

namespace envire { namespace core
{

    /**@class BoundingVolume
    *
    * BoundingVolume class
    */
    class BoundingVolume
    {
    protected:

        BoundingVolume() {};

    public:

        virtual ~BoundingVolume() {};
        virtual void extend(const Eigen::Vector3d& point) = 0;
        virtual bool contains(const Eigen::Vector3d& point) const = 0;
        virtual double exteriorDistance(const Eigen::Vector3d& point) const = 0;
        virtual Eigen::Vector3d center() const = 0;
        virtual void extend(const boost::shared_ptr<BoundingVolume>& bv) = 0;
        virtual bool intersects(const boost::shared_ptr<BoundingVolume>& bv) const = 0;
        virtual boost::shared_ptr<BoundingVolume> intersection(const boost::shared_ptr<BoundingVolume>& bv) const = 0;
        virtual bool contains(const boost::shared_ptr<BoundingVolume>& bv) const = 0;
        virtual double exteriorDistance(const boost::shared_ptr<BoundingVolume>& bv) const = 0;
        virtual void clear() = 0;

    };
}}
#endif
