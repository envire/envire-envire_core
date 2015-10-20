/*
 * FrameEventPublisher.hpp
 *
 *  Created on: Sep 16, 2015
 *      Author: aboeckmann
 */

#pragma once
#include <vector>
#include <cassert>
#include <memory>

#include "GraphEvent.hpp"
#include "GraphEventSubscriber.hpp"

namespace envire { namespace core
{
    class GraphEvent;

    /**
     * Base class for frame-event publishers.
     * Handles the subscription and notification of subscribers.
     */
    class GraphEventPublisher
    {
    private:
      std::vector<GraphEventSubscriber*> subscribers;
      bool eventsEnabled = true;/**<If false notify() has no effect */

    public:
        /**Subscribes the @param handler to all events by this event source */
        void subscribe(GraphEventSubscriber* pSubscriber);
        void unsubscribe(GraphEventSubscriber* pSubscriber);

    protected:
        /**Notify all subscribers about a certain frame event */
        void notify(const GraphEvent& e);

        //there is no use in creating an instance of the publisher
        //on its own.
        GraphEventPublisher() {}
        ~GraphEventPublisher() {}

    };
}}

