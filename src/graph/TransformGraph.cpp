/*
 * TransformTree.cpp
 *
 *  Created on: Sep 21, 2015
 *      Author: aboeckmann
 */
#include "TransformGraph.hpp"
#include "TransformGraphExceptions.hpp"
#include <envire_core/events/TransformAddedEvent.hpp>
#include <envire_core/events/TransformRemovedEvent.hpp>
#include <envire_core/events/TransformModifiedEvent.hpp>
#include <envire_core/events/FrameAddedEvent.hpp>
#include <envire_core/events/FrameRemovedEvent.hpp>
#include <type_traits> //For is_same()
#include <boost/graph/filtered_graph.hpp>

using namespace envire::core;
using namespace std;

TransformGraph::~TransformGraph()
{
}


TransformGraph::TransformGraph(envire::core::Environment const &environment) :
    LabeledTransformGraph (environment)
{
    static_assert(is_same<graph_type::graph_type::graph_type::vertex_list_selector, boost::listS>::value,
                  "vertex list type should be listS to ensure that vertex_descriptors remain valid");
    static_assert(is_same<graph_type::graph_type::graph_type::edge_list_selector, boost::listS>::value,
                  "edge list type should be listS to ensure that vertex_descriptors remain valid");
}

void TransformGraph::addFrame(const FrameId& frame)
{
    vertex_descriptor desc = vertex(frame);
    if(desc == null_vertex())
    {
        desc = add_vertex(frame);
    }
    else
    {
      throw FrameAlreadyExistsException(frame);
    }
}

void TransformGraph::addTransform(const vertex_descriptor origin,
                                  const vertex_descriptor target,
                                  const Transform& tf)
{
    //just a convenience method, it is not actually faster because
    //we need the FrameIds anyway to create the events
    addTransform(getFrameId(origin), getFrameId(target), tf);
}


void TransformGraph::addTransform(const FrameId& origin, const FrameId& target,
                                 const Transform& tf)
{
    addTransform(origin, target, vertex(origin), vertex(target), tf);
}

void TransformGraph::addTransform(const FrameId& origin, const FrameId& target,
                                  vertex_descriptor originDesc, vertex_descriptor targetDesc,
                                  const Transform& tf)
{
    //if they don't exist create them
    if(originDesc == null_vertex())
    {
        originDesc = add_vertex(origin);
    }
    if(targetDesc == null_vertex())
    {
        targetDesc = add_vertex(target);
    }

    //check if there is an edge between the vertices.
    //If a->b exists, b->a also exist. Therefore we need to check only one direction
    EdgePair e = boost::edge(originDesc, targetDesc, *this);
    if(e.second)// edge already exists
    {
        throw TransformAlreadyExistsException(origin, target);
    }
    //we always add two edges, one for the transform and one for the inverse transform
    edge_descriptor originToTarget = add_edge(originDesc, targetDesc, tf, origin, target);

    Transform invTf = tf;
    invTf.setTransform(invTf.transform.inverse());
    edge_descriptor targetToOrigin = add_edge(targetDesc, originDesc, invTf, target, origin);
    
    //note: we only need to add one of the edges to the tree, because the tree
    //      does not care about the edge direction.
    addEdgeToTreeViews(originToTarget);
    
    
}




const Transform TransformGraph::getTransform(const vertex_descriptor originVertex,
                                             const vertex_descriptor targetVertex) const
{
    if(num_edges() == 0)
    {
        throw UnknownTransformException(getFrameId(originVertex), getFrameId(targetVertex));
    }
    //direct edges
    EdgePair pair;
    pair = boost::edge(originVertex, targetVertex, *this);
    if(!pair.second)
    {
        /** It is not a direct edge transformation **/
        Transform tf;
        envire::core::TransformGraphBFSVisitor <vertex_descriptor>visit(targetVertex, this->graph());

        // check whether the vertices exist in the graph
        // search algorithm result on segfault in case some of the vertices do not exist
        if (originVertex == null_vertex() || targetVertex == null_vertex())
        {
            throw UnknownTransformException(getFrameId(originVertex), getFrameId(targetVertex));
        }

        try
        {
            boost::breadth_first_search(this->graph(), originVertex, visitor(visit));

        }catch(const FoundFrameException &e)
        {
            //std::cout<< e.what() << std::endl;
            base::TransformWithCovariance &trans(tf.transform);

            /** Compute the transformation **/
            trans.setTransform (base::Affine3d::Identity());
            std::deque<vertex_descriptor>::iterator it = visit.tree->begin();
            for (; (it+1) != visit.tree->end(); ++it)
            {
                pair = boost::edge(*it, *(it+1), graph());
                trans = trans * (*this)[pair.first].transform.transform;
            }
        }

        if(tf.transform.hasValidTransform())
        {
            return tf;
        }
        else
        {
            throw UnknownTransformException(getFrameId(originVertex), getFrameId(targetVertex));
        }
    }

    return (*this)[pair.first].transform;
}

