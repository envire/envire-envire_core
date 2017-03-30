//
// Copyright (c) 2015, Deutsches Forschungszentrum für Künstliche Intelligenz GmbH.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

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
        subscribers.back()->unsubscribe();
        subscribers.pop_back();
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
