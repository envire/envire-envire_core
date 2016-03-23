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

#include <envire_pcl/PointCloud.hpp>
#include <pcl/io/pcd_io.h>

using namespace envire::core;
using namespace vizkit3d;


int testPcl(int argc, char **argv)
{
  
  plugin_manager::PluginLoader* loader = plugin_manager::PluginLoader::getInstance();
  envire::core::ItemBase::Ptr itemBase;
  loader->createInstance("envire::pcl::PointCloud", itemBase);
  envire::pcl::PointCloud::Ptr cloud = boost::dynamic_pointer_cast<envire::pcl::PointCloud>(itemBase);
  
  pcl::PCDReader reader;
  if(reader.read("/home/arne/git/rock-entern/slam/pcl/test/bunny.pcd", cloud->getData()) != 0) 
  {
    std::cerr << "UNABLE TO READ PCD FILE" << std::endl;
    exit(1);
  }
  
  EnvireGraph graph;
  graph.addFrame("A");
  graph.addFrame("B");
  graph.addItemToFrame("B", cloud);
  Eigen::Quaterniond q1(Eigen::AngleAxisd(0.5, Eigen::Vector3d(1,2,3)));
  Transform ab(base::Position(1, 1, 1), q1);
  graph.addTransform("A", "B", ab);
  

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
    while(true)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      Transform tf = graph.getTransform("A", "B");
      tf.transform.orientation *= base::Quaterniond(Eigen::AngleAxisd(0.23, base::Position(1, 0, 0)));
      graph.updateTransform("A", "B", tf);
    }
  });
  app.exec(); 
  t.join();
}


int main(int argc, char **argv)
{
  return testPcl(argc, argv);
}