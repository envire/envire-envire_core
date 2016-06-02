rock_library(envire_2D_structure_widget 
             MOC     EnvireGraph2DStructurWidget.hpp
                     QZoomableGraphicsView.hpp
             HEADERS EnvireGraph2DStructurWidget.hpp
                     QZoomableGraphicsView.hpp
             SOURCES EnvireGraph2DStructurWidget.cpp
                     QZoomableGraphicsView.cpp
             DEPS_PKGCONFIG QtCore QtGui QtSvg libgvc
             DEPS envire_core)    


rock_vizkit_widget(EnvireGraph2DStructureVisualization
    SOURCES EnvireGraph2DStructureVisualizationPlugin.cpp
    HEADERS EnvireGraph2DStructureVisualization.hpp
    MOC EnvireGraph2DStructureVisualization.hpp
        EnvireGraph2DStructureVisualizationPlugin.hpp
        
    DEPS envire_2D_structure_widget
#    UI ${UI_FILES}
)