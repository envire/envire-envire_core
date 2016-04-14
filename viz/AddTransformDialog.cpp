#include "AddTransformDialog.hpp"
#include "TransformModel.hpp"

namespace envire { namespace viz {

AddTransformDialog::AddTransformDialog(QWidget* parent): QDialog(parent)
{
  window.setupUi(this);
  window.treeView->setModel(&model);
}

QString AddTransformDialog::getFrameId() const
{
  return window.lineEditFrameId->text();
}

const base::TransformWithCovariance& AddTransformDialog::getTransform() const
{
  return model.getTransform();
}

}}