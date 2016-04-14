#pragma once
#include "ui_mainwindow.h"
#include "TransformModel.hpp"
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
  MainWindow(envire::core::EnvireGraph& graph, const std::string& rootNode);
  
  /**called whenever some edge in the graph changes */
  virtual void edgeModified(const envire::core::EdgeModifiedEvent& e);
  
private:
  /**selects the frame named @p name in the 2d listview and 3d window */
  void selectFrame(const QString& name);
  
public slots:
  void addFrame();
  /**Remove the currently selected frame (if any) */
  void removeFrame();
  
  void frameNameAdded(const QString& name);
  void frameNameRemoved(const QString& name);
    
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
  
private:
  
  Ui::MainWindow window;
  envire::core::EnvireGraph& graph;
  std::shared_ptr<EnvireGraphVisualizer> visualzier;//is ptr for lazy instanziation
  std::shared_ptr<Vizkit3dPluginInformation> pluginInfos;//is ptr for lazy instanziation
  QString selectedFrame;//currently selected frame, empty if none
  TransformModel currentTransform;//model of the currently selected transform
  bool ignoreEdgeModifiedEvent;
};
  
  
  
}}