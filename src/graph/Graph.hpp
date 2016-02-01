#pragma once

#include <type_traits>
#include <envire_core/events/GraphEventPublisher.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/concept_check.hpp>

#include "GraphTypes.hpp"
#include "TreeView.hpp"
#include "TransformGraphExceptions.hpp"
#include "TransformGraphVisitors.hpp"


namespace envire { namespace core { namespace graph
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
             // public envire::core::GraphEventPublisher,
              public envire::core::TreeUpdatePublisher
{
public:
    using vertex_descriptor = typename GraphBase<FRAME_PROP, EDGE_PROP>::vertex_descriptor;
    using edge_descriptor = typename GraphBase<FRAME_PROP, EDGE_PROP>::edge_descriptor;
    using EdgePair = std::pair<edge_descriptor, bool>;
    using vertices_size_type = typename GraphBase<FRAME_PROP, EDGE_PROP>::vertices_size_type;
    using edges_size_type = typename GraphBase<FRAME_PROP, EDGE_PROP>::edges_size_type;
    using GraphBase<FRAME_PROP, EDGE_PROP>::null_vertex;
    using GraphBase<FRAME_PROP, EDGE_PROP>::vertex;
    
    Graph();
    
    /** Adds an unconnected frame to the graph.
    *   The frame property is default constructed and the id is set to @p frame.
    * @throw FrameAlreadyExistsException if the frame already exists
    * @return A handle to the newly created frame.
    */
    vertex_descriptor addFrame(const FrameId& frame);
    
    /**Disconnects @p frame from the Graph.
    * I.e. all edges from and to @p frame will be removed.
    * @throw UnknownFrameException if the frame does not exist. */
    void disconnectFrame(const FrameId& frame);
    
    /**Removes @p frame from the Graph.
    * A frame can only be removed if there are no edges connected to
    * or coming from that frame.
    * @throw UnknownFrameException if the frame does not exist.
    * @ŧhrow FrameStillConnectedException if there are still transforms
    *                                     coming from or leading to this
    *                                     frame. */
    void removeFrame(const FrameId& frame);

    /** @return the frame id of the specified @p vertex */
    const envire::core::FrameId& getFrameId(const vertex_descriptor vertex) const;
    
    
    /**@brief Add an Edge
    * Add an edge between two frames.
    * If the frames do not exist they are created.
    * The inverse edge is created automatically and added as well.
    * 
    * @param origin Source of the edge.
    * @param target Target of the edge.
    * @param edgeProperty The value that should be stored in the edge.
    * @return an edge descriptor pointing to the new edge and a boolean.
    *         The boolean is false if the edge already existed.
    *         In that case no new edge was added, instead the existing
    *         edge was updated.
    * @throw EdgeAlreadyExistsException if the edge already exists
    * @note The edge will also be added to all Subscribed TreeViews
    */
  void add_edge(const vertex_descriptor origin,
                const vertex_descriptor target,
                const EDGE_PROP& edgeProperty);
  
  /** @throw UnknownFrameException If @p orign or @p target do not exist. */
  void add_edge(const FrameId& origin,
                const FrameId& target,
                const EDGE_PROP& edgeProperty);
  
  /**Removes the specified edge from the graph.
   * Also removes the inverse
   * FIXME events?!
   * @throw UnknownEdgeException if there is no edge from @p origin to @p target
   **/
  void remove_edge(const vertex_descriptor origin,
                   const vertex_descriptor target);
  /** @throw UnknownFrameException if one of the frames does not exist.*/
  void remove_edge(const FrameId& origin,
                   const FrameId& target);
    
    /** @return the edge between frame @p origin and @p target
      * @throw UnknownFrameException If @p orign or @p target do not exist.
      * @throw UnknownTransformException if there is no such edge  */
    edge_descriptor getEdge(const FrameId& origin, const FrameId& target) const;
    edge_descriptor getEdge(const vertex_descriptor origin, const vertex_descriptor target) const;
    
    /** @throw UnknownFrameException if @p origin or @p target do not exist
     *  @return The edge property of the edge from origin to target*/
    const EDGE_PROP& getEdgeProperty(const FrameId& origin, const FrameId& target) const;
    const EDGE_PROP& getEdgeProperty(const vertex_descriptor origin, const vertex_descriptor target) const;
    const EDGE_PROP& getEdgeProperty(const edge_descriptor edge) const;
    
    
    
    const vertex_descriptor source(const edge_descriptor edge) const;
    const vertex_descriptor target(const edge_descriptor edge) const;
    
    /**Gets the vertex corresponding to @p frame.
    * @throw UnknownFrameException if the frame does not exist */
    vertex_descriptor getVertex(const FrameId& frame) const;
    

    /**Builds a TreeView containing all vertices that are accessible starting
      * from @p root.  */
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
      * if @p keepTreeUpdated is true, the TransformGraph will retain a pointer
      * to @p outView and update it whenenver transformations are added or removed.
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
    
    /**Returns the shortest path from @p origin to @p target.
    * Returns an empty vector if the path doesn't exist.
    * @throw UnknownFrameException if @p orign or @p target don't exist */
    Path getPath(FrameId origin, FrameId target) const;
    
    vertices_size_type num_vertices() const;
    
    edges_size_type num_edges() const;
    
protected:
    using map_type = typename GraphBase<FRAME_PROP, EDGE_PROP>::map_type;
    using GraphBase<FRAME_PROP, EDGE_PROP>::graph;
    using GraphBase<FRAME_PROP, EDGE_PROP>::_map;
    /**@brief Add a vertex
    * @note the frame's name must be unique. */
    vertex_descriptor add_vertex(const FrameId& frameId, const FRAME_PROP& frame);
    
    /**Update all subscribed TreeViews with the new edge.
    * @note When adding a new transform the back-edge is added to the graph
    *       as well. I.e. for each transform two edges are added to the graph.
    *       However only one of the edges should be added to the tree,
    *       because the tree does not care about edge direction.
    *       It does not matter whether you add the back-edge or the edge
    *       to the tree as long as you do ***not*** add both.
    */
    void addEdgeToTreeViews(edge_descriptor newEdge) const;
    void addEdgeToTreeView(edge_descriptor newEdge, TreeView* view) const;
    
    /**Rebuild all subscribed TreeViews */
    void rebuildTreeViews() const;
    
    
    /**TreeViews that need to be updated when the graph is modified */
    std::vector<TreeView*> subscribedTreeViews;
  
};






template <class F, class E>
Graph<F,E>::Graph()
{
    static_assert(std::is_same<typename GraphBase<F, E>::graph_type::graph_type::graph_type::vertex_list_selector, boost::listS>::value,
                  "vertex list type should be listS to ensure that vertex_descriptors remain valid");
    static_assert(std::is_same<typename GraphBase<F, E>::graph_type::graph_type::graph_type::edge_list_selector, boost::listS>::value,
                  "edge list type should be listS to ensure that vertex_descriptors remain valid");

    BOOST_CONCEPT_ASSERT((EdgePropertyConcept<E>));
    BOOST_CONCEPT_ASSERT((FramePropertyConcept<F>));
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
    //FIXME frameAddedEvent
    //notify(FrameAddedEvent(frameId));
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
    vertex_descriptor desc = getVertex(frame);
    boost::clear_vertex(desc, *this);

}

template <class F, class E>
void Graph<F,E>::removeFrame(const FrameId& frame)
{
    vertex_descriptor desc = getVertex(frame); //will throw
    if(boost::degree(desc, *this) > 0)
    {
        throw FrameStillConnectedException(frame);
    }
    
    //explicitly remove all items from the frame to cause ItemRemovedEvents
    //FIXME think about how to call clearFrame now?!
    //clearFrame(frame);
    
    boost::remove_vertex(frame, *this);
    //HACK this is a workaround for bug https://svn.boost.org/trac/boost/ticket/9493
    //If the bug is fixed also remove the #define private protected in GraphTypes.hpp
    typename map_type::iterator it = _map.find(frame);
    if(it != _map.end())
    {
        _map.erase(it);
    }
    //FIXME event wieder einauen
    //notify(FrameRemovedEvent(frame));
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
    outView->root = root;
    
    if(boost::degree(root, graph()) == 0)
    {
        //the TreeBuilderVisitor only looks at the edges, if there are no dges
        //it wouldn't add the root node
        outView->tree[root].parent = null_vertex();
        outView->tree[root].parentRelation = nullptr;
    }
    else
    {
        TreeBuilderVisitor<Graph<F,E>> visitor(*outView, *this);
        boost::breadth_first_search(*this, root, boost::visitor(visitor));
    }
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
Path Graph<F,E>::getPath(FrameId origin, FrameId target) const
{
    vertex_descriptor fromDesc = getVertex(origin); //may throw
    vertex_descriptor toDesc = getVertex(target); //may throw
  
    Path path;
    envire::core::TransformGraphBFSVisitor <vertex_descriptor>visit(toDesc, this->graph());
    try
    {
        boost::breadth_first_search(this->graph(), fromDesc, visitor(visit));

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
const typename Graph<F,E>::vertex_descriptor Graph<F,E>::source(const edge_descriptor edge) const
{
    return boost::source(edge, graph());
}

template <class F, class E>
const typename Graph<F,E>::vertex_descriptor Graph<F,E>::target(const edge_descriptor edge) const
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
    edge_pair =  boost::add_edge(target, origin, edgeProperty.inverse(), *this);
    assert(edge_pair.second);//origin->target has already been checkd before
    
    //note: we only need to add one of the edges to the tree, because the tree
    //      does not care about the edge direction.
    //      In fact: if we add both, both will end up in the cross edges list
    //      which might lead to infinite recursion when updating edges
    addEdgeToTreeViews(edge_pair.first);
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
        originDesc = addFrame(origin);
    }
    if(targetDesc == null_vertex())
    {
        targetDesc = addFrame(target);
    }
    
    return add_edge(originDesc, targetDesc, edgeProperty);
}

template <class F, class E>
void Graph<F,E>::remove_edge(const vertex_descriptor origin,
                             const vertex_descriptor target)
{
    //note: do not use boost::edge_by_label as it will segfault if one of the
    //frames is not part of the tree.
    EdgePair originToTarget = boost::edge(origin, target, graph());
    EdgePair targetToOrigin = boost::edge(target, origin, graph());
    if(!originToTarget.second || !targetToOrigin.second)
    {
        throw UnknownEdgeException(getFrameId(origin), getFrameId(target));
    }
    
    boost::remove_edge(originToTarget.first, *this);
    //FIXME removed events
    //notify(TransformRemovedEvent(origin, target));
    
    boost::remove_edge(targetToOrigin.first, *this);
    //FIXME REMOVE events
    //notify(TransformRemovedEvent(target, origin));
    
    //removing a transform might invalidate the TreeViews.
    //This is a brute-force solution to the problem.
    //FIXME update TreeViews when removing a transform instead of rebuilding them
    rebuildTreeViews();
}

template <class F, class E>
void Graph<F,E>::remove_edge(const FrameId& origin,
                             const FrameId& target)
{
    const vertex_descriptor originDesc = getVertex(origin); //may throw
    const vertex_descriptor targetDesc = getVertex(target); //may throw
    remove_edge(originDesc, targetDesc);
}

template <class F, class E>
void Graph<F,E>::addEdgeToTreeViews(edge_descriptor newEdge) const
{
    for(TreeView* view : subscribedTreeViews)
    {
        addEdgeToTreeView(newEdge, view);
        view->treeUpdated();//notify owners of the view, that it has been updated.
    }
}

template <class F, class E>
void Graph<F,E>::addEdgeToTreeView(edge_descriptor newEdge, TreeView* view) const
{
  
    //We only need to add the edge to the tree, if one of the two vertices is already part
    //of the tree
    
    const vertex_descriptor src = source(newEdge);
    const vertex_descriptor tar = target(newEdge);
    const bool srcInView = view->tree.find(src) != view->tree.end();
    const bool tarInView = view->tree.find(tar) != view->tree.end();
    
    
    vertex_descriptor inView = null_vertex(); //the vertex that is already in the tree
    vertex_descriptor notInView = null_vertex(); //the vertex that is not yet in the tree
    
    //this is either a cross- or back-edge
    if(srcInView && tarInView)
    {
        if(!view->edgeExists(src, tar))
        {
            //if both vertices are in the tree but there is no edge between them
            //this is a cross edge
            view->crossEdges.push_back(newEdge);
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
        //this should never happend if the above logic is correct
        assert(false);
    }
    
    view->tree[inView].children.insert(notInView);
    view->tree[notInView].parent = inView; 
    view->tree[notInView].parentRelation = &view->tree[inView]; 
    
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
        //below notInView because the edges leading to inView are hidden by the filter
        //and thus the bfs will not follow those edges.
        //the visitor will add those edges directly to the view
        TreeBuilderVisitor<Graph<F,E>> visitor(*view, *this);
        boost::breadth_first_search(fg, notInView, boost::visitor(visitor));
    }
}

template <class F, class E>
void Graph<F,E>::rebuildTreeViews() const
{
    for(TreeView* view : subscribedTreeViews)
    {
        view->clear();
        getTree(view->root, view);
        view->treeUpdated();//notify owners of the view, that it has been updated.
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

}}}

