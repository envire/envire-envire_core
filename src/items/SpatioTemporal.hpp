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

#ifndef BASE_SPATIO_TEMPORAL_HPP
#define BASE_SPATIO_TEMPORAL_HPP

#include <string>
#include <base/Time.hpp>
#include <base-logging/Singleton.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/nil_generator.hpp>

namespace envire { namespace core
{

/**
 * This type can be used to augment arbitrary data types with spatio-temporal information.
 * I.e. a timestamp and a frame ID, it also provides a universally unique ID.
 *
 * The main use-case for this type is to augment informations related to a
 * environment representation (e.g. maps) or a geographic information system.
 * Where type transport normally happens sporadicly and data types might not allow introspection.
 *
 * The universally unique ID allows the user to distinguish data types of the same type.
 * E.g. two instances emitted by the same port.
 */
template<class T>
class SpatioTemporal
{
public:
    typedef std::string FrameID;

    /** Reference timestamp */
    base::Time time;

    /** Universally unique identifier */
    boost::uuids::uuid uuid;

    /** Frame ID */
    FrameID frame_id;

    /** User data */
    T data;

public:
    SpatioTemporal() : uuid(boost::uuids::nil_uuid()) {}

    SpatioTemporal(const T& data) : uuid(boost::uuids::nil_uuid()), data(data) {}

    void setTime(const base::Time& time) { this->time = time; }
    const base::Time& getTime() const { return time; }

    void setUUID(const boost::uuids::uuid& id) { uuid = id; }
    const boost::uuids::uuid& getUUID() const { return uuid; }
    const std::string getUUIDString() const { return boost::uuids::to_string(uuid); }

    void setFrameID(const FrameID& frame_id) { this->frame_id = frame_id; }
    const FrameID& getFrameID() const { return frame_id; }

    void setData(const T& data) {this->data = data;}
    const T& getData() {return data;}

    /**
     * Generates a new UUID using a Mersenne Twister pseudo-random generator (mt19937)
     */
    static boost::uuids::uuid generateNewUUID()
    {
        return (*base::Singleton<boost::uuids::random_generator>::getInstance())();
    }
};

}}

#endif