const Transform TransformGraph::getTransform(const FrameId& origin, const FrameId& target) const
{
    if(num_edges() == 0)
    {
        throw UnknownTransformException(origin, target);
    }
    const vertex_descriptor originVertex = getVertex(origin);//will throw
    const vertex_descriptor targetVertex = getVertex(target); //will throw   
    return getTransform(originVertex, targetVertex);
}

const Transform TransformGraph::getTransform(const vertex_descriptor originVertex,
                                             const vertex_descriptor targetVertex,
                                             const TreeView &view) const
{
    if (originVertex == targetVertex)
    {
        /* An identity transformation **/
        return Transform(Eigen::Vector3d::Zero(), Eigen::Quaterniond::Identity());
    }

    base::TransformWithCovariance origin_tf(base::Affine3d::Identity()); // An identity transformation

    /** Get transformation from origin to the root **/
    vertex_descriptor od = originVertex;
    while(!view.isRoot(od))
    {
        EdgePair pair(boost::edge(od, view.tree.at(od).parent, *this));
        if (pair.second)
        {
            origin_tf = origin_tf * (*this)[pair.first].transform.transform;
        }
        od = view.tree.at(od).parent;
    }

    base::TransformWithCovariance target_tf(base::Affine3d::Identity()); // An identity transformation

    /** Get transformation from target to the root **/
    vertex_descriptor td = targetVertex;
    while(!view.isRoot(td))
    {
        EdgePair pair;
        pair = boost::edge(td, view.tree.at(td).parent, *this);
        if (pair.second)
        {
            target_tf = target_tf * (*this)[pair.first].transform.transform;
        }
        td = view.tree.at(td).parent;
    }

    return origin_tf * target_tf.inverse();
}

const Transform TransformGraph::getTransform(const FrameId& origin, const FrameId& target, const TreeView &view) const
{
    const vertex_descriptor originVertex = getVertex(origin);//will throw
    const vertex_descriptor targetVertex = getVertex(target); //will throw
    return getTransform(originVertex, targetVertex, view);
}

void TransformGraph::updateTransform(const FrameId& origin, const FrameId& target,
                                    const Transform& tf)
{
    const vertex_descriptor originVertex = getVertex(origin);//will throw
    const vertex_descriptor targetVertex = getVertex(target); //will throw
    return updateTransform(originVertex, targetVertex, tf);
}

void TransformGraph::updateTransform(const vertex_descriptor origin,
                                     const vertex_descriptor target,
                                     const Transform& tf)
{
    const FrameId& originId = getFrameId(origin); //throws UnknownFrameException
    const FrameId& targetId = getFrameId(target); //throws UnknownFrameException
    
    if(num_edges() <= 0)
    {   //boost::edge_by_label segfaults on empty graphs...
        throw UnknownTransformException(originId, targetId);
    }
    
    EdgePair originToTarget = boost::edge(origin, target, *this);
    
    if(!originToTarget.second)
    {
      throw UnknownTransformException(originId, targetId);
    } 
    
    updateTransform(originToTarget.first, tf);
    
    Transform invTf = tf;//copies the time
    invTf.setTransform(tf.transform.inverse());
    EdgePair targetToOrigin = boost::edge(target, origin, *this);
    assert(targetToOrigin.second);//there should always be an inverse edge
    updateTransform(targetToOrigin.first, invTf);
    
    notify(TransformModifiedEvent(originId, targetId, originToTarget.first,
                                  targetToOrigin.first));    
}


void TransformGraph::removeTransform(const FrameId& origin, const FrameId& target)
{
    //note: do not use boost::edge_by_label as it will segfault if one of the
    //frames is not part of the tree.

    vertex_descriptor originDesc = vertex(origin);
    vertex_descriptor targetDesc = vertex(target);
    if(originDesc == null_vertex() || targetDesc == null_vertex())
    {
        throw UnknownTransformException(origin, target);
    }

    EdgePair originToTarget = boost::edge(originDesc, targetDesc, graph());
    EdgePair targetToOrigin = boost::edge(targetDesc, originDesc, graph());
    if(!originToTarget.second || !targetToOrigin.second)
    {
        throw UnknownTransformException(origin, target);
    }
    
    boost::remove_edge(originToTarget.first, *this);
    notify(TransformRemovedEvent(origin, target));
    
    boost::remove_edge(targetToOrigin.first, *this);
    notify(TransformRemovedEvent(target, origin));
    
    //removing a transform might invalidate the TreeViews.
    //This is a brute-force solution to the problem.
    //FIXME update TreeViews when removing a transform instead of rebuilding them
    rebuildTreeViews();
}

