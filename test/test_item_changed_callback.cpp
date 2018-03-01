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

#include <boost/test/unit_test.hpp>
#include <boost/lexical_cast.hpp>
#define protected public
#include <envire_core/graph/EnvireGraph.hpp>
#include <envire_core/events/GraphEventDispatcher.hpp>
#include <envire_core/events/GraphItemEventDispatcher.hpp>
#include <envire_core/items/Item.hpp>
#include <envire_core/graph/GraphDrawing.hpp>
#include <vector>


using namespace envire::core;
using namespace std;

struct ItemContentReactor{
    
    ItemContentReactor():frame(""),called(false){}
        
    void cb(const ItemBase& item){
        called = true;
        frame = item.getFrame();
    }
    
    void reset(){
        frame = "";
        called = false;
    }
    
    FrameId frame;
    bool called;
    
};

//a simple item event subscriber that subscribes item events when they are available
struct ItemCallbackSubscriber : public GraphItemEventDispatcher<Item<string>>
{
    ItemCallbackSubscriber(EnvireGraph& graph, FrameId targetFrame) : 
      GraphItemEventDispatcher<Item<string>>(&graph),
      targetFrame(targetFrame){}

    virtual void itemAdded(const TypedItemAddedEvent<Item<string>>& event)
    {
        if (event.item->getFrame() == targetFrame){
            event.item->connectContentsChangedCallback(boost::bind(&ItemContentReactor::cb, &reactor,  _1));
        }
    }
    
    virtual void itemRemoved(const TypedItemRemovedEvent<Item<string>>& event)
    {
        if (event.item->getFrame() == targetFrame){
            event.item->disconnectContentsChangedCallback(boost::bind(&ItemContentReactor::cb, &reactor,  _1));
        }
    }

    FrameId targetFrame;
    ItemContentReactor reactor;
    
};


BOOST_AUTO_TEST_CASE(item_changed_callback_lambda)
{
     EnvireGraph graph;
     FrameId frame("frame");
     graph.addFrame(frame);
     
     
     Item<string>::Ptr item(new Item<string>("lalala"));
     ItemContentReactor reactor;
     
     graph.addItemToFrame(frame, item);
     item->connectContentsChangedCallback([&reactor](const ItemBase& item){reactor.frame=item.getFrame();reactor.called=true;});
     item->contentsChanged();
     
     BOOST_CHECK(reactor.called == true);
     BOOST_CHECK(reactor.frame == frame);
}

struct Callback{
    void operator()(const ItemBase& item){
        called = true;
        frame = item.getFrame();
    }
    static bool called;
    static FrameId frame;
    
};

bool Callback::called = false;
FrameId Callback::frame = "";

BOOST_AUTO_TEST_CASE(item_changed_callback_class)
{

    EnvireGraph graph;
     FrameId frame("frame");
     graph.addFrame(frame);
     
     Item<string>::Ptr item(new Item<string>("lalala"));
     Callback callback;
     
     graph.addItemToFrame(frame, item);
     
     //this one copies the callback
     item->connectContentsChangedCallback(callback);
     item->contentsChanged();
     
     BOOST_CHECK(Callback::called == true);
     BOOST_CHECK(Callback::frame == frame);
     
}


BOOST_AUTO_TEST_CASE(item_changed_callback_bind)
{
     EnvireGraph graph;
     FrameId frame("frame");
     graph.addFrame(frame);
     
     
     Item<string>::Ptr item(new Item<string>("lalala"));
     ItemContentReactor reactor;
     
     graph.addItemToFrame(frame, item);
     item->connectContentsChangedCallback(boost::bind(&ItemContentReactor::cb, &reactor,  _1));
     item->contentsChanged();
     
     BOOST_CHECK(reactor.called == true);
     BOOST_CHECK(reactor.frame == frame);
     
     reactor.reset();
     item->disconnectContentsChangedCallback(boost::bind(&ItemContentReactor::cb, &reactor,  _1));
     item->contentsChanged();
     
     BOOST_CHECK(reactor.called == false);
     BOOST_CHECK(reactor.frame == "");
     
}



BOOST_AUTO_TEST_CASE(item_changed_callback_autoconnect)
{
    
     EnvireGraph graph;
     FrameId frame("frame");
     ItemCallbackSubscriber sub(graph,"frame");
     
     graph.addFrame(frame);
     
     Item<string>::Ptr item(new Item<string>("lalala"));
     
     graph.addItemToFrame(frame, item);
     
     item->contentsChanged();
     
     BOOST_CHECK(sub.reactor.called == true);
     BOOST_CHECK(sub.reactor.frame == frame);
     
     //reset ans try to emit again
     sub.reactor.reset();
     graph.clearFrame(frame);
     item->contentsChanged();
     
     BOOST_CHECK(sub.reactor.called == false);
     BOOST_CHECK(sub.reactor.frame == "");
     
     graph.addFrame("other");
     Item<string>::Ptr otheritem(new Item<string>("otherlalala"));
     graph.addItemToFrame("other", otheritem);
     
     otheritem->contentsChanged();
     
     BOOST_CHECK(sub.reactor.called == false);
     BOOST_CHECK(sub.reactor.frame == "");
     
     
}

