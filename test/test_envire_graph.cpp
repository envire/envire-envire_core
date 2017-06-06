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



class EnvireDispatcher : public GraphEventDispatcher {
public:
    vector<ItemAddedEvent> itemAddedEvents;
    vector<ItemRemovedEvent> itemRemovedEvents;
    
    EnvireDispatcher(EnvireGraph& graph) : GraphEventDispatcher(&graph) {}
    EnvireDispatcher() : GraphEventDispatcher() {}
    virtual ~EnvireDispatcher() {}
    
    void itemAdded(const ItemAddedEvent& e) override
    {
        itemAddedEvents.push_back(e);
    }
    
    void itemRemoved(const ItemRemovedEvent& e) override
    {
        itemRemovedEvents.push_back(e);
    }
};


// //a simple item event subscriber that logs all events
struct ItemEventSubscriber : public GraphItemEventDispatcher<Item<string>>,
                             public GraphItemEventDispatcher<Item<int>>,
                             public GraphItemEventDispatcher<Item<float>>
{
    vector<TypedItemAddedEvent<Item<string>>> stringItemAddedEvents;
    vector<TypedItemAddedEvent<Item<int>>> intItemAddedEvents;
    vector<TypedItemAddedEvent<Item<float>>> floatItemAddedEvents; 
    vector<TypedItemRemovedEvent<Item<string>>> stringItemRemovedEvents;
    vector<TypedItemRemovedEvent<Item<int>>> intItemRemovedEvents;
    vector<TypedItemRemovedEvent<Item<float>>> floatItemRemovedEvents; 

    ItemEventSubscriber(EnvireGraph& graph) : 
      GraphItemEventDispatcher<Item<string>>(&graph),
      GraphItemEventDispatcher<Item<int>>(&graph),
      GraphItemEventDispatcher<Item<float>>(&graph) {}

    virtual void itemAdded(const TypedItemAddedEvent<Item<string>>& event)
    {
        stringItemAddedEvents.push_back(event);
    }
    
    virtual void itemAdded(const TypedItemAddedEvent<Item<int>>& event)
    {
        intItemAddedEvents.push_back(event);
    }
    
    virtual void itemAdded(const TypedItemAddedEvent<Item<float>>& event)
    {
        floatItemAddedEvents.push_back(event);
    }
    
    virtual void itemRemoved(const TypedItemRemovedEvent<Item<string>>& event)
    {
        stringItemRemovedEvents.push_back(event);
    }
    
    virtual void itemRemoved(const TypedItemRemovedEvent<Item<int>>& event)
    {
        intItemRemovedEvents.push_back(event);
    }
    
    virtual void itemRemoved(const TypedItemRemovedEvent<Item<float>>& event)
    {
        floatItemRemovedEvents.push_back(event);
    }
};
    

BOOST_AUTO_TEST_CASE(create_envire_graph_test)
{
    EnvireGraph g;
}

BOOST_AUTO_TEST_CASE(copy_with_items_test)
{
    EnvireGraph g;
    const FrameId frame("awesome frame");
    g.addFrame(frame);
    Item<string>::Ptr item(new Item<string>("lalala"));
    Item<string>::Ptr item2(new Item<string>("lululu"));
    
    g.addItemToFrame(frame, item);
    g.addItemToFrame(frame, item2);
        
    EnvireGraph g2(g);
    BOOST_CHECK(g2.getTotalItemCount(frame) == 2);
}

BOOST_AUTO_TEST_CASE(simple_add_item_test)
{
    FrameId aFrame = "frame_a";
    const string text("I am so smart, S M A T");
    
    EnvireGraph g;
    g.addFrame(aFrame);
    
    Item<string>::Ptr item(new Item<string>(text));
    g.addItemToFrame(aFrame, item);

    using Iterator = EnvireGraph::ItemIterator<Item<string>>;
    
    
    Iterator begin, end;
    boost::tie(begin, end) = g.getItems<Item<string>>(aFrame);
    BOOST_CHECK(begin != end);
    BOOST_CHECK(begin->getData().compare(text) == 0);

}

