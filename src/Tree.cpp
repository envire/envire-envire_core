#include "Tree.hpp"

#include <boost/uuid/uuid_generators.hpp>

using namespace envire::core;

Tree::Tree(const std::string &environment_name)
{
    /** Environment properties **/
    this->tree[boost::graph_bundle].uuid = boost::uuids::random_generator()();
    this->tree[boost::graph_bundle].name = environment_name;
}

Tree::Tree()
{
    /** Environment properties **/
    this->tree[boost::graph_bundle].uuid = boost::uuids::random_generator()();
    this->tree[boost::graph_bundle].name = "envire::core::noname";
}

Tree::~Tree()
{
    /** Remove all vertices and edges from the tree **/
    this->tree.clear();
}



