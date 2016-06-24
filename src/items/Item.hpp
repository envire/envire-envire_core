#pragma once

#include "ItemBase.hpp"

#include <utility>
#include <envire_core/serialization/SerializationRegistration.hpp>

#ifdef CMAKE_ENABLE_PLUGINS
  #include <envire_core/plugin/Plugin.hpp>
#endif

namespace envire { namespace core
{

    /**@class Item
    *
    * Item class
    */
    template<class _ItemData>
    class Item : public ItemBase
    {
    public:
        typedef boost::shared_ptr< Item<_ItemData> > Ptr;
        typedef _ItemData TemplateType;

    protected:

        _ItemData user_data;

    public:

        Item() : ItemBase()
        {
        }

        Item(const Item<_ItemData>& item) :  ItemBase(item), user_data(item.user_data)
        {
        }

        Item(Item<_ItemData>&& item) :  ItemBase(std::move(item)), user_data(std::move(item.user_data))
        {
        }

        template <typename... Ts>
        Item(Ts&&... args) : ItemBase(), user_data(std::forward<Ts>(args)...)
        {
        }

        virtual ~Item() {}

        Item<_ItemData>& operator=(const Item<_ItemData>& item)
        {
            ItemBase::operator=(item);
            user_data = item.user_data;
            return *this;
        }

        Item<_ItemData>& operator=(Item<_ItemData>&& item)
        {
            ItemBase::operator=(std::move(item));
            user_data = std::move(item.user_data);
            return *this;
        }

        /**@brief setData
        *
        * Sets the user data
        *
        */
        void setData(const _ItemData& data) { this->user_data = data; }
        void setData(_ItemData&& data) { this->user_data = std::move(data); }

        /**@brief getData
        *
        * Returns the user data
        *
        */
        _ItemData& getData() { return this->user_data; }
        const _ItemData& getData() const { return this->user_data; }

        virtual const std::type_info* getTypeInfo() const 
        {
          return &typeid(envire::core::Item<_ItemData>);
        }
        
        virtual const std::type_info* getEmbeddedTypeInfo() const
        {
          return &typeid(_ItemData);
        }

        virtual void* getRawData() { return &user_data; }

    private:
        /**Grants access to boost serialization */
        friend class boost::serialization::access;

        /**Serializes the members of this class*/
        template <typename Archive>
        void serialize(Archive &ar, const unsigned int version)
        {
            ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(envire::core::ItemBase);
            ar & BOOST_SERIALIZATION_NVP(user_data);
        }

    };

}}
