#pragma once
#ifndef __FRAME_EVENT_SUBSCRIBER_HPP__
#define __FRAME_EVENT_SUBSCRIBER_HPP__

namespace envire { namespace core
{
    class TreeEvent;

    /**
     * Base class for classes that want to subscribe to TransformTree frame-events.
     * I.e. events that inform about state changes of frames in the TransformTree.
     */
    class TreeEventSubscriber
    {
    public:
        /**This method is called by the publisher whenever a new event occurs */
        virtual void notifyTreeEvent(const TreeEvent& event) = 0;
        virtual ~TreeEventSubscriber() {}
    };
}}
#endif
