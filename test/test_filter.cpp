#include <boost/test/unit_test.hpp>

#include <envire_core/graph/EnvireGraph.hpp>
#include <envire_core/items/Item.hpp>

#include <typeinfo>

using namespace envire::core;

BOOST_AUTO_TEST_CASE(test_copy)
{
    FrameId aFrame = "frame_a";
    FrameId bFrame = "frame_b";
    FrameId cFrame = "frame_c";

    Item<std::string>::Ptr item_a_0(new Item<std::string>(std::string("item_in_frame_a_0")));
    Item<std::string>::Ptr item_a_1(new Item<std::string>(std::string("item_in_frame_a_1")));
    Item<std::string>::Ptr item_b_0(new Item<std::string>(std::string("item_in_frame_b_0")));

    
    EnvireGraph *g = new EnvireGraph();
    g->addFrame(aFrame);
    g->addFrame(bFrame);
    g->addFrame(cFrame);
    
    g->addItemToFrame(aFrame, item_a_0);
    g->addItemToFrame(aFrame, item_a_1);
    g->addItemToFrame(bFrame, item_b_0);     

    EnvireGraph g_copy(*g);

    delete g;

    using AIterator = EnvireGraph::ItemIterator<Item<std::string>>;
    
    AIterator aBegin, aEnd;   
    AIterator bBegin, bEnd;    
    AIterator cBegin, cEnd;     

    // frame_a
    std::tie(aBegin, aEnd) = g_copy.getItems<Item<std::string>>(aFrame);
    BOOST_CHECK(aBegin != aEnd);
    BOOST_CHECK(aBegin->getData().compare(item_a_0->getData()) == 0);
    ++aBegin;
    BOOST_CHECK(aBegin->getData().compare(item_a_1->getData()) == 0);
    ++aBegin;    
    BOOST_CHECK(aBegin == aEnd);

    // frame_b
    std::tie(bBegin, bEnd) = g_copy.getItems<Item<std::string>>(bFrame);
    BOOST_CHECK(bBegin != bEnd);
    BOOST_CHECK(bBegin->getData().compare(item_b_0->getData()) == 0);
    ++bBegin;
    BOOST_CHECK(bBegin == bEnd);

    // frame_c
    std::tie(cBegin, cEnd) = g_copy.getItems<Item<std::string>>(cFrame);
    BOOST_CHECK(cBegin == cEnd);    
}


class A 
{
public:
	A(std::string name)
	: name(name) {}
	std::string name;
};

class B
{
public:
    B(std::string name)
    : name(name) {}
    std::string name;
};

BOOST_AUTO_TEST_CASE(test_copy_with_filter)
{
    FrameId aFrame = "frame_a";
    FrameId bFrame = "frame_b";
    FrameId cFrame = "frame_c";
    FrameId dFrame = "frame_d";

    // two object of a class in frame a
    Item<A>::Ptr item_a_in_a(new Item<A>(A("item_a_in_a")));
    Item<A>::Ptr item_a_in_a_1(new Item<A>(A("item_a_in_a_1")));

    // one object of a class in frame b
    // one object of b class in frame b
    Item<A>::Ptr item_a_in_b(new Item<A>(A("item_a_in_b")));
    Item<B>::Ptr item_b_in_b(new Item<B>(B("item_b_in_b")));

    // one object of c class in frame c
    Item<B>::Ptr item_b_in_c(new Item<B>(B("item_b_in_c")));

    
    EnvireGraph g;
    g.addFrame(aFrame);
    g.addFrame(bFrame);
    g.addFrame(cFrame);
    g.addFrame(dFrame);
    
    g.addItemToFrame(aFrame, item_a_in_a);
    g.addItemToFrame(aFrame, item_a_in_a_1);

    g.addItemToFrame(bFrame, item_a_in_b);     
    g.addItemToFrame(bFrame, item_b_in_b); 

    g.addItemToFrame(cFrame, item_b_in_c); 


    BOOST_CHECK(g.getTotalItemCount(aFrame) == 2);
    BOOST_CHECK(g.getTotalItemCount(bFrame) == 2);
    BOOST_CHECK(g.getTotalItemCount(cFrame) == 1);
    BOOST_CHECK(g.getTotalItemCount(dFrame) == 0);
    

    // copy graph, presurve only object of the class A
    std::unordered_set<std::type_index> filter_list;
    filter_list.emplace(typeid(envire::core::Item<A>));

    // RESULT:
    // two object of a class in frame a
    // one object of a class in frame b
    // frame c and d are empty
    EnvireGraph g_copy(g, &filter_list, true);
    
    BOOST_CHECK(g_copy.getTotalItemCount(aFrame) == 2);
    BOOST_CHECK(g_copy.getTotalItemCount(bFrame) == 1);
    BOOST_CHECK(g_copy.getTotalItemCount(cFrame) == 0);
    BOOST_CHECK(g_copy.getTotalItemCount(dFrame) == 0);
    

    using AIterator = EnvireGraph::ItemIterator<Item<A>>;
    using BIterator = EnvireGraph::ItemIterator<Item<B>>;
    
    AIterator aBegin, aEnd;   
    BIterator bBegin, bEnd;  

    // frame_a
    std::tie(aBegin, aEnd) = g_copy.getItems<Item<A>>(aFrame);
    BOOST_CHECK(aBegin != aEnd);
    BOOST_CHECK(aBegin->getData().name.compare(item_a_in_a->getData().name) == 0);
    ++aBegin;
    BOOST_CHECK(aBegin->getData().name.compare(item_a_in_a_1->getData().name) == 0);
    ++aBegin;    
    BOOST_CHECK(aBegin == aEnd);

    std::tie(bBegin, bEnd) = g_copy.getItems<Item<B>>(aFrame);
    BOOST_CHECK(bBegin == bEnd);

    // frame_b
    std::tie(aBegin, aEnd) = g_copy.getItems<Item<A>>(bFrame);
    BOOST_CHECK(aBegin != aEnd);
    BOOST_CHECK(aBegin->getData().name.compare(item_a_in_b->getData().name) == 0);
    ++aBegin;
    BOOST_CHECK(aBegin == aEnd);

 
}
