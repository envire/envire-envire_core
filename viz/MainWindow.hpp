#pragma once
#include "ui_mainwindow.h"
#include <QMainWindow>
#include <memory>


namespace envire { namespace core 
{
  class EnvireGraph;
}}

namespace envire { namespace viz 
{
  class EnvireGraphVisualizer;
  class Vizkit3dPluginInformation;
  
  class MainWindow : public QMainWindow
  {
    Q_OBJECT
  public:
    MainWindow(envire::core::EnvireGraph& graph, const std::string& rootNode);
    
  public slots:
    void addFrame();
    
  private slots:
    void framePicked(const QString&);
    
  private:
    Ui::MainWindow window;
    envire::core::EnvireGraph& graph;
    std::shared_ptr<EnvireGraphVisualizer> visualzier;//is ptr for lazy instanziation
    std::shared_ptr<Vizkit3dPluginInformation> pluginInfos;//is ptr for lazy instanziation
    QString pickedFrame;//currently selected frame, empty if none
  };
  
  
  
}}