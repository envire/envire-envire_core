#pragma once
#include <QGraphicsView>

class QGraphicsScene;
class QWheelEvent;

/** A QGraphicsView that supports zooming using the mouse wheel */
class QZoomableGraphicsView : public QGraphicsView
{
  Q_OBJECT
public:
  QZoomableGraphicsView(QGraphicsScene* scene);
  
protected:
  /**Reimplemented to implement smooth zooming */
  virtual void wheelEvent(QWheelEvent * event);
};