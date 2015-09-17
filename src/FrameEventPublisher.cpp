/*
 * FrameEventPublisher.cpp
 *
 *  Created on: Sep 16, 2015
 *      Author: aboeckmann
 */
#include "FrameEventPublisher.hpp"
#include "FrameEvent.hpp"
#include "FrameEventSubscriber.hpp"

#include <cassert>
#include <algorithm>

using namespace envire::core;

void FrameEventPublisher::subscribe(FrameEventSubscriber* subscriber)
{
    assert(nullptr != subscriber);
    subscribers.push_back(subscriber);
}

void FrameEventPublisher::unsubscribe(FrameEventSubscriber* subscriber)
{
    assert(nullptr != subscriber);
    auto pos = std::find(subscribers.begin(), subscribers.end(), subscriber);
    if(pos != subscribers.end())
    {
      subscribers.erase(pos);
    }
}

void FrameEventPublisher::notify(const FrameEvent& event) const
{
    for(FrameEventSubscriber* sub : subscribers)
    {
        sub->notifyFrameEvent(event);
    }
}

void FrameEventPublisher::frameAdded(const FrameAddedEventArgs& args) const
{
    FrameEvent e(args);
    notify(e);

}

void FrameEventPublisher::frameModified(const FrameModifedEventArgs& args) const
{
  FrameEvent e(args);
  notify(e);
}

void FrameEventPublisher::frameRemoved(const FrameRemovedEventArgs& args) const
{
  FrameEvent e(args);
  notify(e);
}



