rock_find_qt4(OPTIONAL Svg)
rock_find_qt5(OPTIONAL Svg)

if(ROCK_QT_VERSION_4)
rock_library(envire_2D_structure_widget
             MOC     EnvireGraph2DStructurWidget.hpp
                     QZoomableGraphicsView.hpp
             HEADERS EnvireGraph2DStructurWidget.hpp
                     QZoomableGraphicsView.hpp
             SOURCES EnvireGraph2DStructurWidget.cpp
                     QZoomableGraphicsView.cpp
             DEPS_PKGCONFIG libgvc
             LIBS Qt4::QtSvg
             DEPS envire_core)

rock_vizkit_widget(EnvireGraph2DStructureVisualization
    SOURCES EnvireGraph2DStructureVisualizationPlugin.cpp
    HEADERS EnvireGraph2DStructureVisualization.hpp
    MOC     EnvireGraph2DStructureVisualization.hpp
            EnvireGraph2DStructureVisualizationPlugin.hpp
    DEPS    envire_2D_structure_widget
)
endif()

if(ROCK_QT_VERSION_5)
rock_library(envire_2D_structure_widget-qt5
             MOC5     EnvireGraph2DStructurWidget.hpp
                     QZoomableGraphicsView.hpp
             HEADERS EnvireGraph2DStructurWidget.hpp
                     QZoomableGraphicsView.hpp
             SOURCES EnvireGraph2DStructurWidget.cpp
                     QZoomableGraphicsView.cpp
             DEPS_PKGCONFIG libgvc
             LIBS Qt5::Svg
             DEPS envire_core)    

rock_vizkit_widget(EnvireGraph2DStructureVisualization-qt5
    SOURCES EnvireGraph2DStructureVisualizationPlugin.cpp
    HEADERS EnvireGraph2DStructureVisualization.hpp
    MOC5    EnvireGraph2DStructureVisualization.hpp
            EnvireGraph2DStructureVisualizationPlugin.hpp
    DEPS    envire_2D_structure_widget-qt5
)
endif()
