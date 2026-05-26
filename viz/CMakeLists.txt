# temporary quick fix: dont build visualisation plugins for qt4
if (${USE_QT5})
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

    find_package(Boost COMPONENTS system thread REQUIRED)
    rock_find_qt5(Core Gui Svg)

    add_subdirectory(EnvireGraphStructureVisualization)
    add_subdirectory(EnvireGraph2DStructureVisualization)
endif (${USE_QT5})


