#pragma once
#include <QStandardItemModel>
#include <base/TransformWithCovariance.hpp>

namespace envire { namespace viz {
  
/**A qt model for TransformWithCovariance that can be used with tables and
 * tree views etc. */
class TransformModel : public QStandardItemModel
{
  Q_OBJECT
public:
  TransformModel();
  
signals:
  /**Emitted when the transform is changed either by the user or by 
   * setTransform().*/
  void transformChanged(const base::TransformWithCovariance& newValue);
  
public slots:
  /** Used to programatically change the transform */
  void setTransform(const base::TransformWithCovariance& newValue);
  
private slots:

  void  itemChangedSlot(QStandardItem * item);
private:
  QStandardItemModel model;
  base::TransformWithCovariance tf;
  
  QStandardItem transXItem;
  QStandardItem transYItem;
  QStandardItem transZItem;
  QStandardItem rotXItem;
  QStandardItem rotYItem;
  QStandardItem rotZItem;
  QStandardItem rotWItem;
};

}}
