#pragma once

#include <envire_core/items/ItemBase.hpp>

namespace envire { namespace core
{

class ItemHeader
{
public:
    ItemHeader() {}
    ItemHeader(const std::string& class_name) {this->class_name = class_name;}
    ItemHeader(const ItemBase::Ptr& item) {this->class_name = item->getClassName();}

    std::string class_name;
    // TODO extend by data size

private:
    friend class boost::serialization::access;

    template <typename Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        ar & BOOST_SERIALIZATION_NVP(class_name);
    }
};

}}