#include "EnvireVisualizerWindow.hpp"
#include <envire_core/graph/EnvireGraph.hpp>
#include <envire_core/items/Transform.hpp>
#include <thread>
#include <chrono>

//following includes are only needed for writeGraphToFile()
#include <plugin_manager/PluginLoader.hpp>
#include <envire_core/items/ItemBase.hpp>
#include <envire_pcl/PointCloud.hpp>
#include <pcl/io/pcd_io.h>

// #include <vizkit3d/Vizkit3DWidget.hpp>
// #include <vizkit3d/QtThreadedWidget.hpp>
// #include <vizkit3d/QVizkitMainWindow.hpp>
// #include <iostream>
// #include "EnvireGraphVisualizer.hpp"
// #include "Vizkit3dPluginInformation.hpp"
// 

// #include <unordered_map>
// 
// #include <iostream>
// #include <fstream>

// #include <vector>

// 
// 

// using namespace envire::core;
// using namespace vizkit3d;
using namespace envire::viz;

void writeGraphToFile(const std::string& file)
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
  
  envire::core::EnvireGraph graph;
  graph.addFrame("A"); 
  graph.addFrame("B");
  graph.addFrame("C");
  graph.addFrame("D");
  graph.addItemToFrame("B", cloud);
  graph.addItemToFrame("D", cloud2);
  graph.addItemToFrame("A", cloud3); //special case item in root node
  
  envire::core::Transform ab(base::Position(1, 1, 1), Eigen::Quaterniond::Identity());
  graph.addTransform("A", "B", ab);
  envire::core::Transform bc(base::Position(1, 0, 0.3), Eigen::Quaterniond(Eigen::AngleAxisd(0.3, Eigen::Vector3d(1,0,3))));
  graph.addTransform("B", "C", ab);  
  envire::core::Transform cd(base::Position(0, 2, -1), Eigen::Quaterniond(Eigen::AngleAxisd(-0.8, Eigen::Vector3d(0,0,1))));
  graph.addTransform("C", "D", cd);  
  
  graph.addFrame("randTree");
  envire::core::Transform aToForrest(base::Position(0, -3, -2), Eigen::Quaterniond(Eigen::AngleAxisd(0, Eigen::Vector3d(0,0,1))));
  graph.addTransform("A", "randTree", aToForrest);
  
  graph.saveToFile(file);
}

int main(int argc, char **argv)
{
  writeGraphToFile("envire_graph_test_file");

  QApplication app(argc, argv);
  EnvireVisualizerWindow window;
  window.displayGraph("envire_graph_test_file");
  window.show();
  
  std::shared_ptr<envire::core::EnvireGraph> loadedGraph(window.getGraph());
  
  std::thread t([&]() 
  {
    //because the graph is not thread safe, yet  
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    envire::core::Transform tf;
    while(true)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      
      //rotate transformation
      tf = loadedGraph->getTransform("A", "B");
      tf.transform.orientation *= base::Quaterniond(Eigen::AngleAxisd(0.23, base::Position(1, 0, 0)));
      loadedGraph->updateTransform("A", "B", tf);      
    }
  });
  app.exec(); 
  t.join();
  return 0;
}