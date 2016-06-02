#include "EnvireGraph2DStructureVisualizationPlugin.hpp"
#include "EnvireGraph2DStructureVisualization.hpp"

Q_EXPORT_PLUGIN2(EnvireGraph2DStructureVisualization, EnvireGraph2DStructureVisualizationPlugin)

EnvireGraph2DStructureVisualizationPlugin::EnvireGraph2DStructureVisualizationPlugin(QObject *parent)
    : QObject(parent)
{
    initialized = false;
}

EnvireGraph2DStructureVisualizationPlugin::~EnvireGraph2DStructureVisualizationPlugin()
{
}

bool EnvireGraph2DStructureVisualizationPlugin::isContainer() const
{
    return false;
}

bool EnvireGraph2DStructureVisualizationPlugin::isInitialized() const
{
    return initialized;
}

QIcon EnvireGraph2DStructureVisualizationPlugin::icon() const
{
    return QIcon("");
}

QString EnvireGraph2DStructureVisualizationPlugin::domXml() const
{
        return "<ui language=\"c++\">\n"
            " <widget class=\"EnvireGraph2DStructureVisualization\" name=\"enviregraph2dstructurevisualization2\">\n"
            "  <property name=\"geometry\">\n"
            "   <rect>\n"
            "    <x>0</x>\n"
            "    <y>0</y>\n"
            "     <width>300</width>\n"
            "     <height>120</height>\n"
            "   </rect>\n"
            "  </property>\n"
//            "  <property name=\"toolTip\" >\n"
//            "   <string>EnvireGraph2DStructureVisualization</string>\n"
//            "  </property>\n"
//            "  <property name=\"whatsThis\" >\n"
//            "   <string>EnvireGraph2DStructureVisualization</string>\n"
//            "  </property>\n"
            " </widget>\n"
            "</ui>\n";
}

QString EnvireGraph2DStructureVisualizationPlugin::group() const {
    return "Rock-Robotics";
}

QString EnvireGraph2DStructureVisualizationPlugin::includeFile() const {
    return "envire_core/EnvireGraph2DStructureVisualization.hpp";
}

QString EnvireGraph2DStructureVisualizationPlugin::name() const {
    return "EnvireGraph2DStructureVisualization";
}

QString EnvireGraph2DStructureVisualizationPlugin::toolTip() const {
    return whatsThis();
}

QString EnvireGraph2DStructureVisualizationPlugin::whatsThis() const
{
    return "";
}

QWidget* EnvireGraph2DStructureVisualizationPlugin::createWidget(QWidget *parent)
{
    return new EnvireGraph2DStructureVisualization(parent);
}

void EnvireGraph2DStructureVisualizationPlugin::initialize(QDesignerFormEditorInterface *core)
{
     if (initialized)
         return;
     initialized = true;
}