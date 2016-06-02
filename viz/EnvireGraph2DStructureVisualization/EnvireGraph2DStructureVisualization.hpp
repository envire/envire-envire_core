#ifndef ENVIREGRAPH2DSTRUCTUREVISUALIZATION_HPP
#define ENVIREGRAPH2DSTRUCTUREVISUALIZATION_HPP
#include "EnvireGraph2DStructurWidget.hpp"

/**The qt designer lib is installed into a different folder where it is not
 * found by most applications. In order to use the 2d visualizer widget 
 * in other applications as well, the whole functionallity is implemented in 
 * EnvireGraph2DStructurWidget.hpp. This file is just a wrapper for the qt 
 * designer stuff */
class EnvireGraph2DStructureVisualization : public EnvireGraph2DStructurWidget
{
    Q_OBJECT
public:
    EnvireGraph2DStructureVisualization(QWidget *parent = 0) : 
        EnvireGraph2DStructurWidget(parent) {}
};

#endif /* ENVIREGRAPH2DSTRUCTUREVISUALIZATION_HPP */
