#include "Edge.hpp"

#include <boost/uuid/uuid_generators.hpp>

using namespace envire::core;

Edge::Edge():uuid(boost::uuids::random_generator()())
{
    this->idx=0;
    this->transform.invalidateTransform(); /** Remove all elements form the vector */
}

void Edge::setTransform(const base::TransformWithCovariance& tf)
{
    this->transform = tf;
}
