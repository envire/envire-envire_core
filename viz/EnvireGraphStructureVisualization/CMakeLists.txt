rock_vizkit_plugin(envire_core-viz
    EnvireGraphStructureVisualization.cpp
    MOC EnvireGraphStructureVisualization.hpp
    HEADERS EnvireGraphStructureVisualization.hpp
    DEPS envire_core
    LIBS Qt5::Core Qt5::Gui Qt5::Widgets)