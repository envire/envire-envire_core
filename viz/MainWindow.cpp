#include "MainWindow.hpp"
#include "EnvireGraphVisualizer.hpp"
#include "Vizkit3dPluginInformation.hpp"
#include <envire_core/graph/EnvireGraph.hpp>
#include <QMessageBox>
#include <QInputDialog>


using namespace envire::core;
namespace envire { namespace viz
{
  
MainWindow::MainWindow(EnvireGraph& graph, const std::string& rootNode) :
    QMainWindow(), graph(graph)
{
  window.setupUi(this);
  
  //has to be done after setupUi because window.Vizkit3DWidget needs to exist
  pluginInfos.reset(new Vizkit3dPluginInformation(window.Vizkit3DWidget));
  visualzier.reset(new EnvireGraphVisualizer(graph, window.Vizkit3DWidget, rootNode, pluginInfos));
    
  //get initially present frame names
  EnvireGraph::vertex_iterator it, end;
  std::tie(it, end) = graph.getVertices();
  for(; it != end; it++)
  {
    const FrameId& id = graph.getFrameId(*it);
    window.listWidget->addItem(QString::fromStdString(id));
  }
  
  connect(window.actionAdd_Frame, SIGNAL(activated(void)), this, SLOT(addFrame()));
  connect(window.Vizkit3DWidget, SIGNAL(framePicked(const QString&)), this, SLOT(framePicked(const QString&)));
  connect(visualzier.get(), SIGNAL(frameAdded(const QString&)), this, SLOT(frameNameAdded(const QString&)));
  connect(visualzier.get(), SIGNAL(frameRemoved(const QString&)), this, SLOT(frameNameRemoved(const QString&)));
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

void MainWindow::frameNameAdded(const QString& name)
{
  window.listWidget->addItem(name);
}

void MainWindow::frameNameRemoved(const QString& name)
{
  QList<QListWidgetItem*> items = window.listWidget->findItems(name, Qt::MatchExactly);
  assert(items.size() == 1); //the frame ids are unique and should not be in the list more than once
  delete items.first(); //this will remove the item from the listWidget
}
  
  
}}