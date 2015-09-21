#ifndef SRC_FRAMEEVENTDISPATCHER_HPP_
#define SRC_FRAMEEVENTDISPATCHER_HPP_
#include "TreeEventSubscriber.hpp"


namespace envire { namespace core
{
    class FrameAddedEvent;
    class FrameRootAddedEvent;

    /**
     * A TreeEventSubscriber that parses the event and calls different methods
     * based on the event's content.
     *
     * You should override the methods, that you are interested in.
     */
    class TreeEventDispatcher : public TreeEventSubscriber
    {
    public:
        virtual void notifyTreeEvent(const TreeEvent& event);

    protected:
        virtual void frameAdded(const FrameAddedEvent& e){}
        virtual void frameRootAdded(const FrameRootAddedEvent& e) {}
       // virtual void frameModified(const FrameModifedEventArgs& args){}
      //  virtual void frameRemoved(const FrameRemovedEventArgs& args){}
    };
}}
#endif /* SRC_FRAMEEVENTDISPATCHER_HPP_ */
