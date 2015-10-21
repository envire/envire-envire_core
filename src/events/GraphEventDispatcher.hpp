#pragma once
#include "GraphEventSubscriber.hpp"


namespace envire { namespace core
{
    class FrameAddedEvent;
    class VertexAddedEvent;
    class VertexRemovedEvent;
    class TransformAddedEvent;
    class TransformRemovedEvent;
    class TransformModifiedEvent;
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
        GraphEventDispatcher(GraphEventPublisher& publisher);
        virtual ~GraphEventDispatcher() {}
        virtual void notifyTreeEvent(const GraphEvent& event);

    protected:
        virtual void transformAdded(const TransformAddedEvent& e) {}
        virtual void transformRemoved(const TransformRemovedEvent& e) {}
        virtual void transformModified(const TransformModifiedEvent& e) {}
        virtual void frameAdded(const FrameAddedEvent& e) {}
        virtual void frameRemoved(const FrameRemovedEvent& e) {}
        virtual void itemAdded(const ItemAddedEvent& e) {}
        virtual void itemRemoved(const ItemRemovedEvent& e) {}
    };
}}
