//
// Copyright (c) 2015, Deutsches Forschungszentrum für Künstliche Intelligenz GmbH.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

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