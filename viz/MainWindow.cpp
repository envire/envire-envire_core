#include "MainWindow.hpp"
#include "EnvireGraphVisualizer.hpp"
#include "TransformModel.hpp"
#include "Vizkit3dPluginInformation.hpp"
#include "DoubleSpinboxItemDelegate.hpp"
#include <envire_core/graph/EnvireGraph.hpp>
#include <QMessageBox>
#include <QInputDialog>
#include <glog/logging.h>


using namespace envire::core;
using vertex_descriptor = GraphTraits::vertex_descriptor;

namespace envire { namespace viz
{
  
MainWindow::MainWindow(EnvireGraph& graph, const std::string& rootNode) :
    QMainWindow(), graph(graph)
{
  window.setupUi(this);
  
  window.treeView->setModel(&currentTransform);
  DoubleSpinboxItemDelegate* del = new DoubleSpinboxItemDelegate(window.treeView);
  window.treeView->setItemDelegateForColumn(1, del);
  
  //need to call a custom ctor on the vizkit3dwidget to set the correct world_name
  //this is done because I want to see everything in the qt designer. Otherwise
  //we could have just added the vizkit3dwidget manually in the first place
  delete window.Vizkit3DWidget;
  window.Vizkit3DWidget = new vizkit3d::Vizkit3DWidget(window.horizontalSplitter, QString::fromStdString(rootNode));
  window.Vizkit3DWidget->setObjectName(QString::fromUtf8("Vizkit3DWidget"));
  window.horizontalSplitter->addWidget(window.Vizkit3DWidget);
  
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
  
  connect(window.actionRemove_Frame, SIGNAL(activated(void)), this, SLOT(removeFrame()));
  connect(window.actionAdd_Frame, SIGNAL(activated(void)), this, SLOT(addFrame()));
  connect(window.Vizkit3DWidget, SIGNAL(framePicked(const QString&)), this, SLOT(framePicked(const QString&)));
  connect(window.listWidget, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
          this, SLOT(listWidgetItemChanged(QListWidgetItem*, QListWidgetItem*)));
  
  connect(visualzier.get(), SIGNAL(frameAdded(const QString&)), this, SLOT(frameNameAdded(const QString&)));
  connect(visualzier.get(), SIGNAL(frameRemoved(const QString&)), this, SLOT(frameNameRemoved(const QString&)));
  connect(&currentTransform, SIGNAL(transformChanged(const base::TransformWithCovariance&)),
          this, SLOT(transformChanged(const base::TransformWithCovariance&)));
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

void MainWindow::removeFrame()
{
  if(!selectedFrame.isEmpty())
  {
    const FrameId frameId = selectedFrame.toStdString();
    
    //has to be done before anything is removed, because some of the event handlers
    //depend on selectedFrame beeing valid
    selectedFrame = ""; 
    
    graph.disconnectFrame(frameId);
    graph.removeFrame(frameId);
    //this will trigger events, that will remove the frame from the list widget as well.
  }
}

void MainWindow::framePicked(const QString& frame)
{
  selectFrame(frame);
}

void MainWindow::listWidgetItemChanged(QListWidgetItem * current, QListWidgetItem * previous)
{
  selectFrame(current->text());
}

void MainWindow::selectFrame(const QString& name)
{  
  //FIXME aufsplitten
  LOG(ERROR) << "Selected frame: " << name.toStdString();
  if(name != selectedFrame)
  {
      //highlight in 3d
      window.Vizkit3DWidget->setFrameHighlight(name, true);
      //unhighlight old selection
      if(!selectedFrame.isEmpty())//happens if nothing was selected or selection was deleted
        window.Vizkit3DWidget->setFrameHighlight(selectedFrame, false);
      selectedFrame = name;
      
      //select in list widget
      QList<QListWidgetItem*> items = window.listWidget->findItems(name, Qt::MatchExactly);
      assert(items.size() == 1);
      if(!items.first()->isSelected())
        items.first()->setSelected(true);
      
      //display corresponding Transform
      const vertex_descriptor selectedVertex = graph.getVertex(name.toStdString());
      const vertex_descriptor parentVertex = visualzier->getTree().tree.at(selectedVertex).parent;
      
      //disconnect before changing the transform model because changing the
      //value triggers events that are indistinguishable from user input
      disconnect(&currentTransform, SIGNAL(transformChanged(const base::TransformWithCovariance&)),
          this, SLOT(transformChanged(const base::TransformWithCovariance&)));
      if(parentVertex != GraphTraits::null_vertex())
      {
        const Transform tf = graph.getTransform(parentVertex, selectedVertex);
        currentTransform.setTransform(tf.transform);
        currentTransform.setEditable(true);
        window.treeView->setEnabled(true);
      }
      else
      {
        currentTransform.setTransform(base::TransformWithCovariance());
        currentTransform.setEditable(false);
        window.treeView->setEnabled(false);
      }
      connect(&currentTransform, SIGNAL(transformChanged(const base::TransformWithCovariance&)),
              this, SLOT(transformChanged(const base::TransformWithCovariance&)));
      
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

void MainWindow::transformChanged(const base::TransformWithCovariance& newValue)
{
 
  std::cout << "newTrans" << std::endl << newValue.translation.transpose() << std::endl
  << newValue.orientation.coeffs().transpose() << std::endl;
 
  const vertex_descriptor selectedVertex = graph.getVertex(selectedFrame.toStdString());
  const vertex_descriptor parentVertex = visualzier->getTree().tree.at(selectedVertex).parent;
  const FrameId source = graph.getFrameId(parentVertex);
  const FrameId target = selectedFrame.toStdString();
  graph.updateTransform(source, target, newValue);
  
}

}}