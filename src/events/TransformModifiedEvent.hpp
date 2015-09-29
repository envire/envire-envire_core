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
                             const edge_descriptor inverseEdge,
                             const Transform& oldTransform,
                             const Transform& newTransform,
                             const Transform& oldInverseTransform,
                             const Transform& newInverseTransform
                            ) :
        GraphEvent(GraphEvent::TRANSFORMATION_MODIFIED), origin(origin), target(target),
        edge(edge), inverseEdge(inverseEdge), oldTransform(oldTransform),
        newTransform(newTransform), oldInverseTransform(oldInverseTransform),
        newInverseTransform(newInverseTransform){}

      FrameId origin;/**<Source vertex of the transform */
      FrameId target; /**<Target vertex of the transform */
      edge_descriptor edge; /**<Edge of the tree that the transformation is attached to */
      edge_descriptor inverseEdge; 
      Transform oldTransform; /**<Old value of the transformation */
      Transform newTransform; /**<New value of the transformation. */
      Transform oldInverseTransform; /**<old value of the inverse transformation. */
      Transform newInverseTransform; /**<New value of theinverse  transformation. */
    };
}}
