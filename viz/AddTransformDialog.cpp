#include "AddTransformDialog.hpp"
#include "TransformModel.hpp"
#include <QPushButton>

namespace envire { namespace viz {

AddTransformDialog::AddTransformDialog(QWidget* parent): QDialog(parent)
{
  window.setupUi(this);
  window.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
  window.treeView->setModel(&model);
  window.treeView->expandAll();
  
  connect(window.lineEditFrameId, SIGNAL(textChanged(const QString&)), this,
          SLOT(frameIdChanged(const QString&)));
}

QString AddTransformDialog::getFrameId() const
{
  return window.lineEditFrameId->text();
}

const envire::core::Transform& AddTransformDialog::getTransform() const
{
  return model.getTransform();
}

void AddTransformDialog::frameIdChanged(const QString& newId)
{
  if(newId.isEmpty())
  {
    window.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
  }
  else
  {
    window.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
  }
}

}}