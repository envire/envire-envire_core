#include "GraphEventSubscriber.hpp"
#include "GraphEventPublisher.hpp"
using namespace envire::core;
GraphEventSubscriber::GraphEventSubscriber(envire::core::GraphEventPublisher& publisher) :
    publisher(publisher)
{
    publisher.subscribe(this);
}

GraphEventSubscriber::~GraphEventSubscriber()
{
    publisher.unsubscribe(this);
}
