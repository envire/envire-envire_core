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

#ifndef __ENVIRE_CORE_TRANSFORM__
#define __ENVIRE_CORE_TRANSFORM__

#include <vector>
#include <string>
#include <sstream>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost_serialization/BaseTypes.hpp>
#include <boost/format.hpp>
#include <base/Time.hpp>

namespace envire { namespace core
{
  
    /** @param TRANSFORM FIXME define constraints
    /**                  Constraints:
     *                   * TRANSFORM::TRANSFORM(translation, orientation)   
     *                   * TRANSFORM TRANSFORM::inverse()
     *                   * static TRANSFORM TRANSFORM::Identity()
    /** @ */
    template <class TRANSFORM>
    class Transform
    {
    public:
        using innerTransformType = TRANSFORM;
        base::Time time; /** Timestamp */
        TRANSFORM transform; /** the transformation */

    public:
        Transform() { this->transform.invalidateTransform(); };
        
        Transform(const base::Time &_time):time(_time)
        { this->transform.invalidateTransform(); };
        
        Transform(const innerTransformType &_twc):transform(_twc){};
        
        Transform(const base::Time &_time, const innerTransformType &_twc):
            time(_time), transform(_twc){};
            
        Transform(const Transform<innerTransformType> &tf) : time(tf.time), transform(tf.transform) {}

        Transform(Transform<innerTransformType>&& tf) : time(std::move(tf.time)),
                                    transform(std::move(tf.transform)) {}

        Transform<innerTransformType>& operator=(const  Transform<innerTransformType>& other)
        {
          time = other.time;
          transform = other.transform;
          return *this;
        }

         Transform<innerTransformType>& operator=(Transform<innerTransformType>&& other)
        {
          time = std::move(other.time);
          transform = std::move(other.transform);
          return *this;
        }

        void setTransform(const innerTransformType& tf)
        {
            this->transform = tf;
        }

         Transform<innerTransformType> operator*(const  Transform<innerTransformType> &tf) const
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
            return  Transform<innerTransformType>(last_time, this->transform*tf.transform);
        }
        
         Transform<innerTransformType> inverse() const
        {
            return  Transform<innerTransformType>(time, transform.inverse());
        }
        
        const std::string toString() const 
        {
            //FIXME access to orientation etc.
            std::stringstream out;
//             out << time.toString(::base::Time::Seconds) << "\n" <<
//             boost::format("t: (%.2f %.2f %.2f)\nr: (%.2f %.2f %.2f %.2f)") % transform.translation.x() % transform.translation.y() % transform.translation.z()
//             % transform.orientation.w() % transform.orientation.x() % transform.orientation.y() % transform.orientation.z();
            
            return out.str();
        }
        
        void setIdentity()
        {
          //FIXME this changes semantics slightly (references to translation and orientation will break)
          transform = TRANSFORM::Identity();
//           transform.translation.setZero();
//           transform.orientation.setIdentity();
        }
        
        static Transform<innerTransformType> Identity() 
        {
            return Transform<innerTransformType>(innerTransformType::Identity());
        }

    private:
        /**Grants access to boost serialization */
        friend class boost::serialization::access;

        /**Serializes the members of this class*/
        template <typename Archive>
        void serialize(Archive &ar, const unsigned int version)
        {
            ar & boost::serialization::make_nvp("time", time.microseconds);
            ar & BOOST_SERIALIZATION_NVP(transform);
        }

    };

}}
#endif