edge_descriptor TransformGraph::add_edge(const vertex_descriptor origin,
                                         const vertex_descriptor target,
                                         const envire::core::Transform &tf,
                                         const FrameId& originName,
                                         const FrameId& targetName)
{
    TransformProperty tf_prop{tf};
    auto edge_pair =  boost::add_edge(origin, target, tf_prop, *this);
    notify(TransformAddedEvent(originName, targetName, edge_pair.first, tf));
    return edge_pair.first;
}

vertices_size_type TransformGraph::num_vertices() const
{
    return boost::num_vertices(*this);
}

edges_size_type TransformGraph::num_edges() const
{
    return boost::num_edges(*this);
}

vertex_descriptor TransformGraph::add_vertex(const FrameId& frameId)
{
    FrameProperty node_prop(frameId);
    vertex_descriptor v = LabeledTransformGraph::add_vertex(frameId, node_prop);
    notify(FrameAddedEvent(frameId));
    return v;
}

void TransformGraph::clearFrame(const FrameId& frame)
{
    checkFrameValid(frame);
    auto& items = (*this)[frame].frame.items;
    
    for(Frame::ItemMap::iterator it = items.begin(); it != items.end();)
    {
        std::type_index key = it->first;
        Frame::ItemList& list = it->second;
        for(auto it = list.begin(); it != list.end();)
        {
            ItemBase::Ptr removedItem = *it;
            it = list.erase(it);
            notify(ItemRemovedEvent(frame, removedItem, key));
        }
        it = items.erase(it);
    }
}

void TransformGraph::removeFrame(const FrameId& frame)
{
    vertex_descriptor desc = getVertex(frame); //will throw
    if(boost::degree(desc, *this) > 0)
    {
        throw FrameStillConnectedException(frame);
    }
    
    //explicitly remove all items from the frame to cause ItemRemovedEvents
    clearFrame(frame);
    
    boost::remove_vertex(frame, *this);
    //HACK this is a workaround for bug https://svn.boost.org/trac/boost/ticket/9493
    //If the bug is fixed also remove the #define private protected in TransformTreeTypes.hpp
    map_type::iterator it = _map.find(frame);
    if(it != _map.end())
    {
        _map.erase(it);
    }
    notify(FrameRemovedEvent(frame));
}

const envire::core::Frame& TransformGraph::getFrame(const FrameId& frame) const
{
    checkFrameValid(frame);
    return (*this)[frame].frame;
}

const envire::core::Frame& TransformGraph::getFrame(const vertex_descriptor desc) const
{
    return graph()[desc].frame;
}

edge_descriptor TransformGraph::getEdge(const FrameId& origin, const FrameId& target) const
{
    vertex_descriptor originDesc = vertex(origin);
    vertex_descriptor targetDesc = vertex(target);

    if(originDesc == null_vertex() || targetDesc == null_vertex())
    {//boost segfaults if one of them is null
        throw UnknownTransformException(origin, target);
    }

    EdgePair e = boost::edge(originDesc, targetDesc, graph());
    if(!e.second)
    {
        throw UnknownTransformException(origin, target);
    }
    return e.first;
}

void TransformGraph::updateTransform(edge_descriptor ed, const Transform& tf)
{
    boost::put(&TransformProperty::transform, *this, ed, tf);
}

const envire::core::FrameId& TransformGraph::getFrameId(const vertex_descriptor vertex) const
{
    return getFrame(vertex).getName();
}


TreeView TransformGraph::getTree(const vertex_descriptor root) const
{
    TreeView view(root);
    getTree(root, &view);
    return std::move(view);
}

TreeView TransformGraph::getTree(const FrameId rootId) const
{
    const vertex_descriptor root = getVertex(rootId);
    return getTree(root);
}

void TransformGraph::getTree(const FrameId rootId, TreeView* outView) const
{
    vertex_descriptor root = getVertex(rootId);
    getTree(root, outView);
}

void TransformGraph::getTree(const vertex_descriptor root, TreeView* outView) const
{
    outView->root = root;
    
    if(boost::degree(root, graph()) == 0)
    {
        //the TreeBuilderVisitor only looks at the edges, if there are no dges
        //it wouldn't add the root node
        outView->tree[root].parent = null_vertex();
    }
    else
    {
        TreeBuilderVisitor visitor(*outView, *this);
        boost::breadth_first_search(*this, root, boost::visitor(visitor));
    }
}


