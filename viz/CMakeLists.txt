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
                        EnvireGraphVisualizer.cpp
                        Vizkit3dPluginInformation.cpp
                HEADERS EnvireGraphVisualizer.hpp
                        EnvireGraphStructureVisualization.hpp
                        Vizkit3dPluginInformation.hpp
                DEPS_PKGCONFIG vizkit3d
                DEPS envire_core
                DEPS envire_core-viz
                DEPS_PKGCONFIG plugin_manager
                DEPS_PKGCONFIG envire_octomap #FIMXE remove later, only for testing
                DEPS_PKGCONFIG envire_pcl
                DEPS_PKGCONFIG pcl_common-1.7
                DEPS_PKGCONFIG pcl_io-1.7
                DEPS_CMAKE Glog
                LIBS ${Boost_SYSTEM_LIBRARY})