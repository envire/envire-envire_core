#pragma once

#include <boost/serialization/array.hpp>
#include <boost/serialization/nvp.hpp>
#include <Eigen/Core>
#include <Eigen/Geometry>

namespace boost { namespace serialization
{

    /** Serializes all Eigen::Matrix types */
    template<typename _Archive, typename _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols>
    inline void serialize(
        _Archive & ar,
        Eigen::Matrix< _Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols >& matrix,
        const unsigned int version)
    {
        int rows = matrix.rows();
        int cols = matrix.cols();
        ar & BOOST_SERIALIZATION_NVP(rows);
        ar & BOOST_SERIALIZATION_NVP(cols);
        if(rows != matrix.rows() || cols != matrix.cols())
            matrix.resize(rows, cols);
        ar & boost::serialization::make_nvp("matrix", boost::serialization::make_array(matrix.data(), rows * cols));
    }

    /** Serializes all Eigen::Quaternion types */
    template<typename _Archive, typename _Scalar, int _Options>
    inline void serialize(
        _Archive & ar,
        Eigen::Quaternion<_Scalar, _Options>& quaternion,
        const unsigned int version)
    {
        ar & boost::serialization::make_nvp("x", quaternion.x());
        ar & boost::serialization::make_nvp("y", quaternion.y());
        ar & boost::serialization::make_nvp("z", quaternion.z());
        ar & boost::serialization::make_nvp("w", quaternion.w());
    }

}}