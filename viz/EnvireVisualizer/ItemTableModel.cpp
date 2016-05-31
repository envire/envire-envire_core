#include "ItemTableModel.hpp"
#include <glog/logging.h>
#include <envire_core/items/ItemMetadata.hpp>

using namespace envire::core;

namespace envire { namespace viz {

ItemTableModel::ItemTableModel(QObject* parent): QAbstractTableModel(parent),
  numColumns(4)
{}


void ItemTableModel::addItem(envire::core::ItemBase::Ptr item)
{
  {
    beginInsertRows(QModelIndex(), items.size(), items.size());
    items.push_back(item);
    endInsertRows();
  }
}

void ItemTableModel::clear()
{
  if(items.size() > 0)
  {
    beginRemoveRows(QModelIndex(), 0, items.size()-1);
    items.clear();
    endRemoveRows();
  }
}

int ItemTableModel::columnCount(const QModelIndex& parent) const
{
  return numColumns;
}

int ItemTableModel::rowCount(const QModelIndex& parent) const
{
  return items.size();
}

QVariant ItemTableModel::data(const QModelIndex& index, int role) const
{
  const int row = index.row();
  const int col = index.column();

  if(row >= int(items.size()))
  {
    LOG(ERROR) << "ItemTableModel: row index out of range: " << row;
    return QVariant();
  }
  
  if(col >= numColumns)
  {
    LOG(ERROR) << "ItemTableModel: column index out of range: " << col;
    return QVariant();
  }
  
  if (role == Qt::DisplayRole)
  {
    const ItemBase::Ptr item = items[row];
    switch(col)
    {
      case 0: //uuid
        return QVariant(QString::fromStdString(item->getIDString()));
      case 1: //type
        return QVariant(QString::fromStdString(item->getClassName()));
      case 2: //raw type
      {
        std::string rawType("Unknown Type");
        try 
        {
          //this fails if the type was not properly loaded using the plugin system
          rawType = ItemMetadataMapping::getMetadata(*(item->getTypeInfo())).embeddedTypename;
        }
        catch(const std::out_of_range& ex){}
        return QVariant(QString::fromStdString(rawType));
      }
      case 3: //timestamp
        return QVariant(QString::fromStdString(item->getTime().toString()));        
      default:
        LOG(ERROR) << "ItemTableModel: unknown column: " << col;
        return QVariant();
    }
  }
  return QVariant();
}

QVariant ItemTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role != Qt::DisplayRole)
    return QVariant();

  if (orientation == Qt::Horizontal) 
  {
    switch (section) 
    {
      case 0:
        return tr("UUID");
      case 1:
        return tr("Class Name");
      case 2:
        return tr("Raw Type");
      case 3:
        return tr("Timestamp");
      default:
        return QVariant();
    }
  }
  return QVariant();
}

}}
