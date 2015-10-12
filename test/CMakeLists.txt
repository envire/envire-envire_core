find_package(Boost COMPONENTS unit_test_framework system filesystem serialization)

rock_library(VectorPlugin_envire_plugin
    SOURCES vector_plugin.cpp
    DEPS envire_core
    DEPS_PKGCONFIG class_loader)

rock_testsuite(test_suite suite.cpp
    test_plugins.cpp
    test_transform_graph.cpp
    test_boundary.cpp
    test_frame.cpp
    test_serialization.cpp
    DEPS_CMAKE Boost
    DEPS envire_core
    DEPS_PKGCONFIG class_loader)
find_package(Boost COMPONENTS unit_test_framework system filesystem)
target_link_libraries(test_suite ${Boost_LIBRARIES})

rock_testsuite(timing suite.cpp
    graph_performance_test.cpp
    DEPS_CMAKE Boost
    DEPS envire_core)
target_link_libraries(timing ${Boost_LIBRARIES})

rock_testsuite(test_dummy suite.cpp
    test_dummy_graph.cpp
    test_property_tag.cpp)

rock_testsuite(test_parameter_forwarding suite.cpp
    test_dummy_item.cpp
    DEPS_CMAKE Boost
    DEPS envire_core)

rock_executable(dfs_test dfs_test.cpp
    DEPS_CMAKE Boost
	DEPS envire_core)

rock_executable(bfs_test bfs_test.cpp
    DEPS_CMAKE Boost
	DEPS envire_core)

rock_executable(dijkstra_color_test dijkstra_color_test.cpp
    DEPS_CMAKE Boost
	DEPS envire_core)

rock_executable(dijkstra_test dijkstra_test.cpp
    DEPS_CMAKE Boost
	DEPS envire_core)
	
	
	
