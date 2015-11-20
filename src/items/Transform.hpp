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
        base::Affine3d transform; /** the transformation */

    public:
        Transform() : transform(base::Quaterniond(base::Vector4d::Ones() * base::NaN<double>()))
        {};
        
        Transform(const base::Time &_time) : time(_time),
            transform(base::Quaterniond(base::Vector4d::Ones() * base::NaN<double>()))
        {};
        
        Transform(const base::Affine3d &_twc):transform(_twc){};
        
        Transform(const base::Time &_time, const base::Affine3d &_twc):
            time(_time), transform(_twc){};
            
 //       Transform(const base::Position &_translation, const base::Orientation &_orient):
   //         transform(_translation, _orient){};
            
  //      Transform(const base::Time &_time, const base::Position &_translation, const base::Orientation &_orient, const base::Matrix6d &_cov):
    //      time(_time), transform(_translation, _orient, _cov){};


        Transform(const Transform &tf) : time(tf.time), transform(tf.transform) {}

        Transform(Transform&& tf) : time(std::move(tf.time)),
                                    transform(std::move(tf.transform)) {}

        Transform& operator=(const Transform& other)
        {
          time = other.time;
          transform = other.transform;
          return *this;
        }

        Transform& operator=(Transform&& other)
        {
          time = std::move(other.time);
          transform = std::move(other.transform);
          return *this;
        }

        void setTransform(const base::Affine3d& tf)
        {
            transform = tf;
        }

        Transform operator*(const Transform &tf) const
        {
            base::Time last_time;
            if(time > tf.time)
            {
                last_time = time;
            }
            else
            {
                last_time = tf.time;
            }
            return Transform(time, transform*tf.transform);
        }

    };

}}
#endif
