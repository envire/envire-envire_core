rock_library(envire_core
    HEADERS ItemBase.hpp
            Item.hpp
            Node.hpp
            Edge.hpp
            Tree.hpp
    SOURCES ItemBase.cpp
            Node.cpp
            Edge.cpp
            Tree.cpp
    DEPS_PKGCONFIG base-types class_loader)