BOOST_AUTO_TEST_CASE(simple_add_item_non_generic_test)
{
    FrameId aFrame = "frame_a";
    const string text("you win again, gravity!");
    
    EnvireGraph g;
    g.addFrame(aFrame);
    
    ItemBase::Ptr item(new Item<string>(text));
    item->setFrame(aFrame);
    g.addItem(item);
    
    using Iterator = EnvireGraph::ItemIterator<Item<string>>;

    Iterator begin, end;
    boost::tie(begin, end) = g.getItems<Item<string>>(aFrame);
    BOOST_CHECK(begin != end);
    BOOST_CHECK(begin->getData().compare(text) == 0);
}

BOOST_AUTO_TEST_CASE(add_multiple_items_test)
{
    FrameId a = "frame_a";
    struct DataA 
    {
        DataA(const string& value = "") : value(value){}
        string value;
    };
    struct DataB
    {
        DataB() {}
        DataB(const int value) : value(value){}
        int value;
    };
    
    EnvireGraph g;
    g.addFrame(a);


    Item<DataA>::Ptr a1(new Item<DataA>(DataA("Grandpa, why don't you tell a story?")));
    Item<DataA>::Ptr a2(new Item<DataA>(DataA("Yeah Grandpa, you lived a long and interesting life.")));
    Item<DataA>::Ptr a3(new Item<DataA>(DataA("That's a lie and you know it")));
    
    Item<DataB>::Ptr b1(new Item<DataB>(DataB(42)));
    Item<DataB>::Ptr b2(new Item<DataB>(DataB(21)));
    Item<DataB>::Ptr b3(new Item<DataB>(DataB(84)));
    
    g.addItemToFrame(a, a1);
    g.addItemToFrame(a, a2);
    g.addItemToFrame(a, a3);
    g.addItemToFrame(a, b1);
    g.addItemToFrame(a, b2);
    g.addItemToFrame(a, b3);
    
    using AIterator = EnvireGraph::ItemIterator<Item<DataA>>;
    using BIterator = EnvireGraph::ItemIterator<Item<DataB>>;
    
    AIterator aBegin, aEnd;
    boost::tie(aBegin, aEnd) = g.getItems<Item<DataA>>(a);
    BOOST_CHECK(aBegin != aEnd);
    BOOST_CHECK(aBegin->getData().value.compare(a1->getData().value) == 0);
    ++aBegin;
    BOOST_CHECK(aBegin->getData().value.compare(a2->getData().value) == 0);
    ++aBegin;
    BOOST_CHECK(aBegin->getData().value.compare(a3->getData().value) == 0);
    ++aBegin;
    BOOST_CHECK(aBegin == aEnd);
    
    BIterator bBegin, bEnd;
    boost::tie(bBegin, bEnd) = g.getItems<Item<DataB>>(a);
    BOOST_CHECK(bBegin != bEnd);
    BOOST_CHECK(bBegin->getData().value == 42);
    ++bBegin;
    BOOST_CHECK(bBegin->getData().value == 21);
    ++bBegin;
    BOOST_CHECK(bBegin->getData().value == 84);
    ++bBegin;
    BOOST_CHECK(bBegin == bEnd);
}

BOOST_AUTO_TEST_CASE(add_item_to_invalid_frame)
{
    FrameId a = "frame_a";
    const string text("I am so smart, S M A T");
    EnvireGraph g;
    Item<string>::Ptr item(new Item<string>(text));
    BOOST_CHECK_THROW(g.addItemToFrame(a, item), UnknownFrameException); 
}

BOOST_AUTO_TEST_CASE(simple_remove_item_from_frame_test)
{
    FrameId frame = "frame";
    EnvireGraph g;
    g.addFrame(frame);
    const string text("Good news everyone!");
    Item<string>::Ptr item(new Item<string>(text));
    g.addItemToFrame(frame, item);
    
    using Iterator = EnvireGraph::ItemIterator<Item<string>>;
    Iterator it = g.getItem<Item<string>>(frame, 0);
    
    g.removeItemFromFrame(frame, it);
    
    Iterator begin, end;
    boost::tie(begin, end) = g.getItems<Item<string>>(frame);
    BOOST_CHECK(begin == end);
}

