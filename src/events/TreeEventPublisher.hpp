/*
 * FrameEventPublisher.hpp
 *
 *  Created on: Sep 16, 2015
 *      Author: aboeckmann
 */

#pragma once
#include <vector>
#include <cassert>
#include "TreeEvent.hpp"
#include "TreeEventSubscriber.hpp"

namespace envire { namespace core
{
    class TreeEvent;

    /**
     * Base class for frame-event publishers.
     * Handles the subscription and notification of subscribers.
     */
    class TreeEventPublisher
    {
    private:
      std::vector<TreeEventSubscriber*> subscribers;
      bool eventsEnabled = true;/**<If false notify() has no effect */

    public:
        /**Subscribes the @param handler to all events by this event source */
        void subscribe(TreeEventSubscriber* subscriber);
        void unsubscribe(TreeEventSubscriber* subscriber);

    protected:
        /**Notify all subscribers about a certain frame event */
        void notify(const TreeEvent& e) const;

        /**Disables all events. I.e. notify() will have no effect*/
        void disableEvents();
        void enableEvents();

        //there is no use in creating an instance of the publisher
        //on its own.
        TreeEventPublisher() {}
        ~TreeEventPublisher() {}

    };
}}

