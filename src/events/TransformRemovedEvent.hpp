#pragma once
#include <envire_core/TransformTreeTypes.hpp>
#include <envire_core/Transform.hpp>
#include <envire_core/Frame.hpp>
#include "TreeEvent.hpp"

namespace envire { namespace core
{
    class TransformRemovedEvent : public TreeEvent
    {
    public:
      TransformRemovedEvent(const FrameId& origin,
                            const FrameId& target) :
        TreeEvent(TreeEvent::TRANSFORMATION_REMOVED), origin(origin), target(target){}

      FrameId origin;/**<Source vertex of the transform */
      FrameId target; /**<Target vertex of the transform */
    };
}}
