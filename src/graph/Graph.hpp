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

#include <type_traits>

#include <envire_core/events/GraphEventPublisher.hpp>
#include <envire_core/events/FrameEvents.hpp>
#include <envire_core/events/EdgeEvents.hpp>

#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/copy.hpp>
#include <boost/concept_check.hpp>

#include <envire_core/graph/GraphTypes.hpp>
#include <envire_core/graph/TreeView.hpp>
#include <envire_core/graph/GraphExceptions.hpp>
#include <envire_core/graph/GraphVisitors.hpp>
#include <envire_core/graph/Path.hpp>


namespace envire { namespace core
{

/**A double connected labeled graph structure.
 * Each vertex (frame) is identified by a unique FrameId.
 * Each edge is accompanied by it's inverse edge.
 * 
 * 
 * Methods Naming convention
 * Overloading boost methods uses _ delimited separated
 * words, new methods use Camel Case separated words
 * 
 * @param FRAME_PROP should follow the FramePropertyConcept (see GraphTypes.hpp)
 * @param EDGE_PROP should follow the EdgePropertyConcept concept (see GraphTypes.hpp) */
template <class FRAME_PROP, class EDGE_PROP>
class Graph : public GraphBase<FRAME_PROP, EDGE_PROP>, 
              public envire::core::GraphEventPublisher,
              public envire::core::TreeUpdatePublisher
{
public:
     using Base = GraphBase<FRAME_PROP, EDGE_PROP>;
     using vertex_descriptor = GraphTraits::vertex_descriptor;
     using edge_descriptor = GraphTraits::edge_descriptor;
     using EdgePair = std::pair<edge_descriptor, bool>;
     using vertices_size_type = GraphTraits::vertices_size_type;
     using edges_size_type = GraphTraits::edges_size_type;
     using out_edge_iterator = typename Base::out_edge_iterator;
     using vertex_iterator = typename Base::vertex_iterator;
     using edge_iterator = typename Base::edge_iterator;
     using Base::vertex;
    
    Graph();
    
    /**Creates a ***deep*** copy of @p other including edge and frame properties.
     * @note The usual boost copy constructors do create shallow copies, this one
     *       is an exception! If you want the shallow copy, call the base class
     *       copy ctor instead.
     * @note Does **not** copy the event subscribers or TreeView update subscribers.
     *       Only the graph data is copied*/
    explicit Graph(const Graph& other);
    
    /**Adds an unconnected frame to the graph.
    *  The frame property is default constructed and the id is set to @p frame.
    * 
    *  Causes FrameAddedEvent.
    * 
    *  @throw FrameAlreadyExistsException if the frame already exists
    *  @return A handle to the newly created frame.
    */
    vertex_descriptor addFrame(const FrameId& frame);
    
    /**Adds an unconnected frame to the graph.
     * The frame property is constructed using @p args and the id is set to
     * @p frame.
     * 
     * Causes FrameAddedEvent.
     * 
    *  @throw FrameAlreadyExistsException if the frame already exists
    *  @return A handle to the newly created frame.
     */
    template <class... Args>
    vertex_descriptor emplaceFrame(const FrameId&, Args&&... args);
    
    /**Disconnects @p frame from the Graph.
    *  I.e. all edges from and to @p frame will be removed.
    * 
    * Causes EdgeRemovedEvent for each edge that is removed.
    * 
    *  @throw UnknownFrameException if the frame does not exist. */
    void disconnectFrame(const FrameId& frame);
    
    /**Removes @p frame from the Graph.
    *  A frame can only be removed if there are no edges connected to
    *  or coming from that frame.
    * 
    *  Causes FrameRemovedEvent.
    * 
    *  @throw UnknownFrameException if the frame does not exist.
    *  @throw FrameStillConnectedException if there are still edges
    *                                      coming from or leading to this
    *                                      frame. */
    virtual void removeFrame(const FrameId& frame);

    /** @return the id of the specified @p vertex
     *  @throw NullVertexException if vertex is null_vertex */
    const FrameId& getFrameId(const vertex_descriptor vertex) const;
    
    /** @return true if this graph contains a frame with id @p frameId, false
     *          otherwise.*/
    bool containsFrame(const FrameId& frameId) const;
    
    /**@return true if the graph contains a direct edge between @p origin and 
     *         @p target.
     * @throw UnknownFrameException if @p origin or @p target are not part of them
     *                              graph.*/
    bool containsEdge(const FrameId& origin, const FrameId& target) const;
    bool containsEdge(const vertex_descriptor origin, const vertex_descriptor target) const;
    
    /**Add an edge between two frames.
    *  If the frames do not exist they are created.
    *  The inverse edge is created automatically and added as well.
    * 
    *  Causes EdgeAddedEvent.
    *  Causes FrameAddedEvent for each frame if it did not exist before.
    *  
    *  @param origin Source of the edge.
    *  @param target Target of the edge.
    *  @param edgeProperty The value that should be stored in the edge.
    *  @throw EdgeAlreadyExistsException if the edge already exists
    *  @note The edge will also be added to all subscribed TreeViews
    */
    void add_edge(const vertex_descriptor origin,
                  const vertex_descriptor target,
                  const EDGE_PROP& edgeProperty);
    
    void add_edge(const FrameId& origin,
                  const FrameId& target,
                  const EDGE_PROP& edgeProperty);
    
    /**Removes the specified edge from the graph.
    * Also removes the inverse edge.
    * Emits EdgeRemovedEvent for the origin->target edge only.
    * @throw UnknownEdgeException if there is no edge from @p origin to @p target
    **/
    void remove_edge(const vertex_descriptor origin,
                    const vertex_descriptor target);
    /** @throw UnknownFrameException if one of the frames does not exist.*/
    void remove_edge(const FrameId& origin,
                    const FrameId& target);
      
    /** @return the edge between frame @p origin and @p target
      * @throw UnknownFrameException If @p orign or @p target do not exist.
      * @throw UnknownEdgeException if there is no such edge  */
    edge_descriptor getEdge(const FrameId& origin, const FrameId& target) const;
    edge_descriptor getEdge(const vertex_descriptor origin, const vertex_descriptor target) const;
    
    /** @throw UnknownFrameException if @p origin or @p target do not exist
     *  @return The edge property of the edge from origin to target*/
    const EDGE_PROP& getEdgeProperty(const FrameId& origin, const FrameId& target) const;
    const EDGE_PROP& getEdgeProperty(const vertex_descriptor origin, const vertex_descriptor target) const;
    const EDGE_PROP& getEdgeProperty(const edge_descriptor edge) const;
    
    /** @throw UnknownFrameException if @p frame does not exist.*/
    const FRAME_PROP& getFrameProperty(const FrameId& frame) const;
    
    
    /** Sets the property of the edge from @p orign to @p target to @p prop.
     *  Sets the property of the edge from @p target to @p origin to prop.inverse().
     * 
     *  Causes EdgeModifiedEvent.
     * 
     *  @throw UnknownEdgeException If there is no edge between origin and target.
     *  @note There is no setEdgeProperty(edge_descriptor) because it is not trivial
     *        to figure out the inverse edge soley based on the edge_descriptor.*/
    virtual void setEdgeProperty(const vertex_descriptor origin,
                                 const vertex_descriptor target,
                                 const EDGE_PROP& prop);
    
    /** @throw UnknownFrameException If @p orign or @p target do not exist */
    virtual void setEdgeProperty(const FrameId& origin,
                                 const FrameId& target,
                                 const EDGE_PROP& prop);
    
    
    /** @return the source vertex of the @p edge*/
    const vertex_descriptor getSourceVertex(const edge_descriptor edge) const;
    
    /** @return the target vertex of the @p edge*/
    const vertex_descriptor getTargetVertex(const edge_descriptor edge) const;
    
    /**Gets the vertex with id @p frameId.
    * @throw UnknownFrameException if the frame does not exist */
    vertex_descriptor getVertex(const FrameId& frameId) const;
    
    /**Returns a pair of iterators containing all vertices  */
    std::pair<vertex_iterator, vertex_iterator>
    getVertices() const;
    
    /** Returns a pair of iterators containing all edges*/
    std::pair<edge_iterator, edge_iterator>
    getEdges() const;

    /**Builds a TreeView containing all vertices that are accessible starting
      * from @p root.
      * @note The tree is ***not** updated when the Graph changes. */
    TreeView getTree(const vertex_descriptor root) const;
    
    /**Builds a TreeView containing all vertices that are accessible starting
      * from @p rootId.
      * @note The tree is ***not** updated when the Graph changes.
      * @throw UnknownFrameException if the frame does not exist */
    TreeView getTree(const FrameId rootId) const;
    void getTree(const FrameId rootId, TreeView* outView) const;
    void getTree(const vertex_descriptor root, TreeView* outView) const;
    
    /**Builds a TreeView containing all vertices that are accessible starting
      * from @p root and writes it to @p outView.
      * if @p keepTreeUpdated is true, the Graph will retain a pointer
      * to @p outView and update it whenever edges are added or removed.
      * If the TreeView is destroyed it will automatically unsubscribe from
      * the graph. The view can also be unsubscribed manually by calling
      * unsubscribeTreeView()*/
    void getTree(const vertex_descriptor root, const bool keepTreeUpdated, TreeView* outView);
    void getTree(const FrameId rootId, const bool keepTreeUpdated, TreeView* outView);
      
    /**Unsubscribe @p view from TreeView updates */
    virtual void unsubscribeTreeView(TreeView* view);
    
    /**Subscribe @p view to TreeView updates.
      * @note Make sure that the tree is not out of sync already when 
      *       subscribing*/
    virtual void subscribeTreeView(TreeView* view);
    
    /**Returns all frames on the shortest path from @p origin to @p target.
     * Returns an empty vector if no path exists.
     * @throw UnknownFrameException if @p origin or @p target don't exist */
    std::vector<FrameId> getFrames(FrameId origin, FrameId target) const;
    
    /**Returns the shortest path from @p origin to @p target.
     * Returns an empty path if no path exists.
     * 
     * @throw UnknownFrameException if @p origin or @p target don't exist.
     * @param autoUpdating If true, an auto updating path will be returned.
     *                     I.e. a path that is subscribed to graph and
     *                     notices when an edge on the path is removed*/
    Path::Ptr getPath(const FrameId& origin, const FrameId& target,
                                  const bool autoUpdating);
       
    
    /** @return number of frames in this graph*/
    vertices_size_type num_vertices() const;
    
    /** @return number of edges in this graph*/
    edges_size_type num_edges() const;
    
    /** 
     * A vertex_descriptor that does not point to any vertex.
     * "A nullptr for vertices" */
    static vertex_descriptor null_vertex();
    
    /** Visit Graph in bfs order.
     * This is a wrapper around boost::breadth_first_search that correctly
     * parameterizes boost::breadth_first_search to work with this graph. */
    template <class VISITOR>
    void breadthFirstSearch(const vertex_descriptor root, VISITOR visitor) const;
    
    /** Visit Graph in bfs order.
     * This is a wrapper around boost::breadth_first_search that correctly
     * parameterizes boost::breadth_first_search to work with this graph. */
    template <class GRAPH, class VISITOR>
    void breadthFirstSearch(GRAPH& graph, const vertex_descriptor root, VISITOR visitor) const;

    template <class VISITOR>
    EIGEN_DEPRECATED
    void breathFirstSearch(const vertex_descriptor root, VISITOR visitor) const { breadthFirstSearch(root, visitor); }

    template <class GRAPH, class VISITOR>
    EIGEN_DEPRECATED
    void breathFirstSearch(GRAPH& graph, const vertex_descriptor root, VISITOR visitor) const { breadthFirstSearch(graph, root, visitor); }
    
protected:
    using map_type = typename GraphBase<FRAME_PROP, EDGE_PROP>::map_type;
    using GraphBase<FRAME_PROP, EDGE_PROP>::graph;
    using GraphBase<FRAME_PROP, EDGE_PROP>::_map;
    /**@brief Add a vertex
    * @note the frame's name must be unique. */
    vertex_descriptor add_vertex(const FrameId& frameId, const FRAME_PROP& frame);
    
    /**Update all subscribed TreeViews with the new edge.
    * @note When adding a new edge the back-edge is added to the graph
    *       as well. I.e. for each edge two edges are added to the graph.
    *       However only one of the edges should be added to the tree,
    *       because the tree does not care about edge direction.
    *       It does not matter whether you add the back-edge or the edge
    *       to the tree as long as you do ***not*** add both.
    */
    void addEdgeToTreeViews(edge_descriptor newEdge) const;
    void addEdgeToTreeView(edge_descriptor newEdge, TreeView* view) const;
    
    void removeEdgeFromTreeViews(vertex_descriptor origin, vertex_descriptor target) const;
    
    /**Rebuild all subscribed TreeViews */
    void rebuildTreeViews() const;
    
    /**Removes the specified edge.*/
    void remove_edge(const FrameId& origin, const FrameId& target, 
                     const vertex_descriptor originDesc, 
                     const vertex_descriptor targetDesc);

    /**
     * @brief Publishes the current state of the graph.
     */
    virtual void publishCurrentState(GraphEventSubscriber* pSubscriber);

    /**
     * @brief Unpublishes the current state of the graph.
     *        Basically the reverse process of publishCurrentState
     */
    virtual void unpublishCurrentState(GraphEventSubscriber* pSubscriber);
    
    /**Re-generates the content of _map based on the FrameIds.
     * This method is used when de-serializing or copying the graph.*/
    void regenerateLabelMap();
    
    
    /**TreeViews that need to be updated when the graph is modified */
    std::vector<TreeView*> subscribedTreeViews;
    
private:
    /**Grants access to boost serialization */
    friend class boost::serialization::access;

    /**Unserializes this class*/
    template <typename Archive>
    void load(Archive &ar, const unsigned int version);

    /**Serializes this class. Only the directed graph is serialized,
      * subscribers are excluded and the mapping of the labeled graph
      * is regenerated. */
    template <typename Archive>
    void save(Archive &ar, const unsigned int version) const;

    /**Splits boost serialize into a load and save method */
    template <typename Archive>
    void serialize(Archive &ar, const unsigned int version);
};






template <class F, class E>
Graph<F,E>::Graph()
{
    /**These asserts are important because:
     *   * we are handing out vertex_descriptor and edge_descriptor to the user.
     *     If the underlying datatype is a random-access container the vertex_descriptors
     *     could be invalidated each time a vertex/edge is added or removed.
     *   * The null_vertex() definition is different for random_access containers
     *     The definition provided in GraphTypes only works for pointer based containers.*/
    static_assert(std::is_same<typename GraphBase<F, E>::graph_type::graph_type::graph_type::vertex_list_selector, boost::listS>::value,
                  "vertex list type should be listS to ensure that vertex_descriptors remain valid");
    static_assert(std::is_same<typename GraphBase<F, E>::graph_type::graph_type::graph_type::edge_list_selector, boost::listS>::value,
                  "edge list type should be listS to ensure that vertex_descriptors remain valid");

    BOOST_CONCEPT_ASSERT((EdgePropertyConcept<E>));
    BOOST_CONCEPT_ASSERT((FramePropertyConcept<F>));
}

template <class F, class E>
Graph<F,E>::Graph(const Graph<F, E>& other) : Base()
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

template <class F, class E>
typename Graph<F,E>::vertex_descriptor Graph<F,E>::addFrame(const FrameId& frame)
{
    vertex_descriptor desc = vertex(frame);
    if(desc == null_vertex())
    {
        F frameProp;
        frameProp.setId(frame);
        desc = add_vertex(frame, frameProp);
    }
    else
    {
      throw FrameAlreadyExistsException(frame);
    }
    return desc;
}

template <class F, class E>
typename Graph<F,E>::vertex_descriptor Graph<F,E>::add_vertex(const FrameId& frameId,
                                                              const F& frame)
{
    vertex_descriptor v = GraphBase<F, E>::add_vertex(frameId, frame);
    notify(FrameAddedEvent(frameId));
    return v;
}

template <class F, class E>
typename Graph<F,E>::edge_descriptor Graph<F,E>::getEdge(const FrameId& origin,
                                                         const FrameId& target) const
{
    vertex_descriptor originDesc = getVertex(origin); //throws
    vertex_descriptor targetDesc = getVertex(target);
    return getEdge(originDesc, targetDesc);
}

template <class F, class E>
typename Graph<F,E>::edge_descriptor Graph<F,E>::getEdge(const vertex_descriptor origin,
                                                         const vertex_descriptor target) const
{
    EdgePair e = boost::edge(origin, target, graph());
    if(!e.second)
    {
        throw UnknownEdgeException(getFrameId(origin), getFrameId(target));
    }
    return e.first;
}

template <class F, class E>
const FrameId& Graph<F,E>::getFrameId(const vertex_descriptor vertex) const
{
    if(vertex == GraphTraits::null_vertex())
      throw NullVertexException();
    return graph()[vertex].getId();
}

template <class F, class E>
typename Graph<F,E>::vertex_descriptor Graph<F,E>::getVertex(const FrameId& frame) const
{
    vertex_descriptor desc = vertex(frame);
    if(desc == null_vertex())
    {
        throw UnknownFrameException(frame);
    }
    return desc;
}

template <class F, class E>
void Graph<F,E>::disconnectFrame(const FrameId& frame)
{
    const vertex_descriptor frameDesc = getVertex(frame);
    
    out_edge_iterator begin, end;
      
    //removing an edge invalidates the out_edge_iterators, thus the strange
    //loop structure :-)
    do
    {
        boost::tie(begin, end) = boost::out_edges(frameDesc, *this);
        if(begin != end)
        {
            const vertex_descriptor target = boost::target(*begin, *this);
            remove_edge(frameDesc, target); //this will remove the inv edge as well.
        }
    } while(begin != end);
}

template <class F, class E>
void Graph<F,E>::removeFrame(const FrameId& frame)
{
    vertex_descriptor desc = getVertex(frame); //will throw
    if(boost::degree(desc, *this) > 0)
    {
        throw FrameStillConnectedException(frame);
    }
    
    boost::remove_vertex(desc, graph());//If the HACK is removed, remove_vertex needs to be called with frame as first parameter
    //HACK this is a workaround for bug https://svn.boost.org/trac/boost/ticket/9493
    //If the bug is fixed also remove the #define private protected in GraphTypes.hpp
    typename map_type::iterator it = _map.find(frame);
    if(it != _map.end())
    {
        _map.erase(it);
    }
    notify(envire::core::FrameRemovedEvent(frame));
}

template <class F, class E>
envire::core::TreeView Graph<F,E>::getTree(const vertex_descriptor root) const
{
    TreeView view(root);
    getTree(root, &view);
    return std::move(view);
}

template <class F, class E>
envire::core::TreeView Graph<F,E>::getTree(const FrameId rootId) const
{
    const vertex_descriptor root = getVertex(rootId);
    return getTree(root);
}

template <class F, class E>
void Graph<F,E>::getTree(const FrameId rootId, TreeView* outView) const
{
    vertex_descriptor root = getVertex(rootId);
    getTree(root, outView);
}

template <class F, class E>
void Graph<F,E>::getTree(const vertex_descriptor root, const bool keepTreeUpdated, TreeView* outView)
{
    getTree(root, outView);    
    if(keepTreeUpdated)
    {
      subscribeTreeView(outView);
    }
}

template <class F, class E>
void Graph<F,E>::getTree(const FrameId rootId, const bool keepTreeUpdated, TreeView* outView)
{
    const vertex_descriptor root = getVertex(rootId);
    getTree(root, keepTreeUpdated, outView);
}

template <class F, class E>
void Graph<F,E>::getTree(const vertex_descriptor root, TreeView* outView) const
{
    outView->addRoot(root);
    TreeBuilderVisitor<Graph<F,E>> visitor(*outView, *this);
    breadthFirstSearch(root, boost::visitor(visitor));
}

template <class F, class E>
void Graph<F,E>::unsubscribeTreeView(TreeView* view)
{
    subscribedTreeViews.erase(std::remove(subscribedTreeViews.begin(),
                                          subscribedTreeViews.end(), view),
                              subscribedTreeViews.end());
}

template <class F, class E>
void Graph<F,E>::subscribeTreeView(TreeView* view)
{
    assert(view != nullptr);
    subscribedTreeViews.push_back(view);
    view->setPublisher(this); //now the TreeView will automatically unsubscribe on destruction
}

template <class F, class E>
std::vector<FrameId> Graph<F,E>::getFrames(FrameId origin, FrameId target) const
{
    vertex_descriptor fromDesc = getVertex(origin); //may throw
    vertex_descriptor toDesc = getVertex(target); //may throw
  
    std::vector<FrameId> path;
    envire::core::GraphBFSVisitor <vertex_descriptor>visit(toDesc, this->graph());
    try
    {
        breadthFirstSearch(fromDesc, boost::visitor(visit));

    }catch(const FoundFrameException &e)
    {
        typename std::deque<vertex_descriptor>::iterator it = visit.tree->begin();
        for (; (it+1) != visit.tree->end(); ++it)
        {
            path.push_back(getFrameId(*it));
        }
    }
    if(path.size() > 0)
    {
        path.push_back(target);
    }
    //return is fine, compiler will detect this and move instead of copy
    return path;
}

template <class F, class E>
const typename Graph<F,E>::vertex_descriptor Graph<F,E>::getSourceVertex(const edge_descriptor edge) const
{
    return boost::source(edge, graph());
}

template <class F, class E>
const typename Graph<F,E>::vertex_descriptor Graph<F,E>::getTargetVertex(const edge_descriptor edge) const
{
    return boost::target(edge, graph());
}


template <class F, class E>
void Graph<F,E>::add_edge(const vertex_descriptor origin,
                          const vertex_descriptor target,
                          const E& edgeProperty)
{   
    //check if an edge already exists
    //If a->b exists, b->a also exist. Therefore we need to check only one direction
    EdgePair e = boost::edge(origin, target, *this);
    if(e.second)// edge already exists
    {
        throw EdgeAlreadyExistsException(getFrameId(origin), getFrameId(target));
    }
  
    EdgePair edge_pair =  boost::add_edge(origin, target, edgeProperty, *this);
    EdgePair edge_pair_inv =  boost::add_edge(target, origin, edgeProperty.inverse(), *this);
    assert(edge_pair_inv.second);//origin->target has already been checkd before
    
    //note: we only need to add one of the edges to the tree, because the tree
    //      does not care about the edge direction.
    //      In fact: if we add both, both will end up in the cross edges list
    //      which might lead to infinite recursion when updating edges
    addEdgeToTreeViews(edge_pair.first);
    notify(envire::core::EdgeAddedEvent(getFrameId(origin), getFrameId(target), edge_pair.first));
}

template <class F, class E>
void Graph<F,E>::add_edge(const FrameId& origin,
                          const FrameId& target,
                          const E& edgeProperty)
{
    vertex_descriptor originDesc = vertex(origin);
    vertex_descriptor targetDesc = vertex(target);
    
      //if they don't exist create them
    if(originDesc == null_vertex())
    {
        F originProp;
        originProp.setId(origin);
        originDesc = add_vertex(origin, originProp);
    }
    if(targetDesc == null_vertex())
    {
        F targetProp;
        targetProp.setId(target);
        targetDesc = add_vertex(target, targetProp);
    }
    
    return add_edge(originDesc, targetDesc, edgeProperty);
}

template <class F, class E>
void Graph<F,E>::remove_edge(const FrameId& origin, const FrameId& target, 
                             const vertex_descriptor originDesc, 
                             const vertex_descriptor targetDesc)
{
    //note: do not use boost::edge_by_label as it will segfault if one of the
    //frames is not part of the tree.
    EdgePair originToTarget = boost::edge(originDesc, targetDesc, graph());
    EdgePair targetToOrigin = boost::edge(targetDesc, originDesc, graph());
    if(!originToTarget.second || !targetToOrigin.second)
    {
        throw UnknownEdgeException(origin, target);
    }
    
    boost::remove_edge(originToTarget.first, *this);
    notify(envire::core::EdgeRemovedEvent(origin, target));
    
    boost::remove_edge(targetToOrigin.first, *this);
    
    removeEdgeFromTreeViews(originDesc, targetDesc);
    
}

template <class F, class E>
void Graph<F,E>::remove_edge(const vertex_descriptor origin,
                             const vertex_descriptor target)
{
    remove_edge(getFrameId(origin), getFrameId(target), origin, target);
}

template <class F, class E>
void Graph<F,E>::remove_edge(const FrameId& origin,
                             const FrameId& target)
{
    remove_edge(origin, target, getVertex(origin), getVertex(target));
}

template <class F, class E>
void Graph<F,E>::removeEdgeFromTreeViews(vertex_descriptor origin, vertex_descriptor target) const
{
    for(TreeView* view : subscribedTreeViews)
    {
        if(view->edgeExists(origin, target))
            view->removeEdge(origin, target);
    }    
}

template <class F, class E>
void Graph<F,E>::addEdgeToTreeViews(edge_descriptor newEdge) const
{
    for(TreeView* view : subscribedTreeViews)
    {
        addEdgeToTreeView(newEdge, view);
    }
}

template <class F, class E>
void Graph<F,E>::addEdgeToTreeView(edge_descriptor newEdge, TreeView* view) const
{
  
    //We only need to add the edge to the tree, if one of the two vertices is already part
    //of the tree
    
    const vertex_descriptor src = getSourceVertex(newEdge);
    const vertex_descriptor tar = getTargetVertex(newEdge);
    const bool srcInView = view->vertexExists(src);
    const bool tarInView = view->vertexExists(tar);
    
    
    vertex_descriptor inView = null_vertex(); //the vertex that is already in the tree
    vertex_descriptor notInView = null_vertex(); //the vertex that is not yet in the tree
    
    //this is either a cross- or back-edge
    if(srcInView && tarInView)
    {
        if(!view->edgeExists(src, tar))
        {
            //if both vertices are in the tree but there is no edge between them
            //this is a cross edge
            view->addCrossEdge(src, tar, newEdge);
            return;
        }
        else
        {
            //otherwise it's a back-edge that can be ignored
            return;
        }
    }
    else if(srcInView && !tarInView)
    {
        inView = src;
        notInView = tar;
    }
    else if(tarInView && !srcInView)
    {
        inView = tar;
        notInView = src;
    }
    else if(!srcInView && !tarInView)
    {
        //an edge was added to a different subtree that is not connected to this one
        return;
    }
    else
    {
        //this should never happen if the above logic is correct
        assert(false);
    }
    
    view->addEdge(inView, notInView);
    
    //there might be a whole tree connected to notInView which should
    //be added to the tree
    //There are exactly two edges connected to the new vertex if it is only 
    //connected to 'inView'.
    //If there are more edges we need to follow them and add the whole graph
    if(boost::degree(notInView, graph()) > 2)
    {
        /* (1) Create a filtered graph that does not contain the edges between
        *     inView and notInView
        * (2) Build a bfs tree starting from notInView on the filtered graph
        * (3) merge the two trees
        */
        
        //the two edges that should be filtered
        const FrameId id1 = getFrameId(inView);
        const FrameId id2 = getFrameId(notInView);
        
        //create a filter that can be used to hide the two edges from the graph
        EdgeFilter filter;
        filter.edge1 = getEdge(id1, id2);
        filter.edge2 = getEdge(id2, id1);
        
        //everything in this graph will be visible except edge1 and edge2
        boost::filtered_graph<const Graph<F,E>, EdgeFilter> fg(*this, filter);
        
        //use TreeBuilderVisitor to generate a new tree starting from notInView.
        //This tree will only contain vertices that are part of the sub tree that
        //below to notInView because the edges leading to inView are hidden by the filter
        //and thus the bfs will not follow those edges.
        //the visitor will add those edges directly to the view
        TreeBuilderVisitor<Graph<F,E>> visitor(*view, *this);
        breadthFirstSearch(fg, notInView, boost::visitor(visitor));
    }
}

template <class F, class E>
void Graph<F,E>::rebuildTreeViews() const
{
    for(TreeView* view : subscribedTreeViews)
    {
        view->clear();
        getTree(view->root, view);
    }
}

template <class F, class E>
typename Graph<F,E>::vertices_size_type Graph<F,E>::num_vertices() const
{
    return boost::num_vertices(*this);
}

template <class F, class E>
typename Graph<F,E>::edges_size_type Graph<F,E>::num_edges() const
{
    return boost::num_edges(*this);
}

template <class F, class E>
const E& Graph<F,E>::getEdgeProperty(const FrameId& origin, const FrameId& target) const
{
    const edge_descriptor edge = getEdge(origin, target);
    return getEdgeProperty(edge);
}

template <class F, class E>
const E& Graph<F,E>::getEdgeProperty(const vertex_descriptor origin, const vertex_descriptor target) const
{
    const edge_descriptor edge = getEdge(origin, target);
    return getEdgeProperty(edge);
}

template <class F, class E>
const E& Graph<F,E>::getEdgeProperty(const edge_descriptor edge) const
{
  return (*this)[edge];
}


template <class F, class E>
void Graph<F,E>::setEdgeProperty(const FrameId& origin,
                                 const FrameId& target,
                                 const E& prop)
{
    const vertex_descriptor originDesc = getVertex(origin);
    const vertex_descriptor targetDesc = getVertex(target);
    setEdgeProperty(originDesc, targetDesc, prop);
}

template <class F, class E>
void Graph<F,E>::setEdgeProperty(const vertex_descriptor origin,
                                 const vertex_descriptor target,
                                 const E& prop)
{
    EdgePair originToTarget = boost::edge(origin, target, *this);
    
    if(!originToTarget.second)
    {
      throw UnknownEdgeException(getFrameId(origin), getFrameId(target));
    } 
    (*this)[originToTarget.first] = prop;
    
    EdgePair targetToOrigin = boost::edge(target, origin, *this);
    assert(targetToOrigin.second); //there should always be an inverse edge
    (*this)[targetToOrigin.first] = prop.inverse();
    
    notify(EdgeModifiedEvent(getFrameId(origin), getFrameId(target), originToTarget.first, targetToOrigin.first));
}

template <class F, class E>
typename Graph<F,E>::vertex_descriptor Graph<F,E>::null_vertex()
{
    return GraphTraits::null_vertex();
}

template <class F, class E>
template <class... Args>
typename Graph<F,E>::vertex_descriptor Graph<F,E>::emplaceFrame(const FrameId& frame,
                                                                Args&&... args)
{
    vertex_descriptor desc = vertex(frame);
    if(desc == null_vertex())
    {
        F frameProp(std::forward<Args>(args)...);
        frameProp.setId(frame);
        desc = add_vertex(frame, frameProp);
    }
    else
    {
      throw FrameAlreadyExistsException(frame);
    }
    return desc;
}

template <class F, class E>
const F& Graph<F,E>::getFrameProperty(const FrameId& frame) const
{
    vertex_descriptor v = getVertex(frame);
    return graph()[v];
}

template <class F, class E>
void Graph<F,E>::publishCurrentState(GraphEventSubscriber* pSubscriber)
{
    // publish frames
    typename boost::graph_traits<Graph<F,E>>::vertex_iterator vertex_it, vertex_end;
    for (boost::tie( vertex_it, vertex_end ) = boost::vertices( graph() ); vertex_it != vertex_end; ++vertex_it)
    {
        notifySubscriber(pSubscriber, FrameAddedEvent(getFrameId(*vertex_it)));
    }

    // publish edges
    typename boost::graph_traits<Graph<F,E>>::edge_iterator edge_it, edge_end;
    std::vector<edge_descriptor> published_edges;
    for (boost::tie( edge_it, edge_end ) = boost::edges( graph() ); edge_it != edge_end; ++edge_it)
    {
        std::vector<edge_descriptor>::const_iterator it = std::find(published_edges.begin(), published_edges.end(), *edge_it);
        if(it == published_edges.end())
        {
            const vertex_descriptor src = getSourceVertex(*edge_it);
            const vertex_descriptor tar = getTargetVertex(*edge_it);
            notifySubscriber(pSubscriber, EdgeAddedEvent(getFrameId(src), getFrameId(tar), *edge_it));

            // save inverse edge in order to not send it twice
            published_edges.push_back(getEdge(tar, src));
        }
    }
}

template <class F, class E>
void Graph<F,E>::unpublishCurrentState(GraphEventSubscriber* pSubscriber)
{
    // unpublish edges
    typename boost::graph_traits<Graph<F,E>>::edge_iterator edge_it, edge_end;
    std::vector<edge_descriptor> unpublished_edges;
    for (boost::tie( edge_it, edge_end ) = boost::edges( graph() ); edge_it != edge_end; ++edge_it)
    {
        std::vector<edge_descriptor>::const_iterator it = std::find(unpublished_edges.begin(), unpublished_edges.end(), *edge_it);
        if(it == unpublished_edges.end())
        {
            const vertex_descriptor src = getSourceVertex(*edge_it);
            const vertex_descriptor tar = getTargetVertex(*edge_it);
            notifySubscriber(pSubscriber, EdgeRemovedEvent(getFrameId(src), getFrameId(tar)));

            // save inverse edge in order to not send it twice
            unpublished_edges.push_back(getEdge(tar, src));
        }
    }

    // unpublish frames
    typename boost::graph_traits<Graph<F,E>>::vertex_iterator vertex_it, vertex_end;
    for (boost::tie( vertex_it, vertex_end ) = boost::vertices( graph() ); vertex_it != vertex_end; ++vertex_it)
    {
        notifySubscriber(pSubscriber, FrameRemovedEvent(getFrameId(*vertex_it)));
    }
}

template<class F, class E>
template <typename Archive>
void Graph<F,E>::load(Archive &ar, const unsigned int version)
{
    ar >> boost::serialization::make_nvp("directed_graph",  graph());

    // regenerate mapping of the labeled graph
    regenerateLabelMap();
}

template<class F, class E>
template <typename Archive>
void Graph<F,E>::save(Archive &ar, const unsigned int version) const
{
    ar << boost::serialization::make_nvp("directed_graph", graph());
}

template<class F, class E>
template <typename Archive>
void Graph<F,E>::serialize(Archive &ar, const unsigned int version)
{
    boost::serialization::split_member(ar, *this, version);
}

template<class F, class E>
std::pair<typename Graph<F,E>::vertex_iterator, typename Graph<F,E>::vertex_iterator>
Graph<F,E>::getVertices() const
{
  return boost::vertices(*this);
}

template<class F, class E>
bool Graph<F,E>::containsFrame(const FrameId& frameId) const
{
    const vertex_descriptor v = Base::vertex(frameId);
    return v != null_vertex();
}

template<class F, class E>
void Graph<F,E>::regenerateLabelMap()
{
    typename boost::graph_traits<Graph<F,E>>::vertex_iterator it, end;
    for (boost::tie( it, end ) = boost::vertices( graph()); it != end; ++it)
    {
        const FrameId id = getFrameId(*it);
        _map[id] = *it;
    }
}

template<class F, class E>
std::pair<typename Graph<F,E>::edge_iterator, typename Graph<F,E>::edge_iterator>
Graph<F,E>::getEdges() const
{
    return boost::edges(graph());
}

template<class F, class E>
bool Graph<F,E>::containsEdge(const FrameId& origin, const FrameId& target) const
{
    return containsEdge(getVertex(origin), getVertex(target));
}

template<class F, class E>
bool Graph<F,E>::containsEdge(const vertex_descriptor origin, const vertex_descriptor target) const
{
    EdgePair e = boost::edge(origin, target, graph());
    return e.second;   
}
  
template<class F, class E>
Path::Ptr Graph<F,E>::getPath(const FrameId& origin, const FrameId& target,
                                          const bool autoUpdating)
{
    if(autoUpdating)
    {
        return Path::Ptr(new Path(getFrames(origin, target), this));
    }
    else
    {
        return Path::Ptr(new Path(getFrames(origin, target)));
    }
}


template<class F, class E>
template<class VISITOR>
void Graph<F,E>::breadthFirstSearch(vertex_descriptor root, VISITOR visitor) const
{
    breadthFirstSearch(graph(), root, visitor);
}

template<class F, class E>
template <class GRAPH, class VISITOR>
void Graph<F,E>::breadthFirstSearch(GRAPH& graph, const vertex_descriptor root, VISITOR visitor) const
{
    // breadth first search uses a std::vector of default_color_type as default,
    // which is fine for graphs using boost::vecS. Since we are using listS,
    // we need to provide a colormap:
    std::unordered_map<vertex_descriptor, boost::default_color_type> colors;
    auto colorMap = boost::make_assoc_property_map(colors);
    // the alternative would be to provide a custom vertex_index_map which maps
    // each vertex to an integer in [0, num_vertices(g))

    boost::breadth_first_search(graph, root, visitor.color_map(colorMap));    
}


}}

