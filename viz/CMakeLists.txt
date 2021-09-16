SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

find_package(Boost COMPONENTS system thread REQUIRED)
 
add_subdirectory(EnvireGraphStructureVisualization)
add_subdirectory(EnvireGraph2DStructureVisualization)
             
    
