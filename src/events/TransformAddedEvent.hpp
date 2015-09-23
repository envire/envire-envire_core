#pragma once
#include <envire_core/TransformTreeTypes.hpp>
#include <envire_core/Transform.hpp>
#include "TreeEvent.hpp"

namespace envire { namespace core
{
    class TransformAddedEvent : public TreeEvent
    {
    public:
      TransformAddedEvent(const vertex_descriptor& from,
                          const vertex_descriptor& to,
                          const edge_descriptor& edge,
                          const Transform& transform) :
        TreeEvent(TreeEvent::TRANSFORMATION_ADDED), from(from), to(to),
        edge(edge), transform(transform){}

      vertex_descriptor from;/**<Source vertex of the transform */
      vertex_descriptor to; /**<Target vertex of the transform */
      edge_descriptor edge; /**<Edge of the tree that the transformation is attached to */
      Transform transform; /**<value of the transformation */
    };
}}
