#include "MainWindow.hpp"
#include "EnvireGraphVisualizer.hpp"
#include "Vizkit3dPluginInformation.hpp"
#include <envire_core/graph/EnvireGraph.hpp>
#include <QMessageBox>
#include <QInputDialog>


using namespace envire::core;
namespace envire { namespace viz
{
  
MainWindow::MainWindow(envire::core::EnvireGraph& graph, const std::string& rootNode) :
    QMainWindow(), graph(graph)
{
  window.setupUi(this);
  window.treeWidget->setHeaderLabel("Frames");
  
  //has to be done after setupUi because window.Vizkit3DWidget needs to exist
  pluginInfos.reset(new Vizkit3dPluginInformation(window.Vizkit3DWidget));
  visualzier.reset(new EnvireGraphVisualizer(graph, window.Vizkit3DWidget, rootNode, pluginInfos));
  
  connect(window.actionAdd_Frame, SIGNAL(activated(void)), this, SLOT(addFrame()));
  connect(window.Vizkit3DWidget, SIGNAL(framePicked(const QString&)), this, SLOT(framePicked(const QString&)));
}

void MainWindow::addFrame()
{
    bool ok;
    const QString text = QInputDialog::getText(this, tr("Add Frame"),
                                              tr("Frame ID:"), QLineEdit::Normal,
                                              "", &ok);
    if(ok && !text.isEmpty())
    {
      try 
      {
        
        graph.addFrame(text.toStdString());
      }
      catch(FrameAlreadyExistsException& ex)
      {
        QMessageBox::critical(this, tr("Frame already exists"), QString::fromStdString(ex.msg));
      }
    }
}

void MainWindow::framePicked(const QString& frame)
{
  if(frame != pickedFrame)
  {
      std::cout << "MAIN window FRAME PICKD " << frame.toStdString() << std::endl;
      window.Vizkit3DWidget->setFrameHighlight(frame, true);
      if(!pickedFrame.isEmpty())
        window.Vizkit3DWidget->setFrameHighlight(pickedFrame, false);
      pickedFrame = frame;
  }
}

  
  
}}