BOOST_AUTO_TEST_CASE(remove_multiple_items_from_frame_test)
{
    FrameId frame = "frame";
    EnvireGraph g;
    g.addFrame(frame);
    const string text("This Concept Of ‘Wuv’ Confused And Infuriates Us!");
    
    for(int i = 0; i < 42; ++i)
    {
        Item<string>::Ptr item(new Item<string>(text));
        g.addItemToFrame(frame, item);
    }
    using Iterator = EnvireGraph::ItemIterator<Item<string>>;
    Iterator begin, end;
    boost::tie(begin, end) = g.getItems<Item<string>>(frame);
    
    BOOST_CHECK(end - begin == 42);//check if there are really 42 items in the vector now
    for(; begin != end;)
    {
        boost::tie(begin, end) = g.removeItemFromFrame(frame, begin);
    }
    BOOST_CHECK(begin == end);
    boost::tie(begin, end) = g.getItems<Item<string>>(frame);
    BOOST_CHECK(begin == end);
}

BOOST_AUTO_TEST_CASE(get_item_test)
{
    FrameId frame = "Saturn";
    EnvireGraph g;
    g.addFrame(frame);
    const string text("Ford... you're turning into a penguin. Stop it.");
    Item<string>::Ptr item(new Item<string>(text));
    g.addItemToFrame(frame, item);
    
    Item<string>& retItem = *g.getItem<Item<string>>(frame, 0);
    BOOST_CHECK(retItem.getData().compare(text) == 0);
}

BOOST_AUTO_TEST_CASE(get_items_vertex_desc_non_generic_error_case_test)
{
    FrameId frame = "Saturn";
    EnvireGraph g;
    g.addFrame(frame);
    Item<string>::Ptr item(new Item<string>("bla"));
    GraphTraits::vertex_descriptor desc = g.getVertex(frame);
    BOOST_CHECK_THROW(g.getItems(desc, item->getTypeIndex()), NoItemsOfTypeInFrameException);
}

BOOST_AUTO_TEST_CASE(get_item_on_empty_frame_test)
{
    FrameId frame = "Saturn";
    EnvireGraph g;
    g.addFrame(frame);
    BOOST_CHECK_THROW(g.getItem<Item<string>>(frame, 42), NoItemsOfTypeInFrameException);
} 

BOOST_AUTO_TEST_CASE(get_item_wrong_index_test)
{
    FrameId frame = "Mars";
    EnvireGraph g;
    g.addFrame(frame);
    Item<string>::Ptr item(new Item<string>("Arthur: If I asked you where the hell we were, would I regret it? -- Ford: We're safe"));
    g.addItemToFrame(frame, item);
    BOOST_CHECK_THROW(g.getItem<Item<string>>(frame, 42), std::out_of_range);
} 

BOOST_AUTO_TEST_CASE(get_first_item_on_empty_graph_test)
{
    FrameId frame = "blaa";
    EnvireGraph g;
    BOOST_CHECK_THROW(g.getItem<Item<string>>(frame, 11), UnknownFrameException);
}

BOOST_AUTO_TEST_CASE(clear_frame_test)
{
    FrameId frame = "frame";
    EnvireGraph g;
    g.addFrame(frame);
    const string text("424242");
    
    for(int i = 0; i < 3; ++i)
    {
        Item<string>::Ptr item(new Item<string>(text));
        g.addItemToFrame(frame, item);
    }
    g.clearFrame(frame);
    using Iterator = EnvireGraph::ItemIterator<Item<string>>;
    Iterator begin, end;
    boost::tie(begin, end) = g.getItems<Item<string>>(frame);
    BOOST_CHECK(begin == end);
}

BOOST_AUTO_TEST_CASE(clear_frame_event_test)
{
    FrameId frame = "frame";
    EnvireGraph g;
    g.addFrame(frame);
    
    ItemBase::Ptr item1(new Item<string>("bla"));
    ItemBase::Ptr item2(new Item<int>(42));
    
    g.addItemToFrame(frame, item1);
    g.addItemToFrame(frame, item2);
    
    EnvireDispatcher d(g);
    g.clearFrame(frame);
    BOOST_CHECK(d.itemRemovedEvents.size() == 2);
    BOOST_CHECK(d.itemRemovedEvents[0].frame == frame);
    BOOST_CHECK(d.itemRemovedEvents[1].frame == frame);
    BOOST_CHECK(d.itemRemovedEvents[1].item == item1);
    BOOST_CHECK(d.itemRemovedEvents[0].item == item2);
}



