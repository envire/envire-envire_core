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

#pragma once

#include <envire_core/graph/TransformGraph.hpp>
#include <envire_core/items/Frame.hpp>
#include <envire_core/events/ItemAddedEvent.hpp>
#include <envire_core/events/ItemRemovedEvent.hpp>
#include <envire_core/util/Demangle.hpp>

#include <typeindex>
#include <typeinfo>
#include <type_traits>
#include <unordered_set>

#define BOOST_RESULT_OF_USE_DECLTYPE //this is important for the transform_iterator
#include <boost/iterator/transform_iterator.hpp>
#include <boost_serialization/BoostTypes.hpp>
#include <boost/lexical_cast.hpp>


namespace envire { namespace core {

//FIXME comment
class EnvireGraph : public TransformGraph<Frame>
{
public:
    using FRAME_PROP = Frame;
    using Base = TransformGraph<Frame>;
  
    /**Iterator used to down cast from ItemBase::Ptr to @p T::Ptr while
    * iterating. T has to derive from ItemBase for this to work.
    * The iterator returns a T&.
    * Example:
    * @code
    *    EnvireGraph g;
    *    FrameId id("bla");
    *    ItemIterator<Item<Eigen::Vector3d>> it = g.getItems<Item<Eigen::Vector3d>>(id);
    *    Item<Eigen::Vector3d>& item = *it;
    * @endcode
    */
    template <class T>
    using ItemIterator = boost::transform_iterator<ItemBaseCaster<T>, std::vector<ItemBase::Ptr>::const_iterator, T&>;
    template <class T>
    using ItemIteratorPair =  std::pair<ItemIterator<T>, ItemIterator<T>>;

    EnvireGraph();

    /**
    * @param inclusive defines if the types set in filter_list should be preserved in the resulted graph,
    * if true, the filter_list is white list, if false, the filter_list is black list, and the items of types will
    * be excluded from the resulted graph
    *
    */
    EnvireGraph(const EnvireGraph &other, 
                std::unordered_set<std::type_index> *filter_list, bool inclusive);

    EnvireGraph(const EnvireGraph &other);
    

    /** Adds @p item to the item list in the frame of item
    *  Causes ItemAddedEvent.
    *  @throw UnknownFrameException if the frame id is invalid
    *  @param item that should be added. The item provides the frame to which it will be added */
    void addItem(ItemBase::Ptr item);

    /**Removes @p item from @p frame.
      *  Causes ItemRemovedEvent.
      *  Sets @p item->frame_name to "" before causing the event.
      * @throw UnknownFrameException if the frame does not exist.
      * @throw UnknownItemException if the item is not part of the frame's
      *                             item list.
      * @param T should derive from ItemBase.
      * @note Invalidates all iterators of type ItemIterator<T> for the specified @p frame.
      * @return A pair of iterators. The first one points to the element that
      *         comes after @p item and the second one points to the end of the
      *         list.*/ 
    template <class T>
    ItemIteratorPair<T>
    removeItemFromFrame(const FrameId& frameId, ItemIterator<T> item);
    
    /**Removes @p item from its frame.
     * Causes ItemRemovedEvent. 
     * Sets @p item->frame_name to "" before causing the event.
     * @throw UnknownFrameException if @p item.frame is not part of this graph.
     * @throw UnknownItemException if @p item is not part of @p item.frame
     * @note Invalidates all iterators of type ItemIterator<item.getTypeIndex()>*/
    void removeItemFromFrame(const ItemBase::Ptr item);
          
    /**Removes all items from @p frame.
    * Causes ItemRemovedEvent for each item that is removd.
    * @throw UnknownFrameException if the frame does not exist.*/
    void clearFrame(const FrameId& frame);

    /** Adds @p item to the item list of the specified frame 
    *  Causes ItemAddedEvent.
    *  @throw UnknownFrameException if the frame id is invalid
    *  @param frame The frame the item should be added to.
    *  @param item The item that should be added to the frame.
    *  @note item.frame_name will be set to @p frame.
    *        This means that it is not possible to use the same item
    *        in multiple Graphs. */
    void addItemToFrame(const FrameId& frame, ItemBase::Ptr item);

