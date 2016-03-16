#include "EnvireGraphVisualizer.hpp"
#include <envire_core/items/Transform.hpp>
#include <envire_core/graph/EnvireGraph.hpp>
#include <envire_core/events/EdgeEvents.hpp>
#include <string>

using namespace envire::core;
using namespace vizkit3d;


#define ASSERT_NOT_NULL(item) \
  do \
  { \
    if(item == nullptr) \
    { \
      throw std::runtime_error(std::string(#item) + " is null"); \
    } \
  }while(false)

namespace envire { namespace viz 
{
  

  
EnvireGraphVisualizer::EnvireGraphVisualizer(EnvireGraph& graph,
                                             Vizkit3DWidget* widget, 
                                             const FrameId& rootNode) :
 GraphEventDispatcher(&graph), graph(graph), widget(widget)
{ 
  auto edgeLambda = std::bind(&EnvireGraphVisualizer::edgeAddedToTree, this,
                              std::placeholders::_1, std::placeholders::_2);

  tree.edgeAdded.connect(edgeLambda);
  graph.getTree(rootNode, true, &tree);
  widget->setRootFrame(QString::fromStdString(rootNode));
}

void EnvireGraphVisualizer::edgeAddedToTree(vertex_descriptor origin, vertex_descriptor target)
{
    setTransformation(origin, target);
    loadItems(target);
}

std::pair<QQuaternion, QVector3D> EnvireGraphVisualizer::convertTransform(const Transform& tf) const
{
  //normalizing is important, otherwise osg will break when switching the root.
  const base::Quaterniond& rot = tf.transform.orientation.normalized();
  const base::Position& pos = tf.transform.translation;
  const QQuaternion orientation(rot.w(), rot.x(), rot.y(), rot.z());
  const QVector3D translation(pos.x(), pos.y(), pos.z());
  return std::make_pair(orientation, translation);
}

void EnvireGraphVisualizer::loadItems(const vertex_descriptor vertex)
{
  //items are loaded using the qt mechanism that was originally developed for ruby
  //interaction. Thus most of the methods need to be invoked.
  
  //FIXME ausführlich dokumentieren
  for (const auto& it : pluginsNames)
  {
    const std::type_index type = it.first;
    const QString pluginName = it.second;
    
    std::cout << "looking for " << type.name() << std::endl;
    
    if(!graph.containsItems(vertex, type))
      continue;
    
    const Frame::ItemList& items = graph.getItems(vertex, type);
    for(ItemBase::Ptr item : items)
    { 
      //FIXME hier später nicht immer neue plugins erzeugen
      QObject* plugin = widget->loadPlugin("", pluginName);
      ASSERT_NOT_NULL(plugin);
      const QString qFrame = QString::fromStdString(item->getFrame());
      VizPluginBase* vizPlugin = dynamic_cast<VizPluginBase*>(plugin);
      ASSERT_NOT_NULL(vizPlugin);
      vizPlugin->updateDataRaw(item->getRawData(), item->getEmbeddedTypeInfo());
      vizPlugin->setVisualizationFrame(qFrame);
    }
  }
}

void EnvireGraphVisualizer::addPluginInfo(std::type_index type, const QString& pluginName)
{
  if(pluginsNames.find(type) != pluginsNames.end())
  {
    //FIXME better error message
    throw std::runtime_error("Plugin for type already registered");
  }
  pluginsNames[type] = pluginName;
  
  //whenever a plugin is added, we need to revisit the whole tree and check if 
  //FIXME hier sollte das nur für den typ gemacht werden
  tree.visitDfs(tree.root, [&] (vertex_descriptor node, vertex_descriptor parent)
  {
    loadItems(node);
  });
}

void EnvireGraphVisualizer::edgeModified(const EdgeModifiedEvent& e)
{
  //only update if this is an edge of the current tree and not a cross-edge
  const vertex_descriptor origin = graph.getVertex(e.origin);
  const vertex_descriptor target = graph.getVertex(e.target);
  if(tree.edgeExists(origin, target))
  {
    setTransformation(origin, target);
  }
}

void EnvireGraphVisualizer::setTransformation(const vertex_descriptor origin,
                                              const vertex_descriptor target)
{
  const Transform tf = graph.getTransform(origin, target);
  QQuaternion rot;
  QVector3D trans;
  std::tie(rot, trans) = convertTransform(tf);
  const QString qSrc = QString::fromStdString(graph.getFrameId(origin));
  const QString qTarget = QString::fromStdString(graph.getFrameId(target));
  widget->setTransformation(qSrc, qTarget, trans, rot);  
}

void EnvireGraphVisualizer::setTransformation(const FrameId& origin, const FrameId& target)
{
  setTransformation(graph.getVertex(origin), graph.getVertex(target));
}



}}