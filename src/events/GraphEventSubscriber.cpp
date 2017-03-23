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

#include <envire_core/events/GraphEventSubscriber.hpp>
#include <envire_core/events/GraphEventPublisher.hpp>
#include <cassert>

using namespace envire::core;

GraphEventSubscriber::GraphEventSubscriber(envire::core::GraphEventPublisher* pPublisher) :
    pPublisher(nullptr)
{
    subscribe(pPublisher);
}

GraphEventSubscriber::GraphEventSubscriber() : pPublisher(nullptr)
{
}

void GraphEventSubscriber::subscribe(GraphEventPublisher* pPublisher, bool publish_current_state)
{
    assert(pPublisher != nullptr);
    // unsubscribe if already subscribed
    if(this->pPublisher != nullptr)
        this->pPublisher->unsubscribe(this, publish_current_state);
    // subscribe to new publisher
    this->pPublisher = pPublisher;
    this->pPublisher->subscribe(this, publish_current_state);
}

void GraphEventSubscriber::unsubscribe()
{
    if(nullptr != pPublisher)
    {
      pPublisher->unsubscribe(this);
      pPublisher = nullptr;
    }  
}

GraphEventSubscriber::~GraphEventSubscriber()
{
    unsubscribe();
}
