rock_library(envire_core
    HEADERS ItemBase.hpp
            Item.hpp
            Frame.hpp
            Transform.hpp
            Environment.hpp
            TransformGraph.hpp
            TransformGraphTypes.hpp
            TransformGraphExceptions.hpp
            TransformGraphVisitors.hpp
            GraphViz.hpp
            SpatialItem.hpp
            BoundingVolume.hpp
            ClassLoader.hpp
            AlignedBoundingBox.hpp
            DummyItem.hpp
            RandomGenerator.hpp
            events/GraphEvent.hpp
            events/GraphEventSubscriber.hpp
            events/GraphEventDispatcher.hpp
            events/GraphEventPublisher.hpp
            events/TransformAddedEvent.hpp
            events/TransformModifiedEvent.hpp
            events/TransformRemovedEvent.hpp
            
    SOURCES ItemBase.cpp
            ClassLoader.cpp
            BoundingVolume.cpp
            AlignedBoundingBox.cpp
            DummyItem.cpp
            events/GraphEventPublisher.cpp
            events/GraphEventDispatcher.cpp
            TransformGraph.cpp            
    DEPS_CMAKE Boost
    DEPS_PKGCONFIG base-types class_loader)
    
