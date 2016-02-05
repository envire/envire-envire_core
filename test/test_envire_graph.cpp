#include <boost/test/unit_test.hpp>
#include <boost/lexical_cast.hpp>
#define protected public
#include <envire_core/graph/EnvireGraph.hpp>
#include <envire_core/events/GraphEventDispatcher.hpp>
#include <envire_core/events/GraphItemEventDispatcher.hpp>
#include <envire_core/items/Item.hpp>
#include <envire_core/graph/GraphViz.hpp>
#include <vector>


using namespace envire::core;
using namespace std;



class EnvireDispatcher : public GraphEventDispatcher {
public:
    vector<ItemAddedEvent> itemAddedEvents;
    vector<ItemRemovedEvent> itemRemovedEvents;
    
    EnvireDispatcher(EnvireGraph& graph) : GraphEventDispatcher(&graph) {}
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
        DataA(const string& value) : value(value){}
        string value;
    };
    struct DataB
    {
        DataB(const int value) : value(value){}
        int value;
    };
    
    EnvireGraph g;
    g.addFrame(a);


    Item<DataA>::Ptr a1(new Item<DataA>("Grandpa, why don't you tell a story?"));
    Item<DataA>::Ptr a2(new Item<DataA>("Yeah Grandpa, you lived a long and interesting life."));
    Item<DataA>::Ptr a3(new Item<DataA>("That's a lie and you know it"));
    
    Item<DataB>::Ptr b1(new Item<DataB>(42));
    Item<DataB>::Ptr b2(new Item<DataB>(21));
    Item<DataB>::Ptr b3(new Item<DataB>(84));
    
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


BOOST_AUTO_TEST_CASE(check_item_existence_example)
{
    struct Joint
    {
      Joint(int a) : a(a) {}
      int a;
    };
    struct Sensor {};
    
    EnvireGraph g;
    FrameId frame = "frame";
    g.addFrame(frame);
    
    Item<Joint>::Ptr item(new Item<Joint>(42));
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
    
    BOOST_CHECK_THROW(graph.containsItems<Item<string>>(a), UnknownFrameException)
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
    GraphViz viz;
    viz.write(graph, "envireGraph_graphviz_test.dot");
}

BOOST_AUTO_TEST_CASE(complex_graphviz_test)
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
    
    GraphViz viz;
    viz.write(graph, "envireGraph_complex_graphviz_test.dot");
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


