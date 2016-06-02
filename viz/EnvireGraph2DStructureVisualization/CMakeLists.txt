rock_vizkit_widget(EnvireGraph2DStructureVisualization
    SOURCES EnvireGraph2DStructureVisualization.cpp
            EnvireGraph2DStructureVisualizationPlugin.cpp
            QZoomableGraphicsView.cpp
    HEADERS EnvireGraph2DStructureVisualization.hpp
            QZoomableGraphicsView.hpp
    MOC EnvireGraph2DStructureVisualization.hpp
        EnvireGraph2DStructureVisualizationPlugin.hpp
        QZoomableGraphicsView.hpp
    DEPS_PKGCONFIG QtCore QtGui libgvc
    DEPS envire_core
#    UI ${UI_FILES}
)