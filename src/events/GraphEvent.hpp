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
            TRANSFORMATION_MODIFIED
        };
        GraphEvent(const Type type) : type(type) {}
        virtual ~GraphEvent() {}

        Type type;
    };
}}
