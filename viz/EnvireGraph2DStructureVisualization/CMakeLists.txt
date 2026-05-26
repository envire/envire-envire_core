rock_library(envire_2D_structure_widget 
             MOC     EnvireGraph2DStructurWidget.hpp
                     QZoomableGraphicsView.hpp
             HEADERS EnvireGraph2DStructurWidget.hpp
                     QZoomableGraphicsView.hpp
             SOURCES EnvireGraph2DStructurWidget.cpp
                     QZoomableGraphicsView.cpp
             DEPS_PKGCONFIG libgvc
             LIBS Qt5::Core Qt5::Gui Qt5::Svg
             DEPS envire_core)    


rock_vizkit_widget(EnvireGraph2DStructureVisualization
    SOURCES EnvireGraph2DStructureVisualizationPlugin.cpp
    HEADERS EnvireGraph2DStructureVisualization.hpp
    MOC     EnvireGraph2DStructureVisualization.hpp
            EnvireGraph2DStructureVisualizationPlugin.hpp
    LIBS Qt5::Core Qt5::Gui
    DEPS    envire_2D_structure_widget
)
