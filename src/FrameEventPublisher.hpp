/*
 * FrameEventPublisher.hpp
 *
 *  Created on: Sep 16, 2015
 *      Author: aboeckmann
 */

#pragma once
#ifndef FRAME_EVENT_PUBLISHER_HPP
#define FRAME_EVENT_PUBLISHER_HPP
#include <vector>
#include "FrameEventArgs.hpp"
#include "FrameEventSubscriber.hpp"
#include "FrameEvent.hpp"
#include <cassert>

namespace envire { namespace core
{
    class FrameEvent;

    /**
     * Base class for frame-event publishers.
     * Handles the subscription and notification of subscribers.
     */
    class FrameEventPublisher
    {
    private:
      std::vector<FrameEventSubscriber*> subscribers;

    public:
        /**Subscribes the @param handler to all events by this event source */
        void subscribe(FrameEventSubscriber* subscriber);
        void unsubscribe(FrameEventSubscriber* subscriber);

    protected:
        /**Notify all subscribers about a certain frame event */
        void notify(const FrameEvent& e) const;

        //there is no use in creating an instance of the publisher
        //on its own.
        FrameEventPublisher() {}
        ~FrameEventPublisher() {}

    };
}}

#endif
