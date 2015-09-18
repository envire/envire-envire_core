#ifndef __FRAME_EVENT_HPP__
#define __FRAME_EVENT_HPP__

namespace envire { namespace core
{
    
    /** TODO comment
     */
    class FrameEvent
    {
    public:
        //all possible events
        enum Type
        {
            FRAME_ADDED,
            ROOT_FRAME_ADDED,
            FRAME_MODIFIED,
            FRAME_REMOVED
        };
        FrameEvent(const Type type) : type(type) {}
        virtual ~FrameEvent() {}

        Type type;
    };
}}

#endif
