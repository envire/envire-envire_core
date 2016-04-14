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
  /** Used to programatically change the transform
   * This method should be invoked in the gui thread*/
  void setTransform(const base::TransformWithCovariance& newValue);
  /*If set to false, the model will be read only */
  void setEditable(const bool value);
  
  const base::TransformWithCovariance& getTransform() const;
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
