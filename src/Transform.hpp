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
        Transform() { this->transform.invalidateTransform(); };
        Transform(const base::Time &_time):time(_time)
        { this->transform.invalidateTransform(); };
        Transform(const base::Time &_time, const base::TransformWithCovariance &_twc):
            time(_time), transform(_twc){};


        Transform(const Transform &_transform)
        {
            this->time = _transform.time;
            this->transform = _transform.transform;
        }

        void setTransform(const base::TransformWithCovariance& tf)
        {
            this->transform = tf;
        }

        Transform operator*(const Transform &tf) const
        {
            base::Time last_time;
            if(this->time > tf.time)
            {
                last_time = this->time;
            }
            else
            {
                last_time = tf.time;
            }
            return Transform(this->time, this->transform*tf.transform);
        }

    };

}}
#endif
