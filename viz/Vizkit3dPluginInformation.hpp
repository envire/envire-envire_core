#pragma once
#include <vizkit3d/Vizkit3DWidget.hpp>
#include <QMultiHash>

namespace envire { namespace viz
{

/**Contains information about available vizkir3d plugins and what datatypes they support */
class Vizkit3dPluginInformation
{
public:
  
  /**Contains everything needed to invoke a certain update method */
  struct UpdateMethod
  {
    QString methodSignature;
    QString libName; //name of the library (give to loadPlugin() to get an instance)
  };
  
  Vizkit3dPluginInformation(vizkit3d::Vizkit3DWidget* mainWidget);
  using TypeToUpdateMapping = QMultiHash<QString, UpdateMethod>;
  const TypeToUpdateMapping& getTypeToUpdateMethodMapping() const;
  
private:
  //FIXME comments
  void loadData();
  
  /**
   * @param libName Name of the library that @p plugin comes from*/
  void loadPluginData(const QObject* plugin, const QString& libName);
  
  
  
  vizkit3d::Vizkit3DWidget* mainWidget;
  
  /**Contains a mapping from parameter type name to update method.
   * @note there might be more than one updateMethod per type.
   *       */
  TypeToUpdateMapping typeToPlugin;
  
};

}}