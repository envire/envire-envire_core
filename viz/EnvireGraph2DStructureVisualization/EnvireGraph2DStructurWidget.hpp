#pragma once

#include <QtGui>

#ifndef Q_MOC_RUN
//workaround for qt bug ( 4.7.3, 4.7.4, 4.8.0, 4.8.1 ) with boost 1.48
//https://bugreports.qt.io/browse/QTBUG-22829
#include <envire_core/graph/EnvireGraph.hpp>
#endif

namespace envire { namespace viz {

class EnvireGraph2DStructurWidget : public QWidget
{
    Q_OBJECT
public:
    EnvireGraph2DStructurWidget(QWidget *parent = 0);
    
public slots:
    void displayGraph(const envire::core::EnvireGraph& graph);
    /** @param dotGraph The graph in dot format */
    void displayGraph(const QString& dotGrap);
    
private:
    QGraphicsScene* scene; /**The scene displaying the current graph */
};
}}
