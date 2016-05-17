#include "DoubleSpinboxItemDelegate.hpp"
#include <QDoubleSpinBox>
#include <limits>
#include <iostream>

namespace envire { namespace viz {

DoubleSpinboxItemDelegate::DoubleSpinboxItemDelegate(QObject *parent)
  : QStyledItemDelegate(parent), decimals(5)
{
}

QWidget* DoubleSpinboxItemDelegate::createEditor(QWidget *parent,
                                                 const QStyleOptionViewItem &/* option */,
                                                 const QModelIndex &/* index */) const
{
    QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
    editor->setFrame(false);
    editor->setDecimals(decimals);
    //numeric_limits<int> is used instead of double because setMinimum() and
    //setMaximum internally round. The rounding breaks for numeric_limits<double>::min()
    //and just returns 0. Thus we use numeric_limits<int> which should be enough
    //for the use case.
    editor->setMinimum(std::numeric_limits<int>::min());
    editor->setMaximum(std::numeric_limits<int>::max());
    return editor;
}

void DoubleSpinboxItemDelegate::setDecimals(const unsigned int value)
{
  decimals = value;
}

void DoubleSpinboxItemDelegate::setEditorData(QWidget *editor,
                                              const QModelIndex &index) const
{
  const double value = index.model()->data(index, Qt::DisplayRole).toDouble();

  QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
  spinBox->setValue(value);
}

void DoubleSpinboxItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                            const QModelIndex &index) const
{
  QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
  spinBox->interpretText();
  const double value = spinBox->value();

  model->setData(index, value, Qt::EditRole);
}

void DoubleSpinboxItemDelegate::updateEditorGeometry(QWidget *editor,
                                                     const QStyleOptionViewItem &option,
                                                     const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}
  
}}