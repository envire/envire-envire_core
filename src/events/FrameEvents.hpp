#pragma once

#include <envire_core/items/Frame.hpp>
#include "GraphEvent.hpp"

namespace envire { namespace core
{
    class FrameEvent : public GraphEvent
    {
    public:
        FrameEvent() = delete;

        virtual bool mergeable(const GraphEvent& event)
        {
            if(type == FRAME_ADDED && event.getType() == FRAME_REMOVED)
            {
                const FrameEvent& frame_event = dynamic_cast<const FrameEvent&>(event);
                if(frame == frame_event.frame)
                    return true;
            }
            return false;
        }

        FrameId frame;

    protected:
        explicit FrameEvent(const Type type, const FrameId& addedFrame) :
                    GraphEvent(type), frame(addedFrame){}
    };

    class FrameAddedEvent : public FrameEvent
    {
    public:
      explicit FrameAddedEvent(const FrameId& addedFrame) :
        FrameEvent(GraphEvent::FRAME_ADDED, addedFrame) {}

        GraphEvent* clone() const
        {
            return new FrameAddedEvent(frame);
        }
    };

    class FrameRemovedEvent : public FrameEvent
    {
    public:
      explicit FrameRemovedEvent(const FrameId& removedFrame) :
        FrameEvent(GraphEvent::FRAME_REMOVED, removedFrame) {}

        GraphEvent* clone() const
        {
            return new FrameRemovedEvent(frame);
        }
    };
}}
