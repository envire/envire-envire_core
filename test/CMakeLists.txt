find_package(Boost COMPONENTS unit_test_framework system filesystem serialization thread)

if(ENABLE_PLUGINS)
    rock_library(envire_vector_plugin
        SOURCES vector_plugin.cpp
        DEPS envire_core
        DEPS_PKGCONFIG class_loader)

    rock_library(envire_string_plugin
        SOURCES string_plugin.cpp
        DEPS envire_core
        DEPS_PKGCONFIG class_loader)
    
    rock_testsuite(test_plugins suite.cpp
        test_plugins.cpp
        PathSingleton.cpp
        test_serialization.cpp
        DEPS envire_core
        DEPS_PKGCONFIG class_loader boost_serialization
        DEPS_PLAIN
        Boost_THREAD
        Boost_UNIT_TEST_FRAMEWORK
)

#copy test files to build/test directory
file(GLOB xml_files "${CMAKE_CURRENT_SOURCE_DIR}/*.xml")
file(GLOB bin_files "${CMAKE_CURRENT_SOURCE_DIR}/*.bin")
set(test_files "${xml_files};${bin_files}")
add_custom_command(TARGET test_plugins PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                       ${test_files} $<TARGET_FILE_DIR:test_plugins>)

# install(FILES vector_plugin.xml DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    
endif()

rock_testsuite(test_suite suite.cpp
    test_transform_graph.cpp
    test_boundary.cpp
    test_graph.cpp
    test_transform.cpp
    test_envire_graph.cpp
    test_filter.cpp
    test_item_changed_callback.cpp
    PathSingleton.cpp
    DEPS_PKGCONFIG class_loader
    DEPS 
      envire_core
    DEPS_PLAIN
      Boost_THREAD
      Boost_UNIT_TEST_FRAMEWORK
)

   


