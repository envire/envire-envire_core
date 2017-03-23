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

