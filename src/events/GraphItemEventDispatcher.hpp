#pragma once
#include <envire_core/events/GraphEventSubscriber.hpp>
#include <envire_core/events/GraphEventPublisher.hpp>
#include <envire_core/events/ItemAddedEvent.hpp>
#include <envire_core/events/ItemRemovedEvent.hpp>
#include <typeindex>


namespace envire { namespace core
{
     /**
     * A special GraphEventSubscriber that is responsible for handling
     * item events in a type safe way.
     * @param T The type if the item that you care about.
     *          Has to derive from ItemBase
     */
    template <class T>
    class GraphItemEventDispatcher : public GraphEventSubscriber
    {
      
    public:
        GraphItemEventDispatcher(GraphEventPublisher* pPublisher) :
            GraphEventSubscriber(pPublisher), itemType(typeid(T))
        {
            static_assert(std::is_base_of<ItemBase, T>::value,
                          "T should derive from ItemBase"); 
        }
        
        /**Create a dispatcher that is not subscribed to anything, yet*/
        GraphItemEventDispatcher() : GraphEventSubscriber(), itemType(typeid(T)) {}

        virtual ~GraphItemEventDispatcher() {}
        
        void notifyGraphEvent(const GraphEvent& event)
        {
            switch(event.getType())
            {
                case GraphEvent::ITEM_ADDED_TO_FRAME:
                {
                    const ItemAddedEvent& itemEvent = dynamic_cast<const ItemAddedEvent&>(event);
                    if(itemEvent.item->getTypeIndex() == itemType)
                    {
                        itemAdded(TypedItemAddedEvent<T>(itemEvent.frame, boost::dynamic_pointer_cast<T>(itemEvent.item)));
                    }
                }
                    break;
                case GraphEvent::ITEM_REMOVED_FROM_FRAME:  
                {
                    const ItemRemovedEvent itemEvent =  dynamic_cast<const ItemRemovedEvent&>(event);
                    if(itemEvent.item->getTypeIndex() == itemType)
                    {
                        itemRemoved(TypedItemRemovedEvent<T>(itemEvent.frame, boost::dynamic_pointer_cast<T>(itemEvent.item)));
                    }
                }
                    break;
                default:
                  //don't care about anything else
                  break;
            }
        }
        
    protected:
        virtual void itemAdded(const TypedItemAddedEvent<T>& event) {}
        virtual void itemRemoved(const TypedItemRemovedEvent<T>& event) {}
        
    private:
        std::type_index itemType;
    };
}}
