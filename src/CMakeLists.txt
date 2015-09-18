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
            events/FrameEvent.hpp
            events/FrameAddedEvent.hpp
            events/FrameRootAddedEvent.hpp
            events/FrameEventSubscriber.hpp
            events/FrameEventDispatcher.hpp
            events/FrameEventPublisher.hpp
    SOURCES ItemBase.cpp
            ClassLoader.cpp
            BoundingVolume.cpp
            AlignedBoundingBox.cpp
            DummyItem.cpp
            events/FrameEventPublisher.cpp
            events/FrameEventDispatcher.cpp            
    DEPS_CMAKE Boost
    DEPS_PKGCONFIG base-types class_loader)
