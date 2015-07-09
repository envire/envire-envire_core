rock_library(test_envire_plugin
    SOURCES vector_plugin.cpp
    DEPS envire_core
    DEPS_PKGCONFIG class_loader)


rock_testsuite(test_suite suite.cpp
    test_plugins.cpp
    test_tree.cpp
    DEPS envire_core
    DEPS_PKGCONFIG class_loader)

