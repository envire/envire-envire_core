#pragma once
#include <QDialog>
#include "ui_addtransformdialog.h"
#include "TransformModel.hpp"
#include <base/TransformWithCovariance.hpp>

namespace envire { namespace viz {
  
class AddTransformDialog : public QDialog
{
  Q_OBJECT
public:
  AddTransformDialog(QWidget* parent = nullptr);
  QString getFrameId() const;
  const envire::core::Transform& getTransform() const;
  
private slots:
  void frameIdChanged(const QString& newId);
  
private:
  Ui::AddTransformDialog window;
  TransformModel model;
};
}}