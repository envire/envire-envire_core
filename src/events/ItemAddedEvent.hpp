#pragma once
#include <typeindex>
#include <envire_core/graph/TransformGraphTypes.hpp>
#include <envire_core/items/Transform.hpp>
#include <envire_core/items/Frame.hpp>
#include "GraphEvent.hpp"

namespace envire { namespace core
{
    class ItemAddedEvent : public GraphEvent
    {
    public:
      ItemAddedEvent(const FrameId& frame, const ItemBase::Ptr item, std::type_index itemType) :
        GraphEvent(GraphEvent::ITEM_ADDED_TO_FRAME), frame(frame), item(item),
        itemType(itemType){}

      FrameId frame;/**<frame that the item has been added to.*/
      ItemBase::Ptr item; /**<The item */
      std::type_index itemType; /**<Actual type of the item, can be used for downcasting */
    };
    
    //FIXME comments
    template <class T>
    struct TypedItemAddedEvent 
    {
      TypedItemAddedEvent(const FrameId& frame, const T& item) : frame(frame), item(item) {}
      FrameId frame;
      T item;
    };
}}
