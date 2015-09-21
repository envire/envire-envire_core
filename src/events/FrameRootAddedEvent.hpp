/*
 * FrameRootAddedEvent.hpp
 *
 *  Created on: Sep 17, 2015
 *      Author: aboeckmann
 */

#ifndef __FRAME_ROOT_ADDED_EVENT_HPP__
#define __FRAME_ROOT_ADDED_EVENT_HPP__
#include <envire_core/TransformTreeTypes.hpp>
#include <envire_core/Transform.hpp>
#include "TreeEvent.hpp"

namespace envire { namespace core
{
    class FrameRootAddedEvent : public TreeEvent
    {
    public:
      FrameRootAddedEvent(const vertex_descriptor& added) :
            TreeEvent(TreeEvent::ROOT_FRAME_ADDED), addedVertex(added) {}

      vertex_descriptor addedVertex;/**<The vertex that contains the newly added frame */
    };
}}
#endif
