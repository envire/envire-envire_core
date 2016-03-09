#include <iostream>
#include "EnvireGraphStructureVisualization.hpp"
#include <osg/PositionAttitudeTransform>
#include <osg/ShapeDrawable>
#include <vizkit3d/TransformerGraph.hpp>
#include <vizkit3d/NodeLink.hpp>

using namespace vizkit3d;
using namespace envire::core;
using namespace std;


struct EnvireGraphStructureVisualization::Data {
    envire::core::EnvireGraph* pGraph = nullptr; /** Pointer to the last sample is retained to check whether the sample changes */
    osg::ref_ptr<osg::PositionAttitudeTransform> root;
    QStringList nodes; /**List of possible root nodes to be displayed in the gui */
    osg::ref_ptr<osg::Group> rootGroup = nullptr;
    osg::ref_ptr<osg::Node> nextNode = nullptr;
    FrameId currentRoot;
};

QStringList EnvireGraphStructureVisualization::getNodes()
{
  return p->nodes;
}
 
void EnvireGraphStructureVisualization::setNodes(const QStringList& values)
{
  //this method is called whenever the user selects a new root node.
  //For some reason the values list only contains the selected node.
  if(values.size() > 0)
  {
    p->currentRoot = values.first().toStdString();
  }
}

EnvireGraphStructureVisualization::EnvireGraphStructureVisualization()
    : p(new Data)
{
  p->rootGroup = new osg::Group();
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
  if(p->nextNode)
  {
    p->rootGroup->removeChildren(0, p->rootGroup->getNumChildren());
    p->rootGroup->addChild(p->nextNode);
  }
}

void EnvireGraphStructureVisualization::updateDataIntern(envire::core::EnvireGraph const& graph)
{
  initNodeList(graph);
  p->nextNode = drawGraphStructure(graph, p->currentRoot);
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
  if(!p->currentRoot.empty())
  {
    p->nodes.removeAll(QString::fromStdString(p->currentRoot));
    p->nodes.prepend(QString::fromStdString(p->currentRoot));
  }
  else
  {
    p->currentRoot = p->nodes.front().toStdString();
  }
  emit propertyChanged("rootNode");
}

void EnvireGraphStructureVisualization::updateData(envire::core::EnvireGraph const &sample)
{
  vizkit3d::Vizkit3DPlugin<envire::core::EnvireGraph>::updateData(sample);
  
}

osg::ref_ptr<osg::Node> EnvireGraphStructureVisualization::drawGraphStructure(const envire::core::EnvireGraph& graph,
                                                                               const FrameId& root)
{
  TreeView tree;
  osg::ref_ptr<osg::Node> transformerGraph = TransformerGraph::create("transform_graph_world")->asGroup();

  
  tree.edgeAdded.connect([&] (GraphTraits::vertex_descriptor origin, GraphTraits::vertex_descriptor target)
  {
    const Transform tf = graph.getTransform(origin, target);
    const FrameId& originName = graph.getFrameId(origin);
    const FrameId& targetName = graph.getFrameId(target);
    osg::Quat orientation;
    osg::Vec3d translation;
    std::tie(orientation, translation) = convertTransform(tf);
    
    if(TransformerGraph::getFrame(*transformerGraph, originName) == nullptr)
      TransformerGraph::addFrame(*transformerGraph, originName);
    
    if(TransformerGraph::getFrame(*transformerGraph, targetName) == nullptr)
      TransformerGraph::addFrame(*transformerGraph, targetName);
    
    TransformerGraph::setTransformation(*transformerGraph, originName, targetName, orientation, translation);
  });
  
  tree.crossEdgeAdded.connect([&] (GraphTraits::edge_descriptor edge)
  {
    const FrameId& source = graph.getFrameId(graph.source(edge));
    const FrameId& target = graph.getFrameId(graph.target(edge));
    
    osg::Node* srcNode = TransformerGraph::getFrame(*transformerGraph, source);
    osg::Node* tarNode = TransformerGraph::getFrame(*transformerGraph, target);
    //the frames should always exist, otherwise this edge wouldn't be a cross-edge
    assert(srcNode);
    assert(tarNode);
    
    //The NodeLink will update its position automatically to reflect changes in src and target
    osg::Node *link = vizkit::NodeLink::create(srcNode, tarNode, osg::Vec4(255,255,0,255));
    link->setName("crossEdge");
    osg::Group* group = TransformerGraph::getFrameGroup(*transformerGraph, source);    
    group->addChild(link);
  });
  
  
  graph.getTree(root, &tree); //will cause edgeAdded events and trigger the above lambdas
  TransformerGraph::makeRoot(*transformerGraph, root);
  return transformerGraph;
}


std::pair<osg::Quat, osg::Vec3d> EnvireGraphStructureVisualization::convertTransform(const Transform& tf) const
{
  //normalizing is important, otherwise osg will break when switching the root.
  const base::Quaterniond& rot = tf.transform.orientation.normalized();
  const base::Position& pos = tf.transform.translation;
  const osg::Quat orientation(rot.x(), rot.y(), rot.z(), rot.w());
  const osg::Vec3d translation(pos.x(), pos.y(), pos.z()); 
  return std::make_pair(orientation, translation);
}


//Macro that makes this plugin loadable in ruby, this is optional.
VizkitQtPlugin(EnvireGraphStructureVisualization)

