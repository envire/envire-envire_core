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
    class ItemAddedEvent;
    class ItemRemovedEvent;
    class FrameAddedEvent;
    class FrameRemovedEvent;

    /**
     * A TreeEventSubscriber that parses the event and calls different methods
     * based on the event's content.
     *
     * You should override the methods, that you are interested in.
     */
    class GraphEventDispatcher : public GraphEventSubscriber
    {
    public:
        virtual ~GraphEventDispatcher() {}
        virtual void notifyTreeEvent(const GraphEvent& event);

    protected:
        virtual void transformAdded(const TransformAddedEvent& e) {}
        virtual void transformRemoved(const TransformRemovedEvent& e) {}
        virtual void transformModified(const TransformModifiedEvent& e) {}
        virtual void itemAdded(const ItemAddedEvent& e) {}
        virtual void itemRemoved(const ItemRemovedEvent& e) {}
        virtual void frameAdded(const FrameAddedEvent& e) {}
        virtual void frameRemoved(const FrameRemovedEvent& e) {}
    };
}}
