rock_library(envire_core
    HEADERS ItemBase.hpp
            Item.hpp
            Frame.hpp
            Transform.hpp
            EnvireGraph.hpp
            TransformTree.hpp
            SpatialItem.hpp
            BoundingVolume.hpp
            ClassLoader.hpp
    SOURCES ItemBase.cpp
            Frame.cpp
            Transform.cpp
            TransformTree.cpp
            ClassLoader.cpp
            BoundingVolume.cpp
    DEPS_PKGCONFIG base-types class_loader)