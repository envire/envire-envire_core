#pragma once
#include <envire_core/graph/TransformGraphTypes.hpp>
#include <envire_core/items/Frame.hpp>
#include "GraphEvent.hpp"

namespace envire { namespace core
{
    class EdgeRemovedEvent : public GraphEvent
    {
    public:
      EdgeRemovedEvent(const FrameId& origin,
                            const FrameId& target) :
        GraphEvent(GraphEvent::EDGE_REMOVED), origin(origin), target(target){}

      FrameId origin;/**<Source vertex of the transform */
      FrameId target; /**<Target vertex of the transform */
    };
}}
