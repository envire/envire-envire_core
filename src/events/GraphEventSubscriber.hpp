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
        /**This method is called by the publisher whenever a new event occurs */
        virtual void notifyGraphEvent(const GraphEvent& event) = 0;
        virtual ~GraphEventSubscriber();
    private:
      GraphEventPublisher* pPublisher;
    };
}}
