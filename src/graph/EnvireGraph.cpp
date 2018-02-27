//
// Copyright (c) 2015, Deutsches Forschungszentrum für Künstliche Intelligenz GmbH.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//


#include <envire_core/graph/EnvireGraph.hpp>
#include <fstream>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

namespace envire { namespace core {

EnvireGraph::EnvireGraph()
    : TransformGraph<Frame>()
{}

EnvireGraph::EnvireGraph(const EnvireGraph &other) : TransformGraph<Frame>()
{
  //NOTE: we are explicitly avoiding calling any copy constructor because boost
  //      graphs are not deep copied by default. To achieve deep copies
  //      we should use boost::copy_graph but copy_graph does not work for
  //      boost_labeled graph because labeled_graph does not define an
  //      add_vertex(G) method.
  //
  //      Therefore, we use copy_graph to copy the graph structure and add the
  //      labels manually
  
    //copy structure from other into the base directed_graph
    boost::copy_graph(other, graph());
    //copy the labels
    regenerateLabelMap();  
}


EnvireGraph::EnvireGraph(const EnvireGraph &other, std::unordered_set<std::type_index> *filter_list, bool inclusive)
    : TransformGraph<Frame>()
{
  //NOTE: we are explicitly avoiding calling any copy constructor because boost
  //      graphs are not deep copied by default. To achieve deep copies
  //      we should use boost::copy_graph but copy_graph does not work for
  //      boost_labeled graph because labeled_graph does not define an
  //      add_vertex(G) method.
  //
  //      Therefore, we use copy_graph to copy the graph structure and add the
  //      labels manually
  
    //copy structure from other into the base directed_graph
    boost::copy_graph(other, graph());
    //copy the labels
    regenerateLabelMap();    

    if (filter_list != NULL) {
        // parse through all vertexes (frames) in graph
        vertex_iterator vertex_it, vertex_end;
        std::tie(vertex_it, vertex_end) = getVertices();
        for(; vertex_it != vertex_end; ++vertex_it)
        {   
            // vertex_iterator->vertex_descriptor
            Frame::ItemMap& items = graph()[*vertex_it].items;

            // parse through all items in vertex (frame)
            // We erase the elements of the map inside of loop
            Frame::ItemMap::const_iterator item_it;
            for(item_it = items.begin(); item_it != items.end();)
            {
                const bool contains = filter_list->find(item_it->first) != filter_list->end();
                
                // white list, preserve the items of types from filter list
                // delete that are not found
                if (inclusive)
                {
                    if(!contains)
                       item_it = items.erase(item_it);
                    else
                        ++item_it;
                } 
                // black list, delete all objects of the types in the filter list
                else
                {
                    if(contains)
                       item_it = items.erase(item_it);
                    else
                        ++item_it;
                }
            }
        }
    }
}   

  
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

bool EnvireGraph::containsItems(const FrameId& frame, const std::type_index& type) const
{
  const vertex_descriptor vertex = getVertex(frame);
  return containsItems(vertex, type);
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

std::vector<std::type_index> EnvireGraph::getItemTypes(const FrameId& frame) const
{
    return getItemTypes(getVertex(frame));

}

std::vector<std::type_index> EnvireGraph::getItemTypes(const vertex_descriptor vd) const
{
    const Frame& frame = graph()[vd];
    return frame.getItemTypes();
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
    boost::archive::binary_oarchive oa(myfile);
    oa << *this; //may throw archive_exception
    myfile.close();
}

void EnvireGraph::loadFromFile(const std::string& file)
{
    std::ifstream myfile;
    myfile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    myfile.open(file); //may throw  
    boost::archive::binary_iarchive ia(myfile);
    ia >> *this;
    myfile.close();
}

void EnvireGraph::createStructuralCopy(EnvireGraph& destination) const
{
    //note: this is not very efficient but until someone complains there is 
    //      no need to do some fancy stuff just to make it fast :-)
    vertex_iterator it, end;
    std::tie(it, end) = getVertices();
    for(; it != end; ++ it)
    {
        const FrameId id = getFrameId(*it);
        destination.addFrame(id);
    }
    
    edge_iterator edgeIt, edgeEnd;
    std::tie(edgeIt, edgeEnd) = getEdges();
    for(; edgeIt != edgeEnd; ++ edgeIt)
    {
        const vertex_descriptor src = getSourceVertex(*edgeIt);
        const vertex_descriptor tar = getTargetVertex(*edgeIt);
        const FrameId sourceId = getFrameId(src);
        const FrameId targetId = getFrameId(tar);
        const Transform tf(getTransform(src, tar));
        try 
        {
            destination.addTransform(sourceId, targetId, tf);
        }
        catch(EdgeAlreadyExistsException&)
        {
            //happens because addTransform internally already adds the 
            //edge in the opposite direction. Can be ignored.
        }
    }
}

}}
