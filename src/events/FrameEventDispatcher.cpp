/*
 * FramEventDispatcher.cpp
 *
 *  Created on: Sep 16, 2015
 *      Author: aboeckmann
 */
#include "FrameEvent.hpp"
#include "FrameAddedEvent.hpp"
#include "FrameRootAddedEvent.hpp"
#include <cassert>
#include "FrameEventDispatcher.hpp"

using namespace envire::core;

void FrameEventDispatcher::notifyFrameEvent(const FrameEvent& event)
{
    switch(event.type)
    {
    case FrameEvent::FRAME_ADDED:
        frameAdded(dynamic_cast<const FrameAddedEvent&>(event));
        break;
    case FrameEvent::ROOT_FRAME_ADDED:
        frameRootAdded(dynamic_cast<const FrameRootAddedEvent&>(event));
        break;
    case FrameEvent::FRAME_MODIFIED:
      assert(false);
        break;
    case FrameEvent::FRAME_REMOVED:
      assert(false);
        break;
    default:
        assert(false);//you forgot to add an event type
    }
}



