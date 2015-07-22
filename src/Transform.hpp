#ifndef __ENVIRE_CORE_TRANSFORM__
#define __ENVIRE_CORE_TRANSFORM__

#include <vector>
#include <string>

#include <base/Time.hpp>
#include <base/TransformWithCovariance.hpp>

namespace envire { namespace core
{
    class Transform
    {
    public:
        base::Time time; /** Timestamp */
        base::TransformWithCovariance transform; /** the transformation */

    public:
        Transform();
        Transform(const base::Time &_time):time(_time){};

        void setTransform(const base::TransformWithCovariance& tf);

    };

}}
#endif
