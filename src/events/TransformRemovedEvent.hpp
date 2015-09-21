#pragma once
#include <envire_core/TransformTreeTypes.hpp>
#include <envire_core/Transform.hpp>
#include "TreeEvent.hpp"

namespace envire { namespace core
{
    class TransformRemovedEvent : public TreeEvent
    {
    public:
      TransformRemovedEvent(const vertex_descriptor& from,
                          const vertex_descriptor& to,
                          const Transform& transform) :
        TreeEvent(TreeEvent::TRANSFORMATION_REMOVED), from(from), to(to),
        transform(transform){}

      vertex_descriptor from;/**<Source vertex of the transform */
      vertex_descriptor to; /**<Target vertex of the transform */
      Transform transform; /**<value of the transformation */
    };
}}
