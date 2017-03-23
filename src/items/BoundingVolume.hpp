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
