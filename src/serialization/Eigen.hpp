#pragma once

#include <boost/serialization/array.hpp>
#include <Eigen/Core>

namespace boost { namespace serialization
{

    template<typename _Archive, typename _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols>
    inline void serialize(
        _Archive & ar,
        Eigen::Matrix< _Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols >& matrix,
        const unsigned int version)
    {
        int rows = matrix.rows();
        int cols = matrix.cols();
        ar & rows;
        ar & cols;
        if(rows != matrix.rows() || cols != matrix.cols())
            matrix.resize(rows, cols);
        ar & boost::serialization::make_array(matrix.data(), rows * cols);
    }

}}