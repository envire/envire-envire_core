#ifndef __FRAME_EVENT_HPP__
#define __FRAME_EVENT_HPP__
#include "FrameEventArgs.hpp"

namespace envire { namespace core
{
    
/** TODO comment
 */
class FrameEvent
{
public:
    FrameEvent(const FrameAddedEventArgs& addedArgs) : type(FRAME_ADDED),
        addedArgs(addedArgs){}
    FrameEvent(const FrameModifedEventArgs& modifiedArgs) : type(FRAME_MODIFIED),
        modifedArgs(modifedArgs){}
    FrameEvent(const FrameRemovedEventArgs& removedArgs) : type(FRAME_REMOVED),
        removedArgs(removedArgs){}

    //all possible events
    enum Type
    {
        FRAME_ADDED,
        FRAME_MODIFIED,
        FRAME_REMOVED
    };
    Type type;

    //type defines which part is valid.
    union
    {
      FrameAddedEventArgs addedArgs;
      FrameModifedEventArgs modifedArgs;
      FrameRemovedEventArgs removedArgs;
    };
};

}}

#endif
