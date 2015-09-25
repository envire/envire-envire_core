#include "GraphEventDispatcher.hpp"
#include "GraphEvent.hpp"
#include "TransformAddedEvent.hpp"
#include "TransformModifiedEvent.hpp"
#include "TransformRemovedEvent.hpp"
#include "ItemAddedEvent.hpp"

#include <cassert>

using namespace envire::core;

void GraphEventDispatcher::notifyTreeEvent(const GraphEvent& event)
{
    switch(event.type)
    {
    case GraphEvent::TRANSFORMATION_ADDED:
        transformAdded(dynamic_cast<const TransformAddedEvent&>(event));
        break;
    case GraphEvent::TRANSFORMATION_MODIFIED:
        transformModified(dynamic_cast<const TransformModifiedEvent&>(event));
        break;
    case GraphEvent::TRANSFORMATION_REMOVED:
        transformRemoved(dynamic_cast<const TransformRemovedEvent&>(event));
        break;
    case GraphEvent::ITEM_ADDED_TO_FRAME:
        itemAdded(dynamic_cast<const ItemAddedEvent&>(event));
        break;
    default:
        assert(false);//you forgot to add an event type
    }
}



