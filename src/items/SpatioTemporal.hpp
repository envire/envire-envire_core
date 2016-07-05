#ifndef BASE_SPATIO_TEMPORAL_HPP
#define BASE_SPATIO_TEMPORAL_HPP

#include <string>
#include <base/Time.hpp>
#include <base-logging/Singleton.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/nil_generator.hpp>

namespace base
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

}

#endif