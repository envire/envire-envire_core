SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
rock_vizkit_plugin(envire_core-viz
    EnvireGraphStructureVisualization.cpp
    DEPS envire_core
    MOC EnvireGraphStructureVisualization.hpp
    HEADERS EnvireGraphStructureVisualization.hpp)

            
 # rock_executable(name
#     SOURCES source.cpp source1.cpp ...
#     [DEPS target1 target2 target3]
#     [DEPS_PKGCONFIG pkg1 pkg2 pkg3]
#     [DEPS_CMAKE pkg1 pkg2 pkg3]
#     [MOC qtsource1.hpp qtsource2.hpp])
#     [UI qt_window.ui qt_widget.ui]
rock_executable(envire_visualizer 
                SOURCES EnvireVisualizer.cpp
                DEPS_PKGCONFIG vizkit3d
                DEPS envire_core
                DEPS envire_core-viz
                LIBS ${Boost_SYSTEM_LIBRARY})