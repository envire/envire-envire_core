#pragma once

#include <type_traits>
#include <envire_core/events/GraphEventPublisher.hpp>

#include "GraphTypes.hpp"
#include "TreeView.hpp"
#include "TransformGraphExceptions.hpp"


namespace envire { namespace core { namespace graph
{

/**A labeled graph that provides and manages different views of the graph
 * @param FRAME_PROP should extend FramePropertyBase */
template <class FRAME_PROP, class EDGE_PROP>
class Graph : public GraphBase<FRAME_PROP, EDGE_PROP>, 
              public envire::core::GraphEventPublisher,
              public envire::core::TreeUpdatePublisher
{
public:
    using vertex_descriptor = typename GraphBase<FRAME_PROP, EDGE_PROP>::vertex_descriptor;
    using edge_descriptor = typename GraphBase<FRAME_PROP, EDGE_PROP>::edge_descriptor;
    using map_type = typename GraphBase<FRAME_PROP, EDGE_PROP>::map_type;
    using GraphBase<FRAME_PROP, EDGE_PROP>::vertex;
    using GraphBase<FRAME_PROP, EDGE_PROP>::null_vertex;
    using GraphBase<FRAME_PROP, EDGE_PROP>::graph;
    using GraphBase<FRAME_PROP, EDGE_PROP>::_map;
    
    Graph();
    
    /** Adds an unconnected frame to the graph.
    * 
    * @throw FrameAlreadyExistsException if the frame already exists
    */
    void addFrame(const FrameId& frame);
    
    /** @return the edge between frame @p origin and @p target
      * @throw UnknownFrameException If @p orign or @p target do not exist.
      * @throw UnknownTransformException if there is no such edge  */
    edge_descriptor getEdge(const FrameId& origin, const FrameId& target) const;
    edge_descriptor getEdge(const vertex_descriptor origin, const vertex_descriptor target) const;
    
    /**Gets the vertex corresponding to @p frame.
    * @throw UnknownFrameException if the frame does not exist */
    vertex_descriptor getVertex(const FrameId& frame) const;
    
    /** @return the frame id of the specified @p vertex */
    const envire::core::FrameId& getFrameId(const vertex_descriptor vertex) const;

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

  
  
protected:
    using EdgePair = std::pair<edge_descriptor, bool>;
    /**@brief Add a vertex
    * @note the frame's name must be unique. */
    vertex_descriptor add_vertex(const FrameId& frameId, const FRAME_PROP& frame);
  
};






template <class F, class E>
Graph<F,E>::Graph()
{
  
    static_assert(std::is_same<typename GraphBase<F, E>::graph_type::graph_type::graph_type::vertex_list_selector, boost::listS>::value,
                  "vertex list type should be listS to ensure that vertex_descriptors remain valid");
    static_assert(std::is_same<typename GraphBase<F, E>::graph_type::graph_type::graph_type::edge_list_selector, boost::listS>::value,
                  "edge list type should be listS to ensure that vertex_descriptors remain valid");
    static_assert(std::is_base_of<FramePropertyBase, F>::value, "FRAME_PROP should derive from FramePropertyBase");
    
  
}

template <class F, class E>
void Graph<F,E>::addFrame(const FrameId& frame)
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
        throw UnknownTransformException(getFrameId(origin), getFrameId(target));
    }
    return e.first;
}

template <class F, class E>
const FrameId& Graph<F,E>::getFrameId(const vertex_descriptor vertex) const
{
    return graph()[vertex].id;
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


}}}

