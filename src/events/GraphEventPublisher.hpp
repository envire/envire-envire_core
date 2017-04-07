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

/*
 * GraphEventPublisher.hpp
 *
 *  Created on: Sep 16, 2015
 *      Author: aboeckmann
 */

#pragma once
#include <vector>
#include <envire_core/events/GraphEvent.hpp>

namespace envire { namespace core
{
    class GraphEvent;
    class GraphEventSubscriber;

    /**
     * Base class for frame-event publishers.
     * Handles the subscription and notification of subscribers.
     * If the publisher is destroyed. All remaining subscribers will be unsubscribed.
     */
    class GraphEventPublisher
    {
    private:
      std::vector<GraphEventSubscriber*> subscribers;
      
      /**Is true while notify() is called. Is used to detect if the subscriber
       * list is modified while inside a notify() call.*/
      bool insideNotify;
      /**Temporarly stores subscribers that subscribe while inside notify().
       * They will be moved to the subcribers list once notify() has finished*/
      std::vector<GraphEventSubscriber*> toBeSubscribed;
      std::vector<GraphEventSubscriber*> toBeUnsubscribed;

    public:
        /**Subscribes the @param handler to all events by this event source */
        void subscribe(GraphEventSubscriber* pSubscriber, bool publish_current_state = false);
        void unsubscribe(GraphEventSubscriber* pSubscriber, bool unpublish_current_state = false);

    protected:
        /**Notify all subscribers about a certain graph event */
        void notify(const GraphEvent& e);

        /**Notify the given subscriber about a certain graph event */
        void notifySubscriber(GraphEventSubscriber* pSubscriber, const GraphEvent& e);

        /**
         * @brief Publishes the current state of the graph.
         */
        virtual void publishCurrentState(GraphEventSubscriber* pSubscriber) = 0;

        /**
         * @brief Unpublishes the current state of the graph.
         *        Basically the reverse process of publishCurrentState
         */
        virtual void unpublishCurrentState(GraphEventSubscriber* pSubscriber) = 0;
        
        void unsubscribeInternal(GraphEventSubscriber* pSubscriber);

        //there is no use in creating an instance of the publisher
        //on its own.
        GraphEventPublisher();
        
        virtual ~GraphEventPublisher();

    };
}}

