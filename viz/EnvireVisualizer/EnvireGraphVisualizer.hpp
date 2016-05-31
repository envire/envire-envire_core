#pragma once
#include <unordered_map>
#include <typeindex>
#include <vizkit3d/Vizkit3DPlugin.hpp>
#include <vizkit3d/Vizkit3DWidget.hpp>
#include <envire_core/graph/GraphTypes.hpp>
#include <envire_core/graph/TreeView.hpp>
#include <envire_core/events/GraphEventDispatcher.hpp>
#include <envire_core/items/ItemBase.hpp>
#include <boost/uuid/uuid.hpp>
#include <memory>
#include "Vizkit3dPluginInformation.hpp"

namespace envire { namespace core 
{
  class Transform;
  class EnvireGraph;
}}

namespace envire { namespace viz
{

/**Draws an envire graph into a Vizkit3DWidget */
class EnvireGraphVisualizer : public QObject, public  envire::core::GraphEventDispatcher
{
  Q_OBJECT
  using vertex_descriptor = envire::core::GraphTraits::vertex_descriptor;
  using edge_descriptor = envire::core::GraphTraits::edge_descriptor;
  using FrameId = envire::core::FrameId;
public:
  
  /**
   * @param rootNode the name of the frame that should be placed in the world origin*/
  EnvireGraphVisualizer(std::shared_ptr<envire::core::EnvireGraph> graph,
                        vizkit3d::Vizkit3DWidget* widget, 
                        const envire::core::FrameId& rootNode,
                        std::shared_ptr<Vizkit3dPluginInformation> pluginInfos);
  ~EnvireGraphVisualizer();
  
  const QSet<QString>& getFrameNames() const;
  
  /**Returns a reference to the TreeView that is currently visualized. */
  const envire::core::TreeView& getTree() const;
  
protected:
  /**Is invoked whenever a transform changes in the graph */
  virtual void edgeModified(const envire::core::EdgeModifiedEvent& e);
  
signals:
  void frameAdded(const QString& frame);
  void frameRemoved(const QString& frame);
  
private:
  void loadPlugins();
  
  /**Is invoked whenever a new edge is added to the current tree*/
  void edgeAddedToTree(vertex_descriptor origin, vertex_descriptor target);
  /**Is invoked whenever an edge is removed from the current tree */
  void edgeRemovedFromTree(const vertex_descriptor origin, const vertex_descriptor target);
  /**Is invoked whenever a new item is added to the graph*/
  virtual void itemAdded(const envire::core::ItemAddedEvent& e);
  /**Is invoked whenever an item is deleted from the graph */
  virtual void itemRemoved(const envire::core::ItemRemovedEvent& e);
  
  /**Display all items that are in @p vertex */
  void loadItems(const vertex_descriptor vertex);
  /**Display @p item */
  void loadItem(const envire::core::ItemBase::Ptr item);
  
  /**Adds @p name to frameNames and emits frameAdded*/
  void addFrameName(const QString& name);
  /**Removes @p name from frameNames and emits frameRemoved */
  void removeFrameName(const QString& name);
  
  
  /**Gets the current transformation between @p origin and @p target from the
   * graph and sets it in the widget*/
  void setTransformation(const FrameId& origin, const FrameId& target);
  void setTransformation(const vertex_descriptor origin, const vertex_descriptor target);
  
  /**Converts a transformation from envire format to vizkit3d format */
  std::pair<QQuaternion, QVector3D> convertTransform(const envire::core::Transform& tf) const;
  
  using ItemVisualMap = std::unordered_map<boost::uuids::uuid, vizkit3d::VizPluginBase*, boost::hash<boost::uuids::uuid>>;

  std::shared_ptr<envire::core::EnvireGraph> graph; /**< the graph that is visualized*/
  envire::core::TreeView tree;
  vizkit3d::Vizkit3DWidget* widget; /**< Is used to display the graph */
  std::shared_ptr<Vizkit3dPluginInformation> pluginInfos; /**< meta-data needed to figure out which plugins to load*/
  ItemVisualMap itemVisuals; /**<Map of all items that are currently visualized and the plugin visualizing them*/
  QSet<QString> frameNames; //contains the names of all frames in the current tree
};

}}