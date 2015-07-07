#ifndef __ENVIRE_CORE_ITEM__
#define __ENVIRE_CORE_ITEM__

#include <envire_core/ItemBase.hpp>

namespace envire { namespace core
{

    /**@class Item
    *
    * Item class
    */
    template<class _ItemData>
    class Item : public ItemBase
    {
    protected:

        _ItemData user_data;

    public:

        Item() : ItemBase()
        {
            user_data_ptr = &user_data;
        };

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

    };

}}
#endif

