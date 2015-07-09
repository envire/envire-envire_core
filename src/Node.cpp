#include "Node.hpp"

#include <boost/uuid/uuid_generators.hpp>

using namespace envire::core;

Node::Node():uuid(boost::uuids::random_generator()()),frame_name("envire::core::noname")
{
    this->items.clear(); /** Remove all elements form the vector */
}

Node::Node(const std::string &frame_name): uuid(boost::uuids::random_generator()())
{
    this->setFrame(frame_name);
    this->items.clear(); /** Remove all elements form the vector */
}

Node::~Node()
{
    this->items.clear();
}
