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
        };

        template <typename... Ts>
        Item(Ts&&... args) : user_data(std::forward<Ts>(args)...)
        {
            user_data_ptr = &user_data;
        }; 

        virtual ~Item() {}

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
