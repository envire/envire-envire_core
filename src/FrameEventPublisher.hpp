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

namespace envire { namespace core
{
    class FrameEventSubscriber;
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
        /**Send a frame-added event to all subscribers */
        void frameAdded(const FrameAddedEventArgs& args) const;
        /**Send a frame-modifed event to all subscribers */
        void frameModified(const FrameModifedEventArgs& args) const;
        /**Send a frame-removed event to all subscribers */
        void frameRemoved(const FrameRemovedEventArgs& args) const;

        //there is no use in creating an instance of the publisher
        //on its own.
        FrameEventPublisher() {}
        ~FrameEventPublisher() {}

    private:
        /**Sends the specified event to all subscribers */
        void notify(const FrameEvent& event) const;
    };
}}

#endif
