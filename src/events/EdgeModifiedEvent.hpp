#pragma once
#include <envire_core/graph/GraphTypes.hpp>
#include <envire_core/items/ItemBase.hpp>
#include "GraphEvent.hpp"

namespace envire { namespace core
{
    class EdgeModifiedEvent : public GraphEvent
    {
    public:
      EdgeModifiedEvent(const FrameId& origin,
                        const FrameId& target,
                        const GraphTraits::edge_descriptor edge,
                        const GraphTraits::edge_descriptor inverseEdge) :
        GraphEvent(GraphEvent::EDGE_MODIFIED), origin(origin), target(target),
        edge(edge), inverseEdge(inverseEdge){}

      FrameId origin;/**<Source vertex of the transform */
      FrameId target; /**<Target vertex of the transform */
      GraphTraits::edge_descriptor edge; /**<Edge of the tree that the transformation is attached to */
      GraphTraits::edge_descriptor inverseEdge; 
    };
}}
