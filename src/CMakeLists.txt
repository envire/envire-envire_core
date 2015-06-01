rock_library(envire-core
    SOURCES Dummy.cpp
    HEADERS Dummy.hpp)

rock_executable(envire-core_bin Main.cpp
    DEPS envire-core)

