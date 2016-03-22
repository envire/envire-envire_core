#include "Vizkit3dPluginInformation.hpp"
#include "Helpers.hpp"

namespace envire { namespace viz
{
Vizkit3dPluginInformation::Vizkit3dPluginInformation(vizkit3d::Vizkit3DWidget* mainWidget) :
    mainWidget(mainWidget)
{
  ASSERT_NOT_NULL(mainWidget);
  loadData();
}

void Vizkit3dPluginInformation::loadData()
{
  
  /* über alle plugins laufen
   * jeweils gucken welche updateData methoden es gibt und welche typen die haben
   * das alles in ner map aufschreiben
   */
  
  QStringList* availablePlugins = mainWidget->getAvailablePlugins();
  
  Qt::ConnectionType conType = Helpers::determineConnectionType(mainWidget);
  for(const QString& libName : *availablePlugins)
  {
    QObject* plugin = nullptr;
    QMetaObject::invokeMethod(mainWidget, "loadPlugin", conType,
                              Q_RETURN_ARG(QObject*, plugin),
                              Q_ARG(QString, libName), Q_ARG(QString, ""));
    if(nullptr != plugin)
    {
      loadPluginData(plugin, libName);
      QMetaObject::invokeMethod(mainWidget, "removePlugin", conType,
                                Q_ARG(QObject*, plugin));   
    }
  }
}

void Vizkit3dPluginInformation::loadPluginData(const QObject* plugin,
                                               const QString& libName)
{
  const QMetaObject* metaObj = plugin->metaObject();
  ASSERT_NOT_NULL(plugin);
  
  for(int i = 0; i < metaObj->methodCount(); ++i)
  {
    const QMetaMethod method = metaObj->method(i);
    QString signature = QString::fromAscii(method.signature());
    QStringList elements = signature.split("(");     
    const QString methodName = elements[0];
    if(methodName.contains("updateData"))
    {
      const QString typeName = method.typeName();
      QList<QByteArray> paramTypes = method.parameterTypes();
      if(paramTypes.size() == 1 && typeName.isEmpty()) //isEmpty means "void"
      { 
        typeToPlugin.insert(QString(paramTypes.at(0)), {method.signature(), libName});
        std::cout << "found method: " << method.signature() << 
                     " for type: " << QString(paramTypes[0]).toStdString() << std::endl;
        
      }
    }
  }
}

const QMultiHash<QString, Vizkit3dPluginInformation::UpdateMethod>&
Vizkit3dPluginInformation::getTypeToUpdateMethodMapping() const
{
  return typeToPlugin;
}
  
}}