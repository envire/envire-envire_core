#pragma once
#include <envire_core/graph/TransformGraph.hpp>
#include <envire_core/events/GraphEventDispatcher.hpp>
#include <envire_core/events/EdgeModifiedEvent.hpp>
#include <envire_core/items/Transform.hpp>
#include <envire_core/graph/TreeView.hpp>
#include <vizkit3d/TransformerGraph.hpp>
#include <vizkit3d/NodeLink.hpp>
#include <utility>
#include <memory>

/**Visualizes the structure of the given TransformGraph.
 * Automatically updates the structure if the graph changes.
 */
template <class FRAME_PROP>
class TransformGraphStructureVisualizer : public envire::core::GraphEventDispatcher
{
  using vertex_descriptor = envire::core::GraphTraits::vertex_descriptor;
  using Transform = envire::core::Transform;
  using edge_descriptor = envire::core::GraphTraits::edge_descriptor;
  using TransformerGraph = vizkit3d::TransformerGraph;
  using EdgeModifiedEvent = envire::core::EdgeModifiedEvent;
  using PosAtt = osg::PositionAttitudeTransform;
public:
  using GraphPtr = std::shared_ptr<envire::core::TransformGraph<FRAME_PROP>>;  
  using FrameId = envire::core::FrameId;
  using TreeView = envire::core::TreeView;
  
  /**Creates an instance of the visualizer that is not connected to any graph.
   * init() needs to be called in order to use the graph*/
  TransformGraphStructureVisualizer();
  
  /**Creates an instance that is connected to @p graph and displays the structure
   * starting from @p rootNode*/
  TransformGraphStructureVisualizer(GraphPtr graph, const FrameId& rootNode);
  
  /**Connects the visualizer to the specified graph.
   * @note this method may only be called once*/
  void init(GraphPtr graph, const FrameId& rootNode);
  
  /**Only valid after init has been called */
  osg::ref_ptr<osg::Group> getRootNode();
  
  /**Changes the root node of the visualization */
  void changeRoot(const FrameId& newRoot);
  
  /**Event handler for the EDGE_MODIFIED event of the TransformGraph */
  virtual void edgeModified(const EdgeModifiedEvent& e);
  
private:
  
  /**Adds the treeView starting to the transformerGraph */
  void addTreeview(const TreeView& view, const FrameId& rootNode);
  
  void setTransformation(const FrameId& source, const FrameId& target, 
                         const Transform& tf);
  
  /**converts a transform to something that osg does unterstand */
  std::pair<osg::Quat, osg::Vec3d> convertTransform(const Transform& tf) const;
  
  GraphPtr graph;
  TreeView treeView; /**< The view of the graph that is currently visualized*/
  bool initialized = false; /** < */
  osg::ref_ptr<osg::Group> transformerGraph;
  FrameId rootNode; 

};








template <class F>
TransformGraphStructureVisualizer<F>::TransformGraphStructureVisualizer()
{}

template <class F>
TransformGraphStructureVisualizer<F>::TransformGraphStructureVisualizer(GraphPtr graph,
                                                                        const FrameId& rootNode)
{
  init(graph, rootNode);
}

template <class F>
void TransformGraphStructureVisualizer<F>::init(GraphPtr graph, const FrameId& root)
{
  assert(!initialized);
  this->graph = graph;
  transformerGraph = TransformerGraph::create("transform_graph_world")->asGroup();
  rootNode = root;
  graph->getTree(root, true, &treeView);
  addTreeview(treeView, rootNode);
  subscribe(&*graph);
  initialized = true;
}

