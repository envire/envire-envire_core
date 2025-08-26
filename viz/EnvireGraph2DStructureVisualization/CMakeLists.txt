
rock_qt_library(
             TARGETPREFIX envire_2D_structure_widget
             QT4_SUFFIX ""
             MOC     EnvireGraph2DStructurWidget.hpp
                     QZoomableGraphicsView.hpp
             HEADERS EnvireGraph2DStructurWidget.hpp
                     QZoomableGraphicsView.hpp
             SOURCES EnvireGraph2DStructurWidget.cpp
                     QZoomableGraphicsView.cpp
             DEPS_PKGCONFIG libgvc
             LIBS_QT4 Qt4::QtSvg
             LIBS_QT5 Qt5::Svg
             DEPS envire_core)

rock_qt_vizkit_widget(
    TARGETPREFIX EnvireGraph2DStructureVisualization
    QT4_SUFFIX ""
    SOURCES EnvireGraph2DStructureVisualizationPlugin.cpp
    HEADERS EnvireGraph2DStructureVisualization.hpp
    MOC     EnvireGraph2DStructureVisualization.hpp
            EnvireGraph2DStructureVisualizationPlugin.hpp
    DEPS_QT envire_2D_structure_widget
)

