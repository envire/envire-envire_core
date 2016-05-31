#include "EnvireGraph2DStructureVisualization.hpp"

EnvireGraph2DStructureVisualization::EnvireGraph2DStructureVisualization(QWidget *parent)
    : QWidget(parent)
{
    resize(300,120);

    QLabel *label = new QLabel("Rock 'n Robots!");
    label->setFont(QFont("Verdana", 20));
    label->setAlignment(Qt::AlignCenter);

    QVBoxLayout vbox(this);
    vbox.addWidget(label);
    vbox.setAlignment(Qt::AlignVCenter);

    show();
}

EnvireGraph2DStructureVisualization::~EnvireGraph2DStructureVisualization()
{
}
