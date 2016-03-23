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

  
EnvireGraphVisualizer::EnvireGraphVisualizer(EnvireGraph& graph,
                                             Vizkit3DWidget* widget, 
                                             const FrameId& rootNode,
                                             const Vizkit3dPluginInformation& pluginInfos) :
  GraphEventDispatcher(&graph), graph(graph), widget(widget), pluginInfos(pluginInfos)
{ 
  auto edgeLambda = std::bind(&EnvireGraphVisualizer::edgeAddedToTree, this,
                              std::placeholders::_1, std::placeholders::_2);

  tree.edgeAdded.connect(edgeLambda);
  
  //will cause edgeAdded events which will be handled by EnvireGraphVisualizer::edgeAddedToTree
  graph.getTree(rootNode, true, &tree);
  widget->setRootFrame(QString::fromStdString(rootNode));
}

void EnvireGraphVisualizer::edgeAddedToTree(vertex_descriptor origin, vertex_descriptor target)
{
    setTransformation(origin, target);
    loadItems(target);
    //FIXME this will not load items in the root node
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
  const FrameId frame = graph.getFrameId(vertex);
  
  graph.visitItems(frame, [this] (const ItemBase::Ptr item)
  {
    if(itemVisuals.find(item->getID()) != itemVisuals.end())
    {
      LOG(ERROR) << "Skipping item " << item->getIDString() << ". It already has a visual.";
    }
    
    const std::string parameterType = ItemMetadataMapping::getMetadata(*(item->getTypeInfo())).embeddedTypename;
    const QString qParameterType = QString::fromStdString(parameterType);
    const TypeToUpdateMapping& typeToUpdateMethod = pluginInfos.getTypeToUpdateMethodMapping();
    
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
    }
    else
    {
      LOG(WARNING) << "No visualizer found for item type " << parameterType;
    }
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
  //needs to be invoked because we might have been called from the non-gui thread
  QMetaObject::invokeMethod(widget, "setTransformation", Qt::QueuedConnection,
                            Q_ARG(QString, qSrc), Q_ARG(QString, qTarget),
                            Q_ARG(QVector3D, trans), Q_ARG(QQuaternion, rot));
  //widget->setTransformation(qSrc, qTarget, trans, rot);  
}

void EnvireGraphVisualizer::setTransformation(const FrameId& origin, const FrameId& target)
{
  setTransformation(graph.getVertex(origin), graph.getVertex(target));
}



}}