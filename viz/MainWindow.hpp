#pragma once
#include "ui_mainwindow.h"
#include "TransformModel.hpp"
#include "ItemTableModel.hpp"
#include <QMainWindow>
#include <memory>
#include <envire_core/events/GraphEventDispatcher.hpp>
#include <envire_core/graph/GraphTypes.hpp>


namespace envire { namespace core 
{
  class EnvireGraph;
  class EdgeModifiedEvent;
}}

namespace envire { namespace viz 
{
  
class EnvireGraphVisualizer;
class Vizkit3dPluginInformation;

class MainWindow : public QMainWindow, public envire::core::GraphEventDispatcher
{
  Q_OBJECT
public:
  
  /**Create an unitialized envire visualizer that displays nothing.*/
  MainWindow();
  
  /**called whenever some edge in the graph changes */
  virtual void edgeModified(const envire::core::EdgeModifiedEvent& e);
  
  /**Display the specified graph */
  void displayGraph(std::shared_ptr<envire::core::EnvireGraph> graph, const QString& rootNode);
  /**Load a graph and ask the user for the root node*/
  void displayGraph(const QString& filePath);
  
private:
  /**selects the frame named @p name in the 2d listview and 3d window */
  void selectFrame(const QString& name);
  
public slots:
  void addFrame();
  /**Remove the currently selected frame (if any) */
  void removeFrame();
  
  void frameNameAdded(const QString& name);
  void frameNameRemoved(const QString& name);
  
  /**Display a file picker dialog and load a graph from the selected file */
  void loadGraph();
  
  /**Display a file picker dialog and save the graph to the selected file */
  void storeGraph();
  
  /**Is called whenever the user wants to move a frame in the ui using the dragger */
  void frameTranslated(const QString& frame, const QVector3D& translation);
    
private slots:
  void framePicked(const QString&);
  void listWidgetItemChanged(QListWidgetItem * current, QListWidgetItem * previous);
  /**Invoked if the user changes a transformation */
  void transformChanged(const base::TransformWithCovariance& newValue);
  
  /**invoked by edgeModified() for thread switch */
  void edgeModifiedInternal(const QString& originFrame, const QString& tagetFrame);
  
  /**Display the transform from @p parent to @p selected in the transform display */
  void updateDisplayedTransform(const envire::core::GraphTraits::vertex_descriptor parent,
                                const envire::core::GraphTraits::vertex_descriptor selected);
  
  /**Display the items of @p frame in the itemListWidget */
  void displayItems(const QString& frame);
  
private:
  
  Ui::MainWindow window;
  std::shared_ptr<envire::core::EnvireGraph> graph;
  std::shared_ptr<EnvireGraphVisualizer> visualzier;//is ptr for lazy instanziation
  std::shared_ptr<Vizkit3dPluginInformation> pluginInfos;//is ptr for lazy instanziation
  QString selectedFrame;//currently selected frame, empty if none
  QString rootFrame;//the root frame of the displayed tree
  TransformModel currentTransform;//model of the currently selected transform
  ItemTableModel currentItems; //model of the  items of the current frame
  bool ignoreEdgeModifiedEvent;
  bool firstTimeDisplayingItems; //true if no items have been displayed, yet
};
  
  
  
}}