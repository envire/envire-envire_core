#pragma once
#include <envire_core/graph/TransformGraph.hpp>
#include <envire_core/events/GraphEventDispatcher.hpp>
#include <envire_core/items/Transform.hpp>
#include <envire_core/graph/TreeView.hpp>
#include <vizkit3d/TransformerGraph.hpp>
#include <memory>



/**Visualizes the structure of the given TransformGraph.
 * Automatically updates the structure if the graph changes.
 */
template <class FRAME_PROP>
class TransformGraphStructureVisualizer
{
  using GraphPtr = std::shared_ptr<envire::core::TransformGraph<FRAME_PROP>>;
  using FrameId = envire::core::FrameId;
  using TreeView = envire::core::TreeView;
  using vertex_descriptor = envire::core::GraphTraits::vertex_descriptor;
  using Transform = envire::core::Transform;
public:
  /**Creates an instance of the visualizer that is not connected to any graph */
  TransformGraphStructureVisualizer();
  
  /**Creates an instance that is connected to @p graph and displays the structure
   * starting from @p rootNode*/
  TransformGraphStructureVisualizer(GraphPtr graph, const FrameId& rootNode);
  
  /**Connects the visualizer to the specified graph.
   * @note this method may only be called once*/
  void init(GraphPtr graph, const FrameId& rootNode);
  
  /**Only valid after init has been called */
  osg::ref_ptr<osg::Group> getRootNode();
  
private:
  
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
  assert(!initialized); //this method may only be called once
  this->graph = graph;
  graph->getTree(root, true, &treeView);
  initialized = true;
  
  transformerGraph = vizkit3d::TransformerGraph::create(root + "_world")->asGroup();
  rootNode = root;
  vizkit3d::TransformerGraph::addFrame(*transformerGraph, rootNode);
  vizkit3d::TransformerGraph::makeRoot(*transformerGraph, rootNode);
  
  
  const vertex_descriptor rootVertex = graph->getVertex(rootNode);
  //walk the tree and create a scene graph out of it
  treeView.visitDfs(rootVertex, [&] (const vertex_descriptor vd, const vertex_descriptor parent)
  {
    //skip the root node, the corresponding osg node has been created manually beforehand
    if(vd == rootVertex) return; 
                    
    const FrameId& id = graph->getFrameId(vd);
    const FrameId& parentId = graph->getFrameId(parent);
    
    vizkit3d::TransformerGraph::addFrame(*transformerGraph, id);
    
    const Transform tf = graph->getTransform(parent, vd);
    const base::Quaterniond& rot = tf.transform.orientation;
    const base::Position& pos = tf.transform.translation;
    const osg::Quat orientation(rot.x(), rot.y(), rot.z(), rot.w());
    const osg::Vec3d translation(pos.x(), pos.y(), pos.z());  
    
    vizkit3d::TransformerGraph::setTransformation(*transformerGraph, parentId, id, orientation, translation);
  });
}

template <class F>
osg::ref_ptr<osg::Group> TransformGraphStructureVisualizer<F>::getRootNode()
{
  assert(initialized);
  
  return transformerGraph;
}


