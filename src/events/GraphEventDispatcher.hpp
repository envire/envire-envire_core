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
#include <envire_core/events/GraphEventSubscriber.hpp>


namespace envire { namespace core
{
    class FrameAddedEvent;
    class EdgeAddedEvent;
    class EdgeRemovedEvent;
    class EdgeModifiedEvent;
    class ItemRemovedEvent;
    class FrameAddedEvent;
    class FrameRemovedEvent;
    class ItemAddedEvent;
    class GraphEventPublisher;

    /**
     * A GraphEventSubscriber that parses the event and calls different methods
     * based on the event's content.
     * The GraphEventDispatcher can be used for all events, however for the
     * ItemAdded and ItemRemoved events additional type casting is required.
     * You can use the GraphItemEventDispatcher to avoid the type casting.
     * 
     * You should override the methods, that you are interested in.
     */
    class GraphEventDispatcher : public GraphEventSubscriber
    {
    public:
        GraphEventDispatcher(GraphEventPublisher* pPublisher);
        
        /**creates a dispatcher that is not subscribed to anything */
        GraphEventDispatcher();
        virtual ~GraphEventDispatcher() {}
        virtual void notifyGraphEvent(const GraphEvent& event);

    protected:
        virtual void edgeAdded(const EdgeAddedEvent& e) {}
        virtual void edgeRemoved(const EdgeRemovedEvent& e) {}
        virtual void edgeModified(const EdgeModifiedEvent& e) {}
        virtual void frameAdded(const FrameAddedEvent& e) {}
        virtual void frameRemoved(const FrameRemovedEvent& e) {}
        virtual void itemAdded(const ItemAddedEvent& e) {}
        virtual void itemRemoved(const ItemRemovedEvent& e) {}
    };
}}
