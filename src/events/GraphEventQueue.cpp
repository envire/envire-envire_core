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

#include <envire_core/events/GraphEventQueue.hpp>
#include <typeinfo>
#include <iostream>

envire::core::GraphEventQueue::GraphEventQueue() : GraphEventSubscriber()
{
}

envire::core::GraphEventQueue::GraphEventQueue(envire::core::GraphEventPublisher* pPublisher) :
                                GraphEventSubscriber(pPublisher)
{
}

envire::core::GraphEventQueue::~GraphEventQueue()
{
}

void envire::core::GraphEventQueue::notifyGraphEvent(const envire::core::GraphEvent& event)
{
    std::list<GraphEvent*>::iterator it = event_queue.begin();
    bool skip_event = false;
    while(it != event_queue.end())
    {
        // check if the new event supersedes one of the existing events
        if((*it)->mergeable(event))
        {
            // in this case the remove event deosn't need to be published
            if(((*it)->getType() == GraphEvent::EDGE_ADDED && event.getType() == GraphEvent::EDGE_REMOVED) ||
                ((*it)->getType() == GraphEvent::FRAME_ADDED && event.getType() == GraphEvent::FRAME_REMOVED) ||
                ((*it)->getType() == GraphEvent::ITEM_ADDED_TO_FRAME && event.getType() == GraphEvent::ITEM_REMOVED_FROM_FRAME)
            )
            {
                skip_event = true;
            }

            it = event_queue.erase(it);
        }
        else
        {
            it++;
        }
    }

    if(!skip_event)
    {
        event_queue.push_back(event.clone());
    }
}

void envire::core::GraphEventQueue::flush()
{
    std::list<GraphEvent*>::iterator it = event_queue.begin();
    while(it != event_queue.end())
    {
        envire::core::GraphEvent* event = *it;
        process(*event);

        // delete event
        it = event_queue.erase(it);
        delete event;
    }
}