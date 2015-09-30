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
#include <envire_core/events/ItemAddedEvent.hpp>
#include <type_traits> //For is_same()

using namespace envire::core;
using namespace std;
using edgePair = std::pair<edge_descriptor, bool>;


TransformGraph::TransformGraph(envire::core::Environment const &environment) :
    LabeledTransformGraph (environment)
{
    static_assert(is_same<Base::graph_type::graph_type::graph_type::vertex_list_selector, boost::listS>::value,
                  "vertex list type should be listS to ensure that vertex_descriptors remain valid");
    static_assert(is_same<Base::graph_type::graph_type::graph_type::edge_list_selector, boost::listS>::value,
                  "edge list type should be listS to ensure that vertex_descriptors remain valid");
}

void TransformGraph::addTransform(const FrameId& origin, const FrameId& target,
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
    edge_descriptor originToTarget = add_edge(originDesc, targetDesc, tf, origin, target);

    Transform invTf = tf;
    invTf.setTransform(invTf.transform.inverse());
    edge_descriptor targetToOrigin = add_edge(targetDesc, originDesc, invTf, target, origin);
}

const Transform TransformGraph::getTransform(const FrameId& a, const FrameId& b) const
{
    //calling boost::edge_by_label on an empty graph results in a segfault.
    //therefore catch it before that happens.
    if(num_edges() == 0)
    {
        throw UnknownTransformException(a, b);
    }

    //direct edges
    edgePair pair;
    pair = boost::edge_by_label(a, b, *this);
    if(!pair.second)
    {
        /** It is not a direct edge transformation **/
        Transform tf;
        vertex_descriptor origin_frame = vertex(a);
        vertex_descriptor target_frame = vertex(b);
        envire::core::TransformGraphBFSVisitor <vertex_descriptor>visit(target_frame, this->graph());

        // check whether the vertices exist in the graph
        // search algorithm result on segfault in case some of the vertices do not exist
        if (origin_frame == null_vertex() || target_frame == null_vertex())
        {
            throw UnknownTransformException(a, b);
        }

        try
        {
            boost::breadth_first_search(this->graph(), origin_frame, visitor(visit));

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
            throw UnknownTransformException(a, b);
        }
    }

    return (*this)[pair.first].transform;
}

void TransformGraph::updateTransform(const FrameId& origin, const FrameId& target,
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
    
    updateTransform(originToTarget.first, tf);
    
    Transform invTf = tf;//copies the time
    invTf.setTransform(tf.transform.inverse());
    edgePair targetToOrigin = boost::edge_by_label(target, origin, *this);
    assert(targetToOrigin.second);//there should always be an inverse edge
    updateTransform(targetToOrigin.first, invTf);
    
    notify(TransformModifiedEvent(origin, target, originToTarget.first,
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
    return v;
}

void TransformGraph::remove_frame(FrameId fId)
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

const envire::core::Frame& TransformGraph::getFrame(const FrameId& frame) const
{
    if(vertex(frame) == null_vertex())
    {
        throw UnknownFrameException(frame);
    }
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

    edgePair e = boost::edge(originDesc, targetDesc, graph());
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

void TransformGraph::addItemToFrame(const FrameId& frame, ItemBase::Ptr item)
{
    if(vertex(frame) == null_vertex())
    {
        throw UnknownFrameException(frame);
    }
    
    (*this)[frame].frame.items.push_back(item);
    notify(ItemAddedEvent(frame, item));
}

const std::vector<ItemBase::Ptr>& TransformGraph::getItems(const FrameId& frame) const
{
    //FIXME if item events should be added later this method needs to change because it provides a way
    //to modify items without the event system noticing
    if(vertex(frame) == null_vertex())
    {
        throw UnknownFrameException(frame);
    }
    return (*this)[frame].frame.items;
}

const std::vector<ItemBase::Ptr>& TransformGraph::getItems(const vertex_descriptor desc) const
{
    return graph()[desc].frame.items;
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


