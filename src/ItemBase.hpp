#ifndef __ENVIRE_CORE_ITEM_BASE__
#define __ENVIRE_CORE_ITEM_BASE__

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/intrusive_ptr.hpp>
#include <base/Time.hpp>
#include <string>

namespace envire { namespace core
{

    /**@class ItemBase
    *
    * ItemBase class
    */
    class ItemBase
    {
    public:

        typedef boost::intrusive_ptr<ItemBase> Ptr;

    protected:

        /* TBD: using the intrusive pointer the raw pointer and the intrusive_ptr have the same memory layout.
         * But in the serialization the ref count must be tropped */
        long ref_count; /** Reference counter of the smart pointer */

        base::Time time; /** Timestamp */

        boost::uuids::uuid uuid; /** Unique Identifier */

        std::string frame_name; /** Frame name in which the Item is located */

        // TBD: do we want/need this pointer?
        void* user_data_ptr; /** Pointer to the user data */

        static const std::string class_name; /** Name of the inherited class */

    public:

        ItemBase();
        virtual ~ItemBase() {}

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
        *
        * Returns the unique identifier of the item
        *
        */
        const boost::uuids::uuid getID() const { return this->uuid; }
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
        virtual const std::string& getClassName() const { return class_name; }

        /**@brief getRefCount
        *
        * Returns the reference count of the item
        *
        */
        long getRefCount() const { return ref_count; }

    protected:

        friend void intrusive_ptr_add_ref( ItemBase* item );
        friend void intrusive_ptr_release( ItemBase* item );

    };

    void intrusive_ptr_add_ref( ItemBase* item ) { item->ref_count++; }
    void intrusive_ptr_release( ItemBase* item ) { if(!--item->ref_count) delete item; }

}}
#endif


