//
// Copyright (c) 2015, Deutsches Forschungszentrum für Künstliche Intelligenz GmbH.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#pragma once

#include <envire_core/items/ItemBase.hpp>
#include <envire_core/graph/GraphTypes.hpp>
#include <envire_core/events/GraphEvent.hpp>

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
      //EdgeRemovedEvent does not contain an edge_descriptor because it has already been
      //removed from the graph when the event is raised and thus doesnt exist anymore.
        EdgeRemovedEvent(const FrameId& origin, const FrameId& target) :
            EdgeEvent(GraphEvent::EDGE_REMOVED, origin, target) {}

        GraphEvent* clone() const
        {
            return new EdgeRemovedEvent(origin, target);
        }
    };
}}
