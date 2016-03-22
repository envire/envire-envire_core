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

#include <envire_pcl/PointCloud.hpp>
#include <pcl/io/pcd_io.h>

using namespace envire::core;
using namespace vizkit3d;

void testOctoMap(int argc, char **argv)
{
  /*
    EnvireGraph graph;
  graph.addFrame("A");
  graph.addFrame("B");
//   graph.addFrame("C");
//   graph.addFrame("D");
  
  plugin_manager::PluginLoader* loader = plugin_manager::PluginLoader::getInstance();
  envire::core::ItemBase::Ptr itemBase;
  loader->createInstance("envire::octomap::OcTree", itemBase);
  envire::octomap::OcTree::Ptr ocTree = boost::dynamic_pointer_cast<envire::octomap::OcTree>(itemBase);
  
  envire::octomap::AbstractOcTreePtr tree(octomap::AbstractOcTree::read("/home/dfki.uni-bremen.de/aboeckmann/Downloads/freiburg1_360.ot"));
  ocTree->setData(tree);
  
  graph.addItemToFrame("B", ocTree);
  
  Eigen::Quaterniond q1(Eigen::AngleAxisd(0.5, Eigen::Vector3d(1,2,3)));
//   Eigen::Quaterniond q2(Eigen::AngleAxisd(1.3, Eigen::Vector3d(-1,4,2)));
//   Eigen::Quaterniond q3(Eigen::AngleAxisd(-0.75, Eigen::Vector3d(14,-2,0)));
    
  Transform ab(base::Position(1, 1, 1), q1);
//   Transform ac(base::Position(0, 2, 3), q2);
//   Transform cd(base::Position(-1, 0, 1), q3); 


  graph.addTransform("A", "B", ab);
//   graph.addTransform("A", "C", ac); 
//   graph.addTransform("C", "D", cd);

  
//   Transform bd = graph.getTransform("B", "D");  
//   graph.addTransform("B", "D", bd);
  
  //add another subgraph 
//   Eigen::Quaterniond q0 = Eigen::Quaterniond::Identity();
  
//   Transform oab(base::Position(1, 1, 1), q0);
//   Transform oac(base::Position(0, 2, 3), q0);
//   Transform oad(base::Position(-1, 3, 1), q0); 

//   graph.addTransform("other_a", "other_b", oab);
//   graph.addTransform("other_a", "other_c", oac);
//   graph.addTransform("other_a", "other_d", oad);
  
    QtThreadedWidget<Vizkit3DWidget> app;
    app.start();
 // QApplication app(argc, argv);
 // QVizkitMainWindow window;
 // window.show();
 // Vizkit3DWidget* widget = window.getVizkitWidget();
    Vizkit3DWidget* widget = dynamic_cast<Vizkit3DWidget*>(app.getWidget());

  // envire::viz::EnvireGraphVisualizer visualizer(graph, widget, "A");

//  Vizkit3DWidget* widget2 = new Vizkit3DWidget();
//  widget2->show();
  envire::viz::EnvireGraphVisualizer visualizer(graph, widget, "A");
//  usleep(900000);
  visualizer.addPluginInfo(ocTree->getTypeIndex(), "OcTreeVisualization");
  
  while(true){}
  
 /* while(true) 
  {
    usleep(200000);
    Transform tf = graph.getTransform("A", "B");
    tf.transform.orientation *= Eigen::Quaterniond(Eigen::AngleAxisd(0.3, base::Position(1, 0, 0)));
    graph.updateTransform("A", "B", tf);
  }
  */
  
 // return app.exec();*/
}

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
  
  //QtThreadedWidget<Vizkit3DWidget> app;
  //app.start();
  QApplication app(argc, argv);
  QVizkitMainWindow window;
 // Vizkit3DWidget* widget = dynamic_cast<Vizkit3DWidget*>(app.getWidget());
  Vizkit3DWidget* widget = window.getVizkitWidget();
  window.show();
  envire::viz::Vizkit3dPluginInformation info(widget);
  envire::viz::EnvireGraphVisualizer visualizer(graph, widget, "A", info);

  
  app.exec();
  
  while(true) 
  {
  //  usleep(200000);
  //  Transform tf = graph.getTransform("A", "B");
  //  tf.transform.orientation *= Eigen::Quaterniond(Eigen::AngleAxisd(0.3, base::Position(1, 0, 0)));
   // graph.updateTransform("A", "B", tf);
  }
  
  
  
}


int main(int argc, char **argv)
{
  return testPcl(argc, argv);
}