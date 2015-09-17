/*
 * FrameEventPublisher.cpp
 *
 *  Created on: Sep 16, 2015
 *      Author: aboeckmann
 */
#include "FrameEventPublisher.hpp"
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

void FrameEventPublisher::notify(const FrameEvent& e) const
{
    for(FrameEventSubscriber* s : subscribers)
    {
      s->notifyFrameEvent(e);
    }
}



