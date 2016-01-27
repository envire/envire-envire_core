#pragma once

#include <envire_core/items/ItemBase.hpp>

namespace envire { namespace core
{

/**
 * @brief Header of serialized items.
 * It stores the class name of the serialized item in order to obtain the correct
 * serialization handle of this item.
 */
class ItemHeader
{
public:
    ItemHeader() {}
    ItemHeader(const std::string& class_name) {this->class_name = class_name;}
    ItemHeader(const ItemBase::Ptr& item) {this->class_name = item->getClassName();}

    std::string class_name;

private:
    friend class boost::serialization::access;

    template <typename Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        ar & BOOST_SERIALIZATION_NVP(class_name);
    }
};

}}