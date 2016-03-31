#pragma once
#include <vizkit3d/Vizkit3DWidget.hpp>
#include <QMultiHash>

namespace envire { namespace viz
{

/**Contains information about updateData methods of all available Vizkit3D plugins*/
class Vizkit3dPluginInformation
{
public:
  
  /**Contains everything needed to invoke a certain update method */
  struct UpdateMethod
  {
    QMetaMethod method; //meta infos about the method
    QString libName; //name of the library (give to loadPlugin() to get an instance)
  };
  
  Vizkit3dPluginInformation(vizkit3d::Vizkit3DWidget* mainWidget);
  
  using TypeToUpdateMapping = QMultiHash<QString, UpdateMethod>;
  const TypeToUpdateMapping& getTypeToUpdateMethodMapping() const;
  
private:
  
  /**Populate ::typeToPlugin by loading all plugins and parsing their meta data */
  void loadData();
  
  /** Add meta data of a specific plugin to ::typeToPlugin
   * @param libName Name of the library that @p plugin comes from*/
  void loadPluginData(const QObject* plugin, const QString& libName);
  
  /**Widget that is used to load the plugins*/
  vizkit3d::Vizkit3DWidget* mainWidget;
  /**Contains a mapping from parameter type name to update method.
   * @note there might be more than one updateMethod per type.
   *       */
  TypeToUpdateMapping typeToPlugin;
};

}}