    /**Returns all items of type @p T that are stored in @p frame.
    * @throw UnknownFrameException if the @p frame id is invalid.
    * @param T has to derive from ItemBase.
    * @return a pair iterators [begin, end]. If no items of type @p T
    *         exist, both iterators are equal and invalid */
    template<class T>
    const std::pair<ItemIterator<T>, ItemIterator<T>> getItems(const FrameId& frame) const;
    template<class T>
    const std::pair<ItemIterator<T>, ItemIterator<T>> getItems(const vertex_descriptor frame) const;

    /** @return a list of all items of @p type in @p frame
     *  @throw NoItemsOfTypeInFrameException if no items of the type are in the frame*/
    const envire::core::Frame::ItemList& getItems(const vertex_descriptor frame,
                                                  const std::type_index& type) const;
    /** @throw UnknownFrameException if @p frame is not part of this graph*/                                                  
    const envire::core::Frame::ItemList& getItems(const FrameId& frame,
                                                  const std::type_index& type) const;
                                                  
    /**Visits all items in the given frame (ignoring the type).
     * @param func should be a callable with operator(const ItemBase::Ptr)
     * @throw UnknownFrameException if @p frame is not part of this graph */
    template <class T>
    void visitItems(const FrameId& frame, T func) const;                                                  
                                                  
    /**Convenience method that returns an iterator to the @p i'th item of type @p T from @p frame.
      * @param T has to derive from ItemBase.
      * @throw UnknownFrameException if the @p frame id is invalid.
      * @throw NoItemsOfTypeInFrameException if no items of type @p T exist in the frame.
      * @throw std::out_of_range if @p i is out of range*/
    template <class T>
    const ItemIterator<T> getItem(const FrameId& frame, const int i = 0) const;

    template <class T>
    const ItemIterator<T> getItem(const vertex_descriptor frame, const int i = 0) const;

    /** @return true if the @p frame contains at least one item of type @p T
      *  @param T should derive from ItemBase
      *  @throw UnknownFrameException if the @p frame id is invalid.*/
    template <class T>
    bool containsItems(const FrameId& frame) const;
    template <class T>
    bool containsItems(const vertex_descriptor frame) const;
    /** @return true if @p frame contains at least one item of @p type. 
      *  @param type The described type should derive from ItemBase*/
    bool containsItems(const vertex_descriptor frame, const std::type_index& type) const;
    bool containsItems(const FrameId& frame, const std::type_index& type) const;
    /** @return the number of items of type @p T in @p frame.
      *  @param T should derive from ItemBase
      *  @throw UnknownFrameException if the @p frame id is invalid.*/
    template <class T>
    size_t getItemCount(const FrameId& frame) const;
    template <class T>
    size_t getItemCount(const vertex_descriptor vd) const;        

    /** @return the number of all items independent of their type in @p frame.
      *  @throw UnknownFrameException if the @p frame id is invalid.*/
    size_t getTotalItemCount(const FrameId& frame) const;
    size_t getTotalItemCount(const vertex_descriptor vd) const;
    
    /** @return a list containing the types of all items inside @p frame
      * @throw UnknownFrameException if the @p frame id is invalid.*/
    std::vector<std::type_index> getItemTypes(const FrameId& frame) const;
    std::vector<std::type_index> getItemTypes(const vertex_descriptor vd) const;
    
    
    /**Removes @p frame from the Graph.
    *  A frame can only be removed if there are no edges connected to
    *  or coming from that frame.
    *
    *  Causes FrameRemovedEvent.
    *  Causes ItemRemovedEvent for each item in the frame.
    * 
    * 
    *  @throw UnknownFrameException if the frame does not exist.
    *  @throw FrameStillConnectedException if there are still edges
    *                                      coming from or leading to this
    *                                      frame. */
    virtual void removeFrame(const FrameId& frame) override;
    
