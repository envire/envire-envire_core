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

#ifndef __ENVIRE_CORE_ALIGNED_BOUNDING_BOX__
#define __ENVIRE_CORE_ALIGNED_BOUNDING_BOX__

#include <Eigen/Geometry>
#include <boost/shared_ptr.hpp>
#include "BoundingVolume.hpp"

namespace envire { namespace core
{

    class AlignedBoundingBox : public BoundingVolume
    {
    public:
        typedef boost::shared_ptr<AlignedBoundingBox> Ptr;

    protected:
        Eigen::AlignedBox<double,3> box;

    public:
        AlignedBoundingBox();
        AlignedBoundingBox(const Eigen::AlignedBox<double,3>& box);
        void extend(const Eigen::Vector3d& point);
        void extend(const boost::shared_ptr<BoundingVolume>& bv);
        bool intersects(const boost::shared_ptr<BoundingVolume>& bv) const;
        boost::shared_ptr<BoundingVolume> intersection(const boost::shared_ptr<BoundingVolume>& bv) const;
        bool contains(const Eigen::Vector3d& point) const;
        bool contains(const boost::shared_ptr<BoundingVolume>& bv) const;
        double exteriorDistance(const Eigen::Vector3d& point) const;
        double exteriorDistance(const boost::shared_ptr<BoundingVolume>& bv) const;
        Eigen::Vector3d center() const;
        void clear();

    };

}}

#endif
