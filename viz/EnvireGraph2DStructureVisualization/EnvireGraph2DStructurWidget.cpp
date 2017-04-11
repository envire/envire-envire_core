//
// Copyright (c) 2015, Deutsches Forschungszentrum für Künstliche Intelligenz GmbH.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#include "EnvireGraph2DStructurWidget.hpp"
#include <QGraphicsScene>
#include "QZoomableGraphicsView.hpp"
#include <QtSvg/QGraphicsSvgItem>
#include <QtSvg/QSvgRenderer>
#include <envire_core/graph/GraphDrawing.hpp>
#include <boost/graph/graphviz.hpp>
#include <gvc.h>


using namespace envire::core;

namespace envire { namespace viz {

EnvireGraph2DStructurWidget::EnvireGraph2DStructurWidget(int updateIntervalMs, 
                                                         QWidget *parent)
    : QWidget(parent), renderer(nullptr), item(nullptr), needRedraw(false),
      updateInterval(updateIntervalMs), runLayoutThread(true), 
      layoutThread(&EnvireGraph2DStructurWidget::layoutGraph, this)
    
{
    qRegisterMetaType<QSvgRenderer*>("QSvgRenderer*");
    
    resize(300,120);
    
    scene = new QGraphicsScene(this);
    view = new QZoomableGraphicsView(scene);    
    view->setDragMode(QGraphicsView::ScrollHandDrag);
    QVBoxLayout* vbox = new QVBoxLayout();
    vbox->addWidget(view);
    setLayout(vbox);
    renderer = new QSvgRenderer();
    item = new QGraphicsSvgItem();

    scene->addItem(item);
    


    show();
}

EnvireGraph2DStructurWidget::~EnvireGraph2DStructurWidget()
{
    runLayoutThread = false;
    layoutThread.join();
}

void EnvireGraph2DStructurWidget::layoutGraph()
{
    //layouting is done in a different thread to reduce gui lag for large graphs
    GVC_t* gvc = gvContext();
    
    while(runLayoutThread)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(updateInterval));
        
        if(!runLayoutThread)
            break;
        
        if(!needRedraw)
            continue;
        
        
        QString dotStr;
        {
            std::lock_guard<std::mutex> lock(currentGraphMutex);
            dotStr = currentGraph;
            needRedraw = false;
        }
        
        Agraph_t* graph = agmemread(dotStr.toStdString().c_str());
        gvLayout(gvc, graph, "dot");
        char* svg;
        unsigned int svgSize = 0;
        gvRenderData (gvc, graph, "svg", &svg, &svgSize);
        
        QByteArray qSvg(svg, svgSize);
        QSvgRenderer* r = new QSvgRenderer();
        r->load(qSvg);
        r->moveToThread(this->thread());
        QMetaObject::invokeMethod(this, "displaySvg", Qt::QueuedConnection,
                            Q_ARG(QSvgRenderer*, r));  
        
        gvFreeRenderData(svg);
        gvFreeLayout(gvc, graph);
        agclose(graph);
    }
    
    gvFreeContext(gvc);
}


void EnvireGraph2DStructurWidget::displaySvg(QSvgRenderer* r)
{
    item->setSharedRenderer(r);
}



void EnvireGraph2DStructurWidget::displayGraph(const QString& dotStr)
{   
    
    std::lock_guard<std::mutex> lock(currentGraphMutex);
    currentGraph = dotStr;
    needRedraw = true;
}
    
}}
