#pragma once
#include <envire_core/graph/GraphTypes.hpp>
#include <envire_core/items/ItemBase.hpp>
#include "GraphEvent.hpp"

namespace envire { namespace core
{
    class ItemRemovedEvent : public GraphEvent
    {
    public:
      ItemRemovedEvent(const FrameId& frame, const ItemBase::Ptr item, std::type_index itemType) :
          GraphEvent(GraphEvent::ITEM_REMOVED_FROM_FRAME), frame(frame), item(item),
          itemType(itemType){}

      FrameId frame;/**<frame that the no longer contains the item.*/
      ItemBase::Ptr item; /**<The item that has been removed*/
      std::type_index itemType; /**<Actual type of the item, can be used for downcasting */
    };
    
    //a type safe version of the above event
    template <class T>
    struct TypedItemRemovedEvent 
    {
      TypedItemRemovedEvent(const FrameId& frame, const ItemBase::PtrType<T> item) : frame(frame), item(item) {}
      FrameId frame;
      ItemBase::PtrType<T> item;
    };
}}
