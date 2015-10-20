#pragma once
#include "GraphEventSubscriber.hpp"
#include "GraphEventPublisher.hpp"
#include "ItemAddedEvent.hpp"
#include <envire_core/util/MetaProgramming.hpp>
#include <typeindex>


namespace envire { namespace core
{
     /**
     * A special GraphEventSubscriber that is responsible for handling
     * item events in a type safe way.
     * @param T The type if the item that you care about.
     *          Has to be of type ItemBase::PtrType<X> where X derives ItemBase
     */
    template <class T>
    class GraphItemEventDispatcher : public GraphEventSubscriber
    {
    public:
        GraphItemEventDispatcher(GraphEventPublisher& publisher) :
            GraphEventSubscriber(publisher), itemType(typeid(ItemType)){}

        virtual ~GraphItemEventDispatcher() {}
        
        void notifyTreeEvent(const GraphEvent& event)
        {
            switch(event.type)
            {
                case GraphEvent::ITEM_ADDED_TO_FRAME:
                {
                    const ItemAddedEvent& itemEvent = dynamic_cast<const ItemAddedEvent&>(event);
                    if(itemEvent.itemType == itemType)
                    {
                        using Type = typename extract_value_type<T>::value_type;
                        itemAdded(TypedItemAddedEvent<T>(itemEvent.frame, boost::dynamic_pointer_cast<Type>(itemEvent.item)));
                    }
                }
                    break;
                case GraphEvent::ITEM_REMOVED_FROM_FRAME:  
                    //TODO
                    break;
                default:
                  //don't care about anything else
                  break;
            }
        }
        
    protected:
        virtual void itemAdded(const TypedItemAddedEvent<T>& event) {}
        //TODO itemRemoved
        
    private:
        using ItemType = T;
        std::type_index itemType;
    };
}}
