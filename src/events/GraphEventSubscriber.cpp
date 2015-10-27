#include "GraphEventSubscriber.hpp"
#include "GraphEventPublisher.hpp"
#include <cassert>

using namespace envire::core;

GraphEventSubscriber::GraphEventSubscriber(envire::core::GraphEventPublisher* pPublisher) :
    pPublisher(pPublisher)
{
    subscribe(pPublisher);
}

GraphEventSubscriber::GraphEventSubscriber() : pPublisher(nullptr)
{
}

void GraphEventSubscriber::subscribe(GraphEventPublisher* pPublisher)
{
    assert(pPublisher != nullptr);
    pPublisher->subscribe(this);
}


GraphEventSubscriber::~GraphEventSubscriber()
{
    if(nullptr != pPublisher)
    {
      pPublisher->unsubscribe(this);
      pPublisher = nullptr;
    }
}
