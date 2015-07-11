#include "Frame.hpp"

#include <boost/uuid/uuid_generators.hpp>

using namespace envire::core;

Frame::Frame():name("envire::core::noname"),uuid(boost::uuids::random_generator()())
{
}

Frame::Frame(const std::string &name): uuid(boost::uuids::random_generator()())
{
    this->setName(name);
}

Frame::~Frame()
{
    this->items.clear(); /** Remove all elements form the vector */
}
