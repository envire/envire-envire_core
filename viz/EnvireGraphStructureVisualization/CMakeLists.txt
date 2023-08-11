
if(ROCK_QT_VERSION_4)
    rock_vizkit_plugin(envire_core-viz
        EnvireGraphStructureVisualization.cpp
        MOC EnvireGraphStructureVisualization.hpp
        HEADERS EnvireGraphStructureVisualization.hpp
        DEPS envire_core
        LIBS Qt4::QtCore Qt4::QtGui)
endif(ROCK_QT_VERSION_4)

if(ROCK_QT_VERSION_5)
    rock_vizkit_plugin_qt5(envire_core-viz-qt5
        EnvireGraphStructureVisualization.cpp
        MOC5 EnvireGraphStructureVisualization.hpp
        HEADERS EnvireGraphStructureVisualization.hpp
        DEPS envire_core
        LIBS Qt5::Core Qt5::Gui Qt5::Widgets)
endif(ROCK_QT_VERSION_5)
