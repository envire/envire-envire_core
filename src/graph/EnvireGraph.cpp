#include "EnvireGraph.hpp"

namespace envire { namespace core {

  
void EnvireGraph::addItem(ItemBase::Ptr item)
{
    addItemToFrame(item->getFrame(), item);
}

void EnvireGraph::addItemToFrame(const FrameId& frame, ItemBase::Ptr item)
{
    checkFrameValid(frame);
    const std::type_index i(item->getTypeIndex());
    (*this)[frame].items[i].push_back(item);
    item->setFrame(frame);
    notify(ItemAddedEvent(frame, item));
}

void EnvireGraph::clearFrame(const FrameId& frame)
{
    checkFrameValid(frame);
    auto& items = (*this)[frame].items;
    
    for(Frame::ItemMap::iterator it = items.begin(); it != items.end();)
    {
        Frame::ItemList& list = it->second;
        for(auto it = list.begin(); it != list.end();)
        {
            ItemBase::Ptr removedItem = *it;
            it = list.erase(it);
            notify(ItemRemovedEvent(frame, removedItem));
        }
        it = items.erase(it);
    }
}

bool EnvireGraph::containsItems(const vertex_descriptor vertex, const std::type_index& type) const
{
  const Frame& frame = graph()[vertex];

  auto mapEntry = frame.items.find(type);
  return mapEntry != frame.items.end();     
}

void EnvireGraph::checkFrameValid(const FrameId& frame) const
{
    if(vertex(frame) == null_vertex())
    {
        throw UnknownFrameException(frame);
    }
}

size_t EnvireGraph::getTotalItemCount(const FrameId& frame) const
{
    return getTotalItemCount(getVertex(frame));
}

size_t EnvireGraph::getTotalItemCount(const vertex_descriptor vd) const
{
    const Frame& frame = graph()[vd];
    return frame.calculateTotalItemCount();
}

const Frame::ItemList& EnvireGraph::getItems(const vertex_descriptor frame,
                                             const std::type_index& type) const
{
    const Frame::ItemMap& items = graph()[frame].items;
    
    if(items.find(type) == items.end())
    {
        throw NoItemsOfTypeInFrameException(getFrameId(frame), demangleTypeName(type));
    }
    return items.at(type);   
}

const Frame::ItemList& EnvireGraph::getItems(const FrameId& frame,
                                             const std::type_index& type) const
{
    const vertex_descriptor frameDesc = getVertex(frame); //may throw
    return getItems(frameDesc, type);
}

void EnvireGraph::removeFrame(const FrameId& frame) 
{
    //explicitly remove all items from the frame to cause ItemRemovedEvents
    clearFrame(frame);
    Base::removeFrame(frame);
}
 

void EnvireGraph::removeItemFromFrame(const ItemBase::Ptr item)
{
    const vertex_descriptor frame = getVertex(item->getFrame()); //may throw UnknownFrameException
    //the const_cast is fine because we are inside the EnvireGraph and know what
    //we are doing. The method returns const because the user should not be
    //able to manipulate the ItemLists directly.
    Frame::ItemList& items = const_cast<Frame::ItemList&>(getItems(frame, item->getTypeIndex()));
    
    Frame::ItemList::iterator itemIt = std::find(items.begin(), items.end(), item);
    if(itemIt == items.end())
    {
        throw UnknownItemException(item->getFrame(), item->getID());
    }
    items.erase(itemIt);   
    item->setFrame("");
    notify(ItemRemovedEvent(item->getFrame(), item));
}


}}
