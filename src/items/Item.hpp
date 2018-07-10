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

#pragma once

#include "ItemBase.hpp"
#include "ItemMetadata.hpp"
#include "SpatioTemporal.hpp"

#include <utility>
#include <boost/serialization/string.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost_serialization/BoostTypes.hpp>

namespace envire { namespace core
{

    /**@class Item
    *
    * @Note: The _ItemData type must have a default constructor and
    *        copy operator.
    */
    template<class _ItemData>
    class Item : public ItemBase
    {
    public:
        typedef boost::shared_ptr< Item<_ItemData> > Ptr;
        typedef _ItemData TemplateType;

    protected:
        SpatioTemporal<_ItemData> spatio_temporal_data;

    public:

        Item() : ItemBase()
        {
            spatio_temporal_data.time = base::Time::now();
            spatio_temporal_data.uuid = SpatioTemporal<_ItemData>::generateNewUUID();
        }

        Item(const _ItemData& data) : ItemBase(), spatio_temporal_data(data)
        {
            spatio_temporal_data.time = base::Time::now();
            spatio_temporal_data.uuid = SpatioTemporal<_ItemData>::generateNewUUID();
        }

        Item(const Item<_ItemData>& item) : ItemBase(item)
        {
            spatio_temporal_data.time = item.spatio_temporal_data.time;
            spatio_temporal_data.uuid = item.spatio_temporal_data.uuid;
            spatio_temporal_data.frame_id = item.spatio_temporal_data.frame_id;
            spatio_temporal_data.data = item.spatio_temporal_data.data;
        }

        Item(Item<_ItemData>&& item) : ItemBase(std::move(item))
        {
            spatio_temporal_data.time = std::move(item.spatio_temporal_data.time);
            spatio_temporal_data.uuid = std::move(item.spatio_temporal_data.uuid);
            spatio_temporal_data.frame_id = std::move(item.spatio_temporal_data.frame_id);
            spatio_temporal_data.data = std::move(item.spatio_temporal_data.data);
        }

        //https://stackoverflow.com/questions/12255546/c-deep-copying-a-base-class-pointer
        virtual ItemBase::Ptr clone() const {
            //has to use this constructor, derived items need default constructor otherwise
            ItemBase::Ptr ptr = ItemBase::Ptr(new Item<_ItemData>(this->getData()));
            ptr->setTime(this->getTime());
            ptr->setID(this->getID());
            ptr->setFrame(this->getFrame());
            return ptr;
        }

        virtual ~Item() {}

        Item<_ItemData>& operator=(const Item<_ItemData>& item)
        {
            ItemBase::operator=(item);
            spatio_temporal_data.time = item.spatio_temporal_data.time;
            spatio_temporal_data.uuid = item.spatio_temporal_data.uuid;
            spatio_temporal_data.frame_id = item.spatio_temporal_data.frame_id;
            spatio_temporal_data.data = item.spatio_temporal_data.data;
            return *this;
        }

        Item<_ItemData>& operator=(Item<_ItemData>&& item)
        {
            ItemBase::operator=(std::move(item));
            spatio_temporal_data.time = std::move(item.spatio_temporal_data.time);
            spatio_temporal_data.uuid = std::move(item.spatio_temporal_data.uuid);
            spatio_temporal_data.frame_id = std::move(item.spatio_temporal_data.frame_id);
            spatio_temporal_data.data = std::move(item.spatio_temporal_data.data);
            return *this;
        }

        /**@brief setTime
        *
        * Sets the timestamp of the item
        *
        */
        virtual void setTime(const base::Time& time) { this->spatio_temporal_data.time = time; }

        /**@brief getTime
        *
        * Returns the timestamp of the item
        *
        */
        virtual const base::Time& getTime() const { return this->spatio_temporal_data.time; }

        /**@brief setID
        *
        * Sets the unique identifier of the item
        *
        */
        virtual void setID(const boost::uuids::uuid& id) { this->spatio_temporal_data.uuid = id; }

        /**@brief getID
        *TARGET
        * Returns the unique identifier of the item
        *
        */
        virtual const boost::uuids::uuid& getID() const { return this->spatio_temporal_data.uuid; }

        /**@brief setFrame
        *
        * Sets the frame name of the item
        *
        */
        virtual void setFrame(const std::string& frame_name) { this->spatio_temporal_data.frame_id = frame_name; }

        /**@brief getFrame
        *
        * Returns the frame name of the item
        *
        */
        virtual const std::string& getFrame() const { return this->spatio_temporal_data.frame_id; }

        /**@brief setData
        *
        * Sets the user data
        *
        */
        void setData(const _ItemData& data) { this->spatio_temporal_data.data = data; }
        void setData(_ItemData&& data) { this->spatio_temporal_data.data = std::move(data); }

        /**@brief getData
        *
        * Returns the user data
        *
        */
        _ItemData& getData() { return this->spatio_temporal_data.data; }
        const _ItemData& getData() const { return this->spatio_temporal_data.data; }


        envire::core::SpatioTemporal<_ItemData>& asSpatioTemporal() {return spatio_temporal_data;}


        virtual bool getClassName(std::string& class_name) const
        {
            if (ItemMetadataMapping::containsMetadata(*getTypeInfo()))
            {
                class_name = ItemMetadataMapping::getMetadata(*getTypeInfo()).className;
                return true;
            }
            return false;
        }

        bool getEmbeddedTypeName(std::string& embedded_type_name) const
        {
            if (ItemMetadataMapping::containsMetadata(*getTypeInfo()))
            {
                embedded_type_name = ItemMetadataMapping::getMetadata(*getTypeInfo()).embeddedTypename;
                return true;
            }
            return false;
        }

        virtual const std::type_info* getTypeInfo() const 
        {
          return &typeid(envire::core::Item<_ItemData>);
        }
        
        virtual const std::type_info* getEmbeddedTypeInfo() const
        {
          return &typeid(_ItemData);
        }

        virtual void* getRawData() { return &spatio_temporal_data.data; }

    private:
        /**Grants access to boost serialization */
        friend class boost::serialization::access;

        /**Serializes the members of this class*/
        template <typename Archive>
        void serialize(Archive &ar, const unsigned int version)
        {
            ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(envire::core::ItemBase);
            ar & boost::serialization::make_nvp("time", spatio_temporal_data.time.microseconds);
            ar & boost::serialization::make_nvp("uuid", spatio_temporal_data.uuid);
            ar & boost::serialization::make_nvp("frame_name", spatio_temporal_data.frame_id);
            ar & boost::serialization::make_nvp("user_data", spatio_temporal_data.data);
        }

    };

}}
