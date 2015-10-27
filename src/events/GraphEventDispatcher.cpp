#include "GraphEventDispatcher.hpp"
#include "GraphEvent.hpp"
#include "TransformAddedEvent.hpp"
#include "TransformModifiedEvent.hpp"
#include "TransformRemovedEvent.hpp"
#include "ItemAddedEvent.hpp"
#include "ItemRemovedEvent.hpp"
#include "FrameAddedEvent.hpp"
#include "FrameRemovedEvent.hpp"

#include <cassert>

using namespace envire::core;

GraphEventDispatcher::GraphEventDispatcher(GraphEventPublisher* pPublisher): GraphEventSubscriber(pPublisher)
{}


void GraphEventDispatcher::notifyGraphEvent(const GraphEvent& event)
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
    case GraphEvent::FRAME_ADDED:
        frameAdded(dynamic_cast<const FrameAddedEvent&>(event));
        break;
    case GraphEvent::FRAME_REMOVED:
        frameRemoved(dynamic_cast<const FrameRemovedEvent&>(event));
        break;
    case GraphEvent::ITEM_ADDED_TO_FRAME:
        itemAdded(dynamic_cast<const ItemAddedEvent&>(event));
        break;
    case GraphEvent::ITEM_REMOVED_FROM_FRAME:
        itemRemoved(dynamic_cast<const ItemRemovedEvent&>(event));
        break;
    default:
      break;
    //no default case because we only handle basic event types here. Item events are handled
    //by a different class
    }
}



