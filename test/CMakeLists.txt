rock_library(VectorPlugin_envire_plugin
    SOURCES vector_plugin.cpp
    DEPS envire_core
    DEPS_PKGCONFIG class_loader)


rock_testsuite(test_suite suite.cpp
    test_plugins.cpp
    test_transform_tree.cpp
    test_labeled_transform_tree.cpp
    test_boundary.cpp
    test_frame.cpp
    DEPS_CMAKE Boost
    DEPS envire_core
    DEPS_PKGCONFIG class_loader)

rock_testsuite(test_dummy suite.cpp
    test_dummy_graph.cpp
    test_property_tag.cpp)

rock_testsuite(test_parameter_forwarding suite.cpp
    test_dummy_item.cpp
    DEPS_CMAKE Boost
    DEPS envire_core)
