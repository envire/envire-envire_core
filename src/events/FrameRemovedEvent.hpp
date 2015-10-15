#pragma once
#include <envire_core/items/Frame.hpp>
#include "GraphEvent.hpp"

namespace envire { namespace core
{
    class FrameRemovedEvent : public GraphEvent
    {
    public:
      explicit FrameRemovedEvent(const FrameId& removedFrame) :
        GraphEvent(GraphEvent::FRAME_REMOVED), removedFrame(removedFrame){}
      FrameId removedFrame;
    };
}}
