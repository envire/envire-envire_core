#include <vizkit3d/Vizkit3DWidget.hpp>
#include <vizkit3d/QtThreadedWidget.hpp>
#include <iostream>
#include "EnvireGraphVisualizer.hpp"
#include <envire_core/graph/EnvireGraph.hpp>
#include <unordered_map>
#include <plugin_manager/PluginLoader.hpp>
#include <envire_octomap/OcTree.hpp> //FIXME eigentlich sollte das ohne gehen?
#include <iostream>

using namespace envire::core;
using namespace vizkit3d;

int main(int argc, char **argv)
{
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
  
  //  QtThreadedWidget<Vizkit3DWidget> app;
  //  app.start();
  //  Vizkit3DWidget* widget = dynamic_cast<Vizkit3DWidget*>(app.getWidget());

  // envire::viz::EnvireGraphVisualizer visualizer(graph, widget, "A");
  QApplication app(argc, argv);
  Vizkit3DWidget* widget2 = new Vizkit3DWidget();
  widget2->show();
  envire::viz::EnvireGraphVisualizer visualizer(graph, widget2, "A");
  visualizer.addPluginInfo(ocTree->getTypeIndex(), "OcTreeVisualization");
  return app.exec();
}