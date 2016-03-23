#include <vizkit3d/Vizkit3DWidget.hpp>
#include <vizkit3d/QtThreadedWidget.hpp>
#include <vizkit3d/QVizkitMainWindow.hpp>
#include <iostream>
#include "EnvireGraphVisualizer.hpp"
#include "Vizkit3dPluginInformation.hpp"
#include <envire_core/graph/EnvireGraph.hpp>
#include <unordered_map>
#include <plugin_manager/PluginLoader.hpp>
#include <envire_octomap/OcTree.hpp> //FIXME eigentlich sollte das ohne gehen?
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <boost/uuid/uuid.hpp>

#include <envire_pcl/PointCloud.hpp>
#include <pcl/io/pcd_io.h>

using namespace envire::core;
using namespace vizkit3d;


int testPcl(int argc, char **argv)
{
  
  plugin_manager::PluginLoader* loader = plugin_manager::PluginLoader::getInstance();
  envire::core::ItemBase::Ptr cloudItem;
  envire::core::ItemBase::Ptr cloudItem2;
  loader->createInstance("envire::pcl::PointCloud", cloudItem);
    loader->createInstance("envire::pcl::PointCloud", cloudItem2);
  envire::pcl::PointCloud::Ptr cloud = boost::dynamic_pointer_cast<envire::pcl::PointCloud>(cloudItem);
  envire::pcl::PointCloud::Ptr cloud2 = boost::dynamic_pointer_cast<envire::pcl::PointCloud>(cloudItem2);
  
  //FIXME path relative to rock instead of absolut?
  pcl::PCDReader reader;
  if(reader.read("/home/arne/git/rock-entern/slam/pcl/test/bunny.pcd", cloud->getData()) != 0) 
  {
    std::cerr << "UNABLE TO READ PCD FILE" << std::endl;
    exit(1);
  }
    
  if(reader.read("/home/arne/git/rock-entern/slam/pcl/test/bunny.pcd", cloud2->getData()) != 0) 
  {
    std::cerr << "UNABLE TO READ PCD FILE" << std::endl;
    exit(1);
  }
  
  EnvireGraph graph;
  graph.addFrame("A");
  graph.addFrame("B");
  graph.addFrame("C");
  graph.addFrame("D");
  graph.addItemToFrame("B", cloud);
  graph.addItemToFrame("D", cloud2);
  Transform ab(base::Position(1, 1, 1), Eigen::Quaterniond (Eigen::AngleAxisd(0.5, Eigen::Vector3d(1,2,3))));
  graph.addTransform("A", "B", ab);
  Transform bc(base::Position(1, 0, 0.3), Eigen::Quaterniond(Eigen::AngleAxisd(0.3, Eigen::Vector3d(1,0,3))));
  graph.addTransform("B", "C", ab);  
  Transform cd(base::Position(0, 2, -1), Eigen::Quaterniond(Eigen::AngleAxisd(-0.8, Eigen::Vector3d(0,0,1))));
  graph.addTransform("C", "D", cd);  
  
  graph.addFrame("randTree");
  Transform aToForrest(base::Position(0, -3, -2), Eigen::Quaterniond(Eigen::AngleAxisd(0, Eigen::Vector3d(0,0,1))));
  graph.addTransform("A", "randTree", aToForrest);
  QApplication app(argc, argv);
  QVizkitMainWindow window;
  Vizkit3DWidget* widget = window.getVizkitWidget();
  window.show();
  envire::viz::Vizkit3dPluginInformation info(widget);
  envire::viz::EnvireGraphVisualizer visualizer(graph, widget, "A", info);

  std::thread t([&graph]()
  {
    //because the graph is not thread safe, yet  
    std::this_thread::sleep_for(std::chrono::seconds(4));
    
    std::vector<FrameId> randTreeNodes;
    randTreeNodes.push_back("randTree");
    boost::uuids::random_generator generator;
    int i = 0;
    
    while(true)
    {
      ++i;
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      
      //rotate transformation
      Transform tf = graph.getTransform("A", "B");
      tf.transform.orientation *= base::Quaterniond(Eigen::AngleAxisd(0.23, base::Position(1, 0, 0)));
      graph.updateTransform("A", "B", tf);
      
      
      //change length of transformation
      tf = graph.getTransform("C", "D");
      if(tf.transform.translation.norm() >= 10)
      {
        tf.transform.translation << 0, 1, 0;
      }
      else
      {
        tf.transform.translation.x() += 0.1;
      }
      graph.updateTransform("C", "D", tf);
      
      //random growing tree
      if((i % 15) == 0)
      {
        const int forrestElem = rand() % randTreeNodes.size();
        boost::uuids::uuid id = generator();
        const FrameId nextName = boost::uuids::to_string(id);
        graph.addFrame(nextName);
        tf.transform.orientation = base::Quaterniond(Eigen::AngleAxisd(0.0, base::Position(0, 0, 1)));
        tf.transform.translation << (rand() % 40) / 20.0, (rand() % 40) / 20.0, 0;
        graph.addTransform(randTreeNodes[forrestElem], nextName, tf);
        randTreeNodes.push_back(nextName);
      }
      
    }
  });
  app.exec(); 
  t.join();
}


int main(int argc, char **argv)
{
  return testPcl(argc, argv);
}