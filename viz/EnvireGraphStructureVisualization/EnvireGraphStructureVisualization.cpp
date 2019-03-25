//
// Copyright (c) 2015, Deutsches Forschungszentrum für Künstliche Intelligenz GmbH.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

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
    QStringList nextNodes;
    osg::ref_ptr<osg::Group> rootGroup = nullptr;
    osg::ref_ptr<osg::Node> nextNode = nullptr;
    FrameId currentRoot;
    
      /**Convert envire transform to osg transform */
    std::pair<osg::Quat, osg::Vec3d> convertTransform(const envire::core::transformType& tf) const;
    
};

QStringList EnvireGraphStructureVisualization::getNodes()
{
  //FIXME not sure why i need to copy the list in here?
  //this is copy paste code from vizkit3dwidget
  QStringList list = p->nextNodes;
  QString qCurrentRoot = QString::fromStdString(p->currentRoot);
  if(!qCurrentRoot.isEmpty() && !list.isEmpty())
  {
      list.removeOne(qCurrentRoot);
      list.prepend(qCurrentRoot);
  }
  return list;
}
 
void EnvireGraphStructureVisualization::setNodes(const QStringList& values)
{
  p->nodes = values;
  //this method is called whenever the user selects a new root node.
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
  
  //this is done in here because this method is called from the gui thread, thus
  //avoiding any threading issues that will otherwise occur
  if(!areSame(p->nextNodes, p->nodes))
  {
    p->nodes = p->nextNodes;
    emit propertyChanged("rootNode");
  }
}

void EnvireGraphStructureVisualization::updateDataIntern(envire::core::EnvireGraph const& graph)
{
  initNodeList(graph); //also updates p->currentRoot
  p->nextNode = drawGraphStructure(graph, p->currentRoot);
}

void EnvireGraphStructureVisualization::initNodeList(envire::core::EnvireGraph const &graph)
{
  p->nextNodes.clear();
  EnvireGraph::vertex_iterator begin, end;
  boost::tie(begin, end) = graph.getVertices();
  for(; begin != end; ++begin)
  {
    p->nextNodes.append(QString(graph.getFrameId(*begin).c_str()));
  }
  
  if(p->nextNodes.size() == 0)
  {//special case: empty graph
    p->currentRoot = "";
  }
  else if(p->nextNodes.contains(QString::fromStdString(p->currentRoot)))
  {
    //if the root is in the list, move it to the front of the list (otherwise it will not be shown as root in the ui)
    p->nextNodes.removeAll(QString::fromStdString(p->currentRoot));
    p->nextNodes.prepend(QString::fromStdString(p->currentRoot));
  }
  else
  {
    //otherwise the root has never been choosen or the root has been removed
    //in both cases take the first node as root
    p->currentRoot = p->nextNodes.front().toStdString();
  }
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
  
  if(graph.num_vertices() > 0)
  {
    tree.edgeAdded.connect([&] (GraphTraits::vertex_descriptor origin, GraphTraits::vertex_descriptor target)
    {
      const transformType tf = graph.getTransform(origin, target);
      const FrameId& originName = graph.getFrameId(origin);
      const FrameId& targetName = graph.getFrameId(target);
      osg::Quat orientation;
      osg::Vec3d translation;
      std::tie(orientation, translation) = p->convertTransform(tf);
      
      if(TransformerGraph::getFrame(*transformerGraph, originName) == nullptr)
        TransformerGraph::addFrame(*transformerGraph, originName);
      
      if(TransformerGraph::getFrame(*transformerGraph, targetName) == nullptr)
        TransformerGraph::addFrame(*transformerGraph, targetName);
      
      TransformerGraph::setTransformation(*transformerGraph, originName, targetName, orientation, translation);
    });
    
    tree.crossEdgeAdded.connect([&] (const TreeView::CrossEdge& edge)
    {
      const FrameId& source = graph.getFrameId(edge.origin);
      const FrameId& target = graph.getFrameId(edge.target);
      
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
  }
  return transformerGraph;
}


std::pair<osg::Quat, osg::Vec3d> EnvireGraphStructureVisualization::Data::convertTransform(const envire::core::transformType& tf) const
{
  //normalizing is important, otherwise osg will break when switching the root.
  const base::Quaterniond& rot = tf.transform.orientation.normalized();
  const base::Position& pos = tf.transform.translation;
  const osg::Quat orientation(rot.x(), rot.y(), rot.z(), rot.w());
  const osg::Vec3d translation(pos.x(), pos.y(), pos.z()); 
  return std::make_pair(orientation, translation);
}

bool EnvireGraphStructureVisualization::areSame(const QStringList& a, const QStringList& b) const
{
  if(a.length() != b.length())
    return false;
  for(int i = 0; i < a.length(); ++i)
  {
    if(a[i] != b[i])
      return false;
  }
  return true;
}


//Macro that makes this plugin loadable in ruby, this is optional.
VizkitQtPlugin(EnvireGraphStructureVisualization)

