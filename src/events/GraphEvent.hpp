#pragma once
#include <envire_core/events/GraphEventExceptions.hpp>

namespace envire { namespace core
{

    /** TODO comment
     */
    class GraphEvent
    {
    public:
        //all possible events
        enum Type
        {
            EDGE_ADDED,
            EDGE_REMOVED,
            EDGE_MODIFIED,
            ITEM_ADDED_TO_FRAME,
            ITEM_REMOVED_FROM_FRAME,
            FRAME_ADDED,
            FRAME_REMOVED
        };

        GraphEvent() = delete;
        virtual ~GraphEvent() {}

        /**
         * This method is used to identify if a new event supersedes and existing event.
         * @returns true if the given event supersedes this one.
         */
        virtual bool mergeable(const GraphEvent& event)
        {
            // not implemented
            return false;
        }

        /**
         * @returns the type
         */
        Type getType() const { return type; }

        /**
         * This method can be overloaded to allow the event to be cloned.
         */
        virtual GraphEvent* clone() const { throw CloneMethodNotImplementedException(); }

        friend std::ostream& operator<<(std::ostream&, const GraphEvent&);

    protected:
        explicit GraphEvent(const Type type) : type(type) {}

        Type type;
    };

}}
