/*
 * FrameAddedEvent.hpp
 *
 *  Created on: Sep 17, 2015
 *      Author: aboeckmann
 */

#ifndef __VERTEX_ADDED_EVENT_HPP__
#define __VERTEX_ADDED_EVENT_HPP__
#include <envire_core/TransformTreeTypes.hpp>
#include <envire_core/Transform.hpp>
#include "TreeEvent.hpp"

namespace envire { namespace core
{

    class VertexAddedEvent : public TreeEvent
    {
    public:
      VertexAddedEvent(const vertex_descriptor& newVertex) :
            FrameEvent(TreeEvent::VERTEX_ADDED), addedVertex(newVertex) {}

        vertex_descriptor addedVertex;/**<The vertex that contains the newly added frame */
    };
}}
#endif
