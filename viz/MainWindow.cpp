#include "MainWindow.hpp"
#include "EnvireGraphVisualizer.hpp"
#include "TransformModel.hpp"
#include "Helpers.hpp"
#include "Vizkit3dPluginInformation.hpp"
#include "DoubleSpinboxItemDelegate.hpp"
#include "AddTransformDialog.hpp"
#include <envire_core/graph/EnvireGraph.hpp>
#include <envire_core/events/EdgeEvents.hpp>
#include <QMessageBox>
#include <QInputDialog>
#include <glog/logging.h>
#include <fstream>
#include <boost/archive/polymorphic_text_iarchive.hpp> //FIXME just for testing


using namespace envire::core;
using vertex_descriptor = GraphTraits::vertex_descriptor;

namespace envire { namespace viz
{
  
MainWindow::MainWindow(): QMainWindow(), GraphEventDispatcher(),
rootFrame(""), ignoreEdgeModifiedEvent(false), firstTimeDisplayingItems(true)
{
  window.setupUi(this);
    
  window.treeView->setModel(&currentTransform);
  window.treeView->expandAll();
  DoubleSpinboxItemDelegate* del = new DoubleSpinboxItemDelegate(window.treeView);
  window.treeView->setItemDelegateForColumn(1, del);
  window.listWidget->setSortingEnabled(true);
  window.tableViewItems->setModel(&currentItems);//tableView will not take ownership
  window.tableViewItems->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
  
  connect(window.Vizkit3DWidget, SIGNAL(framePicked(const QString&)), this, SLOT(framePicked(const QString&)));
  connect(window.Vizkit3DWidget, SIGNAL(frameMoved(const QString&, const QVector3D&, const QQuaternion)),
          this, SLOT(frameMoved(const QString&, const QVector3D&, const QQuaternion&)));          
  connect(window.actionRemove_Frame, SIGNAL(activated(void)), this, SLOT(removeFrame()));
  connect(window.actionAdd_Frame, SIGNAL(activated(void)), this, SLOT(addFrame()));
  connect(window.actionLoad_Graph, SIGNAL(activated(void)), this, SLOT(loadGraph()));
  connect(window.actionSave_Graph, SIGNAL(activated(void)), this, SLOT(storeGraph()));
  connect(window.listWidget, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
          this, SLOT(listWidgetItemChanged(QListWidgetItem*, QListWidgetItem*)));
  
  connect(&currentTransform, SIGNAL(transformChanged(const base::TransformWithCovariance&)),
          this, SLOT(transformChanged(const base::TransformWithCovariance&)));
  
  pluginInfos.reset(new Vizkit3dPluginInformation(window.Vizkit3DWidget));
  
  //disable everything until a graph is loaded
  window.treeView->setEnabled(false);
  window.listWidget->setEnabled(false);
  window.Vizkit3DWidget->setEnabled(false);
  window.actionAdd_Frame->setEnabled(false);
  window.actionRemove_Frame->setEnabled(false);
  window.actionSave_Graph->setEnabled(false);
}
  
void MainWindow::displayGraph(std::shared_ptr<envire::core::EnvireGraph> graph,
                              const QString& rootNode)
{
  
  this->graph = graph;
  //reset the widget because this might not be the first time the user loads a graph
  //window.Vizkit3DWidget->clear(); //FIXME reimplement vitkit clear
  //window.Vizkit3DWidget->setWorldName(rootNode); //FIXME reimplement setWorldName
  
  visualzier.reset(new EnvireGraphVisualizer(graph, window.Vizkit3DWidget,
                                             rootNode.toStdString(), pluginInfos));
  
  connect(visualzier.get(), SIGNAL(frameAdded(const QString&)), this,
          SLOT(frameNameAdded(const QString&)));
  connect(visualzier.get(), SIGNAL(frameRemoved(const QString&)), this,
          SLOT(frameNameRemoved(const QString&)));
    
  //get initially present frame names
  window.listWidget->clear();
  EnvireGraph::vertex_iterator it, end;
  std::tie(it, end) = graph->getVertices();
  for(; it != end; it++) 
  {
    const FrameId& id = graph->getFrameId(*it);
    window.listWidget->addItem(QString::fromStdString(id));
  }
     
  window.treeView->setEnabled(true);
  window.listWidget->setEnabled(true);
  window.Vizkit3DWidget->setEnabled(true);
  window.actionAdd_Frame->setEnabled(true);
  window.actionRemove_Frame->setEnabled(true);
  window.actionSave_Graph->setEnabled(true);
  
  rootFrame = rootNode;
  selectedFrame = "";//otherwise we might try to unhighlight a no longer existing frame
  selectFrame(rootNode);//done after enabling gui because it might disable parts of the gui again
}

void MainWindow::displayGraph(const QString& filePath)
{
  try 
  {
    std::shared_ptr<EnvireGraph> pGraph(new EnvireGraph());
    pGraph->loadFromFile(filePath.toStdString());
    
    QStringList frames;
    EnvireGraph::vertex_iterator it, end;
    std::tie(it, end) = pGraph->getVertices();
    for(; it != end; it++)
    {
      const FrameId id = pGraph->getFrameId(*it);
      frames << QString::fromStdString(id);
    }

    bool ok;
    QString rootNode = QInputDialog::getItem(this, tr("Select World Frame"),
                                              tr("Frame:"), frames, 0, false, &ok);
    if (ok && !rootNode.isEmpty())
    {
      displayGraph(pGraph, rootNode);
    }
  }
  catch(const boost::archive::archive_exception& ex)
  {
    LOG(ERROR) << "Error while loading envire graph: " << ex.what();
  }
  catch(std::ios_base::failure& ex)
  {
    LOG(ERROR) << "Error while loading envire graph: " << ex.what();
  }
}

void MainWindow::addFrame()
{
  AddTransformDialog dialog(this);
  if(dialog.exec() == QDialog::Accepted)
  {
    Transform tf(dialog.getTransform());
    const FrameId frame = dialog.getFrameId().toStdString();
    if(frame.size() > 0)
    {
      graph->addTransform(selectedFrame.toStdString(), frame, tf);
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
    
    graph->disconnectFrame(frameId);
    graph->removeFrame(frameId);
    //this will trigger events, that will remove the frame from the list widget as well.
  }
}

void MainWindow::framePicked(const QString& frame)
{
  selectFrame(frame);
}

void MainWindow::listWidgetItemChanged(QListWidgetItem * current, QListWidgetItem * previous)
{
  //current is nullptr if the list is cleared
  if(current != nullptr)
    selectFrame(current->text());
}

void MainWindow::selectFrame(const QString& name)
{  
  if(name != selectedFrame)
  {
      //highlight in 3d
      //window.Vizkit3DWidget->setFrameHighlight(name, true); //FIXME reimplement setFrameHighlight
      //unhighlight old selection
     // if(!selectedFrame.isEmpty())//happens if nothing was selected or selection was deleted
        //window.Vizkit3DWidget->setFrameHighlight(selectedFrame, false); //FIXME reimplement setFrameHighlight
      
      
      //select in list widget
      QList<QListWidgetItem*> items = window.listWidget->findItems(name, Qt::MatchExactly);
      assert(items.size() == 1);
      if(!items.first()->isSelected())
        items.first()->setSelected(true);
      
      //display corresponding Transform
      const vertex_descriptor selectedVertex = graph->getVertex(name.toStdString());
      const vertex_descriptor parentVertex = visualzier->getTree().tree.at(selectedVertex).parent;
      updateDisplayedTransform(parentVertex, selectedVertex);
      
      //user should not be able to delete the root frame
      if(name == rootFrame)
        window.actionRemove_Frame->setEnabled(false);
      else
        window.actionRemove_Frame->setEnabled(true);
      
      selectedFrame = name;
      displayItems(selectedFrame);
  }
}

void MainWindow::updateDisplayedTransform(const vertex_descriptor parent, const vertex_descriptor selected)
{
  //disconnect before changing the transform model because changing the
  //value triggers events that are indistinguishable from user input
  disconnect(&currentTransform, SIGNAL(transformChanged(const base::TransformWithCovariance&)),
      this, SLOT(transformChanged(const base::TransformWithCovariance&)));
  if(parent != GraphTraits::null_vertex())
  {
    const Transform tf = graph->getTransform(parent, selected);
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
 
  const vertex_descriptor selectedVertex = graph->getVertex(selectedFrame.toStdString());
  const vertex_descriptor parentVertex = visualzier->getTree().tree.at(selectedVertex).parent;
  const FrameId source = graph->getFrameId(parentVertex);
  const FrameId target = selectedFrame.toStdString();
  ignoreEdgeModifiedEvent = true;
  graph->updateTransform(source, target, newValue);//will trigger EdgeModifiedEvent
  ignoreEdgeModifiedEvent = false;
  
}

void MainWindow::edgeModified(const EdgeModifiedEvent& e)
{
  const QString origin = QString::fromStdString(e.origin);
  const QString target = QString::fromStdString(e.target);
  //need to invoke because the graph might have been modified from a different
  //thread
  const Qt::ConnectionType conType = Helpers::determineConnectionType(this);
  QMetaObject::invokeMethod(this, "edgeModifiedInternal", conType,
                            Q_ARG(QString, origin), Q_ARG(QString, target));  
}

void MainWindow::edgeModifiedInternal(const QString& originFrame, const QString& targetFrame)
{
  vertex_descriptor originVertex = graph->getVertex(originFrame.toStdString());
  vertex_descriptor targetVertex = graph->getVertex(targetFrame.toStdString());
  const TreeView& tree = visualzier->getTree();
  
  if(tree.vertexExists(originVertex) && tree.vertexExists(targetVertex))
  {
    if(selectedFrame == originFrame)
    {
      if(tree.isParent(targetVertex, originVertex))
      {
        updateDisplayedTransform(targetVertex, originVertex);
      }
    }
    else if(selectedFrame == targetFrame)
    {
      if(tree.isParent(originVertex, targetVertex))
      {
        updateDisplayedTransform(originVertex, targetVertex);
      }
    }
  }  
}

void MainWindow::loadGraph()
{
  //DontUseNativeDialog is used because the native dialog on xfce hangs and crashes...
  const QString file = QFileDialog::getOpenFileName(this, tr("Load Envire Graph"),
                                                    QDir::currentPath(), QString(),
                                                    0, QFileDialog::DontUseNativeDialog);
  if(!file.isEmpty())
  {
    LOG(INFO) << "Loading graph from " << file.toStdString();
    displayGraph(file);
  }
}

void MainWindow::storeGraph()
{
  //DontUseNativeDialog is used because the native dialog on xfce hangs and crashes...
  const QString file = QFileDialog::getSaveFileName(this, tr("Save Envire Graph"),
                                                    QDir::currentPath(), QString(),
                                                    0, QFileDialog::DontUseNativeDialog);

  if(!file.isEmpty())
  {
    LOG(INFO) << "Saving graph to " << file.toStdString();
    if(graph)
    {
      graph->saveToFile(file.toStdString());
    }
  }
}

void MainWindow::displayItems(const QString& frame)
{
  const FrameId frameId = frame.toStdString();
  currentItems.clear();
  bool visited = false;//changed to true if at least one item is visited
  graph->visitItems(frameId, [this, &visited] (const ItemBase::Ptr item)
  {
    this->currentItems.addItem(item);
    visited = true;
  });
  
  //resize the table on the first time it has some content.
  //afterwards the user may change it and we not want to override the users
  //size choice
  if(firstTimeDisplayingItems && visited)
  {
    firstTimeDisplayingItems = false;
    window.tableViewItems->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
  }
  
}

void MainWindow::frameMoved(const QString& frame, const QVector3D& trans, const QQuaternion& rot)
{
  const vertex_descriptor movedVertex = graph->getVertex(frame.toStdString());
  if(movedVertex != graph->null_vertex() && visualzier->getTree().vertexExists(movedVertex))
  {
    const vertex_descriptor parentVertex = visualzier->getTree().tree.at(movedVertex).parent;
    if(parentVertex != graph->null_vertex())
    {
      Transform tf = graph->getTransform(parentVertex, movedVertex);
      const base::Vector3d translation(trans.x(), trans.y(), trans.z());
      //const base::Vector3d trans(0, 0, 0);
//       std::cout << "translation         : " << translation.transpose() << std::endl;
     // std::cout << "parent translation  : " << tf.transform.translation.transpose() << std::endl;
     // std::cout << "parent rotation  : " << tf.transform.orientation.coeffs().transpose() << std::endl;
      
      
      tf.transform.translation += tf.transform.orientation * translation;
//       std::cout << "after update trans  : " << tf.transform.translation.transpose() << std::endl;
      graph->updateTransform(parentVertex, movedVertex, tf);
    }
    else
    {
      LOG(ERROR) << "Tried to move the root vertex, this should not be possible";
    }
  }
  else
  {
    LOG(ERROR) << "Moved a frame that is not part of the graph: " << frame.toStdString();
  }

}



}}