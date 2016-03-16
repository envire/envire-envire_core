#pragma once
#include <unordered_map>
#include <typeindex>
#include <vizkit3d/Vizkit3DPlugin.hpp>
#include <vizkit3d/Vizkit3DWidget.hpp>
#include <envire_core/graph/GraphTypes.hpp>
#include <envire_core/graph/TreeView.hpp>
#include <envire_core/events/GraphEventDispatcher.hpp>
#include <envire_core/items/ItemBase.hpp>

namespace envire { namespace core 
{
  class Transform;
  class EnvireGraph;
}}

namespace envire { namespace viz
{

class EnvireGraphVisualizer : envire::core::GraphEventDispatcher
{
  using vertex_descriptor = envire::core::GraphTraits::vertex_descriptor;
  using edge_descriptor = envire::core::GraphTraits::edge_descriptor;
  using FrameId = envire::core::FrameId;
public:
  
  /**
   * @param rootNode the name of the frame that should be placed in the world origin*/
  EnvireGraphVisualizer(envire::core::EnvireGraph& graph,
                        vizkit3d::Vizkit3DWidget* widget, 
                        const envire::core::FrameId& rootNode);
  void addPluginInfo(std::type_index type, const QString& pluginName);
  
protected:
  /**Is invoked whenever a transform changes in the graph */
  virtual void edgeModified(const envire::core::EdgeModifiedEvent& e);
  
private:
  void loadPlugins();
  
  /**event handler*/
  void edgeAddedToTree(vertex_descriptor origin, vertex_descriptor target);
  
  /**Display all items that are in @p vertex */
  void loadItems(const vertex_descriptor vertex);
   
  /**Gets the current transformation between @p origin and @p target from the
   * graph and sets it in the widget*/
  void setTransformation(const FrameId& origin, const FrameId& target);
  void setTransformation(const vertex_descriptor origin, const vertex_descriptor target);
  
  
  std::pair<QQuaternion, QVector3D> convertTransform(const envire::core::Transform& tf) const;
  
  
  /**Mapping from item type to the name of the plugin that can display this type*/
  std::unordered_map<std::type_index, QString> pluginsNames;
  envire::core::EnvireGraph& graph; /**< the graph that is visualized*/
  envire::core::TreeView tree;
  vizkit3d::Vizkit3DWidget* widget;
};

}}