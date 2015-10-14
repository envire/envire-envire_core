/**\file TransformTree.hpp
 *
 * This class provided stores and manages the transformation tree
 *
 * @author Javier Hidalgo Carrio et. al (see THANKS  for the full Author list)
 * See LICENSE for the license information
 *
 */


#ifndef __ENVIRE_CORE_TRANSFORM_GRAPH__
#define __ENVIRE_CORE_TRANSFORM_GRAPH__


#include <boost/uuid/uuid.hpp>
#include <cassert>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <type_traits>


#include "TransformGraphTypes.hpp"
#include "TransformGraphExceptions.hpp"
#include "TransformGraphVisitors.hpp"
#include <envire_core/events/GraphEventPublisher.hpp>
#define BOOST_RESULT_OF_USE_DECLTYPE
#include <boost/iterator/transform_iterator.hpp>
#include <envire_core/util/MetaProgramming.hpp>


namespace envire { namespace core
{
    /**
     * A tree-like graph structure.
     * Each vertex contains a labeled Frame. The label must be unique.
     *
     * @warning Do **not** manipulate the tree directly using boost functions.
     *          If you do that you will **break** the event system.
     *          Instead use the methods provided by this class for manipulation.
     *          I.e. do not modify transforms, frames or items directly!!!!
     *          
    */
    class TransformGraph : public LabeledTransformGraph, public GraphEventPublisher
    {
    public:
        /**Transform iterator used to down cast from ItemBase to @p T while
         * iterating */
        template<class T>
        using ItemIterator = boost::transform_iterator<ItemBaseCaster<typename T::element_type>, std::vector<ItemBase::Ptr>::const_iterator, T>;
        
        /***************************************************
         * Methods Naming convention
         * Overloading boost methods uses delimited separated
         * words, new methods use Camel Case separated words
         ***************************************************/
    
        TransformGraph(envire::core::Environment const &environment = Environment());
        
        
        /** Adds an unconnected frame to the graph.
         * 
         * @throw FrameAlreadyExistsException if the frame already exists
         */
        void addFrame(const FrameId& frame);

        /**Adds a transform from frame @p origin to frame @p target.
         * If the frames do not exist, they will be created.
         * If the transform already exists, it will **not** be updated.
         *
         * Causes a TransformAddedEvent.
         * Causes FrameAddedEvents if the frames did not exist before. Those events
         * will occur **before** the TransformAddedEvent.
         *
         * The inverse transform will be added automatically.
         *
         * @throw TransformAlreadyExistsException if the transformation already exists.*/
        void addTransform(const FrameId& origin, const FrameId& target,
                          const Transform& tf);

        /**Updates the value of the transform from @p origin to
         * @p target and the inverse transformation according to @p tf.
         * Both frames need to exist beforehand.
         * A direct transformation has to exist between @p orign and @p target
         * for this method to work.
         *
         * Causes a TransformUpdated event.
         *
         * @throw UnknownTransformException if no direct transformation between
         *                                  @p origin and @p target exists.
         * @throw UnknownFrameException if @p origin or @p target do not exist.
         */
        void updateTransform(const FrameId& origin, const FrameId& target,
                             const Transform& tf);
        void updateTransform(const vertex_descriptor origin,
                             const vertex_descriptor target, const Transform& tf);

        /**Removes the transform between @p origin and @p target as well as
         * the transform between @p target and @p origin.
         *
         * Causes TransformRemoved event.
         *
         * @throw UnknownTransformException if the transformation doesn't exist */
        void removeTransform(const FrameId& origin, const FrameId& target);

        /** @return the transform between a and b. Calculating it if necessary.
         * @throw UnknownTransformException if the transformation doesn't exist*/
        const Transform getTransform(const FrameId& origin, const FrameId& target) const;
        const Transform getTransform(const vertex_descriptor origin, const vertex_descriptor target) const;
        

        /** @return the edge between frame @p origin and @p target
         * @throw UnknownTransformException if there is no such edge  */
        edge_descriptor getEdge(const FrameId& origin, const FrameId& target) const;

        /** @return a reference to the frame identified by the id.
         *  @throw UnknownFrameException if the frame id is invalid **/
        const envire::core::Frame& getFrame(const FrameId& frame) const;
        const envire::core::Frame& getFrame(const vertex_descriptor desc) const;
        
        /** @return the frame id of the specified @p vertex */
        const envire::core::FrameId& getFrameId(const vertex_descriptor vertex) const;
                       
