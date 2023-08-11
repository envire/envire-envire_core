SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

find_package(Boost COMPONENTS system thread REQUIRED)
rock_find_qt4(OPTIONAL QtSvg)
rock_find_qt5(OPTIONAL Svg)

add_subdirectory(EnvireGraphStructureVisualization)
add_subdirectory(EnvireGraph2DStructureVisualization)
             
    
