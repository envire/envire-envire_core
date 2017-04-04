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

}

void EnvireGraph2DStructurWidget::displayGraph(const envire::core::EnvireGraph& graph)
{
//     std::stringstream stream;
//     GraphViz::write(graph, stream);
//     displayGraph(QString::fromStdString(stream.str()));
}
}}
