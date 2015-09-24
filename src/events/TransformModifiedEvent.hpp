#pragma once
#include <envire_core/TransformGraphTypes.hpp>
#include <envire_core/Transform.hpp>
#include <envire_core/Frame.hpp>
#include "GraphEvent.hpp"

namespace envire { namespace core
{
    class TransformModifiedEvent : public GraphEvent
    {
    public:
      TransformModifiedEvent(const FrameId& origin,
                             const FrameId& target,
                             const edge_descriptor& edge,
                             const Transform& oldTransform,
                             const Transform& newTransform) :
        GraphEvent(GraphEvent::TRANSFORMATION_MODIFIED), origin(origin), target(target),
        edge(edge), oldTransform(oldTransform), newTransform(newTransform){}

      FrameId origin;/**<Source vertex of the transform */
      FrameId target; /**<Target vertex of the transform */
      edge_descriptor edge; /**<Edge of the tree that the transformation is attached to */
      Transform oldTransform; /**<Old value of the transformation */
      Transform newTransform; /**<New value of the transformation. */
    };
}}
