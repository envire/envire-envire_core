#include "Transform.hpp"

using namespace envire::core;

Transform::Transform()
{
    this->transform.invalidateTransform(); /** Invalidate transformation */
}

void Transform::setTransform(const base::TransformWithCovariance& tf)
{
    this->transform = tf;
}
