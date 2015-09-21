#pragma once
#include <envire_core/TransformTreeTypes.hpp>
#include <envire_core/Transform.hpp>
#include "TreeEvent.hpp"

namespace envire { namespace core
{
    class TransformModifiedEvent : public TreeEvent
    {
    public:
      TransformModifiedEvent(const vertex_descriptor& from,
                             const vertex_descriptor& to,
                             const edge_descriptor& edge,
                             const Transform& oldTransform,
                             const Transform& newTransform) :
        TreeEvent(TreeEvent::TRANSFORMATION_MODIFIED), from(from), to(to),
        edge(edge), oldTransform(oldTransform), newTransform(newTransform){}

      vertex_descriptor from;/**<Source vertex of the transform */
      vertex_descriptor to; /**<Target vertex of the transform */
      edge_descriptor edge; /**<Edge of the tree that the transformation is attachted to */
      Transform oldTransform; /**<Old value of the transformation */
      Transform newTransform; /**<New value of the transformation. */
    };
}}
