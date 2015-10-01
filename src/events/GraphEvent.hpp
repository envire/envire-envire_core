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
            TRANSFORMATION_ADDED,
            TRANSFORMATION_REMOVED,
            TRANSFORMATION_MODIFIED,
            ITEM_ADDED_TO_FRAME,
            ITEM_REMOVED_FROM_FRAME
        };
        GraphEvent(const Type type) : type(type) {}
        virtual ~GraphEvent() {}

        Type type;
    };
}}
