#pragma once
#include <QAbstractTableModel>
#include <envire_core/items/ItemBase.hpp>
#include <vector>
#include <mutex>


namespace envire { namespace viz {
  
/**A simple model to display information about ItemBase in a QTableView */
class ItemTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    ItemTableModel(QObject *parent=nullptr);
    
    void clear();
    void addItem(envire::core::ItemBase::Ptr item);
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    
private:
  std::vector<envire::core::ItemBase::Ptr> items;
  const int numColumns; //number of columns
};

}}