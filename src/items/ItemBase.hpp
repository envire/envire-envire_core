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

#ifndef __ENVIRE_CORE_ITEM_BASE__
#define __ENVIRE_CORE_ITEM_BASE__

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/export.hpp>
#include <boost/signals2.hpp>
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
        virtual ItemBase::Ptr clone() const = 0;

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
        * @deprecated use getClassName(std::string& class_name) instead
        */
        std::string getClassName() const;

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
        
        /** Emits the itemContentsChanged event */
        void contentsChanged();
        
        /**
         * registeres a changed callback function ponter
         * @warning to receive callbacks, the contentsChanged() method must be called manually to emit the signal
         * 
         * @param callback the function to call on change compatible with boost signal (using lambda functions or boost::bind)
         * The signature of the callback function is (const ItemBase& item)
         * e.g.  connectContentsChangedCallback([&reactor](const ItemBase& item){reactor.frame=item.getFrame();reactor.called=true;});
         * connectContentsChangedCallback(boost::bind(&ItemContentReactor::cb, &reactor,  _1));
         * @warning Lambda functions cannot be disconnected.
         * 
         */
        template<class CALLBACK> void connectContentsChangedCallback(const CALLBACK &callback){
            itemContentsChanged.connect(callback);
        }
        
        /**
         * disconnects a connected callback
         * 
         * @param callback the function to call on change compatible with boost signale (using functor objects or boost::bind)
         */
        template<class CALLBACK> void disconnectContentsChangedCallback(const CALLBACK &callback){
            itemContentsChanged.disconnect(callback);
        }

    private:
        /**Grands access to boost serialization */
        friend class boost::serialization::access;

        /**Serializes the members of this class*/
        template <typename Archive>
        void serialize(Archive &ar, const unsigned int version)
        {
        }
        
        /** This signal can be manually emitted to notify about the change */
        boost::signals2::signal<void (ItemBase& item)> itemContentsChanged;
        
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


