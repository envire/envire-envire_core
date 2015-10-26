#pragma once
namespace envire { namespace core
{
    class GraphEvent;
    class GraphEventPublisher;
    
    /**
     * Base class for classes that want to subscribe to TransformTree frame-events.
     * I.e. events that inform about state changes of frames in the TransformTree.
     * Automatically subscribes on creation and unsubscribes on destruction
     */
    class GraphEventSubscriber
    {
    public:
        /**create a new subscriber that automatically subscribes to the publisher */
        GraphEventSubscriber(GraphEventPublisher& publisher);
        /**This method is called by the publisher whenever a new event occurs */
        virtual void notifyGraphEvent(const GraphEvent& event) = 0;
        virtual ~GraphEventSubscriber();
    private:
      GraphEventPublisher& publisher;
    };
}}
