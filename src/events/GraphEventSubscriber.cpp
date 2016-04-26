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


GraphEventSubscriber::~GraphEventSubscriber()
{
    if(nullptr != pPublisher)
    {
      pPublisher->unsubscribe(this);
      pPublisher = nullptr;
    }
}
