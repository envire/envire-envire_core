#pragma once
#include <envire_core/graph/TransformGraphTypes.hpp>
#include <envire_core/items/Transform.hpp>
#include <envire_core/items/Frame.hpp>
#include "GraphEvent.hpp"

namespace envire { namespace core
{
    class ItemRemovedEvent : public GraphEvent
    {
    public:
      ItemRemovedEvent(const FrameId& frame, const ItemBase::Ptr item) :
        GraphEvent(GraphEvent::ITEM_REMOVED_FROM_FRAME), frame(frame), item(item){}

      FrameId frame;/**<frame that the no longer contains the item.*/
      ItemBase::Ptr item; /**<The item that has been removed*/
    };
}}
