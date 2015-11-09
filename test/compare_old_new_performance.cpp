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


const double oldAdd10000Children()
{
    using namespace envire;

    boost::scoped_ptr<Environment> env(new Environment());
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

const double oldAdd10000Chain()
{
  using namespace envire;

  boost::scoped_ptr<Environment> env(new Environment());
  Affine3d transform(Translation3d( 0.0, 0.0, 0.5 ));
  
  auto start = chrono::steady_clock::now();
  
  FrameNode* node = env->getRootNode();
  for(int i = 0; i < 10000; ++i)
  {
      FrameNode* child = new FrameNode();
      child->setTransform(transform);
      env->addChild(node, child); 
      node = child;
  }
  
  auto end = chrono::steady_clock::now();
  double diff = chrono::duration <double, milli> (end - start).count();
  return diff;
}

const double newAdd10000Chain()
{
    using namespace envire::core;
    TransformGraph graph;
    envire::core::Transform tf;
    
    //create frameids beforehand. we dont want to benchmark boost::lexical_cast
    vector<FrameId> ids;
    for(int i = 0; i < 10000; ++i)
    {
        ids.push_back(boost::lexical_cast<FrameId>(i));
    }
    
    auto start = chrono::steady_clock::now();
    for(int i = 0; i < 10000 - 1; ++i)
    {
        graph.addTransform(ids[i], ids[i+1], tf);
    }
    auto end = chrono::steady_clock::now();
    double diff = chrono::duration <double, milli> (end - start).count();
    return diff;     
}

/**Helper method that builds a tree of depth @p i with 4 children per node */
void addFourChildren(envire::core::TransformGraph& graph, const envire::core::FrameId& id, const int i)
{
  if(i == 0) 
      return;
  for(int j = 0; j < 4; ++j)
  {
      envire::core::Transform tf;
      tf.setTransform(base::TransformWithCovariance::Identity());
      const envire::core::FrameId newId = id + boost::lexical_cast<envire::core::FrameId>(j);
      graph.addTransform(id, newId, tf);
      addFourChildren(graph, newId, i - 1);
  }
}

/**Calculate the transform for a path of length 4 in a tree of depth 4 where
   each vertex has 4 children*/
const double newTranformSearch10000()
{
    using namespace envire::core;
    TransformGraph graph;
    const FrameId root("4");
    graph.addFrame(root);
    addFourChildren(graph, root, 3);
    
    GraphViz viz;
    viz.write(graph, "test.dot");
    
    //we search from "4" to "4333" which is the rightmost child
    //according to the naming schema of addFourChildren()
    
    auto start = chrono::steady_clock::now();
    for(int i = 0; i < 10000; ++i)
    {
        const envire::core::Transform tf = graph.getTransform("4", "4333");
    }
    auto end = chrono::steady_clock::now();
    double diff = chrono::duration <double, milli> (end - start).count();
    return diff;  
}

/** same as addFourChildren() for the old envire. returns the rightmost child */
envire::FrameNode* addFourChildrenOld(envire::Environment* env, envire::FrameNode* parent, const int i)
{
  envire::FrameNode* target = NULL;
  if(i == 0)
      return parent;
  for(int j = 0; j < 4; ++j)
  {
      Affine3d transform(Translation3d( 0.0, 0.0, 0.5 ));
      envire::FrameNode* child = new envire::FrameNode();
      child->setTransform(transform);
      env->addChild(parent, child);
      target =  addFourChildrenOld(env, child, i - 1);
  }  
  return target;
}


const double oldTransformSearch10000()
{
    using namespace envire;
    boost::scoped_ptr<Environment> env(new Environment());
    FrameNode* root = new FrameNode();
    FrameNode* target = addFourChildrenOld(env.get(), root, 3);
    auto start = chrono::steady_clock::now();
    for(int i = 0; i < 10000; ++i)
    {
        const auto transform = env->relativeTransform(root, target);
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
    cout << endl;
    
    cout << "-------------------------" << endl;
    cout << "Add 10000 children chain:" << endl;
    cout << "-------------------------" << endl;  
    const double oldChain = oldAdd10000Chain();
    const double newChain = newAdd10000Chain();
    cout << "old: " << oldChain << " millis, (" << oldChain / 10000.0 << " millis/frame)" << endl;
    cout << "new: " << newChain << " millis, (" << newChain / 10000.0 << " millis/frame)" << endl;
    cout << endl;
    
    cout << "-------------------------" << endl;
    cout << "Calculate transform 10000 times:" << endl;
    cout << "-------------------------" << endl;  
    const double oldSearch = oldTransformSearch10000();
    const double newSearch = newTranformSearch10000();
    cout << "old: " << oldSearch << " millis, (" << oldSearch / 10000.0 << " millis/search)" << endl;
    cout << "new: " << newSearch << " millis, (" << newSearch / 10000.0 << " millis/search)" << endl;
    
    
}