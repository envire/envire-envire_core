#pragma once

namespace envire { namespace core
{
    class GraphEvent;

    /**
     * Base class for classes that want to subscribe to TransformTree frame-events.
     * I.e. events that inform about state changes of frames in the TransformTree.
     */
    class GraphEventSubscriber
    {
    public:
        /**This method is called by the publisher whenever a new event occurs */
        virtual void notifyTreeEvent(const GraphEvent& event) = 0;
        virtual ~GraphEventSubscriber() {}
    };
}}