template <class F>
void TransformGraphStructureVisualizer<F>::addTreeview(const TreeView& view,
                                                       const FrameId& rootNode
)
{
  TransformerGraph::addFrame(*transformerGraph, rootNode);
  TransformerGraph::makeRoot(*transformerGraph, rootNode);
  
  const vertex_descriptor rootVertex = graph->getVertex(rootNode);
  
  //walk the tree and create a scene graph out of it
  treeView.visitDfs(rootVertex, [&] (const vertex_descriptor vd, const vertex_descriptor parent)
  {
    //skip the root node, the corresponding osg node has been created manually beforehand
    if(vd == rootVertex) return; 
                    
    const FrameId& id = graph->getFrameId(vd);
    const FrameId& parentId = graph->getFrameId(parent);
    
    TransformerGraph::addFrame(*transformerGraph, id);
    const Transform tf = graph->getTransform(parent, vd);
    setTransformation(parentId, id, tf);
  });
  
  //when the minimal tree has been added, the cross-edges can be added.
  //we need to add them manually because the TransformerGraph breaks when
  //adding loops
  for(const edge_descriptor edge : treeView.crossEdges)
  {
    const FrameId& source = graph->getFrameId(graph->source(edge));
    const FrameId& target = graph->getFrameId(graph->target(edge));
    
    PosAtt* srcNode = dynamic_cast<PosAtt*>(TransformerGraph::getFrame(*transformerGraph, source));
    PosAtt* tarNode = dynamic_cast<PosAtt*>(TransformerGraph::getFrame(*transformerGraph, target));
    assert(srcNode);
    assert(tarNode);
    
    
    //The NodeLink will update its position automatically to reflect changes in src and target
    osg::Node *link = vizkit::NodeLink::create(srcNode, tarNode, osg::Vec4(255,255,0,255));
    link->setName("crossEdge");
    osg::Group* group = TransformerGraph::getFrameGroup(*transformerGraph, source);    
    group->addChild(link);
  }
}
  

template <class F>
void TransformGraphStructureVisualizer<F>::setTransformation(const FrameId& source,
                                                             const FrameId& target,
                                                             const Transform& tf)
{
  //normalizing is important, otherwise osg will break when switching the root.
  osg::Quat orientation;
  osg::Vec3d translation;
  std::tie(orientation, translation) = convertTransform(tf);
  
  std::cout << source << " -> " << target << translation.x() << " " << translation.y() << " " 
            << translation.z() << std::endl;
  
  TransformerGraph::setTransformation(*transformerGraph, source, target, orientation, translation);
}

template <class F>
void TransformGraphStructureVisualizer<F>::changeRoot(const FrameId& newRoot)
{
  //check if the new root is part of the current TreeView
  //if it is, the TransformerGraph already knows about it and we can just
  //change the root. Otherwise the whole tree needs to be re-generated.
  //This only happens when the graph consists of multiple disconnected graphs
  const vertex_descriptor newRootDesc = graph->getVertex(newRoot);
  if(treeView.tree.find(newRootDesc) != treeView.tree.end())
  {
    TransformerGraph::makeRoot(*transformerGraph, newRoot);
  }
  else
  {
    treeView.unsubscribe();
    treeView.clear();
    vizkit3d::TransformerGraph::removeFrame(*transformerGraph, rootNode);//clear the visualization
    graph->getTree(newRoot, true, &treeView);
    addTreeview(treeView, newRoot);
    rootNode = newRoot;
  }
}

template <class F>
osg::ref_ptr<osg::Group> TransformGraphStructureVisualizer<F>::getRootNode()
{
  assert(initialized);
  
  return transformerGraph;
}

template <class F>
void TransformGraphStructureVisualizer<F>::edgeModified(const EdgeModifiedEvent& e)
{
  assert(initialized);
  Transform tf = graph->getTransform(e.origin, e.target);
  osg::Quat orientation;
  osg::Vec3d translation;
  std::tie(orientation, translation) = convertTransform(tf);
  TransformerGraph::setTransformation(*transformerGraph, e.origin, e.target, orientation, translation);
}

template <class F>
std::pair<osg::Quat, osg::Vec3d> TransformGraphStructureVisualizer<F>::convertTransform(const Transform& tf) const
{
  const base::Quaterniond& rot = tf.transform.orientation.normalized();
  const base::Position& pos = tf.transform.translation;
  const osg::Quat orientation(rot.x(), rot.y(), rot.z(), rot.w());
  const osg::Vec3d translation(pos.x(), pos.y(), pos.z()); 
  return std::make_pair(orientation, translation);
}


