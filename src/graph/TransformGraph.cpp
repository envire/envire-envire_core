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

using namespace envire::core;
using namespace std;



TransformGraph::TransformGraph(envire::core::Environment const &environment) :
    LabeledTransformGraph (environment)
{
    static_assert(is_same<Base::graph_type::graph_type::graph_type::vertex_list_selector, boost::listS>::value,
                  "vertex list type should be listS to ensure that vertex_descriptors remain valid");
    static_assert(is_same<Base::graph_type::graph_type::graph_type::edge_list_selector, boost::listS>::value,
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


void TransformGraph::addTransform(const FrameId& origin, const FrameId& target,
                                 const Transform& tf)
{
    //try to find vertices
    vertex_descriptor originDesc = vertex(origin);
    vertex_descriptor targetDesc = vertex(target);
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
}


const Transform TransformGraph::getTransform(const FrameId& origin, const FrameId& target,
                                             const vertex_descriptor originVertex,
                                             const vertex_descriptor targetVertex) const
{
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
            throw UnknownTransformException(origin, target);
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
            throw UnknownTransformException(origin, target);
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
    return getTransform(origin, target, vertex(origin), vertex(target));
}

const Transform TransformGraph::getTransform(const vertex_descriptor origin, const vertex_descriptor target) const
{
    if(num_edges() == 0)
    {
        throw UnknownTransformException(getFrameId(origin),  getFrameId(target));
    }
    return getTransform(getFrameId(origin), getFrameId(target), origin, target);    
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


VertexMap TransformGraph::getTree(const vertex_descriptor root) const
{
    VertexMap map;
    TreeBuilderVisitor visitor(map);
    boost::breadth_first_search(*this, root, boost::visitor(visitor));
    return map;
}

VertexMap TransformGraph::getTree(const FrameId rootId) const
{
    const vertex_descriptor root = getVertex(rootId);
    return getTree(root);
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


