#pragma once

#include "EigenTypes.hpp"
#include <base/TransformWithCovariance.hpp>

namespace boost { namespace serialization
{

    /** Serializes base::TransformWithCovariance */
    template<typename _Archive>
    inline void serialize(
        _Archive & ar,
        base::TransformWithCovariance& twc,
        const unsigned int version)
    {
        ar & boost::serialization::make_nvp("translation", twc.translation);
        ar & boost::serialization::make_nvp("orientation", twc.orientation);
        ar & boost::serialization::make_nvp("covariance", twc.cov);
    }

}}