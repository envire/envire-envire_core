#pragma once

#include <QStyledItemDelegate>

namespace envire { namespace viz {

/**A double spin box item delegate with high precision */
class DoubleSpinboxItemDelegate : public QStyledItemDelegate
{
  Q_OBJECT

/**NOTE: This whole class only exists because the default precision of
  *       QDoubleSpinBox is 2 and there is no way to change it inside a 
  *       TreeView or TableView without providing a custom  delegate.
  *
  * This class is mostly copied from
  * http://doc.qt.io/qt-5/qtwidgets-itemviews-spinboxdelegate-example.html
  */
  
public:
  DoubleSpinboxItemDelegate(QObject *parent = 0);

  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                        const QModelIndex &index) const;

  void setEditorData(QWidget *editor, const QModelIndex &index) const;
  void setModelData(QWidget *editor, QAbstractItemModel *model,
                    const QModelIndex &index) const;

  void updateEditorGeometry(QWidget *editor,
  const QStyleOptionViewItem &option, const QModelIndex &index) const;
      
  void setDecimals(const unsigned int value);
        
private:
  unsigned int decimals;
};
}}