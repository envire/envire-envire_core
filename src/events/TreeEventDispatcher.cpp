#include "TreeEventDispatcher.hpp"
#include "FrameAddedEvent.hpp"
#include "FrameRootAddedEvent.hpp"
#include "TreeEvent.hpp"

#include <cassert>

using namespace envire::core;

void TreeEventDispatcher::notifyTreeEvent(const TreeEvent& event)
{
    switch(event.type)
    {
    case TreeEvent::FRAME_ADDED:
        frameAdded(dynamic_cast<const FrameAddedEvent&>(event));
        break;
    case TreeEvent::ROOT_FRAME_ADDED:
        frameRootAdded(dynamic_cast<const FrameRootAddedEvent&>(event));
        break;
    case TreeEvent::FRAME_MODIFIED:
      assert(false);
        break;
    case TreeEvent::FRAME_REMOVED:
      assert(false);
        break;
    default:
        assert(false);//you forgot to add an event type
    }
}



