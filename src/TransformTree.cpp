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
    //FIXME for now this only works with direct edges
    edgePair pair = boost::edge_by_label(a, b, *this);
    if(!pair.second)
    {
        throw UnknownTransformException(a, b);
    }
    return (*this)[pair.first].transform;
}

void TransformTree::updateTransform(const FrameId& a, const FrameId& b, const Transform& tf)
{
//  edgePair aToB = boost::edge(aDesc, bDesc, *this);
//  if(!aToB.second)
//  {
//      throw UnknownTransformException(a, b);
//  }
//  setTransform(aToB.first, tf);
//  Transform invTf = tf;//copies the time
//  invTf.setTransform(tf.inverse());
//  edgePair bToA = boost::edge(aDesc, bDesc, *this);
//  assert(bToA.second);//there should always be an inverse edge
//  setTransform(bToA, invTf);
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

std::pair<edge_iterator, edge_iterator> TransformTree::edges()
{
    return boost::edges(*this);
}

std::pair<vertex_iterator, vertex_iterator> TransformTree::vertices()
{
    return boost::vertices(*this);
}

vertex_descriptor TransformTree::source(const edge_descriptor it_node)
{
    return boost::source(it_node, *this);
}

vertex_descriptor TransformTree::target(const edge_descriptor it_node)
{
    return boost::target(it_node, *this);
}

vertices_size_type TransformTree::num_vertices() const
{
    return boost::num_vertices(*this);
}

edges_size_type TransformTree::num_edges() const
{
    return boost::num_edges(*this);
}

degree_size_type TransformTree::degree(const vertex_descriptor v) const
{
    return boost::degree(v, *this);
}

vertex_descriptor TransformTree::add_vertex(const FrameId& frameId)
{
    FrameProperty node_prop;
    vertex_descriptor v = LabeledTransformGraph::add_vertex(frameId, node_prop);
    return v;
}

void TransformTree::remove_vertex(vertex_descriptor v)
{
    assert(degree(v) <= 0);
    graph().remove_vertex(v);
}

const envire::core::Frame& TransformTree::getFrame(const FrameId& frame)
{
    return (*this)[frame].frame;
}

edge_descriptor TransformTree::getEdge(const FrameId& origin, const FrameId& target) const
{
    edgePair e = boost::edge_by_label(origin, target, *this);
    if(!e.second)
    {
        throw UnknownTransformException(origin, target);
    }
    return e.first;
}