BOOST_AUTO_TEST_CASE(check_item_existence_example)
{
    struct Joint
    {
      Joint() {}
      Joint(int a) : a(a) {}
      int a;
    };
    struct Sensor {};
    
    EnvireGraph g;
    FrameId frame = "frame";
    g.addFrame(frame);
    
    Item<Joint>::Ptr item(new Item<Joint>(Joint(42)));
    g.addItemToFrame(frame, item);
    Item<Sensor>::Ptr item2(new Item<Sensor>());
    g.addItemToFrame(frame, item2);
    
    bool contains = g.containsItems<Item<string>>(frame);
    BOOST_CHECK(!contains);
    
    using Iterator = EnvireGraph::ItemIterator<Item<Joint>>;
    Iterator begin, end;
    boost::tie(begin, end) = g.getItems<Item<Joint>>(frame);
    BOOST_CHECK(begin != end);
    ++begin;
    BOOST_CHECK(begin == end);

}

BOOST_AUTO_TEST_CASE(check_item_existence_example_typeId)
{
    struct Joint
    {
      Joint() {}
      Joint(int a) : a(a) {}
      int a;
    };
    struct Sensor {};
    
    EnvireGraph g;
    FrameId frame = "frame";
    g.addFrame(frame);
    
    Item<Joint>::Ptr item(new Item<Joint>(Joint(42)));
    g.addItemToFrame(frame, item);
    Item<Sensor>::Ptr item2(new Item<Sensor>());
    g.addItemToFrame(frame, item2);
    
    const std::type_index stringType(typeid(Item<string>));
    bool contains = g.containsItems(frame, stringType);
    BOOST_CHECK(!contains);
    
    const std::type_index jointType(typeid(Item<Joint>));
    using Iterator = EnvireGraph::ItemIterator<Item<Joint>>;
    Iterator begin, end;
    BOOST_CHECK(g.getItems(frame, jointType).size() > 0);
}


BOOST_AUTO_TEST_CASE(get_item_empty_graph_test)
{
    FrameId a = "frame_a";
    const string text("Don't Panic.");
    EnvireGraph g;
    BOOST_CHECK_THROW(g.getItems<Item<string>>(a), UnknownFrameException);
}

BOOST_AUTO_TEST_CASE(clear_unknown_frame_test)
{
    FrameId a = "frame_a";
    EnvireGraph graph;
    BOOST_CHECK_THROW(graph.clearFrame(a), UnknownFrameException);
}

BOOST_AUTO_TEST_CASE(add_item_event_test)
{
    EnvireGraph graph;
    FrameId a("a");
    FrameId b("b");
    Transform tf;
    
    graph.addTransform(a, b, tf);
    
    Item<string>::Ptr item1(new Item<string>("Time is an illusion. Lunchtime doubly so."));
    Item<int>::Ptr item2(new Item<int>(42));
    Item<float>::Ptr item3(new Item<float>(21.0f)); 
    
    ItemEventSubscriber sub(graph);

    graph.addItemToFrame(a, item1);
    graph.addItemToFrame(b, item2);
    graph.addItemToFrame(a, item3);
    

    
    BOOST_CHECK(sub.floatItemAddedEvents.size() == 1);
    BOOST_CHECK(sub.intItemAddedEvents.size() == 1);
    BOOST_CHECK(sub.stringItemAddedEvents.size() == 1);   
    
    BOOST_CHECK(sub.floatItemAddedEvents.front().frame == a);
    BOOST_CHECK(sub.intItemAddedEvents.front().frame == b);
    BOOST_CHECK(sub.stringItemAddedEvents.front().frame == a);
    BOOST_CHECK(sub.stringItemAddedEvents.front().item == item1);
    BOOST_CHECK(sub.intItemAddedEvents.front().item == item2);
    BOOST_CHECK(sub.floatItemAddedEvents.front().item == item3);
}

BOOST_AUTO_TEST_CASE(multiple_item_event_destructor_test)
{
    struct ItemEventSubscriber : public GraphItemEventDispatcher<Item<string>>,
                                 public GraphItemEventDispatcher<Item<int>>,
                                 public GraphItemEventDispatcher<Item<float>>
    {
        ItemEventSubscriber(EnvireGraph& graph) : 
        GraphItemEventDispatcher<Item<string>>(&graph),
        GraphItemEventDispatcher<Item<int>>(&graph),
        GraphItemEventDispatcher<Item<float>>(&graph)
        {
        }
        
        virtual ~ItemEventSubscriber()
        {
        }
    };
    EnvireGraph g;
    ItemEventSubscriber s(g);
}

