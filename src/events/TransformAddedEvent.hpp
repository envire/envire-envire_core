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
                          const edge_descriptor& inverseEdge,
                          const Transform& transform,
                          const Transform& inverseTransform) :
        TreeEvent(TreeEvent::TRANSFORMATION_ADDED), from(from), to(to),
        edge(edge), inverseEdge(inverseEdge), transform(transform),
        inverseTransform(inverseTransform){}

      vertex_descriptor from;/**<Source vertex of the transform */
      vertex_descriptor to; /**<Target vertex of the transform */
      edge_descriptor edge; /**<Edge of the tree that the transformation is attached to */
      edge_descriptor inverseEdge; /**<The inverse of edge */
      Transform transform; /**<value of the transformation */
      Transform inverseTransform;
    };
}}
