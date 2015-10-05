#include <boost/test/unit_test.hpp>
#include <envire_core/all>
#include <vector>
#include <chrono>

using namespace envire::core;
using namespace std;


vector<FrameId> ids;

void fillIds()
{
    for(int i = 0; i <= 5000000; ++i)
    {
        ids.push_back(boost::lexical_cast<FrameId>(i));
    }    
}

void addTransforms()
{
    TransformGraph graph;
    //create ids beforehand. we are not benchmarking boost

    
    Transform tf;
    auto start = chrono::steady_clock::now();
    for(int i = 0; i < ids.size() - size_t(1); ++i)
    {
        graph.addTransform(ids[i], ids[i + 1], tf);
    }
    auto end = chrono::steady_clock::now();
    auto diff = end - start;
    cout << ids.size() << " insert: " << chrono::duration <double, milli> (diff).count() << " ms" << endl;

}

void getTransform()
{
    TransformGraph graph;    
    Transform tf;
    for(int i = 0; i < ids.size() - size_t(1); ++i)
    {
        graph.addTransform(ids[i], ids[i + 1], tf);
    }    

    auto start = chrono::steady_clock::now();
    for(int i = 0; i < ids.size() - size_t(1); ++i)
    {
        graph.getTransform(ids[i], ids[i + 1]);
    }    
    auto end = chrono::steady_clock::now();
    auto diff = end - start;
    cout << ids.size() << " get 0: " << chrono::duration <double, milli> (diff).count() << " ms" << endl;
}

void getTransform1(const int len)
{
    TransformGraph graph;    
    Transform tf;
    for(int i = 0; i < ids.size() - size_t(1); ++i)
    {
        graph.addTransform(ids[i], ids[i + 1], tf);
    }  
    
    auto start = chrono::steady_clock::now();
    for(int i = 0; i < ids.size() - size_t(len + 1); ++i)
    {
        graph.getTransform(ids[i], ids[i + len]);
    }    
    auto end = chrono::steady_clock::now();
    auto diff = end - start;
    cout << ids.size() << " get " << len <<": " << chrono::duration <double, milli> (diff).count() << " ms" << endl;
}


BOOST_AUTO_TEST_CASE(add_transforms)
{
    fillIds();
    addTransforms();
    getTransform();
    for(int i = 1; i < 10; ++i)
      getTransform1(i);
    
  
}
