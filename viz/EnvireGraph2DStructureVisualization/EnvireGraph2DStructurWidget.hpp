#pragma once

#include <QtGui>
#include <envire_core/graph/EnvireGraph.hpp>

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