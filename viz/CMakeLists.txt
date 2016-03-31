SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

find_package(Boost COMPONENTS system REQUIRED)

rock_executable(envire_visualizer 
                SOURCES EnvireVisualizer.cpp
                        EnvireGraphVisualizer.cpp
                        Vizkit3dPluginInformation.cpp
                HEADERS EnvireGraphVisualizer.hpp
                        Vizkit3dPluginInformation.hpp
                        Helpers.hpp
                DEPS_PKGCONFIG vizkit3d
                DEPS_PKGCONFIG envire_core
                DEPS_PKGCONFIG plugin_manager
                DEPS_PKGCONFIG envire_octomap #FIMXE remove later, only for testing
                DEPS_PKGCONFIG envire_pcl
                DEPS_PKGCONFIG pcl_common-1.7
                DEPS_PKGCONFIG pcl_io-1.7
                DEPS_CMAKE Glog
                DEPS_PKGCONFIG base-lib
                LIBS ${Boost_SYSTEM_LIBRARY}) 