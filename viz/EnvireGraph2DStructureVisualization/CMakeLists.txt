rock_vizkit_widget(EnvireGraph2DStructureVisualization
    SOURCES EnvireGraph2DStructureVisualization.cpp EnvireGraph2DStructureVisualizationPlugin.cpp
    HEADERS EnvireGraph2DStructureVisualization.hpp
    MOC EnvireGraph2DStructureVisualization.hpp EnvireGraph2DStructureVisualizationPlugin.hpp
    DEPS_PKGCONFIG QtCore QtGui
    DEPS envire_core
#    UI ${UI_FILES}
)