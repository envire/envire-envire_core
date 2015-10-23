/*
 * GraphEventPublisher.cpp
 *
 *  Created on: Sep 16, 2015
 *      Author: aboeckmann
 */
#include <algorithm>
#include "GraphEventPublisher.hpp"
#include "GraphEventSubscriber.hpp"
#include <cassert>

using namespace envire::core;
using namespace std;

void GraphEventPublisher::subscribe(GraphEventSubscriber* pSubscriber)
{
    assert(nullptr != pSubscriber);
    subscribers.push_back(pSubscriber);
}

void GraphEventPublisher::unsubscribe(GraphEventSubscriber* pSubscriber)
{
    assert(nullptr != pSubscriber);
    auto pos = std::find(subscribers.begin(), subscribers.end(), pSubscriber);
    if(pos != subscribers.end())
    {
      subscribers.erase(pos);
    }
}

void GraphEventPublisher::notify(const GraphEvent& e)
{
    if(eventsEnabled)
    {
        for(GraphEventSubscriber* pSubscriber : subscribers)
        {
            pSubscriber->notifyGraphEvent(e);
        }
    }
}




