#include "GraphEventDispatcher.hpp"
#include "GraphEvent.hpp"
#include "EdgeAddedEvent.hpp"
#include "EdgeModifiedEvent.hpp"
#include "EdgeRemovedEvent.hpp"
#include "ItemAddedEvent.hpp"
#include "ItemRemovedEvent.hpp"
#include "FrameAddedEvent.hpp"
#include "FrameRemovedEvent.hpp"

#include <cassert>

using namespace envire::core;

GraphEventDispatcher::GraphEventDispatcher(GraphEventPublisher* pPublisher): GraphEventSubscriber(pPublisher)
{}

GraphEventDispatcher::GraphEventDispatcher()
{}

void GraphEventDispatcher::notifyGraphEvent(const GraphEvent& event)
{
    switch(event.type)
    {
    case GraphEvent::EDGE_ADDED:
        edgeAdded(dynamic_cast<const EdgeAddedEvent&>(event));
        break;
    case GraphEvent::EDGE_MODIFIED:
        edgeModified(dynamic_cast<const EdgeModifiedEvent&>(event));
        break;
    case GraphEvent::EDGE_REMOVED:
        edgeRemoved(dynamic_cast<const EdgeRemovedEvent&>(event));
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



