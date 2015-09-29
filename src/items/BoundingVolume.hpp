#ifndef __ENVIRE_CORE_BOUNDING_VOLUME__
#define __ENVIRE_CORE_BOUNDING_VOLUME__

#include <Eigen/Geometry>
#include <boost/intrusive_ptr.hpp>

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

        long ref_count; /** Reference counter of the smart pointer */

    protected:

        friend void intrusive_ptr_add_ref( BoundingVolume* bv );
        friend void intrusive_ptr_release( BoundingVolume* bv );

    public:

        virtual ~BoundingVolume() {};
        virtual void extend(const Eigen::Vector3d& point) = 0;
        virtual bool contains(const Eigen::Vector3d& point) const = 0;
        virtual double exteriorDistance(const Eigen::Vector3d& point) const = 0;
        virtual Eigen::Vector3d center() const = 0;
        virtual void extend(const boost::intrusive_ptr<BoundingVolume>& bv) = 0;
        virtual bool intersects(const boost::intrusive_ptr<BoundingVolume>& bv) const = 0;
        virtual boost::intrusive_ptr<BoundingVolume> intersection(const boost::intrusive_ptr<BoundingVolume>& bv) const = 0;
        virtual bool contains(const boost::intrusive_ptr<BoundingVolume>& bv) const = 0;
        virtual double exteriorDistance(const boost::intrusive_ptr<BoundingVolume>& bv) const = 0;
        virtual void clear() = 0;

    protected:

        template<class T>
        bool cast(const boost::intrusive_ptr< BoundingVolume >& bv, boost::intrusive_ptr< T >& derived_type) const
        {
            T* derived_volume = dynamic_cast<T*>(bv.get());
            if(derived_volume != NULL)
            {
                derived_type.reset(derived_volume);
                return true;
            }
            return false;
        }

    };

    void intrusive_ptr_add_ref( BoundingVolume* bv );
    void intrusive_ptr_release( BoundingVolume* bv );

}}
#endif