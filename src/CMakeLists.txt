rock_library(envire_core
    HEADERS ItemBase.hpp
            Item.hpp
            Frame.hpp
            Transform.hpp
            Environment.hpp
            TransformTree.hpp
            TransformTreeTypes.hpp
            LabeledTransformTree.hpp
            GraphViz.hpp
            SpatialItem.hpp
            BoundingVolume.hpp
            ClassLoader.hpp
            AlignedBoundingBox.hpp
            DummyItem.hpp
            RandomGenerator.hpp
            events/TreeEvent.hpp
            events/FrameAddedEvent.hpp
            events/FrameRootAddedEvent.hpp
            events/TreeEventSubscriber.hpp
            events/TreeEventDispatcher.hpp
            events/TreeEventPublisher.hpp
    SOURCES ItemBase.cpp
            ClassLoader.cpp
            BoundingVolume.cpp
            AlignedBoundingBox.cpp
            DummyItem.cpp
            events/TreeEventPublisher.cpp
            events/TreeEventDispatcher.cpp
            TransformTree.cpp            
    DEPS_CMAKE Boost
    DEPS_PKGCONFIG base-types class_loader)
    
    
SET_TARGET_PROPERTIES(envire_core PROPERTIES
                      COMPILE_FLAGS "-fprofile-arcs -ftest-coverage"
                      LINK_FLAGS "-fprofile-arcs -ftest-coverage -lgcov")
