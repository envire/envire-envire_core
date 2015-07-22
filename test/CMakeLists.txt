rock_library(VectorPlugin_envire_plugin
    SOURCES vector_plugin.cpp
    DEPS envire_core
    DEPS_PKGCONFIG class_loader)


rock_testsuite(test_suite suite.cpp
    test_plugins.cpp
    test_labeled_tree.cpp
    test_boundary.cpp
    DEPS envire_core
    DEPS_PKGCONFIG class_loader)

rock_testsuite(test_dummy suite.cpp
    test_dummy_graph.cpp
    test_property_tag.cpp)


