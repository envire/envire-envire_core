/*
 * GraphEventPublisher.cpp
 *
 *  Created on: Sep 16, 2015
 *      Author: aboeckmann
 */
#include <algorithm>
#include <envire_core/events/GraphEventPublisher.hpp>
#include <envire_core/events/GraphEventSubscriber.hpp>
#include <cassert>

using namespace envire::core;
using namespace std;


GraphEventPublisher::GraphEventPublisher() : insideNotify(false)
{
    subscribers.reserve(10000);
}

void GraphEventPublisher::subscribe(GraphEventSubscriber* pSubscriber, bool publish_current_state)
{
    assert(nullptr != pSubscriber);
    if(publish_current_state)
        publishCurrentState(pSubscriber);

    if(insideNotify)
      toBeSubscribed.push_back(pSubscriber);
    else
      subscribers.push_back(pSubscriber);
}

void GraphEventPublisher::unsubscribe(GraphEventSubscriber* pSubscriber, bool unpublish_current_state)
{
    assert(nullptr != pSubscriber);

    if(unpublish_current_state)
        unpublishCurrentState(pSubscriber);

    if(insideNotify)
    {
      toBeUnsubscribed.push_back(pSubscriber);
    }
    else
    {
      unsubscribeInternal(pSubscriber);
    }
}

void GraphEventPublisher::notify(const GraphEvent& e)
{
    insideNotify = true;
    
    for(GraphEventSubscriber* pSubscriber : subscribers)
    {
        pSubscriber->notifyGraphEvent(e);
    }
    
    //update subscribers list (it might have been changed by event handlers)
    //NOTE This is ***not*** meant to handle multithreading issues. It is only
    //     meant to handle recursions in the same thread. This does ***not*** make
    //     it thread-safe.
    for(GraphEventSubscriber* pSubscriber : toBeSubscribed)
    {
        subscribers.push_back(pSubscriber);
    }
    toBeSubscribed.clear();
    
    for(GraphEventSubscriber* pSubscriber : toBeUnsubscribed)
    {
        unsubscribeInternal(pSubscriber);
    }    
    
    insideNotify = false;
}

void GraphEventPublisher::notifySubscriber(GraphEventSubscriber* pSubscriber, const GraphEvent& e)
{
    pSubscriber->notifyGraphEvent(e);
}

GraphEventPublisher::~GraphEventPublisher()
{
    //use while loop because unsubscribe() modifies the list
    while(subscribers.size() > 0)
    {
        subscribers.front()->unsubscribe();
    }
}

void GraphEventPublisher::unsubscribeInternal(GraphEventSubscriber* pSubscriber)
{
    auto pos = std::find(subscribers.begin(), subscribers.end(), pSubscriber);
    if(pos != subscribers.end())
    {
      subscribers.erase(pos);
    }  
}
