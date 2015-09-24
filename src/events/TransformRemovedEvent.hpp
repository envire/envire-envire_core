#pragma once
#include <envire_core/TransformGraphTypes.hpp>
#include <envire_core/Transform.hpp>
#include <envire_core/Frame.hpp>
#include "GraphEvent.hpp"

namespace envire { namespace core
{
    class TransformRemovedEvent : public GraphEvent
    {
    public:
      TransformRemovedEvent(const FrameId& origin,
                            const FrameId& target) :
        GraphEvent(GraphEvent::TRANSFORMATION_REMOVED), origin(origin), target(target){}

      FrameId origin;/**<Source vertex of the transform */
      FrameId target; /**<Target vertex of the transform */
    };
}}
