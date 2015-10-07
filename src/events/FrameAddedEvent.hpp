#pragma once
#include <envire_core/items/Frame.hpp>
#include "GraphEvent.hpp"

namespace envire { namespace core
{
    class FrameAddedEvent : public GraphEvent
    {
    public:
      FrameAddedEvent(const FrameId& addedFrame) :
        GraphEvent(GraphEvent::FRAME_ADDED), addedFrame(addedFrame){}
      FrameId addedFrame;
    };
}}
