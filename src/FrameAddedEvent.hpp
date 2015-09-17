/*
 * FrameAddedEvent.hpp
 *
 *  Created on: Sep 17, 2015
 *      Author: aboeckmann
 */

#ifndef SRC_FRAMEADDEDEVENT_HPP_
#define SRC_FRAMEADDEDEVENT_HPP_
#include "FrameEvent.hpp"
#include "TransformTreeTypes.hpp"
#include "Transform.hpp"

namespace envire { namespace core
{
    class FrameAddedEvent : public FrameEvent
    {
    public:
        FrameAddedEvent(const vertex_descriptor& parent,
                        const vertex_descriptor& newVertex,
                        const Transform& tf) :
            FrameEvent(FrameEvent::FRAME_ADDED),
            parent(parent), addedVertex(newVertex), transform(tf) {}

        vertex_descriptor parent; /**<The parent vertex of the added frame*/
        vertex_descriptor addedVertex;/**<The vertex that contains the newly added frame */
        Transform transform;/**<The transform between parent and newly added vertex */
    };
}}
#endif /* SRC_FRAMEADDEDEVENT_HPP_ */
