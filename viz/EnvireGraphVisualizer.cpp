#include "EnvireGraphVisualizer.hpp"
#include <envire_core/items/Transform.hpp>
#include <envire_core/graph/EnvireGraph.hpp>
#include <envire_core/events/EdgeEvents.hpp>
#include "Helpers.hpp"
#include <string>
#include <glog/logging.h>

using namespace envire::core;
using namespace vizkit3d;



namespace envire { namespace viz 
{
using TypeToUpdateMapping = Vizkit3dPluginInformation::TypeToUpdateMapping;  

  
EnvireGraphVisualizer::EnvireGraphVisualizer(std::shared_ptr<envire::core::EnvireGraph> graph,
                                             Vizkit3DWidget* widget, 
                                             const FrameId& rootNode,
                                             std::shared_ptr<Vizkit3dPluginInformation> pluginInfos) :
  GraphEventDispatcher(graph.get()), graph(graph), widget(widget), pluginInfos(pluginInfos)
{ 
  auto edgeAdded = std::bind(&EnvireGraphVisualizer::edgeAddedToTree, this,
                             std::placeholders::_1, std::placeholders::_2);
  tree.edgeAdded.connect(edgeAdded);
  
  auto edgeRemoved = std::bind(&EnvireGraphVisualizer::edgeRemovedFromTree, this,
                               std::placeholders::_1, std::placeholders::_2);
  tree.edgeRemoved.connect(edgeRemoved);
  
  
  
  //will cause edgeAdded events which will be handled by EnvireGraphVisualizer::edgeAddedToTree
  graph->getTree(rootNode, true, &tree);
  widget->setRootFrame(QString::fromStdString(rootNode));
}

void EnvireGraphVisualizer::edgeAddedToTree(vertex_descriptor origin, vertex_descriptor target)
{
  setTransformation(origin, target);
  //Since we are drawing a tree structure every "origin", has been a "target" in
  //a previous call (except the root node). Thus we only need to load items
  //from the origin if it is the root.
  if(tree.isRoot(origin))
  {
    loadItems(origin);
    addFrameName(QString::fromStdString(graph->getFrameId(origin)));
  }
  loadItems(target);
  addFrameName(QString::fromStdString(graph->getFrameId(target)));
  
  LOG(INFO) << "Added edge " << graph->getFrameId(origin) << " -- " << graph->getFrameId(target);
}

void EnvireGraphVisualizer::edgeRemovedFromTree(const vertex_descriptor origin, const vertex_descriptor target)
{
  const QString targetId = QString::fromStdString(graph->getFrameId(target));
  Qt::ConnectionType conType = Helpers::determineConnectionType(widget);
  QMetaObject::invokeMethod(widget, "removeFrame", conType, Q_ARG(QString, targetId));
  
  removeFrameName(QString::fromStdString(graph->getFrameId(target)));
  
  LOG(INFO) << "Removed edge " << graph->getFrameId(origin) << " -- " << graph->getFrameId(target);
}

void EnvireGraphVisualizer::itemAdded(const envire::core::ItemAddedEvent& e)
{
  const GraphTraits::vertex_descriptor vd = graph->getVertex(e.frame);
  //if the vertex that the item was added to is part of the current tree
  if(tree.vertexExists(vd))
  {
    loadItem(e.item);
  }
}

void EnvireGraphVisualizer::itemRemoved(const envire::core::ItemRemovedEvent& e)
{
  const GraphTraits::vertex_descriptor vd = graph->getVertex(e.frame);
  if(tree.vertexExists(vd))
  {
    if(itemVisuals.find(e.item->getID()) == itemVisuals.end())
    {
      LOG(ERROR) << "No item visual for id: " << e.item->getIDString();
      return;
    }
    VizPluginBase* itemViz = itemVisuals.at(e.item->getID());//may throw
    itemVisuals.erase(e.item->getID());
    ASSERT_NOT_NULL(itemViz);
    Qt::ConnectionType conType = Helpers::determineConnectionType(widget);
    QMetaObject::invokeMethod(widget, "removePlugin", conType, Q_ARG(QObject*, itemViz));
    LOG(INFO) << "Removed item " << e.item->getIDString();
  }
}

std::pair<QQuaternion, QVector3D> EnvireGraphVisualizer::convertTransform(const Transform& tf) const
{
  //normalizing is important, otherwise the transfirmation in osg will break when switching the root.
  const base::Quaterniond& rot = tf.transform.orientation.normalized();
  const base::Position& pos = tf.transform.translation;
  const QQuaternion orientation(rot.w(), rot.x(), rot.y(), rot.z());
  const QVector3D translation(pos.x(), pos.y(), pos.z());
  return std::make_pair(orientation, translation);
}

void EnvireGraphVisualizer::loadItems(const vertex_descriptor vertex)
{
  const FrameId frame = graph->getFrameId(vertex);
  
  graph->visitItems(frame, [this] (const ItemBase::Ptr item)
  {
    loadItem(item);
  });
}

void EnvireGraphVisualizer::loadItem(const envire::core::ItemBase::Ptr item)
{
  if(itemVisuals.find(item->getID()) != itemVisuals.end())
  {
    LOG(ERROR) << "Ignoring item " << item->getIDString() << ". It already has a visual.";
    return;
  }
  
  const bool hasMetadata = ItemMetadataMapping::containsMetadata(*(item->getTypeInfo()));
  if(!hasMetadata)
  {
    LOG(ERROR) << "Ignoring item " << item->getIDString() << ". No metadata available. This usually means that the type was not loaded using envire plugins";
    return;
  }
  const std::string parameterType = ItemMetadataMapping::getMetadata(*(item->getTypeInfo())).embeddedTypename;
  const QString qParameterType = QString::fromStdString(parameterType);
  const TypeToUpdateMapping& typeToUpdateMethod = pluginInfos->getTypeToUpdateMethodMapping();
  
  TypeToUpdateMapping::ConstIterator it = typeToUpdateMethod.find(qParameterType);
  
  if(typeToUpdateMethod.count(qParameterType) > 1)
  {
    LOG(WARNING) << "Multiple update methods registered for type " 
                  << parameterType << ". Using the most recently added one from: "
                  << it->libName.toStdString();
  }
  
  if(it != typeToUpdateMethod.end())
  {
    const Vizkit3dPluginInformation::UpdateMethod& info = it.value();
    QObject* plugin = nullptr;
    const Qt::ConnectionType conType = Helpers::determineConnectionType(widget);
    QMetaObject::invokeMethod(widget, "loadPlugin", conType,
                              Q_RETURN_ARG(QObject*, plugin),
                              Q_ARG(QString, info.libName), Q_ARG(QString, ""));
    ASSERT_NOT_NULL(plugin);//loading should never fail (has been loaded successfully before)
    VizPluginBase* vizPlugin = dynamic_cast<VizPluginBase*>(plugin);
    ASSERT_NOT_NULL(vizPlugin);//everything loaded with vizkit should inherit from VizPluginBase
    
    //call the updateData method
    it->method.invoke(plugin, conType, QGenericArgument(parameterType.c_str(), item->getRawData()));
    
    const QString qFrame = QString::fromStdString(item->getFrame());
    vizPlugin->setVisualizationFrame(qFrame);
    
    itemVisuals[item->getID()] = vizPlugin;
    
    LOG(INFO) << "Added item " << item->getIDString() << " using vizkit plugin " << info.libName.toStdString();
  }
  else
  {
    LOG(WARNING) << "No visualizer found for item type " << parameterType;
  }  
}

void EnvireGraphVisualizer::edgeModified(const EdgeModifiedEvent& e)
{
  //only update if this is an edge of the current tree and not a cross-edge
  const vertex_descriptor origin = graph->getVertex(e.origin);
  const vertex_descriptor target = graph->getVertex(e.target);
  if(tree.edgeExists(origin, target))
  {
    setTransformation(origin, target);
  }
}

void EnvireGraphVisualizer::setTransformation(const vertex_descriptor origin,
                                              const vertex_descriptor target)
{
  const Transform tf = graph->getTransform(origin, target);
  QQuaternion rot;
  QVector3D trans;
  std::tie(rot, trans) = convertTransform(tf);
  const QString qSrc = QString::fromStdString(graph->getFrameId(origin));
  const QString qTarget = QString::fromStdString(graph->getFrameId(target));
  //needs to be invoked because we might have been called from the non-gui thread
  QMetaObject::invokeMethod(widget, "setTransformation", Qt::QueuedConnection,
                            Q_ARG(QString, qSrc), Q_ARG(QString, qTarget),
                            Q_ARG(QVector3D, trans), Q_ARG(QQuaternion, rot));
}

void EnvireGraphVisualizer::setTransformation(const FrameId& origin, const FrameId& target)
{
  setTransformation(graph->getVertex(origin), graph->getVertex(target));
}

const QSet<QString>& EnvireGraphVisualizer::getFrameNames() const
{
  return frameNames;
}


void EnvireGraphVisualizer::addFrameName(const QString& name)
{
  if(frameNames.contains(name))
  {
    LOG(INFO) << "Ignoring frame name " << name.toStdString() << ". Has already been added";
    return;
  }
  frameNames.insert(name);
  emit frameAdded(name);
}

void EnvireGraphVisualizer::removeFrameName(const QString& name)
{
  frameNames.remove(name);
  emit frameRemoved(name);
}

const TreeView& EnvireGraphVisualizer::getTree() const
{
  return tree;
}

EnvireGraphVisualizer::~EnvireGraphVisualizer()
{
  //unsubscribe manually because we might destroy the graph before 
  //destroying base. In that case the automatic unsubscribe from base would crash.
  tree.unsubscribe();
  unsubscribe();
  itemVisuals.clear();
  graph.reset();
}



}}