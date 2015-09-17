/*
 * FramEventDispatcher.cpp
 *
 *  Created on: Sep 16, 2015
 *      Author: aboeckmann
 */
#include "FramEventDispatcher.hpp"
#include "FrameEvent.hpp"
#include <cassert>

using namespace envire::core;

void FrameEventDispatcher::notifyFrameEvent(const FrameEvent& event)
{
    switch(event.type)
    {
    case FrameEvent::FRAME_ADDED:
        frameAdded(event.addedArgs);
        break;
    case FrameEvent::FRAME_MODIFIED:
      frameModified(event.modifedArgs);
        break;
    case FrameEvent::FRAME_REMOVED:
      frameRemoved(event.removedArgs);
        break;
    default:
        assert(false);//you forgot to add an event type
    }
}



