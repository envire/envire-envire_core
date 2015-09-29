#pragma once
#include <envire_core/items/Transform.hpp>
#include <envire_core/items/Frame.hpp>
#include <envire_core/graph/TransformGraphTypes.hpp>
#include "GraphEvent.hpp"

namespace envire { namespace core
{
    class TransformAddedEvent : public GraphEvent
    {
    public:
      TransformAddedEvent(const FrameId& origin,
                          const FrameId& target,
                          const edge_descriptor& edge,
                          const Transform& transform) :
        GraphEvent(GraphEvent::TRANSFORMATION_ADDED), origin(origin), target(target),
        edge(edge), transform(transform){}

      FrameId origin;/**<Source vertex of the transform */
      FrameId target; /**<Target vertex of the transform */
      edge_descriptor edge; /**<Edge of the tree that the transformation is attached to */
      Transform transform; /**<value of the transformation */
    };
}}
