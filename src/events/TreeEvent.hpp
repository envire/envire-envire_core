#pragma once

namespace envire { namespace core
{
    
    /** TODO comment
     */
    class TreeEvent
    {
    public:
        //all possible events
        enum Type
        {
            FRAME_ADDED,
            FRAME_MODIFIED,
            FRAME_REMOVED,
            VERTEX_ADDED,
            VERTEX_REMOVED,
        };
        TreeEvent(const Type type) : type(type) {}
        virtual ~TreeEvent() {}

        Type type;
    };
}}
