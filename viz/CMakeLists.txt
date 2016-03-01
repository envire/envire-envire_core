SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
rock_vizkit_plugin(envire_core-viz
    EnvireGraphStructureVisualization.cpp
    DEPS envire_core
    MOC EnvireGraphStructureVisualization.hpp
    HEADERS EnvireGraphStructureVisualization.hpp
            TransformGraphStructureVisualizer.hpp)

