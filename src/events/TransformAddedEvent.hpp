#pragma once
#include <envire_core/TransformGraphTypes.hpp>
#include <envire_core/Transform.hpp>
#include "GraphEvent.hpp"

namespace envire { namespace core
{
    class TransformAddedEvent : public GraphEvent
    {
    public:
      TransformAddedEvent(const vertex_descriptor& origin,
                          const vertex_descriptor& target,
                          const edge_descriptor& edge,
                          const Transform& transform) :
        GraphEvent(GraphEvent::TRANSFORMATION_ADDED), origin(origin), target(target),
        edge(edge), transform(transform){}

      vertex_descriptor origin;/**<Source vertex of the transform */
      vertex_descriptor target; /**<Target vertex of the transform */
      edge_descriptor edge; /**<Edge of the tree that the transformation is attached to */
      Transform transform; /**<value of the transformation */
    };
}}
