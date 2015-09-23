#include "TreeEventDispatcher.hpp"
#include "TreeEvent.hpp"
#include "TransformAddedEvent.hpp"
#include "TransformModifiedEvent.hpp"
#include "TransformRemovedEvent.hpp"

#include <cassert>

using namespace envire::core;

void TreeEventDispatcher::notifyTreeEvent(const TreeEvent& event)
{
    switch(event.type)
    {
    case TreeEvent::TRANSFORMATION_ADDED:
        transformAdded(dynamic_cast<const TransformAddedEvent&>(event));
        break;
    case TreeEvent::TRANSFORMATION_MODIFIED:
        transformModified(dynamic_cast<const TransformModifiedEvent&>(event));
        break;
    case TreeEvent::TRANSFORMATION_REMOVED:
        transformRemoved(dynamic_cast<const TransformRemovedEvent&>(event));
        break;
    default:
        assert(false);//you forgot to add an event type
    }
}



