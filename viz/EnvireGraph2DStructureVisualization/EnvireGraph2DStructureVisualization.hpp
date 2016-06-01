#ifndef ENVIREGRAPH2DSTRUCTUREVISUALIZATION_HPP
#define ENVIREGRAPH2DSTRUCTUREVISUALIZATION_HPP

#include <QtGui>
#include <envire_core/graph/EnvireGraph.hpp>

class EnvireGraph2DStructureVisualization : public QWidget
{
    Q_OBJECT
public:
    EnvireGraph2DStructureVisualization(QWidget *parent = 0);
    virtual ~EnvireGraph2DStructureVisualization();
    
public slots:
    void displayGraph(const envire::core::EnvireGraph& graph);
    /** @param dotGraph The graph in dot format */
    void displayGraph(const QString& dotGrap);
    
private:
    QGraphicsScene* scene; /**The scene displaying the current graph */
};

#endif /* ENVIREGRAPH2DSTRUCTUREVISUALIZATION_HPP */
