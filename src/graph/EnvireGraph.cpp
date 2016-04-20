#include "EnvireGraph.hpp"
#include <fstream>
#include <boost/archive/polymorphic_binary_oarchive.hpp>
#include <boost/archive/polymorphic_binary_iarchive.hpp>

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
    const FrameId frameId = item->getFrame();
    const vertex_descriptor frame = getVertex(frameId); //may throw UnknownFrameException
    //the const_cast is fine because we are inside the EnvireGraph and know what
    //we are doing. The method returns const because the user should not be
    //able to manipulate the ItemLists directly.
    Frame::ItemList& items = const_cast<Frame::ItemList&>(getItems(frame, item->getTypeIndex()));
    
    Frame::ItemList::iterator itemIt = std::find(items.begin(), items.end(), item);
    if(itemIt == items.end())
    {
        throw UnknownItemException(frameId, item->getID());
    }
    items.erase(itemIt);
    
    item->setFrame("");
    notify(ItemRemovedEvent(frameId, item));

}

void EnvireGraph::publishCurrentState(GraphEventSubscriber* pSubscriber)
{
    // publish vertices and edges
    envire::core::Graph< envire::core::Frame, envire::core::Transform >::publishCurrentState(pSubscriber);

    // publish items
    typename EnvireGraph::vertex_iterator vertex_it, vertex_end;
    for (boost::tie( vertex_it, vertex_end ) = boost::vertices( graph() ); vertex_it != vertex_end; ++vertex_it)
    {
        const Frame& frame = graph()[*vertex_it];
        for(Frame::ItemMap::const_iterator item_group = frame.items.begin(); item_group != frame.items.end(); item_group++)
        {
            for(Frame::ItemList::const_iterator item = item_group->second.begin(); item != item_group->second.end(); item++)
            {
                notifySubscriber(pSubscriber, ItemAddedEvent(frame.getId(), *item));
            }
        }
    }
}

void EnvireGraph::unpublishCurrentState(GraphEventSubscriber* pSubscriber)
{
    // unpublish items
    typename EnvireGraph::vertex_iterator vertex_it, vertex_end;
    for (boost::tie( vertex_it, vertex_end ) = boost::vertices( graph() ); vertex_it != vertex_end; ++vertex_it)
    {
        const Frame& frame = graph()[*vertex_it];
        for(Frame::ItemMap::const_iterator item_group = frame.items.begin(); item_group != frame.items.end(); item_group++)
        {
            for(Frame::ItemList::const_iterator item = item_group->second.begin(); item != item_group->second.end(); item++)
            {
                notifySubscriber(pSubscriber, ItemRemovedEvent(frame.getId(), *item));
            }
        }
    }

    // unpublish vertices and edges
    envire::core::Graph< envire::core::Frame, envire::core::Transform >::unpublishCurrentState(pSubscriber);
}

void EnvireGraph::saveToFile(const std::string& file) const
{
    std::ofstream myfile;
    //set exception bits to ensure that myfile throws in case of error
    myfile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    myfile.open(file); //may throw
    boost::archive::polymorphic_binary_oarchive oa(myfile);
    oa << *this; //may throw archive_exception
}

void EnvireGraph::loadFromFile(const std::string& file)
{
  std::ifstream myfile;
  myfile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  myfile.open(file); //may throw  
  boost::archive::polymorphic_binary_iarchive ia(myfile);
  ia >> *this;
}


}}
