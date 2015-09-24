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

    /**
     * A TreeEventSubscriber that parses the event and calls different methods
     * based on the event's content.
     *
     * You should override the methods, that you are interested in.
     */
    class GraphEventDispatcher : public GraphEventSubscriber
    {
    public:
        virtual void notifyTreeEvent(const GraphEvent& event);

    protected:
        virtual void frameAdded(const FrameAddedEvent& e){}
       // virtual void frameModified(const FrameModifedEventArgs& args){}
      //  virtual void frameRemoved(const FrameRemovedEventArgs& args){}
        virtual void transformAdded(const TransformAddedEvent& e) {}
        virtual void transformRemoved(const TransformRemovedEvent& e) {}
        virtual void transformModified(const TransformModifiedEvent& e) {}
    };
}}
