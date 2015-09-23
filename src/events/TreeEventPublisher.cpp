/*
 * TreeEventPublisher.cpp
 *
 *  Created on: Sep 16, 2015
 *      Author: aboeckmann
 */
#include <algorithm>
#include "TreeEventPublisher.hpp"


using namespace envire::core;
using namespace std;

void TreeEventPublisher::subscribe(shared_ptr<TreeEventSubscriber> subscriber)
{
    assert(nullptr != subscriber);
    subscribers.push_back(subscriber);
}

void TreeEventPublisher::unsubscribe(shared_ptr<TreeEventSubscriber> subscriber)
{
    assert(nullptr != subscriber);
    auto pos = std::find(subscribers.begin(), subscribers.end(), subscriber);
    if(pos != subscribers.end())
    {
      subscribers.erase(pos);
    }
}

void TreeEventPublisher::notify(const TreeEvent& e)
{
    if(eventsEnabled)
    {
        for(shared_ptr<TreeEventSubscriber>& s : subscribers)
        {
            s->notifyTreeEvent(e);
        }
    }
}

void TreeEventPublisher::disableEvents()
{
    eventsEnabled = false;
}
void TreeEventPublisher::enableEvents()
{
    eventsEnabled = true;
}



