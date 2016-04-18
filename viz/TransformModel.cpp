#include "TransformModel.hpp"

namespace envire { namespace viz {

  
TransformModel::TransformModel() :
  QStandardItemModel()
{
  setColumnCount(2);
  QStringList headerLabels;
  headerLabels.append("");
  headerLabels.append("Value");
  setHorizontalHeaderLabels(headerLabels);
  
  QStandardItem *parentItem = invisibleRootItem();
  
  QList<QStandardItem*> transItemList;
  QStandardItem* transItem = new QStandardItem("Translation");
  QStandardItem* transItemValue = new QStandardItem("");
  transItem->setEditable(false);
  transItemValue->setEditable(false);
  transItemList.append(transItem);
  transItemList.append(transItemValue);
  parentItem->appendRow(transItemList);
  
  QList<QStandardItem*> rotItemList;
  QStandardItem* rotItem = new QStandardItem("Orientation");
  QStandardItem* rotItemValue = new QStandardItem("");
  rotItem->setEditable(false);
  rotItemValue->setEditable(false);
  rotItemList.append(rotItem);
  rotItemList.append(rotItemValue);
  parentItem->appendRow(rotItemList);

  QList<QStandardItem*> transX;
  QList<QStandardItem*> transY;
  QList<QStandardItem*> transZ;
  
  QStandardItem* transXText = new QStandardItem("X");
  transXText->setEditable(false);
  QStandardItem* transYText = new QStandardItem("Y");
  transYText->setEditable(false);
  QStandardItem* transZText = new QStandardItem("Z");
  transZText->setEditable(false);
  
  transX.append(transXText);
  transY.append(transYText);
  transZ.append(transZText);
  transX.append(&transXItem);
  transY.append(&transYItem);
  transZ.append(&transZItem);
  
  transItem->appendRow(transX);
  transItem->appendRow(transY);
  transItem->appendRow(transZ);
  
  QList<QStandardItem*> rotX;
  QList<QStandardItem*> rotY;
  QList<QStandardItem*> rotZ;
  QList<QStandardItem*> rotW;
  
  QStandardItem* rotXText = new QStandardItem("X");
  QStandardItem* rotYText = new QStandardItem("Y");
  QStandardItem* rotZText = new QStandardItem("Z");
  QStandardItem* rotWText = new QStandardItem("W");
  rotXText->setEditable(false);
  rotYText->setEditable(false);
  rotZText->setEditable(false);
  rotWText->setEditable(false);
  
  rotX.append(rotXText);
  rotY.append(rotYText);
  rotZ.append(rotZText);
  rotW.append(rotWText);
  
  rotX.append(&rotXItem);
  rotY.append(&rotYItem);
  rotZ.append(&rotZItem);
  rotW.append(&rotWItem);
  
  rotItem->appendRow(rotX);
  rotItem->appendRow(rotY);
  rotItem->appendRow(rotZ);
  rotItem->appendRow(rotW);
  
  connect(this, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(itemChangedSlot(QStandardItem*)));
  setTransform(tf);//works because tf is default constructed to Identity  
}

void TransformModel::setTransform(const base::TransformWithCovariance& newValue)
{
  tf = newValue;
  //block signals while changing the transform programatically, to avoid
  //infinite loops
  transXItem.setData(QVariant::fromValue<double>(tf.translation.x()) ,Qt::DisplayRole);
  transYItem.setData(QVariant::fromValue<double>(tf.translation.y()) ,Qt::DisplayRole);
  transZItem.setData(QVariant::fromValue<double>(tf.translation.z()) ,Qt::DisplayRole);
  
  rotXItem.setData(QVariant::fromValue<double>(tf.orientation.x()) ,Qt::DisplayRole);
  rotYItem.setData(QVariant::fromValue<double>(tf.orientation.y()) ,Qt::DisplayRole);
  rotZItem.setData(QVariant::fromValue<double>(tf.orientation.z()) ,Qt::DisplayRole);
  rotWItem.setData(QVariant::fromValue<double>(tf.orientation.w()) ,Qt::DisplayRole);
}

void  TransformModel::itemChangedSlot(QStandardItem * item)
{
  const double data = item->data(Qt::DisplayRole).toDouble();
  if(item == &transXItem)
  {
    tf.translation.x() = data;
  }
  else if(item == &transYItem)
  {
    tf.translation.y() = data;
  }
  else if(item == &transZItem)
  {
    tf.translation.z() = data;
  }
  else if(item == &rotXItem)
  {
    tf.orientation.x() = data;
  }
  else if(item == &rotYItem)
  {
    tf.orientation.y() = data;
  }
  else if(item == &rotZItem)
  {
    tf.orientation.z() = data;
  }
  else if(item == &rotWItem)
  {
    tf.orientation.w() = data;
  }  
  emit transformChanged(tf);
}

void TransformModel::setEditable(const bool value)
{
  transXItem.setEditable(value);
  transYItem.setEditable(value);
  transZItem.setEditable(value);
  
  rotXItem.setEditable(value);
  rotYItem.setEditable(value);
  rotZItem.setEditable(value);
  rotWItem.setEditable(value);
}

const base::TransformWithCovariance& TransformModel::getTransform() const
{
  return tf;
}
  
  
}}