void TransformGraph::getTree(const vertex_descriptor root, const bool keepTreeUpdated, TreeView* outView)
{
    getTree(root, outView);    
    if(keepTreeUpdated)
    {
      subscribeTreeView(outView);
    }
}

void TransformGraph::getTree(const FrameId rootId, const bool keepTreeUpdated, TreeView* outView)
{
    const vertex_descriptor root = getVertex(rootId);
    getTree(root, keepTreeUpdated, outView);
}

Path TransformGraph::getPath(FrameId origin, FrameId target) const
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
        std::deque<vertex_descriptor>::iterator it = visit.tree->begin();
        for (; (it+1) != visit.tree->end(); ++it)
        {
            path.push_back(getFrameId(*it));
        }
    }
    if(path.size() > 0)
    {
        path.push_back(target);
    }
    return path;
}

void TransformGraph::disconnectFrame(const FrameId& frame)
{
    vertex_descriptor desc = getVertex(frame);
    boost::clear_vertex(desc, *this);

}

void TransformGraph::checkFrameValid(const FrameId& frame) const
{
    if(vertex(frame) == null_vertex())
    {
        throw UnknownFrameException(frame);
    }
}

vertex_descriptor TransformGraph::getVertex(const FrameId& frame) const
{
    vertex_descriptor desc = vertex(frame);
    if(desc == null_vertex())
    {
        throw UnknownFrameException(frame);
    }
    return desc;
}

const vertex_descriptor TransformGraph::source(const edge_descriptor edge) const
{
    return boost::source(edge, graph());
}
const vertex_descriptor TransformGraph::target(const edge_descriptor edge) const
{
    return boost::target(edge, graph());
}

void TransformGraph::unsubscribeTreeView(TreeView* view)
{
    subscribedTreeViews.erase(std::remove(subscribedTreeViews.begin(),
                                          subscribedTreeViews.end(), view),
                              subscribedTreeViews.end());
}

void TransformGraph::subscribeTreeView(TreeView* view)
{
    assert(view != nullptr);
    subscribedTreeViews.push_back(view);
    view->setPublisher(this); //now the TreeView will automatically unsubscribe on destruction
}


void TransformGraph::addEdgeToTreeViews(edge_descriptor newEdge) const
{
    for(TreeView* view : subscribedTreeViews)
    {
        addEdgeToTreeView(newEdge, view);
        view->treeUpdated();//notify owners of the view, that it has been updated.
    }
}



void TransformGraph::addEdgeToTreeView(edge_descriptor newEdge, TreeView* view) const
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
        if(!edgeExists(src, tar, view))
        {
            //if both vertices are in the tree but there is no edge between them
            //this is a cross edge
            view->crossEdges.push_back(newEdge);
            return;
        }
        else
        {
            //otherwise it's a back-edge that can be ignored
            //addTransform() only calls this method once, therefore back-edges
            //should never occur
            assert(false);
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
    
    //FIXME there might be a whole tree connected to notInView which should
    //be added to the tree
    view->tree[inView].children.insert(notInView);
    view->tree[notInView].parent = inView; 
    
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
        boost::filtered_graph<const TransformGraph, EdgeFilter> fg(*this, filter);
        
        //use TreeBuilderVisitor to generate a new tree starting from notInView.
        //This tree will only contain vertices that are part of the sub tree that
        //below notInView because the edges leading to inView are hidden by the filter
        //and thus the bfs will not follow those edges.
        //the visitor will add those edges directly to the view
        TreeBuilderVisitor visitor(*view, *this);
        boost::breadth_first_search(fg, notInView, boost::visitor(visitor));
    }
}

bool TransformGraph::edgeExists(const vertex_descriptor a, const vertex_descriptor b,
                                const TreeView* view) const
{
  //an edge exists if either a is the parent of b and aChildren contains b
  //or the other way around.
  
  const VertexRelation& aRelation = view->tree.at(a);
  const VertexRelation& bRelation = view->tree.at(b);
  
  //If we assume that we made no mistake when populating the tree we could just 
  //return (aRelation.parent == b || bRelation.parent == a)
  //but using asserts is always better :D
  
  //the if will be optimized out if asserts are disabled
  if(aRelation.parent == b) //b is parent of a
  {
    assert(bRelation.children.find(a) != bRelation.children.end());
  }
  else if(bRelation.parent == a) //a is parent of b
  {
    assert(aRelation.children.find(b) != aRelation.children.end());
  }
  return aRelation.parent == b || bRelation.parent == a;
}


void TransformGraph::rebuildTreeViews() const
{
    for(TreeView* view : subscribedTreeViews)
    {
        view->clear();
        getTree(view->root, view);
        view->treeUpdated();//notify owners of the view, that it has been updated.
    }
}



