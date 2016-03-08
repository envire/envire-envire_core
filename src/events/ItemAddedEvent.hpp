#pragma once
#include <typeindex>
#include <envire_core/items/ItemBase.hpp>
#include "GraphEvent.hpp"

namespace envire { namespace core
{
    class ItemAddedEvent : public GraphEvent
    {
    public:
      ItemAddedEvent(const FrameId& frame, const ItemBase::Ptr item) :
        GraphEvent(GraphEvent::ITEM_ADDED_TO_FRAME), frame(frame), item(item){}

        GraphEvent* clone() const
        {
            return new ItemAddedEvent(frame, item);
        }

      FrameId frame;/**<frame that the item has been added to.*/
      ItemBase::Ptr item; /**<The item */
    };
    
    //a type safe version of the above event
    template <class T>
    struct TypedItemAddedEvent 
    {
      TypedItemAddedEvent(const FrameId& frame, const ItemBase::PtrType<T> item) : frame(frame), item(item) {}

        GraphEvent* clone() const
        {
            return new ItemAddedEvent(frame, item);
        }

      FrameId frame;
      ItemBase::PtrType<T> item;
    };
}}