BOOST_AUTO_TEST_CASE(remove_item_event_test)
{
    EnvireGraph graph;
    FrameId a("a");
    FrameId b("b");
    Transform tf;
    
    graph.addTransform(a, b, tf);
    Item<string>::Ptr item1(new Item<string>("The ships hung in the sky in much the same way that bricks don't."));
    Item<int>::Ptr item2(new Item<int>(42));
    Item<float>::Ptr item3(new Item<float>(21.0f)); 
    
    ItemEventSubscriber sub(graph);
    graph.addItemToFrame(a, item1);
    graph.addItemToFrame(b, item2);
    graph.addItemToFrame(a, item3);
    
    using StrIterator = EnvireGraph::ItemIterator<Item<string>>;
    using FloatIterator = EnvireGraph::ItemIterator<Item<float>>;
    
    
    StrIterator strIt = graph.getItem<Item<string>>(a, 0);
    graph.removeItemFromFrame(a, strIt);
    BOOST_CHECK(sub.stringItemRemovedEvents.size() == 1);
    BOOST_CHECK(sub.stringItemRemovedEvents.front().frame == a);
    BOOST_CHECK(sub.stringItemRemovedEvents.front().item == item1);

    FloatIterator fltIt = graph.getItem<Item<float>>(a, 0);
    graph.removeItemFromFrame(a, fltIt);
    BOOST_CHECK(sub.floatItemRemovedEvents.size() == 1);
    BOOST_CHECK(sub.floatItemRemovedEvents.front().frame == a);
    BOOST_CHECK(sub.floatItemRemovedEvents.front().item == item3);
    
    using Iterator = EnvireGraph::ItemIterator<Item<int>>;
    Iterator begin, end;
    boost::tie(begin, end) = graph.getItems<Item<int>>(b);
    graph.removeItemFromFrame(b, begin);
    BOOST_CHECK(sub.intItemRemovedEvents.size() == 1);
    BOOST_CHECK(sub.intItemRemovedEvents.front().frame == b);
    BOOST_CHECK(sub.intItemRemovedEvents.front().item == item2);
}

BOOST_AUTO_TEST_CASE(item_count_test)
{
    EnvireGraph graph;
    FrameId frame("frame");  
    FrameId unknownFrame("unknown frame");  
    graph.addFrame(frame);
    GraphTraits::vertex_descriptor vertex = graph.vertex(frame);
    Item<int>::Ptr i(new Item<int>(42));
    Item<int>::Ptr j(new Item<int>(21));
    Item<int>::Ptr k(new Item<int>(11));
    
    BOOST_CHECK(graph.getItemCount<Item<int>>(frame) == 0);
    BOOST_CHECK(graph.getItemCount<Item<int>>(vertex) == 0);
    BOOST_CHECK_THROW(graph.getItemCount<Item<int>>(unknownFrame), UnknownFrameException);
    
    graph.addItemToFrame(frame, i);
    BOOST_CHECK(graph.getItemCount<Item<int>>(frame) == 1);
    BOOST_CHECK(graph.getItemCount<Item<int>>(vertex) == 1);
    
    graph.addItemToFrame(frame, j);
    BOOST_CHECK(graph.getItemCount<Item<int>>(frame) == 2);
    BOOST_CHECK(graph.getItemCount<Item<int>>(vertex) == 2);
    
    BOOST_CHECK(graph.getItemCount<Item<float>>(vertex) == 0);
}

BOOST_AUTO_TEST_CASE(contains_item_test)
{
    EnvireGraph graph;
    FrameId a("a");
    Item<string>::Ptr item(new Item<string>("For a moment, nothing happened. Then, after a second or so, nothing continued to happen."));
    
    BOOST_CHECK_THROW(graph.containsItems<Item<string>>(a), UnknownFrameException);
    graph.addFrame(a);
    graph.addItemToFrame(a, item);
    BOOST_CHECK(graph.containsItems<Item<string>>(a));
    BOOST_CHECK(!graph.containsItems<Item<int>>(a));
}


