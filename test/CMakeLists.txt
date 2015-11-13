find_package(Boost COMPONENTS unit_test_framework system filesystem serialization)

if(ENABLE_PLUGINS)
    rock_library(VectorPlugin_envire_plugin
        SOURCES vector_plugin.cpp
        DEPS envire_core
        DEPS_PKGCONFIG class_loader)
    
    rock_testsuite(test_plugins suite.cpp
        test_plugins.cpp
        test_serialization.cpp
        DEPS_CMAKE Boost
        DEPS envire_core
        DEPS_PKGCONFIG class_loader)
    
endif()

rock_testsuite(test_suite suite.cpp
    test_transform_graph.cpp
    test_boundary.cpp
    test_frame.cpp
    DEPS_CMAKE Boost
    DEPS envire_core)
find_package(Boost COMPONENTS unit_test_framework system filesystem thread)
target_link_libraries(test_suite ${Boost_LIBRARIES})

#rock_testsuite(timing suite.cpp
#    graph_performance_test.cpp
#    DEPS_CMAKE Boost
#    DEPS envire_core)
#target_link_libraries(timing ${Boost_LIBRARIES})

rock_testsuite(test_dummy suite.cpp
    test_dummy_graph.cpp
    test_property_tag.cpp)

rock_executable(dfs_test dfs_test.cpp
    DEPS_CMAKE Boost
    DEPS envire_core)

rock_executable(bfs_test bfs_test.cpp
    DEPS_CMAKE Boost
    DEPS envire_core)

rock_executable(dijkstra_color_test dijkstra_color_test.cpp
    DEPS_CMAKE Boost
    DEPS envire_core)
    


#only enable the performance comparision when the old envire is installed
pkg_check_modules(OLD_ENVIRE envire)
if(OLD_ENVIRE_FOUND)
    rock_executable(performance_comapare compare_old_new_performance.cpp
        DEPS_CMAKE Boost
        DEPS envire_core
        DEPS_PKGCONFIG envire)
else()
    message(STATUS "envire not found ... NOT building the performance comparision test")
endif()
rock_executable(dijkstra_test dijkstra_test.cpp
    DEPS_CMAKE Boost
    DEPS envire_core)
	
	
	
