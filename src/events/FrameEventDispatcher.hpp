/*
 * FramEventDispatcher.hpp
 *
 *  Created on: Sep 16, 2015
 *      Author: aboeckmann
 */

#ifndef SRC_FRAMEEVENTDISPATCHER_HPP_
#define SRC_FRAMEEVENTDISPATCHER_HPP_
#include "FrameEventSubscriber.hpp"


namespace envire { namespace core
{
    class FrameAddedEvent;
    class FrameRootAddedEvent;

    /**
     * A FrameEventSubscriber that parses the event and calls different methods
     * based on the event's content.
     *
     * You should override the methods, that you are interested in.
     */
    class FrameEventDispatcher : public FrameEventSubscriber
    {
    public:
        virtual void notifyFrameEvent(const FrameEvent& event);

    protected:
        virtual void frameAdded(const FrameAddedEvent& e){}
        virtual void frameRootAdded(const FrameRootAddedEvent& e) {}
       // virtual void frameModified(const FrameModifedEventArgs& args){}
      //  virtual void frameRemoved(const FrameRemovedEventArgs& args){}
    };
}}
#endif /* SRC_FRAMEEVENTDISPATCHER_HPP_ */