    /**Stores the graph in @p file.
     * Boost serialization is used to store the graph.
     * @throw boost::archive::archive_exception if the serialization failed
     * @throw std::ios_base::failure if the file operation failed*/
    void saveToFile(const std::string& file) const;
    
    /**Loads the graph from @p file.
     * Boost serialization is used to load the graph.
     * Only use this with files that have been created by saveToFile().
     * @throw boost::archive::archive_exception if the serialization failed
     * @throw std::ios_base::failure if the file operation failed
     * FIXME I have no idea what happens when the graph already contains data*/
    void loadFromFile(const std::string& file);
    
    /** Copies all frames and edges from this graph to @p target.
     *  Excludes all items. 
     */
    void createStructuralCopy(EnvireGraph& target) const;
    
protected:

    /** @return A range that contains all items of type @p T in frame @p frame
    *  @param T should derive from ItemBase
    **/
    template<class T>
    const ItemIteratorPair<T>
    getItemsInternal(const vertex_descriptor frame, const FrameId& frameId) const;
    

    /**Throws UnknownFrameException if @p frame is not part of this graph */
    void checkFrameValid(const FrameId& frame) const;
    
    /**Assert that @p T derives from ItemBase */
    template <class T>
    void assertDerivesFromItemBase() const;

    /**
     * @brief Publishes the current state of the graph.
     */
    virtual void publishCurrentState(GraphEventSubscriber* pSubscriber);

    /**
     * @brief Unpublishes the current state of the graph.
     *        Basically the reverse process of publishCurrentState
     */
    virtual void unpublishCurrentState(GraphEventSubscriber* pSubscriber);
    
private:
    /**Grants access to boost serialization */
    friend class boost::serialization::access;
    
