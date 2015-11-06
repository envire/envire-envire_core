#include <boost/test/unit_test.hpp>
#include <envire_core/all>
#include <envire/Core.hpp>
#include <chrono>
#include <iostream>

using namespace std;
using namespace Eigen;

/** 
 *  1.) add 100 children to root node.
 *  2.) create a chain of 100 children.
 *  3.) update each transform in the chain once.
 * 
 * 
 *  Repeat all tests several times and calc avg
 * 
 * 
 * */



/**returns the avg time to add 1 after 1000 tries*/
const double newAdd10000Children()
{
    using namespace envire::core;
    TransformGraph graph;
    const FrameId root("root");
    graph.addFrame(root);
    
    //create frameids beforehand. we dont want to benchmark boost::lexical_cast
    vector<FrameId> ids;
    for(int i = 0; i < 10000; ++i)
    {
        ids.push_back(boost::lexical_cast<FrameId>(i));
    }
    
    envire::core::Transform tf;
    
    auto start = chrono::steady_clock::now();
    for(size_t i = 0; i < 10000; ++i)
    {
        graph.addTransform(root, ids[i], tf);
    }
    auto end = chrono::steady_clock::now();
    double diff = chrono::duration <double, milli> (end - start).count();
    return diff;
    
}

/**returns the avg time to add 1 after 1000 tries*/
const double oldAdd10000Children()
{
    using namespace envire;
    // set up an environment
    boost::scoped_ptr<Environment> env( new Environment() );
    Affine3d transform(Translation3d( 0.0, 0.0, 0.5 ));
    FrameNode* root = env->getRootNode();
    
    auto start = chrono::steady_clock::now();
    for(size_t i = 0; i < 10000; ++i)
    {
        FrameNode* pNode = new FrameNode();
        pNode->setTransform(transform);
        env->addChild(root, pNode);
    }
    auto end = chrono::steady_clock::now();
    double diff = chrono::duration <double, milli> (end - start).count();
    return diff;
}


int main()
{
    cout << "-------------------------" << endl;
    cout << "Add 10000 children to root:" << endl;
    cout << "-------------------------" << endl;
    
    const double old10000 = oldAdd10000Children();
    const double new10000 = newAdd10000Children();
    cout << "old: " << old10000 << " millis, (" << old10000 / 10000.0 << " millis/frame)" << endl;
    cout << "new: " << new10000 << " millis, (" << new10000 / 10000.0 << " millis/frame)" << endl;
    
}