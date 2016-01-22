#ifndef __ENVIRE_CORE_ITEM_BASE__
#define __ENVIRE_CORE_ITEM_BASE__

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/nvp.hpp>
#include <base/Time.hpp>
#include <string>
#include <type_traits>
#include <typeindex>
#include <envire_core/serialization/BoostTypes.hpp>

namespace envire { namespace core
{
    using FrameId = std::string;

    /**@class ItemBase
    *
    * ItemBase class
    */
    class ItemBase
    {
    public:
        template<class T> 
        using PtrType = boost::shared_ptr<T>;
        
        typedef PtrType<ItemBase> Ptr;

    protected:

        base::Time time; /** Timestamp */

        boost::uuids::uuid uuid; /** Unique Identifier */

        FrameId frame_name; /** Frame name in which the Item is located */

        // TBD: do we want/need this pointer?
        void* user_data_ptr; /** Pointer to the user data */

    public:

        ItemBase();
        ItemBase(const ItemBase& item);
        ItemBase(ItemBase&& item);
        virtual ~ItemBase() {}

        ItemBase& operator=(const ItemBase& item);
        ItemBase& operator=(ItemBase&& item);

        /**@brief setTime
        *
        * Sets the timestamp of the item
        *
        */
        void setTime(const base::Time& time) { this->time = time; }

        /**@brief getTime
        *
        * Returns the timestamp of the item
        *
        */
        const base::Time& getTime() const { return this->time; }

        /**@brief setID
        *
        * Sets the unique identifier of the item
        *
        */
        void setID(const boost::uuids::uuid& id) { this->uuid = id; }

        /**@brief getID
        *TARGET
        * Returns the unique identifier of the item
        *
        */
        const boost::uuids::uuid& getID() const { return this->uuid; }
        const std::string getIDString() const { return boost::uuids::to_string(this->uuid); }

        /**@brief setFrame
        *
        * Sets the frame name of the item
        *
        */
        void setFrame(const std::string& frame_name) { this->frame_name = frame_name; }

        /**@brief getFrame
        *
        * Returns the frame name of the item
        *
        */
        const std::string& getFrame() const { return this->frame_name; }

        /**@brief getClassName
        *
        * Returns the class name of the item
        *
        */
        virtual std::string getClassName() const { return "UnknownItem"; }

        virtual std::type_index getTypeIndex() const = 0;

        void* getRawData() const { return user_data_ptr; }

    private:
        /**Grands access to boost serialization */
        friend class boost::serialization::access;

        /**Serializes the members of this class*/
        template <typename Archive>
        void serialize(Archive &ar, const unsigned int version)
        {
            ar & boost::serialization::make_nvp("time", time.microseconds);
            ar & BOOST_SERIALIZATION_NVP(uuid);
            ar & BOOST_SERIALIZATION_NVP(frame_name);
        }
    };

    /**Mark this class as abstract class */
    BOOST_SERIALIZATION_ASSUME_ABSTRACT(envire::core::ItemBase);
    
    template <class TARGET>
    struct ItemBaseCaster 
    {
        ItemBase::PtrType<TARGET> operator()(const ItemBase::Ptr p) const
        {
            //FIXME static_assert that TARGET is ItemBase::PtrType<X>
            return boost::dynamic_pointer_cast<TARGET>(p);
        }
    };
}}

#endif


