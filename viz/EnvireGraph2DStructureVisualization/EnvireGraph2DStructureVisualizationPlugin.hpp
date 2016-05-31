#ifndef ENVIREGRAPH2DSTRUCTUREVISUALIZATION2PLUGIN_HPP
#define ENVIREGRAPH2DSTRUCTUREVISUALIZATION2PLUGIN_HPP

#include <QtGui>
#include <QtDesigner/QDesignerCustomWidgetInterface>

class EnvireGraph2DStructureVisualizationPlugin : public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    EnvireGraph2DStructureVisualizationPlugin(QObject *parent = 0);
    virtual ~EnvireGraph2DStructureVisualizationPlugin();

    bool isContainer() const;
    bool isInitialized() const;
    QIcon icon() const;
    QString domXml() const;
    QString group() const;
    QString includeFile() const;
    QString name() const;
    QString toolTip() const;
    QString whatsThis() const;
    QWidget* createWidget(QWidget *parent);
    void initialize(QDesignerFormEditorInterface *core);

private:
    bool initialized; 
};

#endif /* ENVIREGRAPH2DSTRUCTUREVISUALIZATION2PLUGIN_HPP */  
