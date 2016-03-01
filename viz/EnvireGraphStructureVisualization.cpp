#include <iostream>
#include "EnvireGraphStructureVisualization.hpp"
#include <osg/PositionAttitudeTransform>
#include <osg/ShapeDrawable>
#include "TransformGraphStructureVisualizer.hpp"

using namespace vizkit3d;
using namespace envire::core;
using namespace std;

using Visualizer = TransformGraphStructureVisualizer<EnvireGraph::FRAME_PROP>;
using VisualizerPtr = unique_ptr<Visualizer>;
using EnvireGraphPtr = shared_ptr<envire::core::EnvireGraph>;

struct EnvireGraphStructureVisualization::Data {
    // Copy of the value given to updateDataIntern.
    //
    // Making a copy is required because of how OSG works
    EnvireGraphPtr graph;
    osg::ref_ptr<osg::PositionAttitudeTransform> root;
    double sphereSize = 0.1;
    QStringList nodes;
    VisualizerPtr visualizer;
    osg::ref_ptr<osg::Group> rootGroup;
};

QStringList EnvireGraphStructureVisualization::getNodes()
{
  return p->nodes;
}
 
void EnvireGraphStructureVisualization::setNodes(const QStringList& values)
{
  //this method is called whenever the user selects a new root node.
  //For some reason the values list only contains the selected node.
  //if(values.size() > 0)
 // {
 //   p->currentRootNode = values.first();
 // }
}



EnvireGraphStructureVisualization::EnvireGraphStructureVisualization()
    : p(new Data)
{
  p->rootGroup = new osg::Group();
  std::cerr << "waiting for debugger" << std::endl;
  int a;
  std::cin >> a;
}

EnvireGraphStructureVisualization::~EnvireGraphStructureVisualization()
{
}

osg::ref_ptr<osg::Node> EnvireGraphStructureVisualization::createMainNode()
{
  return p->rootGroup;
}

void EnvireGraphStructureVisualization::updateMainNode ( osg::Node* node )
{
  if(p->rootGroup->getNumChildren() == 0 &&
     p->visualizer)
  {
    p->rootGroup->addChild(p->visualizer->getRootNode());
  }
}

void EnvireGraphStructureVisualization::updateDataIntern(envire::core::EnvireGraph const& graph)
{
  std::cout << graph.num_vertices() << std::endl;
  //if we haven't created a copy, yet and the graph has content
  if(!p->graph && graph.num_vertices() > 0)
  {
    std::cout << "graph created" << std::endl;
    p->graph.reset(new EnvireGraph(graph));
    initNodeList(graph);
    auto g = std::dynamic_pointer_cast<TransformGraph<EnvireGraph::FRAME_PROP>>(p->graph);
    p->visualizer.reset(new Visualizer(g, p->nodes.front().toStdString()));
  }
}

void EnvireGraphStructureVisualization::initNodeList(envire::core::EnvireGraph const &graph)
{
  p->nodes.clear();
  EnvireGraph::vertex_iterator begin, end;
  boost::tie(begin, end) = graph.getVertices();
  for(; begin != end; ++begin)
  {
    p->nodes.append(QString(graph.getFrameId(*begin).c_str()));
  }
  emit propertyChanged("rootNode");
}


//Macro that makes this plugin loadable in ruby, this is optional.
VizkitQtPlugin(EnvireGraphStructureVisualization)

