/**Contains two examples showing the usage of the EnvireGraphVisualizer.
 * minimalExample() shows how to use the visualizer in a seperate thread.
 * test() shows how to use the visualizer in the main thread. */
#include <vizkit3d/Vizkit3DWidget.hpp>
#include <vizkit3d/QtThreadedWidget.hpp>
#include <vizkit3d/QVizkitMainWindow.hpp>
#include <iostream>
#include "EnvireGraphVisualizer.hpp"
#include "Vizkit3dPluginInformation.hpp"
#include <envire_core/graph/EnvireGraph.hpp>
#include "MainWindow.hpp"
#include <unordered_map>
#include <plugin_manager/PluginLoader.hpp>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <vector>
#include <boost/uuid/uuid.hpp>

#include <envire_pcl/PointCloud.hpp>
#include <pcl/io/pcd_io.h>

using namespace envire::core;
using namespace vizkit3d;
using namespace envire::viz;

int test(int argc, char **argv)
{
  
  plugin_manager::PluginLoader* loader = plugin_manager::PluginLoader::getInstance();
  envire::core::ItemBase::Ptr cloudItem;
  envire::core::ItemBase::Ptr cloudItem2;
  envire::core::ItemBase::Ptr cloudItem3;
  loader->createInstance("envire::pcl::PointCloud", cloudItem);
  loader->createInstance("envire::pcl::PointCloud", cloudItem2);
  loader->createInstance("envire::pcl::PointCloud", cloudItem3);
  envire::pcl::PointCloud::Ptr cloud = boost::dynamic_pointer_cast<envire::pcl::PointCloud>(cloudItem);
  envire::pcl::PointCloud::Ptr cloud2 = boost::dynamic_pointer_cast<envire::pcl::PointCloud>(cloudItem2);
  envire::pcl::PointCloud::Ptr cloud3 = boost::dynamic_pointer_cast<envire::pcl::PointCloud>(cloudItem3);
  
  //FIXME path relative to rock instead of absolut?
  pcl::PCDReader reader;
  reader.read("/home/arne/git/rock-entern/slam/pcl/test/bunny.pcd", cloud->getData());
  reader.read("/home/arne/git/rock-entern/slam/pcl/test/bunny.pcd", cloud2->getData());
  reader.read("/home/arne/git/rock-entern/slam/pcl/test/cturtle.pcd", cloud3->getData());
  
  EnvireGraph graph;
  graph.addFrame("A"); 
  graph.addFrame("B");
  graph.addFrame("C");
  graph.addFrame("D");
  graph.addItemToFrame("B", cloud);
  graph.addItemToFrame("D", cloud2);
  graph.addItemToFrame("A", cloud3); //special case item in root node
  
  Transform ab(base::Position(1, 1, 1), Eigen::Quaterniond::Identity());
  graph.addTransform("A", "B", ab);
  Transform bc(base::Position(1, 0, 0.3), Eigen::Quaterniond(Eigen::AngleAxisd(0.3, Eigen::Vector3d(1,0,3))));
  graph.addTransform("B", "C", ab);  
  Transform cd(base::Position(0, 2, -1), Eigen::Quaterniond(Eigen::AngleAxisd(-0.8, Eigen::Vector3d(0,0,1))));
  graph.addTransform("C", "D", cd);  
  
  graph.addFrame("randTree");
  Transform aToForrest(base::Position(0, -3, -2), Eigen::Quaterniond(Eigen::AngleAxisd(0, Eigen::Vector3d(0,0,1))));
  graph.addTransform("A", "randTree", aToForrest);
  
  graph.saveToFile("envire_graph_test23");
  
  QApplication app(argc, argv);
  MainWindow window;
  window.displayGraph("envire_graph_test23");
  window.show();
  
  std::shared_ptr<EnvireGraph> loadedGraph(window.getGraph());
  
  
  std::thread t([&]() 
  {
    //because the graph is not thread safe, yet  
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    std::vector<FrameId> randTreeNodes;
    std::vector<ItemBase::Ptr> randTreeItems;
    randTreeNodes.push_back("randTree");
    boost::uuids::random_generator generator;
    int i = 0;
    bool removeSubTree = false;
    
    Transform tf;
    while(true)
    {
      ++i;
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      
      //rotate transformation
      tf = loadedGraph->getTransform("A", "B");
      tf.transform.orientation *= base::Quaterniond(Eigen::AngleAxisd(0.23, base::Position(1, 0, 0)));
      loadedGraph->updateTransform("A", "B", tf);
      
      
//       //change length of transformation
//       tf = graph.getTransform("C", "D");
//       if(tf.transform.translation.norm() >= 10)
//       {
//         tf.transform.translation << 0, 1, 0;
//       }
//       else
//       {
//         tf.transform.translation.x() += 0.1;
//       }
//       graph.updateTransform("C", "D", tf);
      
//       //random growing tree
//       if((i % 10) == 0)
//       {
//         const int elem = rand() % randTreeNodes.size();
//         boost::uuids::uuid id = generator();
//         const FrameId nextName = boost::uuids::to_string(id);
//         graph.addFrame(nextName);
//         tf.transform.orientation = base::Quaterniond(Eigen::AngleAxisd(0.0, base::Position(0, 0, 1)));
//         tf.transform.translation << (rand() % 40) / 20.0, (rand() % 40) / 20.0, 0;
//         graph.addTransform(randTreeNodes[elem], nextName, tf);
//         randTreeNodes.push_back(nextName);
//       }
      
//       //randomly add items to the growing tree
//       if((i % 20) == 0)
//       {
//         //FIXME path
//         envire::core::ItemBase::Ptr item;
//         loader->createInstance("envire::pcl::PointCloud", item);
//         envire::pcl::PointCloud::Ptr milk = boost::dynamic_pointer_cast<envire::pcl::PointCloud>(item);
//         if(reader.read("/home/arne/git/rock-entern/slam/pcl/test/bunny.pcd", milk->getData()) == 0)
//         {
//           const int elem = rand() % randTreeNodes.size();
//           graph.addItemToFrame(randTreeNodes[elem], milk);
//           randTreeItems.push_back(milk);
//         }
//       }
//       //remove items from the tree
//       if((i % 30) == 0 && randTreeItems.size() > 0)
//       {
//         LOG(INFO) << "removing item from frame " << randTreeItems.back()->getFrame();
//         graph.removeItemFromFrame(randTreeItems.back());
//         randTreeItems.pop_back();
//       } 
//       
//       
//       if((i % 12 == 0))
//       {
//         if(graph.getTotalItemCount("A") > 0)
//         {
//           graph.clearFrame("A");
//         }
//         else
//         {
//           graph.addItemToFrame("A", cloud3);
//         }
//       }
      /*
      // add a sub tree
      if((i % 10) == 0) 
      {
        if(!removeSubTree)
        {
          tf.transform.orientation = base::Quaterniond(Eigen::AngleAxisd(0.0, base::Position(0, 0, 1)));
          tf.transform.translation << 3,4,5;
          graph->addTransform("A", "sub1", tf);
          tf.transform.orientation = base::Quaterniond(Eigen::AngleAxisd(0.0, base::Position(0, 0, 1)));
          tf.transform.translation << 1,0,2;
          graph->addTransform("sub1", "sub2", tf);
          tf.transform.translation << 0,1,2;
          graph->addTransform("sub1", "sub3", tf);
          removeSubTree = true;
        }
        else
        {
          graph->remove_edge("sub1", "sub2");
          graph->remove_edge("sub1", "sub3");
          graph->remove_edge("A", "sub1");
          removeSubTree = false;
        } 
        } */
    
    }
  });
  app.exec(); 
  t.join();
  return 0;
}


int main(int argc, char **argv)
{
  //return testWindow(argc, argv);
  return test(argc, argv);
 //return testThreaded(argc, argv);
}