#include <vizkit3d/Vizkit3DWidget.hpp>
#include <vizkit3d/QtThreadedWidget.hpp>
#include <iostream>
#include "EnvireGraphStructureVisualization.hpp"
#include <envire_core/graph/EnvireGraph.hpp>

using namespace envire::core;
using namespace vizkit3d;

int main()
{
  QtThreadedWidget<Vizkit3DWidget> app;
  app.start();
  Vizkit3DWidget* widget = dynamic_cast<Vizkit3DWidget*>(app.getWidget());
  
  EnvireGraphStructureVisualization viz;
  widget->addPlugin(&viz);
  
  EnvireGraph graph;
  graph.addFrame("A");
  graph.addFrame("B");
  graph.addFrame("C");
  graph.addFrame("D");
  
  
  Eigen::Quaterniond q1(Eigen::AngleAxisd(0.5, Eigen::Vector3d(1,2,3)));
  Eigen::Quaterniond q2(Eigen::AngleAxisd(1.3, Eigen::Vector3d(-1,4,2)));
  Eigen::Quaterniond q3(Eigen::AngleAxisd(-0.75, Eigen::Vector3d(14,-2,0)));
    
  Transform ab(base::Position(1, 1, 1), q1);
  Transform ac(base::Position(0, 2, 3), q2);
  Transform cd(base::Position(-1, 0, 1), q3); 


  graph.addTransform("A", "B", ab);
  graph.addTransform("A", "C", ac);
  graph.addTransform("C", "D", cd);

  Transform bd = graph.getTransform("B", "D");  
  graph.addTransform("B", "D", bd);
  
  //add another subgraph 
  Eigen::Quaterniond q0 = Eigen::Quaterniond::Identity();
  
  Transform oab(base::Position(1, 1, 1), q0);
  Transform oac(base::Position(0, 2, 3), q0);
  Transform oad(base::Position(-1, 3, 1), q0); 

  graph.addTransform("other_a", "other_b", oab);
  graph.addTransform("other_a", "other_c", oac);
  graph.addTransform("other_a", "other_d", oad);

  while(true)
  {
    usleep(1000);
    viz.updateData(graph);
  }

  return 0;
}