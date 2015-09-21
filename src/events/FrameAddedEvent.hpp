#pragma once
#include <envire_core/TransformTreeTypes.hpp>
#include <envire_core/Transform.hpp>
#include "TreeEvent.hpp"

namespace envire { namespace core
{
    /**
     * A frame-added event is caused whenever a new frame has been added
     * to the tree.
     */
    class FrameAddedEvent : public TreeEvent
    {
    public:
        FrameAddedEvent(const vertex_descriptor& parent,
                        const vertex_descriptor& newVertex,
                        const Transform& tf) :
            TreeEvent(TreeEvent::FRAME_ADDED),
            parent(parent), addedVertex(newVertex), transform(tf) {}

        vertex_descriptor parent; /**<The parent vertex of the added frame*/
        vertex_descriptor addedVertex;/**<The vertex that contains the newly added frame */
        Transform transform;/**<The transform between parent and newly added vertex */
    };
}}
