#pragma once

#include <envire_core/events/GraphEventSubscriber.hpp>
#include <envire_core/events/GraphEvent.hpp>
#include <list>

namespace envire { namespace core
{

class GraphEventQueue : public GraphEventSubscriber
{
public:
    GraphEventQueue();
    GraphEventQueue(GraphEventPublisher* pPublisher);
    virtual ~GraphEventQueue();

    /**This method is called by the publisher whenever a new event occurs */
    virtual void notifyGraphEvent(const GraphEvent& event);

    /** Send all events currently stored in the queue to process */
    void flush();

    /** This callback is called with each queued event when flush() is called */
    virtual void process( const GraphEvent& event ) = 0;

private:
    std::list<GraphEvent*> event_queue;
};

}}