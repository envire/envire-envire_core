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

void GraphEventPublisher::subscribe(GraphEventSubscriber* pSubscriber, bool publish_current_state)
{
    assert(nullptr != pSubscriber);

    if(publish_current_state)
        publishCurrentState(pSubscriber);

    subscribers.push_back(pSubscriber);
}

void GraphEventPublisher::unsubscribe(GraphEventSubscriber* pSubscriber, bool unpublish_current_state)
{
    assert(nullptr != pSubscriber);

    if(unpublish_current_state)
        unpublishCurrentState(pSubscriber);

    auto pos = std::find(subscribers.begin(), subscribers.end(), pSubscriber);
    if(pos != subscribers.end())
    {
      subscribers.erase(pos);
    }
}

void GraphEventPublisher::notify(const GraphEvent& e)
{
    for(GraphEventSubscriber* pSubscriber : subscribers)
    {
        pSubscriber->notifyGraphEvent(e);
    }
}

void GraphEventPublisher::notifySubscriber(GraphEventSubscriber* pSubscriber, const GraphEvent& e)
{
    pSubscriber->notifyGraphEvent(e);
}
