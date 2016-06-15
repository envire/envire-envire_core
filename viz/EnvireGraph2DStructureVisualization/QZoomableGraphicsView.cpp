#include "QZoomableGraphicsView.hpp"
#include <QWheelEvent>
#include <QGraphicsScene>
#include <QTimeLine>
#include <iostream> //FIXME remove after debugging

QZoomableGraphicsView::QZoomableGraphicsView(QGraphicsScene* scene):
    QGraphicsView(scene)
{
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
}

void QZoomableGraphicsView::wheelEvent(QWheelEvent* event)
{
  const double numDegrees = event->delta() / 8.0;
  const double numSteps = numDegrees / 15.0; // see QWheelEvent documentation
  
  const double factor = 1.0 + numSteps / 10.0;
  scale(factor, factor);
}
