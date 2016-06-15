#include "EnvireGraph2DStructurWidget.hpp"
#include <QGraphicsScene>
#include "QZoomableGraphicsView.hpp"
#include <QtSvg/QGraphicsSvgItem>
#include <QtSvg/QSvgRenderer>
#include <gvc.h>
#include <envire_core/graph/GraphViz.hpp>
#include <sstream>
#include <boost/graph/graphviz.hpp>

using namespace envire::core;

namespace envire { namespace viz {

EnvireGraph2DStructurWidget::EnvireGraph2DStructurWidget(QWidget *parent)
    : QWidget(parent) 
{
    resize(300,120);
    
    scene = new QGraphicsScene();
    QZoomableGraphicsView* view = new QZoomableGraphicsView(scene);    
    view->setDragMode(QGraphicsView::ScrollHandDrag);
    QVBoxLayout* vbox = new QVBoxLayout();
    vbox->addWidget(view);
    setLayout(vbox);
      
    show();
}

void EnvireGraph2DStructurWidget::displayGraph(const QString& dotGraph)
{
    //see: http://www.graphviz.org/doc/libguide/libguide.pdf
    const QByteArray data = dotGraph.toAscii();
    Agraph_t* graph = agmemread(data.constData());
    GVC_t* gvc = gvContext();
    gvLayout(gvc, graph, "dot");
    char* svg;
    unsigned int svgSize = 0;
    gvRenderData (gvc, graph, "svg", &svg, &svgSize);
    
    QByteArray qSvg(svg, svgSize);
    
    gvFreeRenderData(svg);
    gvFreeLayout(gvc, graph);
    agclose(graph);
    gvFreeContext(gvc);
    
    QSvgRenderer* renderer = new QSvgRenderer(qSvg);
    QGraphicsSvgItem *item = new QGraphicsSvgItem();
    item->setSharedRenderer(renderer);
    scene->clear();
    scene->addItem(item);
}

void EnvireGraph2DStructurWidget::displayGraph(const envire::core::EnvireGraph& graph)
{
    std::stringstream stream;
    GraphViz::write(graph, stream);
    displayGraph(QString::fromStdString(stream.str()));
}
}}
