/*
 * TreeEventPublisher.cpp
 *
 *  Created on: Sep 16, 2015
 *      Author: aboeckmann
 */
#include <algorithm>
#include "GraphEventPublisher.hpp"


using namespace envire::core;
using namespace std;

void GraphEventPublisher::subscribe(shared_ptr<GraphEventSubscriber> subscriber)
{
    assert(nullptr != subscriber);
    subscribers.push_back(subscriber);
}

void GraphEventPublisher::unsubscribe(shared_ptr<GraphEventSubscriber> subscriber)
{
    assert(nullptr != subscriber);
    auto pos = std::find(subscribers.begin(), subscribers.end(), subscriber);
    if(pos != subscribers.end())
    {
      subscribers.erase(pos);
    }
}

void GraphEventPublisher::notify(const GraphEvent& e)
{
    if(eventsEnabled)
    {
        for(shared_ptr<GraphEventSubscriber>& s : subscribers)
        {
            s->notifyTreeEvent(e);
        }
    }
}

void GraphEventPublisher::disableEvents()
{
    eventsEnabled = false;
}
void GraphEventPublisher::enableEvents()
{
    eventsEnabled = true;
}



