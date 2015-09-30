#pragma once
#include <envire_core/graph/TransformGraphTypes.hpp>
#include <envire_core/items/Transform.hpp>
#include <envire_core/items/Frame.hpp>
#include "GraphEvent.hpp"

namespace envire { namespace core
{
    class TransformModifiedEvent : public GraphEvent
    {
    public:
      TransformModifiedEvent(const FrameId& origin,
                             const FrameId& target,
                             const edge_descriptor edge,
                             const edge_descriptor inverseEdge) :
        GraphEvent(GraphEvent::TRANSFORMATION_MODIFIED), origin(origin), target(target),
        edge(edge), inverseEdge(inverseEdge){}

      FrameId origin;/**<Source vertex of the transform */
      FrameId target; /**<Target vertex of the transform */
      edge_descriptor edge; /**<Edge of the tree that the transformation is attached to */
      edge_descriptor inverseEdge; 
    };
}}
