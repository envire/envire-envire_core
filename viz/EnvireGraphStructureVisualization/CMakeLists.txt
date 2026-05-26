
rock_qt_vizkit_plugin(
    TARGETPREFIX envire_core-viz
    QT4_SUFFIX ""
    MISSINGQTDEPS_NOBUILD
    SOURCES EnvireGraphStructureVisualization.cpp
    MOC EnvireGraphStructureVisualization.hpp
    HEADERS EnvireGraphStructureVisualization.hpp
    DEPS envire_core)

