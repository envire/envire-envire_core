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
    
    # The plugin test suit cannot use the rock test macros because it needs to set
    # the library path before executing the test
    
    add_definitions(-DBOOST_TEST_DYN_LINK)
    
    add_executable(test_plugins suite.cpp
        test_plugins.cpp
        PathSingleton.cpp
        test_serialization.cpp)
        
    pkg_check_modules(CLS_LOADER REQUIRED class_loader)
        
    target_link_libraries(test_plugins 
                          envire_core
                          ${Boost_LIBRARIES}
                          ${class_loader_PKGCONFIG_LIBRARIES})
    
    add_test(NAME test-plugins-cxx
             COMMAND "${CMAKE_CURRENT_BINARY_DIR}/test_plugins")
    set_tests_properties(test-plugins-cxx PROPERTIES ENVIRONMENT LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}:$ENV{LD_LIBRARY_PATH})


#copy test files to build/test directory
file(GLOB xml_files "${CMAKE_CURRENT_SOURCE_DIR}/*.xml")
file(GLOB bin_files "${CMAKE_CURRENT_SOURCE_DIR}/*.bin")
file(GLOB asc_files "${CMAKE_CURRENT_SOURCE_DIR}/*.asc")
set(test_files "${xml_files};${bin_files};${asc_files}")
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
    test_graph_event_dispatcher.cpp
    PathSingleton.cpp
    DEPS_PKGCONFIG class_loader
    DEPS 
      envire_core
    DEPS_PLAIN
      Boost_THREAD
      Boost_UNIT_TEST_FRAMEWORK
)

   


