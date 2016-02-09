#include <boost/test/unit_test.hpp>
#include <envire/Core.hpp>
#include <envire_core/all>
#include <envire_core/graph/GraphTypes.hpp>
#include <chrono>
#include <iostream>

using namespace std;
using namespace Eigen;

const double newAdd10000Children()
{
    using namespace envire::core;
    EnvireGraph graph;
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
    EnvireGraph graph;
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
void addFourChildren(envire::core::EnvireGraph& graph, const envire::core::FrameId& id, const int i)
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
    EnvireGraph graph;
    const FrameId root("4");
    graph.addFrame(root);
    addFourChildren(graph, root, 3);
    
    
    //we search from "4" to "4333" which is the rightmost child
    //according to the naming schema of addFourChildren()
    
    auto start = chrono::steady_clock::now();
    envire::core::Transform tf;
    for(int i = 0; i < 10000; ++i)
    {
        tf = graph.getTransform("4", "4333");
    }
    auto end = chrono::steady_clock::now();
    double diff = chrono::duration <double, milli> (end - start).count();
    return diff;  
}

/**Calculate the transform for a path of length 4 in a tree of depth 4 where
   each vertex has 4 children. This method uses a priori tree */
const double newTranformSearchUsingTree10000()
{
    using namespace envire::core;
    EnvireGraph graph;
    const FrameId root("4");
    graph.addFrame(root);
    addFourChildren(graph, root, 3);

    /** Build a tree view from the graph **/
    envire::core::TreeView view = graph.getTree(root);

    //we search from "4" to "4333" which is the rightmost child
    //according to the naming schema of addFourChildren()
    const envire::core::GraphTraits::vertex_descriptor from = graph.getVertex("4");
    const envire::core::GraphTraits::vertex_descriptor to = graph.getVertex("4333");
    auto start = chrono::steady_clock::now();
    envire::core::Transform tf;
    for(int i = 0; i < 10000; ++i)
    {
        tf = graph.getTransform(from, to, view);
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
  {
      return parent;
  }
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
    FrameNode* root = env->getRootNode();
    FrameNode* target = addFourChildrenOld(env.get(), root, 3);
    auto start = chrono::steady_clock::now();
    envire::Transform tf;
    for(int i = 0; i < 10000; ++i)
    {
      tf = env->relativeTransform(target, root);
    }
    auto end = chrono::steady_clock::now();
    double diff = chrono::duration <double, milli> (end - start).count();
    return diff;  
}


const double oldUpdateTransform10000()
{
    using namespace envire;
    Environment* env = new Environment();
    FrameNode* root = env->getRootNode();
    FrameNode* other = new FrameNode();
    env->addChild(root, other);
    Affine3d transform(Translation3d( 0.0, 0.0, 0.5 ));
    other->setTransform(transform);
    auto start = chrono::steady_clock::now();
    for(int i = 0; i < 10000; ++i)
    {
        other->setTransform(transform);
    }
    auto end = chrono::steady_clock::now();
    double diff = chrono::duration <double, milli> (end - start).count();
    return diff;     
}

const double newUpdateTransform10000()
{
    using namespace envire::core;
    EnvireGraph graph;
    const FrameId root("4");
    const FrameId other("other");
    envire::core::Transform tf;
    graph.addTransform(root, other, tf);
    
    auto start = chrono::steady_clock::now();
    for(int i = 0; i < 10000; ++i)
    {
        graph.updateTransform(root, other, tf);
    }
    auto end = chrono::steady_clock::now();
    double diff = chrono::duration <double, milli> (end - start).count();
    return diff;      
}

const double oldGetTransform10000()
{
    using namespace envire;
    Environment* env = new Environment();
    FrameNode* root = env->getRootNode();
    FrameNode* other = new FrameNode();
    env->addChild(root, other);
    Affine3d transform(Translation3d( 0.0, 0.0, 0.5 ));
    other->setTransform(transform);
    
    
    auto start = chrono::steady_clock::now();
    envire::TransformWithUncertainty tf;
    for(int i = 0; i < 10000; ++i)
    {
        tf= other->getTransformWithUncertainty();
    }
    auto end = chrono::steady_clock::now();
    double diff = chrono::duration <double, milli> (end - start).count();
    return diff;       
}

const double newGetTransform10000()
{
    using namespace envire::core;
    EnvireGraph graph;
    const FrameId roo("4");
    const FrameId othe("other");
    envire::core::Transform tf;
    graph.addTransform(roo, othe, tf);
    
    envire::core::GraphTraits::vertex_descriptor root = graph.vertex(roo);
    envire::core::GraphTraits::vertex_descriptor other = graph.vertex(othe);
    
    auto start = chrono::steady_clock::now();
    for(int i = 0; i < 10000; ++i)
    {
        tf = graph.getTransform(root, other);
    }
    auto end = chrono::steady_clock::now();
    double diff = chrono::duration <double, milli> (end - start).count();
    return diff;        
}



class MyMap : public envire::CartesianMap
{
public:
  MyMap(const std::string& id) : CartesianMap(id) {}
  virtual int getDimension() const override {return 42;}
};

const double newAddItemCartesianMap10000()
{
    using namespace envire::core;
    using Item = Item<MyMap>;
    
    EnvireGraph graph;
    const FrameId root("4");
    graph.addFrame(root);
        
    //create items beforehand, we dont want ti benchmark the item ctor
    vector<Item::Ptr> items;
    for(int i = 0; i < 10000; ++i)
    {
      items.emplace_back(new Item("bla")); //id is not used, therefore just use "bla"
    }
    
    auto start = chrono::steady_clock::now();
    for(int i = 0; i < 10000; ++i)
    {
        graph.addItemToFrame(root, items[i]);
    }
    auto end = chrono::steady_clock::now();
    double diff = chrono::duration <double, milli> (end - start).count();
    return diff;  
}

const double oldAddItemCartesianMap10000()
{
    using namespace envire;
    Environment* env = new Environment();
    FrameNode* root = env->getRootNode();
    
    //create items beforehand, we dont want ti benchmark the item ctor
    vector<MyMap*> items;
    for(int i = 0; i < 10000; ++i)
    {
      const string id = string("root/") + boost::lexical_cast<string>(i);
      items.emplace_back(new MyMap(id)); //id is not used, therefore just use "bla"
    }
    
    auto start = chrono::steady_clock::now();
    for(int i = 0; i < 10000; ++i)
    {
        env->attachItem(items[i], root);
    }
    auto end = chrono::steady_clock::now();
    double diff = chrono::duration <double, milli> (end - start).count();
    return diff;  
}

const double oldGetItemCartesianMap10000()
{
    using namespace envire;
    Environment* env = new Environment();
    FrameNode* root = env->getRootNode();
    MyMap* map = new MyMap("root/test");
    env->attachItem(map, root);
    
    auto start = chrono::steady_clock::now();
    for(int i = 0; i < 10000; ++i)
    {
        volatile EnvironmentItem::Ptr p = env->getItem("root/test");
    }
    auto end = chrono::steady_clock::now();
    double diff = chrono::duration <double, milli> (end - start).count();
    return diff;      
}

const double newGetItemCartesianMap10000()
{
    using namespace envire::core;
    using Item = Item<MyMap>;
    
    EnvireGraph graph;
    const FrameId root("4");
    graph.addFrame(root);
    Item::Ptr item(new Item("bla"));
    graph.addItemToFrame(root, item);
    const envire::core::GraphTraits::vertex_descriptor rootV = graph.vertex(root);

    auto start = chrono::steady_clock::now();
    for(int i = 0; i < 10000; ++i)
    {
      (*graph.getItem<Item>(rootV)); //dereference to ensure that the dynamic_cast is included in the timing
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

    cout << "-----------------------------------------------------" << endl;
    cout << "Calculate transform 10000 times directly in the graph:" << endl;
    cout << "-----------------------------------------------------" << endl;  
    const double oldSearch = oldTransformSearch10000();
    const double newSearch = newTranformSearch10000();
    cout << "old: " << oldSearch << " millis, (" << oldSearch / 10000.0 << " millis/search)" << endl;
    cout << "new: " << newSearch << " millis, (" << newSearch / 10000.0 << " millis/search)" << endl;

    cout << "-------------------------------------------------" << endl;
    cout << "Calculate transform 10000 times using a tree view:" << endl;
    cout << "-------------------------------------------------" << endl;
    const double newTreeSearch = newTranformSearchUsingTree10000();
    cout << "old: " << oldSearch << " millis, (" << oldSearch / 10000.0 << " millis/search)" << endl;
    cout << "new: " << newTreeSearch << " millis, (" << newTreeSearch / 10000.0 << " millis/search)" << endl;

    cout << "-------------------------" << endl;
    cout << "Update transform 10000 times:" << endl;
    cout << "-------------------------" << endl;  
    const double oldUpdateTransform = oldUpdateTransform10000();
    const double newUpdateTransform = newUpdateTransform10000();
    cout << "old: " << oldUpdateTransform << " millis, (" << oldUpdateTransform / 10000.0 << " millis/update)" << endl;
    cout << "new: " << newUpdateTransform << " millis, (" << newUpdateTransform / 10000.0 << " millis/update)" << endl;

    cout << "-------------------------" << endl;
    cout << "Get direct transform 10000 times:" << endl;
    cout << "-------------------------" << endl;  
    const double oldGet = oldGetTransform10000();
    const double newGet = newGetTransform10000();
    cout << "old: " << oldGet << " millis, (" << oldGet / 10000.0 << " millis/get)" << endl;
    cout << "new: " << newGet << " millis, (" << newGet / 10000.0 << " millis/get)" << endl;

    cout << "-------------------------" << endl;
    cout << "Add 10000 CartesianMaps to frame:" << endl;
    cout << "-------------------------" << endl;  
    const double oldAddItem = oldAddItemCartesianMap10000();
    double newAddItem = newAddItemCartesianMap10000();
    cout << "old: " << oldAddItem << " millis, (" << oldAddItem / 10000.0 << " millis/item)" << endl;
    cout << "new: " << newAddItem << " millis, (" << newAddItem / 10000.0 << " millis/item)" << endl;

    cout << "-------------------------" << endl;
    cout << "Get 10000 CartesianMaps from frame:" << endl;
    cout << "-------------------------" << endl;  
    const double oldGetItem = oldGetItemCartesianMap10000();
    double newGetItem = newGetItemCartesianMap10000();
    cout << "old: " << oldGetItem << " millis, (" << oldGetItem / 10000.0 << " millis/item)" << endl;
    cout << "new: " << newGetItem << " millis, (" << newGetItem / 10000.0 << " millis/item)" << endl;
}


