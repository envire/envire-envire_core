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

#pragma once
namespace envire { namespace core
{
    class GraphEvent;
    class GraphEventPublisher;
    
    /**
     * Base class for classes that want to subscribe to TransformGraph events.
     * I.e. events that inform about state changes of the TransformGraph.
     * 
     * Automatically unsubscribes on destruction if subscribed. Can only subscribe
     * to one graph.
     */
    class GraphEventSubscriber
    {
    public:
        /**create a new subscriber that automatically subscribes to the publisher */
        GraphEventSubscriber(GraphEventPublisher* pPublisher);
        /**Creates a subscriber that is not subscribed to any publisher. */
        GraphEventSubscriber();
        /**Subscribe to the specified publisher. Only works if not subscribed already.*/
        void subscribe(GraphEventPublisher* pPublisher, bool publish_current_state = false);
        /**unsubscribe from the current publisher. Does nothing if not subscribed. */
        virtual void unsubscribe();
        /**This method is called by the publisher whenever a new event occurs */
        virtual void notifyGraphEvent(const GraphEvent& event) = 0;
        virtual ~GraphEventSubscriber();
    private:
      GraphEventPublisher* pPublisher;
    };
}}