BOOST_AUTO_TEST_CASE(graphviz_test)
{
    EnvireGraph graph;
    
    for(int i = 0; i < 12; ++i)
    {
        FrameId origin = "frame_" + boost::lexical_cast<std::string>(i);
        FrameId target = "frame_" + boost::lexical_cast<std::string>(i + 1);
        Transform tf;
        graph.addTransform(origin, target, tf);
    }
    
    GraphDrawing::write(graph, "simple_svg_test.dot");
}

BOOST_AUTO_TEST_CASE(complex_draw_test)
{
    EnvireGraph graph;
    
    const FrameId a = "frame_a";
    const FrameId b = "frame_b";
    const FrameId c = "frame_c";
    Transform aToB;
    aToB.transform.translation << 1, 2, 3;
    Transform bToC;
    bToC.transform.translation << 42, 44, -3;
    
    graph.addTransform(a, b, aToB);
    graph.addTransform(b, c, bToC);
    
    Item<string>::Ptr item1(new Item<string>("So say we all!"));
    Item<int>::Ptr item2(new Item<int>(42));
    Item<float>::Ptr item3(new Item<float>(21.0f)); 
    
    graph.addItemToFrame(a, item1);
    graph.addItemToFrame(a, item2);
    graph.addItemToFrame(a, item3);
    
    GraphDrawing::write(graph, "complex_svg_test.dot");
}

BOOST_AUTO_TEST_CASE(remove_frame_item_events_test)
{
    EnvireGraph graph;
    const FrameId a = "Lt. W. Thomas Riker";
    
    Item<string>::Ptr item1(new Item<string>("Don't call me Tiny."));
    Item<int>::Ptr item2(new Item<int>(42));
    

    graph.addFrame(a);
    graph.addItemToFrame(a, item1);
    graph.addItemToFrame(a, item2);
        
    EnvireDispatcher d(graph);
    graph.removeFrame(a);
    BOOST_CHECK(d.itemRemovedEvents.size() == 2);
    //note: the order in which the items will be removed is undefined
    BOOST_CHECK(d.itemRemovedEvents[1].item->getTypeIndex() == item1->getTypeIndex());
    BOOST_CHECK(d.itemRemovedEvents[0].item->getTypeIndex() == item2->getTypeIndex());  
}


BOOST_AUTO_TEST_CASE(remove_item_using_pointer_test)
{
    EnvireGraph graph;
    const FrameId a = "Malcolm Reynolds";
    Item<string>::Ptr item1(new Item<string>("Zoe Washburne"));
    Item<string>::Ptr item2(new Item<string>("Hoban Washburne"));
    Item<string>::Ptr item3(new Item<string>("Inara Serra"));
    Item<string>::Ptr item4(new Item<string>("Jayne Cobb"));
    Item<string>::Ptr item5(new Item<string>("Kaylee Frye"));
    
    graph.addFrame(a);
    graph.addItemToFrame(a, item1); 
    graph.addItemToFrame(a, item2);
    graph.addItemToFrame(a, item3);
    graph.addItemToFrame(a, item4);
    graph.addItemToFrame(a, item5);
  
    graph.removeItemFromFrame(item2);
    BOOST_CHECK(item2->getFrame() == "");
    
    const Frame::ItemList& items = graph.getItems(a, item1->getTypeIndex());
    BOOST_CHECK(std::find(items.begin(), items.end(), item2) == items.end());
    
    item2->setFrame(a); //hack to test the exception, do NOT do this in real code!
    BOOST_CHECK_THROW(graph.removeItemFromFrame(item2), UnknownItemException);
    
    item2->setFrame("unknown frame");
    BOOST_CHECK_THROW(graph.removeItemFromFrame(item2), UnknownFrameException);
 
}

BOOST_AUTO_TEST_CASE(envire_graph_serialization_test)
{
    FrameId a = "AA";
    FrameId b = "BBB";
    FrameId c = "CCCC";
    EnvireGraph graph;
    Transform ab;
    Transform bc;
    
    ab.transform.translation << 1, 2, 3;
    ab.transform.orientation.coeffs() << 0, 1, 2, 3; 
    ab.time = base::Time::now();
    bc.transform.translation << 4, 5, 6;
    bc.transform.orientation.coeffs() << -0, 2, 4, 6;
  
    graph.addTransform(a, b, ab);
    graph.addTransform(b,c, bc);
    
    std::stringstream stream;
    boost::archive::binary_oarchive oa(stream);
    oa << graph;
    boost::archive::binary_iarchive ia(stream);
    EnvireGraph graph2;
    ia >> graph2;   
    
}

