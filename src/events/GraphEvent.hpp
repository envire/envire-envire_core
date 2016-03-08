#pragma once

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
         * @returns the type
         */
        Type getType() const { return type; }

    protected:
        explicit GraphEvent(const Type type) : type(type) {}

        Type type;
    };
}}
