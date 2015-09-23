/*
 * TransformTree.cpp
 *
 *  Created on: Sep 21, 2015
 *      Author: aboeckmann
 */
#include "TransformTree.hpp"

using namespace envire::core;
using edgePair = std::pair<edge_descriptor, bool>;

void TransformTree::addTransform(const FrameId& origin, const FrameId& target,
                                 const Transform& tf)
{
    //try to find vertices
    vertex_descriptor originDesc = vertex(origin);
    vertex_descriptor targetDesc = vertex(target);
    //if they don't exist create them
    if(originDesc == null_vertex())
        originDesc = add_vertex(origin);
    if(targetDesc == null_vertex())
        targetDesc = add_vertex(target);

    //check if there is an edge between the vertices.
    //If a->b exists, b->a also exist. Therefore we need to check only one direction
    edgePair e = boost::edge(originDesc, targetDesc, *this);
    if(e.second)// edge already exists
    {
        throw TransformAlreadyExistsException(origin, target);
    }
    //we always add two edges, one for the transform and one for the inverse transform
    edge_descriptor originToTarget = add_edge(originDesc, targetDesc, tf);

    Transform invTf = tf;
    invTf.setTransform(invTf.transform.inverse());
    edge_descriptor targetToOrigin = add_edge(targetDesc, originDesc, invTf);
}

const Transform& TransformTree::getTransform(const FrameId& a, const FrameId& b) const
{
    //calling boost::edge_by_label on an empty graph results in a segfault.
    //therefore catch it before that happens.
    if(num_edges() == 0)
    {
        throw UnknownTransformException(a, b);
    }
    //FIXME for now this only works with direct edges
    edgePair pair = boost::edge_by_label(a, b, *this);
    if(!pair.second)
    {
        throw UnknownTransformException(a, b);
    }
    return (*this)[pair.first].transform;
}

void TransformTree::updateTransform(const FrameId& origin, const FrameId& target,
                                    const Transform& tf)
{
    if(num_edges() <= 0)
    {   //boost::edge_by_label segfaults on empty graphs...
        throw UnknownTransformException(origin, target);
    }

    edgePair originToTarget = boost::edge_by_label(origin, target, *this);
    if(!originToTarget.second)
    {
      throw UnknownTransformException(origin, target);
    }

    updateTransform(originToTarget.first, tf, origin, target);
    Transform invTf = tf;//copies the time
    invTf.setTransform(tf.transform.inverse());
    edgePair targetToOrigin = boost::edge_by_label(target, origin, *this);
    assert(targetToOrigin.second);//there should always be an inverse edge
    updateTransform(targetToOrigin.first, invTf, target, origin);
}

void TransformTree::removeTransform(const FrameId& origin, const FrameId& target)
{
    //note: do not use boost::edge_by_label as it will segfault if one of the
    //frames is not part of the tree.

    vertex_descriptor originDesc = vertex(origin);
    vertex_descriptor targetDesc = vertex(target);
    if(originDesc == null_vertex() || targetDesc == null_vertex())
    {
        throw UnknownTransformException(origin, target);
    }

    edgePair originToTarget = boost::edge(originDesc, targetDesc, graph());
    edgePair targetToOrigin = boost::edge(targetDesc, originDesc, graph());
    if(!originToTarget.second || !targetToOrigin.second)
    {
        throw UnknownTransformException(origin, target);
    }
    boost::remove_edge(originToTarget.first, *this);
    notify(TransformRemovedEvent(origin, target));
    boost::remove_edge(targetToOrigin.first, *this);
    notify(TransformRemovedEvent(target, origin));

    //remove dangling frames
    if(boost::degree(vertex(origin), *this) <= 0)
    {
        remove_frame(origin);
    }
    if(boost::degree(vertex(target), *this) <= 0)
    {
        remove_frame(target);
    }
}

edge_descriptor TransformTree::add_edge(const vertex_descriptor node_from,
                                        const vertex_descriptor node_to,
                                        const envire::core::Transform &tf)
{
    TransformProperty tf_prop;
    tf_prop.transform = tf;
    auto edge_pair =  boost::add_edge(node_from, node_to, tf_prop, *this);
    notify(TransformAddedEvent(node_from, node_to, edge_pair.first, tf));
    return edge_pair.first;
}

vertices_size_type TransformTree::num_vertices() const
{
    return boost::num_vertices(*this);
}

edges_size_type TransformTree::num_edges() const
{
    return boost::num_edges(*this);
}

vertex_descriptor TransformTree::add_vertex(const FrameId& frameId)
{
    FrameProperty node_prop;
    vertex_descriptor v = LabeledTransformGraph::add_vertex(frameId, node_prop);
    return v;
}

void TransformTree::remove_frame(FrameId fId)
{
    assert(boost::degree(vertex(fId), *this) <= 0);
    boost::remove_vertex(fId, *this);
    //HACK this is a workaround for bug https://svn.boost.org/trac/boost/ticket/9493
    //It should be removed as soon as the bug is fixed in boost.
    //If the bug is fixed also remove the #define private protected in TransformTreeTypes
    map_type::iterator it = _map.find(fId);
    if(it != _map.end())
        _map.erase(it);
}

const envire::core::Frame& TransformTree::getFrame(const FrameId& frame)
{
    if(vertex(frame) == null_vertex())
    {
        throw UnknownFrameException(frame);
    }
    return (*this)[frame].frame;
}

edge_descriptor TransformTree::getEdge(const FrameId& origin, const FrameId& target) const
{
    vertex_descriptor originDesc = vertex(origin);
    vertex_descriptor targetDesc = vertex(target);

    if(originDesc == null_vertex() || targetDesc == null_vertex())
    {//boost segfaults if one of them is null
        throw UnknownTransformException(origin, target);
    }

    edgePair e = boost::edge(originDesc, targetDesc, graph());
    if(!e.second)
    {
        throw UnknownTransformException(origin, target);
    }
    return e.first;
}

void TransformTree::updateTransform(edge_descriptor ed, const Transform& tf,
                                    const FrameId& origin, const FrameId& target)
{
    Transform old = (*this)[ed].transform;
    boost::put(&TransformProperty::transform, *this, ed, tf);
    notify(TransformModifiedEvent(origin, target, ed, old, (*this)[ed].transform));
}
