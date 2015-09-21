#pragma once
#include <envire_core/TransformTreeTypes.hpp>
#include <envire_core/Transform.hpp>
#include "TreeEvent.hpp"

namespace envire { namespace core
{
    class VertexAddedEvent : public TreeEvent
    {
    public:
      VertexAddedEvent(const vertex_descriptor& newVertex) :
        TreeEvent(TreeEvent::VERTEX_ADDED), addedVertex(newVertex) {}

        vertex_descriptor addedVertex;/**<The vertex that contains the newly added frame */
    };
}}
