#pragma once
#include <envire_core/items/ItemBase.hpp>
#include <envire_core/graph/GraphTypes.hpp>
#include "GraphEvent.hpp"

namespace envire { namespace core
{
    class EdgeAddedEvent : public GraphEvent
    {
    public:
      EdgeAddedEvent(const FrameId& origin,
                     const FrameId& target,
                     const GraphTraits::edge_descriptor& edge) :
        GraphEvent(GraphEvent::EDGE_ADDED), origin(origin), target(target),
        edge(edge){}

      FrameId origin;/**<Source vertex of the transform */
      FrameId target; /**<Target vertex of the transform */
      GraphTraits::edge_descriptor edge; /**<Edge of the tree that the transformation is attached to */
    };
}}
