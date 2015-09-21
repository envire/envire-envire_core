/*
 * TreeEventPublisher.cpp
 *
 *  Created on: Sep 16, 2015
 *      Author: aboeckmann
 */
#include <algorithm>
#include "TreeEventPublisher.hpp"

using namespace envire::core;

void TreeEventPublisher::subscribe(TreeEventSubscriber* subscriber)
{
    assert(nullptr != subscriber);
    subscribers.push_back(subscriber);
}

void TreeEventPublisher::unsubscribe(TreeEventSubscriber* subscriber)
{
    assert(nullptr != subscriber);
    auto pos = std::find(subscribers.begin(), subscribers.end(), subscriber);
    if(pos != subscribers.end())
    {
      subscribers.erase(pos);
    }
}

void TreeEventPublisher::notify(const TreeEvent& e) const
{
    for(TreeEventSubscriber* s : subscribers)
    {
      s->notifyTreeEvent(e);
    }
}



