SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

find_package(Boost COMPONENTS system REQUIRED)
find_package(Qt4 REQUIRED QtCore QtGui)
 
add_subdirectory(EnvireGraphStructureVisualization)
add_subdirectory(EnvireGraph2DStructureVisualization)
add_subdirectory(EnvireVisualizer)
             
    