BOOST_AUTO_TEST_CASE(envire_graph_publish_current_state_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    EnvireGraph graph;
    Transform ab;
    ItemBase::Ptr item1(new Item<string>("bla"));
    ItemBase::Ptr item2(new Item<int>(42));

    graph.addTransform(a, b, ab);
    graph.addItemToFrame(a, item1);
    graph.addItemToFrame(b, item2);

    EnvireDispatcher d;
    graph.subscribe(&d, true);

    BOOST_CHECK(d.itemAddedEvents.size() == 2);
    BOOST_CHECK(d.itemRemovedEvents.size() == 0);

    graph.unsubscribe(&d, true);

    BOOST_CHECK(d.itemRemovedEvents.size() == 2);
}

BOOST_AUTO_TEST_CASE(envire_graph_save_load_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    EnvireGraph graph;
    Transform ab;
//     ItemBase::Ptr item1(new Item<string>("bla"));
//     ItemBase::Ptr item2(new Item<int>(42));

    graph.addTransform(a, b, ab);
    
    //note: there is no easy way to test with items because we need to load
    //      them as plugins to get the class name for serialization.
    //      Thus we only test with a simple transform.
    
//     graph.addItemToFrame(a, item1);
//     graph.addItemToFrame(b, item2);
    BOOST_CHECK_NO_THROW(graph.saveToFile("save_envire_graph_test"));
    
    EnvireGraph loadGraph;
    BOOST_CHECK_NO_THROW(loadGraph.loadFromFile("save_envire_graph_test"));
    
    BOOST_CHECK(loadGraph.containsFrame(a));
    BOOST_CHECK(loadGraph.containsFrame(b));
    BOOST_CHECK_NO_THROW(loadGraph.getTransform(a, b));
    BOOST_CHECK_NO_THROW(loadGraph.getTransform(b, a));
//     BOOST_CHECK(loadGraph.getItemCount<Item<string>>(a) == 1);
//     BOOST_CHECK(loadGraph.getItemCount<Item<int>>(b) == 1);
    
    
}

BOOST_AUTO_TEST_CASE(envire_graph_structural_copy_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    FrameId c = "frame_c";
    EnvireGraph graph;
    Transform tf;
    ItemBase::Ptr item1(new Item<string>("bla"));
    ItemBase::Ptr item2(new Item<int>(42));

    graph.addTransform(a, b, tf);
    graph.addItemToFrame(a, item1);
    graph.addItemToFrame(b, item2);
    graph.addTransform(a, c, tf);
    graph.addTransform(c, b, tf);
    graph.addItemToFrame(c, item2);
    graph.addItemToFrame(c, item1);
    
    EnvireGraph copy;
    graph.createStructuralCopy(copy);
    
    BOOST_CHECK(copy.num_edges() == graph.num_edges());
    BOOST_CHECK(copy.num_vertices() == graph.num_vertices());
    BOOST_CHECK_NO_THROW(copy.getTransform(a, b));
    BOOST_CHECK_NO_THROW(copy.getTransform(c, b));
    BOOST_CHECK_NO_THROW(copy.getTransform(a, c));
    
    BOOST_CHECK(copy.getTotalItemCount(a) == 0);
    BOOST_CHECK(copy.getTotalItemCount(b) == 0);
    BOOST_CHECK(copy.getTotalItemCount(c) == 0);
}


BOOST_AUTO_TEST_CASE(bfs_visitor_test)
{
    /**If the bfs visitor is not parameterized correctly, this will segfault */
    EnvireGraph graph;

    const FrameId a = "a";
    const FrameId b = "b";

    graph.addFrame(b);
    graph.addFrame(a);
    
    BOOST_CHECK_NO_THROW(graph.getFrames(a, a));
    graph.removeFrame(b);
    BOOST_CHECK_NO_THROW(graph.getFrames(a, a));
}

