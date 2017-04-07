//
// Copyright (c) 2015, Deutsches Forschungszentrum für Künstliche Intelligenz GmbH.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#include <envire_core/events/GraphEventDispatcher.hpp>
#include <envire_core/events/GraphEvent.hpp>
#include <envire_core/events/EdgeEvents.hpp>
#include <envire_core/events/ItemAddedEvent.hpp>
#include <envire_core/events/ItemRemovedEvent.hpp>
#include <envire_core/events/FrameEvents.hpp>

#include <cassert>

using namespace envire::core;

GraphEventDispatcher::GraphEventDispatcher(GraphEventPublisher* pPublisher): GraphEventSubscriber(pPublisher)
{}

GraphEventDispatcher::GraphEventDispatcher()
{}

void GraphEventDispatcher::notifyGraphEvent(const GraphEvent& event)
{
    switch(event.getType())
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