        /**Removes @p item from @p frame.
         *  Causes ItemRemovedEvent.
         * @throw UnknownFrameException if the frame does not exist.
         * @throw UnknownItemException if the item is not part of the frame's
         *                             item list.
         * @param T should be ItemBase::PtrType<X> where X is derived from ItemBase
         * @note Invalidates all iterators of type ItemIterator<ItemBase::PtrType<T>> for the
         *       specified @p frame.
         * @return A pair of iterators. The first one points to the element that
         *         comes after @p item and the second one points to the end of the
         *         list.*/ 
        template <class T>
        std::pair<TransformGraph::ItemIterator<T>, TransformGraph::ItemIterator<T>>
        removeItemFromFrame(const FrameId& frameId, ItemIterator<T> item);
        
        /**Searches for @p item in @p frame and removes it if present.
         * Causes ItemRemovedEvent.
         * @throw UnknownFrameException if the @ frame does not exist.
         * @throw UnknownItemException if the @p frame does not contain the @p item.
         * @note Invalidates all iterators of type ItemIterator<ItemBase::PtrType<T>> for the
         *       specified @p frame.
         *       I.e. you cannot iterate over all items and use this method at the same time.
         * */
        template <class T>
        void removeItemFromFrame(const FrameId& frameId, ItemBase::PtrType<T> item);
        
        
        /**Disconnects @p frame from the Graph.
         * I.e. all transformations from and to @p frame will be removed.
         * @throw UnknownFrameException if the frame does not exist. */
        void disconnectFrame(const FrameId& frame);
        
        /**Removes @p frame from the Graph.
         * A frame can only be removed if there are no transforms connected to
         * or coming from that frame.
         * @throw UnknownFrameException of the frame does not exist.
         * @ŧhrow FrameStillConnectedException if there are still transforms
         *                                     coming from or leading to this
         *                                     frame. */
        void removeFrame(const FrameId& frame);
        
        /**Builds a tree containing all vertices that are accessible starting
         * from @p root.  */
        VertexMap getTree(const vertex_descriptor root) const;
        /**Builds a tree containing all vertices that are accessible starting
         * from @p root.
         * @throw UnknownFrameException if the frame does not exist */
        VertexMap getTree(const FrameId rootId) const;

        vertices_size_type num_vertices() const;
        edges_size_type num_edges() const;
        
         /** Adds @p item to the item list of the specified frame 
         *  Causes ItemAddedEvent.
         *  @throw UnknownFrameException if the frame id is invalid
         *  @param T type of the item that should be added. The item has to 
         *           be of type ItemBase::PtrType<U> where U derives from ItemBase */
        template<class T>
        void addItemToFrame(const FrameId& frame, T item);
        
        /**Returns all items of type @p T that are stored in @p frame.
         * @throw UnknownFrameException if the @p frame id is invalid.
         * @throw NoItemsOfTypeInFrameException if no items of the given type exist in the frame.
         * @return a pair iterators [begin, end] */
        //FIXME document type constraints for T
        template<class T>
        const std::pair<ItemIterator<T>, ItemIterator<T>> getItems(const FrameId& frame) const;
        template<class T>
        const std::pair<ItemIterator<T>, ItemIterator<T>> getItems(const vertex_descriptor frame) const;
        
    protected:
        using EdgePair = std::pair<edge_descriptor, bool>;
      
         /**@brief Add a vertex
         * @note the frame's name must be unique. */
        vertex_descriptor add_vertex(const FrameId& frameId);

        /**@brief Add an Edge
         * Add an edge between two vertices.
         *
         * Causes TransformAddedEvent or TransformModifiedEvent.
         *
         * @return an edge descriptor pointing to the new edge and a boolean.
         *         The boolean is false if the edge already existed.
         *         In that case no new edge was added, instead the existing
         *         edge was updated.
         */
        edge_descriptor add_edge(const vertex_descriptor node_from,
                                 const vertex_descriptor node_to,
                                 const envire::core::Transform &tf,
                                 const FrameId& originName,
                                 const FrameId& targetName);
        
        /**returns the transform between origin and target.
         * @throw UnknownTransformException if the transformation doesn't exist.
         * @param origin Name of the origin frame (used in exception msg only)
         * @param target Name of the target frame (used in exception msg only)
         * @param originVertex vertex_descriptor of the origin frame
         * @param targetVertex vertex_descriptor of the target frame */
        const Transform getTransform(const FrameId& origin, const FrameId& target,
                                    const vertex_descriptor originVertex,
                                    const vertex_descriptor targetVertex) const;
//     if(vertex(frame) == null_vertex())
//     {
//         throw UnknownFrameException(frame);
//     }
//     vector<ItemBase::Ptr>& items = (*this)[frame].frame.items;
//     auto it = std::find(items.begin(), items.end(), item);
//     if(it == items.end())
//     {
//         throw UnknownItemException(frame, item);
//     }
//     items.erase(it);
//     notify(ItemRemovedEvent(frame, item));
        /**Sets the transform value and causes transformModified event. */
        void updateTransform(edge_descriptor ed, const Transform& tf);
        
    private:
      /**Ensures that T is ItemBase::PtrType<X> where X derives from ItemBase  */
      template <class T>
      static void checkItemType();
        
    };
    
