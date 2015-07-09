#ifndef __ENVIRE_CORE_EDGE__
#define __ENVIRE_CORE_EDGE__

#include <boost/uuid/uuid.hpp> /** uuid class */
#include <vector>
#include <string>

#include <base/TransformWithCovariance.hpp>
#include <base/Time.hpp>

namespace envire { namespace core
{
    class Edge
    {
    public:
        int idx;
        base::Time time; /** Timestamp */
        boost::uuids::uuid uuid; /** Unique Identifier */
        base::TransformWithCovariance transform; /** the transformation */

    public:
        Edge();

        void setTransform(const base::TransformWithCovariance& tf);


    };
}}
#endif
