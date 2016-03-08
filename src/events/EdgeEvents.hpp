#pragma once

#include <envire_core/items/ItemBase.hpp>
#include <envire_core/graph/GraphTypes.hpp>
#include "GraphEvent.hpp"

namespace envire { namespace core
{
    class EdgeEvent : public GraphEvent
    {
    public:
        EdgeEvent() = delete;

        virtual bool mergeable(const GraphEvent& event)
        {
            /* We assume here the only allowed order of events is [added -> modified -> removed].
             * In this case removed supersedes all other events and modified only supersedes other
             * modified events.
             */
            if((event.getType() == EDGE_MODIFIED && type == EDGE_MODIFIED) || event.getType() == EDGE_REMOVED)
            {
                // check if the edge is the same
                const EdgeEvent& edge_event = dynamic_cast<const EdgeEvent&>(event);
                if(*this == edge_event)
                    return true;
            }
            return false;
        }

        FrameId origin;/**<Source vertex of the transform */
        FrameId target; /**<Target vertex of the transform */

    protected:
        EdgeEvent(const Type type,
                    const FrameId& origin,
                    const FrameId& target) :
            GraphEvent(type), origin(origin), target(target) {}


        bool operator==(const EdgeEvent& edge_event) const
        {
            return (origin == edge_event.origin && target == edge_event.target) ||
                   (origin == edge_event.target && target == edge_event.origin);
        }
    };


    class EdgeAddedEvent : public EdgeEvent
    {
    public:
        EdgeAddedEvent(const FrameId& origin,
                        const FrameId& target,
                        const GraphTraits::edge_descriptor& edge) :
            EdgeEvent(GraphEvent::EDGE_ADDED, origin, target), edge(edge){}

        GraphEvent* clone() const
        {
            return new EdgeAddedEvent(origin, target, edge);
        }

        GraphTraits::edge_descriptor edge; /**<Edge of the tree that the transformation is attached to */
    };


    class EdgeModifiedEvent : public EdgeEvent
    {
    public:
        EdgeModifiedEvent(const FrameId& origin,
                        const FrameId& target,
                        const GraphTraits::edge_descriptor edge,
                        const GraphTraits::edge_descriptor inverseEdge) :
        EdgeEvent(GraphEvent::EDGE_MODIFIED, origin, target), edge(edge), inverseEdge(inverseEdge){}

        GraphEvent* clone() const
        {
            return new EdgeModifiedEvent(origin, target, edge, inverseEdge);
        }

        GraphTraits::edge_descriptor edge; /**<Edge of the tree that the transformation is attached to */
        GraphTraits::edge_descriptor inverseEdge;
    };

    
    class EdgeRemovedEvent : public EdgeEvent
    {
    public:
        EdgeRemovedEvent(const FrameId& origin, const FrameId& target) :
            EdgeEvent(GraphEvent::EDGE_REMOVED, origin, target) {}

        GraphEvent* clone() const
        {
            return new EdgeRemovedEvent(origin, target);
        }
    };
}}