    template <class T>
    void TransformGraph::checkItemType()
    {
        using ItemType = typename extract_value_type<T>::value_type;
        static_assert(std::is_same<T, ItemBase::PtrType<ItemType>>::value,
            "Item should be of type ItemBase::PtrType<T>");
        static_assert(std::is_base_of<ItemBase, ItemType>::value,
            "Item is of type ItemBase::PtrType<T> but T does not derive from ItemBase"); 
    }

    
    template <class T>
    void TransformGraph::addItemToFrame(const FrameId& frame, T item)
    {
        checkItemType<T>();
        (*this)[frame].frame.items[std::type_index(typeid(T))].push_back(item);
        //FIXME event
        //notify(ItemAddedEvent(frame, item));
    }
    
    template<class T>
    const std::pair<TransformGraph::ItemIterator<T>, TransformGraph::ItemIterator<T>> TransformGraph::getItems(const FrameId& frame) const
    {
        checkItemType<T>();
        
        if(vertex(frame) == null_vertex())
        {
            throw UnknownFrameException(frame);
        }

        const std::unordered_map<std::type_index, Frame::ItemList>& items = (*this)[frame].frame.items;
        const std::type_index key(typeid(T));
        
        if(items.find(key) == items.end())
        {
            throw NoItemsOfTypeInFrameException(frame, typeid(T).name());
        }
        
        auto begin = items.at(std::type_index(typeid(T))).begin();
        auto end = items.at(std::type_index(typeid(T))).end();
        
        //T::element_type only works if T is a shared_ptr
        ItemIterator<T> beginIt(begin, ItemBaseCaster<typename T::element_type>()); 
        ItemIterator<T> endIt(end, ItemBaseCaster<typename T::element_type>()); 
        return std::make_pair(beginIt, endIt);
    }
    
    template<class T>
    const std::pair<TransformGraph::ItemIterator<T>, TransformGraph::ItemIterator<T>> TransformGraph::getItems(const vertex_descriptor frame) const
    {
        //FIXME implement
        checkItemType<T>();
        throw "NOT IMPLEMENTED";
    }    
    
    template <class T>
    std::pair<TransformGraph::ItemIterator<T>, TransformGraph::ItemIterator<T>>
    TransformGraph::removeItemFromFrame(const FrameId& frameId, ItemIterator<T> item)
    {
        checkItemType<T>();
        
        if(vertex(frameId) == null_vertex())
        {
            throw UnknownFrameException(frameId);
        }
        
        Frame& frame = (*this)[frameId].frame;
        const std::type_index key(typeid(T));
        auto mapEntry = frame.items.find(key);
        if(mapEntry == frame.items.end())
        {
          //FIXME exception type und so
           // throw UnknownItemException(frame, item);
        }
        std::vector<ItemBase::Ptr>& items = mapEntry->second;
        std::vector<ItemBase::Ptr>::const_iterator baseIterator = item.base();
        //HACK This is a workaround for gcc bug 57158 which is still present as of gcc 4.8.4.
        //     In C++11 the parameter type of vector::erase changed from iterator
        //     to const_iterator (which is exactly what we need), but  gcc has not
        //     yet implemented that change. 
        std::vector<ItemBase::Ptr>::iterator nonConstBaseIterator = items.begin() + (baseIterator - items.cbegin()); //vector iterator const cast hack
        std::vector<ItemBase::Ptr>::const_iterator next = items.erase(nonConstBaseIterator);
        //FIXME event
        //     notify(ItemRemovedEvent(frame, item));
        ItemIterator<T> nextIt(next, ItemBaseCaster<typename T::element_type>()); 
        ItemIterator<T> endIt(items.cend(), ItemBaseCaster<typename T::element_type>()); 
        return std::make_pair(nextIt, endIt);
    }
    
    template <class T>
    void TransformGraph::removeItemFromFrame(const FrameId& frameId, ItemBase::PtrType<T> item)
    {
        static_assert(std::is_base_of<ItemBase, T>::value,
            "T does not derive from ItemBase"); 
        
        if(vertex(frameId) == null_vertex())
        {
            throw UnknownFrameException(frameId);
        }
        
        Frame& frame = (*this)[frameId].frame;
        const std::type_index key(typeid(ItemBase::PtrType<T>));
        auto mapEntry = frame.items.find(key);
        if(mapEntry == frame.items.end())
        {
            throw UnknownItemException(frameId, item->getID());
        }
        std::vector<ItemBase::Ptr>& items = mapEntry->second;
        auto searchResult = std::find(items.begin(), items.end(), item);
        if(searchResult == items.end())
        {
            throw UnknownItemException(frameId, item->getID());
        }
        items.erase(searchResult);
        //FIXME event
        //notify(ItemRemovedEvent(frame, item));
    }
    
}}
#endif
