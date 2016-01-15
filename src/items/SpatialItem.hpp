#ifndef __ENVIRE_CORE_SPATIAL_ITEM__
#define __ENVIRE_CORE_SPATIAL_ITEM__

#include <envire_core/items/Item.hpp>
#include <envire_core/items/BoundingVolume.hpp>
#include <boost/shared_ptr.hpp>
#include <Eigen/Geometry>

namespace envire { namespace core
{

    /**@class SpatialItem
    *
    * SpatialItem class
    */
    template<class _ItemData>
    class SpatialItem : public Item<_ItemData>
    {

    public:
        typedef boost::shared_ptr< SpatialItem<_ItemData> > Ptr;

    protected:
        boost::shared_ptr<BoundingVolume> boundary;

    public:

        SpatialItem() : Item<_ItemData>()
        {

        };

        virtual ~SpatialItem() {}

        void setBoundary(const boost::shared_ptr<BoundingVolume>& boundary) {this->boundary = boundary;}

        boost::shared_ptr<BoundingVolume> getBoundary() {return boundary;}
        const boost::shared_ptr<BoundingVolume>& getBoundary() const {return boundary;}

        void extendBoundary(const Eigen::Vector3d& point)
        {
            checkBoundingVolume();
            boundary->extend(point);
        }

        template<typename _Data>
        void extendBoundary(const SpatialItem<_Data>& spatial_item)
        {
            checkBoundingVolume();
            boundary->extend(spatial_item.getBoundary());
        }

        template<typename _Data>
        bool intersects(const SpatialItem<_Data>& spatial_item) const
        {
            checkBoundingVolume();
            return boundary->intersects(spatial_item.getBoundary());
        }

        template<typename _Data>
        boost::shared_ptr<BoundingVolume> intersection(const SpatialItem<_Data>& spatial_item) const
        {
            checkBoundingVolume();
            return boundary->intersection(spatial_item.getBoundary());
        }

        bool contains(const Eigen::Vector3d& point) const
        {
            checkBoundingVolume();
            return boundary->contains(point);
        }

        template<typename _Data>
        bool contains(const SpatialItem<_Data>& spatial_item) const
        {
            checkBoundingVolume();
            return boundary->contains(spatial_item.getBoundary());
        }

        double exteriorDistance(const Eigen::Vector3d& point) const
        {
            checkBoundingVolume();
            return boundary->exteriorDistance(point);
        }

        template<typename _Data>
        double exteriorDistance(const SpatialItem<_Data>& spatial_item) const
        {
            checkBoundingVolume();
            return boundary->exteriorDistance(spatial_item.getBoundary());
        }

        Eigen::Vector3d centerOfBoundary() const
        {
            checkBoundingVolume();
            return boundary->center();
        }

    protected:
        void checkBoundingVolume() const
        {
            if(boundary.get() == NULL)
                throw std::runtime_error("BoundingVolume is not available!");
        }


    };

}}
#endif

