#pragma once
#include <envire_core/graph/TransformGraphTypes.hpp>
#include <envire_core/items/Transform.hpp>
#include <envire_core/items/Frame.hpp>
#include "GraphEvent.hpp"

namespace envire { namespace core
{
    class ItemAddedEvent : public GraphEvent
    {
    public:
      ItemAddedEvent(const FrameId& frame, const ItemBase::Ptr item) :
        GraphEvent(GraphEvent::ITEM_ADDED_TO_FRAME), frame(frame), item(item){}

      FrameId frame;/**<frame that the item has been added to.*/
      ItemBase::Ptr item; /**<The item */
    };
}}
