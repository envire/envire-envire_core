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
#include <envire_core/graph/EnvireGraph.hpp>
#include <envire_core/events/GraphEventDispatcher.hpp>
#include <envire_core/events/GraphItemEventDispatcher.hpp>
#include <envire_core/items/Item.hpp>
#include <envire_core/graph/GraphDrawing.hpp>
#include <vector>


using namespace envire::core;

class TestGraphEventDispatcher : public GraphEventDispatcher
    {
    public:
        TestGraphEventDispatcher(GraphEventPublisher* pPublisher):GraphEventDispatcher(pPublisher){
            initValues();
        };
        

        virtual ~TestGraphEventDispatcher() {}

        int edgeAddedEvents,edgeRemovedEvents,edgeModifiedEvents,frameAddedEvents,frameRemovedEvents,itemAddedEvents,itemRemovedEvents;

        void initValues(){
            edgeAddedEvents = 0;
            edgeRemovedEvents = 0;
            edgeModifiedEvents = 0;
            frameAddedEvents = 0;
            frameRemovedEvents = 0;
            itemAddedEvents = 0;
            itemRemovedEvents = 0;
        }


    protected:
        virtual void edgeAdded(const EdgeAddedEvent& e) {edgeAddedEvents++;}
        virtual void edgeRemoved(const EdgeRemovedEvent& e) {edgeRemovedEvents++;}
        virtual void edgeModified(const EdgeModifiedEvent& e) {edgeModifiedEvents++;}
        virtual void frameAdded(const FrameAddedEvent& e) {frameAddedEvents++;}
        virtual void frameRemoved(const FrameRemovedEvent& e) {frameRemovedEvents++;}
        virtual void itemAdded(const ItemAddedEvent& e) {itemAddedEvents++;}
        virtual void itemRemoved(const ItemRemovedEvent& e) {itemRemovedEvents++;}        

    };


BOOST_AUTO_TEST_CASE(event_dispatcher_add_item_event_test)
{
    envire::core::EnvireGraph graph;
    FrameId a("a");
    FrameId b("b");
    Item<std::string>::Ptr item1(new Item<std::string>("Time is an illusion. Lunchtime doubly so."));
    Transform tf;
    
    TestGraphEventDispatcher disp(&graph);
    
    //edgeAddedEvent, frameadded x2
    graph.addTransform(a, b, tf);
    //addItemEvent
    graph.addItemToFrame(a, item1);
    //edgeModified
    graph.updateTransform(a, b, tf);
    //edgeRemoved
    graph.removeTransform(a,b);
    //itemRemovedEvents
    graph.removeItemFromFrame(item1);
    //frameRemovedEvents
    graph.removeFrame(a);
    graph.removeFrame(b);

    //graph should be empty again

    BOOST_CHECK(disp.edgeAddedEvents == 1);   
    BOOST_CHECK(disp.frameAddedEvents == 2);   
    BOOST_CHECK(disp.itemAddedEvents == 1);   
    BOOST_CHECK(disp.edgeModifiedEvents == 1);   
    BOOST_CHECK(disp.edgeRemovedEvents == 1);
    BOOST_CHECK(disp.itemRemovedEvents == 1);
    BOOST_CHECK(disp.frameRemovedEvents == 2);

    //clear stats
    disp.initValues();

    //disable callbacks
    disp.enable(false);

    //call same events again
    //edgeAddedEvent, frameadded x2
    graph.addTransform(a, b, tf);
    //addItemEvent
    graph.addItemToFrame(a, item1);
    //edgeModified
    graph.updateTransform(a, b, tf);
    //edgeRemoved
    graph.removeTransform(a,b);
    //itemRemovedEvents
    graph.removeItemFromFrame(item1);
    //frameRemovedEvents
    graph.removeFrame(a);
    graph.removeFrame(b);
    
    //graph should be empty again

    BOOST_CHECK(disp.edgeAddedEvents == 0);   
    BOOST_CHECK(disp.frameAddedEvents == 0);   
    BOOST_CHECK(disp.itemAddedEvents == 0);   
    BOOST_CHECK(disp.edgeModifiedEvents == 0);   
    BOOST_CHECK(disp.edgeRemovedEvents == 0);
    BOOST_CHECK(disp.itemRemovedEvents == 0);
    BOOST_CHECK(disp.frameRemovedEvents == 0);

    //re-enable events

    disp.enable();

    //call same events again
    //edgeAddedEvent, frameadded x2
    graph.addTransform(a, b, tf);
    //addItemEvent
    graph.addItemToFrame(a, item1);
    //edgeModified
    graph.updateTransform(a, b, tf);
    //edgeRemoved
    graph.removeTransform(a,b);
    //itemRemovedEvents
    graph.removeItemFromFrame(item1);
    //frameRemovedEvents
    graph.removeFrame(a);
    graph.removeFrame(b);
    
    //graph should be empty again

    BOOST_CHECK(disp.edgeAddedEvents == 1);   
    BOOST_CHECK(disp.frameAddedEvents == 2);   
    BOOST_CHECK(disp.itemAddedEvents == 1);   
    BOOST_CHECK(disp.edgeModifiedEvents == 1);   
    BOOST_CHECK(disp.edgeRemovedEvents == 1);
    BOOST_CHECK(disp.itemRemovedEvents == 1);
    BOOST_CHECK(disp.frameRemovedEvents == 2);

}