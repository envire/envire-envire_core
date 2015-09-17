/*
 * FramEventDispatcher.cpp
 *
 *  Created on: Sep 16, 2015
 *      Author: aboeckmann
 */
#include "FramEventDispatcher.hpp"
#include "FrameEvent.hpp"
#include "FrameAddedEvent.hpp"
#include <cassert>

using namespace envire::core;

void FrameEventDispatcher::notifyFrameEvent(const FrameEvent& event)
{
    switch(event.type)
    {
    case FrameEvent::FRAME_ADDED:
        frameAdded(dynamic_cast<const FrameAddedEvent&>(event));
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



