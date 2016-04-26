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
