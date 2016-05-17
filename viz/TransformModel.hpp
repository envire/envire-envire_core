#pragma once
#include <QStandardItemModel>
#include <envire_core/items/Transform.hpp>

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
  void transformChanged(const envire::core::Transform& newValue);
  
public slots:
  /** Used to programatically change the transform
   * This method should be invoked in the gui thread*/
  void setTransform(const envire::core::Transform& newValue);
  /*If set to false, the model will be read only */
  void setEditable(const bool value);
  
  const envire::core::Transform& getTransform() const;
private slots:

  void  itemChangedSlot(QStandardItem * item);
private:
  QStandardItemModel model;
  envire::core::Transform tf;
  
  QStandardItem transXItem;
  QStandardItem transYItem;
  QStandardItem transZItem;
  QStandardItem rotXItem;
  QStandardItem rotYItem;
  QStandardItem rotZItem;
  QStandardItem rotWItem;
  QStandardItem timestampItem;
};

}}
