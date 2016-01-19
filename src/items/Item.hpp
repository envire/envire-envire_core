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
            user_data_ptr = &user_data;
        }

        Item(const Item<_ItemData>& item) :  ItemBase(item), user_data(item.user_data)
        {
            user_data_ptr = &this->user_data;
        }

        Item(Item<_ItemData>&& item) :  ItemBase(item), user_data(std::move(item.user_data))
        {
            user_data_ptr = &this->user_data;
        }

        template <typename... Ts>
        Item(Ts&&... args) : ItemBase(), user_data(std::forward<Ts>(args)...)
        {
            user_data_ptr = &user_data;
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
            ItemBase::operator=(item);
            user_data = std::move(item.user_data);
            return *this;
        }

        /**@brief setData
        *
        * Sets the user data
        *
        */
        void setData(const _ItemData& data) { this->user_data = data; }

        /**@brief getData
        *
        * Returns the user data
        *
        */
        _ItemData& getData() { return this->user_data; }

        virtual std::type_index getTypeIndex() const
        {
            return std::type_index(typeid(envire::core::Item<_ItemData>));
        }

    private:
        friend class boost::serialization::access;

        template <typename Archive>
        void serialize(Archive &ar, const unsigned int version)
        {
            ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(envire::core::ItemBase);
            ar & BOOST_SERIALIZATION_NVP(user_data);
        }

    };

}}