    /**boost serialization method*/
    template <typename Archive>
    void serialize(Archive &ar, const unsigned int version);
                                                            
};


template <class T>
void EnvireGraph::assertDerivesFromItemBase() const
{
    static_assert(std::is_base_of<ItemBase, T>::value,
        "T should derive from ItemBase"); 
}


template<class T>
const EnvireGraph::ItemIteratorPair<T>
EnvireGraph::getItems(const FrameId& frame) const
{
    assertDerivesFromItemBase<T>();
    const vertex_descriptor desc = vertex(frame);
    if(desc == null_vertex())
    {
        throw UnknownFrameException(frame);
    }

    return getItemsInternal<T>(desc, frame);
}

template <class T>
void EnvireGraph::visitItems(const FrameId& frameId, T func) const
{
  checkFrameValid(frameId);
  const Frame& frame = (*this)[frameId];
  frame.visitItems(func);
}

template<class T>
const EnvireGraph::ItemIteratorPair<T>
EnvireGraph::getItems(const vertex_descriptor frame) const
{
    assertDerivesFromItemBase<T>();
    return getItemsInternal<T>(frame, getFrameId(frame));
}  

template<class T>
const EnvireGraph::ItemIteratorPair<T>
EnvireGraph::getItemsInternal(const vertex_descriptor frame, const FrameId& frameId) const
{
    assertDerivesFromItemBase<T>();
    
    const Frame::ItemMap& items = graph()[frame].items;
    const std::type_index key(typeid(T));
    
    if(items.find(key) == items.end())
    {
        ItemIterator<T> invalid;
        return std::make_pair(invalid, invalid);
    }
    
    auto begin = items.at(key).begin();
    auto end = items.at(key).end();
    assert(begin != end); //if a list exists it should not be empty
    
    ItemIterator<T> beginIt(begin, ItemBaseCaster<T>()); 
    ItemIterator<T> endIt(end, ItemBaseCaster<T>()); 
    return std::make_pair(beginIt, endIt);        
}

template <class T>
const EnvireGraph::ItemIterator<T> EnvireGraph::getItem(const FrameId& frame, const int i) const
{
    assertDerivesFromItemBase<T>();
    const vertex_descriptor vd = getVertex(frame); //may throw
    return getItem<T>(vd, i);

}

template <class T>
const EnvireGraph::ItemIterator<T> EnvireGraph::getItem(const vertex_descriptor frame, const int i) const
{
    assertDerivesFromItemBase<T>();
    
    if(i < 0)
    {
       throw std::out_of_range("Out of range: " + boost::lexical_cast<std::string>(i)); 
    }
    const Frame::ItemMap& items = graph()[frame].items;
    const std::type_index key(typeid(T));
    if(items.find(key) == items.end())
    {
        throw NoItemsOfTypeInFrameException(getFrameId(frame), demangleTypeName(key));
    }
    const Frame::ItemList& list = items.at(key);
    assert(list.size() > 0); //if everything is implemented correctly empty lists can never exist in the map
    if((size_t)i >= list.size()) //i is always >= 0 thus cast to size_t is always safe
    {
      throw std::out_of_range("Out of range");
    }
    return ItemIterator<T>(list.begin() + i, ItemBaseCaster<T>()); 
}

template <class T>
EnvireGraph::ItemIteratorPair<T>
EnvireGraph::removeItemFromFrame(const FrameId& frameId, ItemIterator<T> item)
{
    assertDerivesFromItemBase<T>();
    checkFrameValid(frameId);
    assert(frameId.compare(item->getFrame()) == 0);
    
    Frame& frame = (*this)[frameId];
    const std::type_index key(typeid(T));
    auto mapEntry = frame.items.find(key);
    if(mapEntry == frame.items.end())
    {
        throw NoItemsOfTypeInFrameException(frameId, demangleTypeName(key));
    }
    std::vector<ItemBase::Ptr>& items = mapEntry->second;
    std::vector<ItemBase::Ptr>::const_iterator baseIterator = item.base();
    //HACK This is a workaround for gcc bug 57158.
    //     In C++11 the parameter type of vector::erase changed from iterator
    //     to const_iterator (which is exactly what we need), but  gcc has not
    //     yet implemented that change. 
    std::vector<ItemBase::Ptr>::iterator nonConstBaseIterator = items.begin() + (baseIterator - items.cbegin()); //vector iterator const cast hack
    ItemBase::Ptr deletedItem = *nonConstBaseIterator;//backup item so we can notify the user
    std::vector<ItemBase::Ptr>::const_iterator next = items.erase(nonConstBaseIterator);
    deletedItem->setFrame("");
    notify(ItemRemovedEvent(frameId, deletedItem));
    
    ItemIterator<T> nextIt(next, ItemBaseCaster<T>()); 
    ItemIterator<T> endIt(items.cend(), ItemBaseCaster<T>()); 
    
    //remove the map entry if there are no more values in the vector
    if(nextIt == endIt)
    {
      //erase invalidates the iterators that we are about to return, but
      //that doesnt matter because it only happens when they both point
      //to end() anyway.
      frame.items.erase(key);
    }
    
    return std::make_pair(nextIt, endIt);
}
    
template <class T>
bool EnvireGraph::containsItems(const FrameId& frameId) const
{
    assertDerivesFromItemBase<T>();
    const vertex_descriptor vd = getVertex(frameId); //may throw
    return containsItems<T>(vd);
}

template <class T>
bool EnvireGraph::containsItems(const vertex_descriptor vertex) const
{
    assertDerivesFromItemBase<T>();
    const std::type_index type(typeid(T));
    return containsItems(vertex, type);
}

template <class T>
size_t EnvireGraph::getItemCount(const FrameId& frameId) const
{
    assertDerivesFromItemBase<T>();
    vertex_descriptor vd = getVertex(frameId);
    return getItemCount<T>(vd);
}

template <class T>
size_t EnvireGraph::getItemCount(const vertex_descriptor vd) const
{
    assertDerivesFromItemBase<T>();
    const Frame& frame = graph()[vd];
    const std::type_index key(typeid(T));
    auto mapEntry = frame.items.find(key);
    if(mapEntry == frame.items.end())
    {
        return 0;
    }
    else
    {
        return mapEntry->second.size();
    }
}

template <typename Archive>
void EnvireGraph::serialize(Archive &ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Base);
}

}}
