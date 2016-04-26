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