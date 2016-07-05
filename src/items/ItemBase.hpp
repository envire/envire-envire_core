#ifndef __ENVIRE_CORE_ITEM_BASE__
#define __ENVIRE_CORE_ITEM_BASE__

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/export.hpp>
#include <base/Time.hpp>
#include <string>
#include <type_traits>
#include <typeindex>

namespace envire { namespace core
{
    using FrameId = std::string;

    /**@class ItemBase
    *
    * The ItemBase class is a abstract interface class of all
    * Item<T> classes in EvniRe.
    */
    class ItemBase
    {
    public:
        template<class T> 
        using PtrType = boost::shared_ptr<T>;
        
        typedef PtrType<ItemBase> Ptr;

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
        virtual void setTime(const base::Time& time) = 0;

        /**@brief getTime
        *
        * Returns the timestamp of the item
        *
        */
        virtual const base::Time& getTime() const = 0;

        /**@brief setID
        *
        * Sets the unique identifier of the item
        *
        */
        virtual void setID(const boost::uuids::uuid& id) = 0;

        /**@brief getID
        *TARGET
        * Returns the unique identifier of the item
        *
        */
        virtual const boost::uuids::uuid& getID() const = 0;
        const std::string getIDString() const { return boost::uuids::to_string(this->getID()); }

        /**@brief setFrame
        *
        * Sets the frame name of the item
        *
        */
        virtual void setFrame(const std::string& frame_name) = 0;

        /**@brief getFrame
        *
        * Returns the frame name of the item
        *
        */
        virtual const std::string& getFrame() const = 0;

        /**@brief getClassName
        *
        * Returns the class name of the item
        *
        */
        virtual bool getClassName(std::string& class_name) const { return false; }

        std::type_index getTypeIndex() const
        {
          return std::type_index(*getTypeInfo());
        }
        
        /**Returns the data type of the embedded data*/
        std::type_index getEmbeddedTypeIndex() const
        {
          return std::type_index(*getEmbeddedTypeInfo());
        }
        
        
        /** Returns the typeinfo of the item. 
         * @note The lifetime of std::type_info is till the end of the program. Thus
         * storing and using pointers to std::type_info is safe*/
        virtual const std::type_info* getTypeInfo() const = 0;
        
        /**Returns the data type of the embedded data*/
        virtual const std::type_info* getEmbeddedTypeInfo() const = 0;

        /** Returns a raw pointer to the data of an Item */
        virtual void* getRawData() { return NULL; }
        

    private:
        /**Grands access to boost serialization */
        friend class boost::serialization::access;

        /**Serializes the members of this class*/
        template <typename Archive>
        void serialize(Archive &ar, const unsigned int version)
        {
        }
    };

    /**Mark this class as abstract class */
    BOOST_SERIALIZATION_ASSUME_ABSTRACT(envire::core::ItemBase);
    
    template <class TARGET>
    struct ItemBaseCaster 
    {
        TARGET& operator()(const ItemBase::Ptr p) const
        {
            //FIXME static_assert that TARGET is ItemBase::PtrType<X>
            return *(boost::dynamic_pointer_cast<TARGET>(p));
        }
    };
}}

#endif


