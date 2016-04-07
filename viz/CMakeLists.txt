SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

find_package(Boost COMPONENTS system REQUIRED)
find_package(Qt4 REQUIRED)
 
qt4_wrap_ui(SOURCES_UI_H mainwindow.ui)
include_directories(${CMAKE_CURRENT_BINARY_DIR}) #to be able to include ui_mainwindow.h

rock_library(envire_visualizer
             MOC     MainWindow.hpp
                     EnvireGraphVisualizer.hpp
             HEADERS Vizkit3dPluginInformation.hpp
                     Helpers.hpp
                     ${SOURCES_UI_H}
             SOURCES EnvireGraphVisualizer.cpp
                     Vizkit3dPluginInformation.cpp
                     ${SOURCES_MOC}
                     mainwindow.ui #to create a dependency and regen if the ui changes
                     MainWindow.cpp
             DEPS_PKGCONFIG vizkit3d
             DEPS_PKGCONFIG envire_core
             DEPS_PKGCONFIG base-lib)

rock_executable(envire_visualizer_bin
                SOURCES EnvireVisualizer.cpp

                DEPS_PKGCONFIG vizkit3d
                DEPS_PKGCONFIG envire_core
                DEPS_PKGCONFIG plugin_manager
                DEPS_PKGCONFIG envire_octomap
                DEPS_PKGCONFIG envire_pcl
                DEPS_PKGCONFIG pcl_common-1.7
                DEPS_PKGCONFIG pcl_io-1.7
                DEPS_CMAKE Glog
                DEPS_PKGCONFIG base-lib
                DEPS envire_visualizer
                LIBS ${Boost_SYSTEM_LIBRARY}) 