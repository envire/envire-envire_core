rock_library(envire_core
    HEADERS items/ItemBase.hpp
            items/Item.hpp
            items/Frame.hpp
            items/Transform.hpp
            items/Environment.hpp
            items/AlignedBoundingBox.hpp
            items/DummyItem.hpp
            items/RandomGenerator.hpp
            items/SpatialItem.hpp
            items/BoundingVolume.hpp
            graph/TransformGraph.hpp
            graph/TransformGraphTypes.hpp
            graph/TransformGraphExceptions.hpp
            graph/TransformGraphVisitors.hpp
            graph/GraphViz.hpp
            plugin/ClassLoader.hpp
            events/GraphEvent.hpp
            events/GraphEventSubscriber.hpp
            events/GraphEventDispatcher.hpp
            events/GraphEventPublisher.hpp
            events/TransformAddedEvent.hpp
            events/TransformModifiedEvent.hpp
            events/TransformRemovedEvent.hpp
            events/ItemAddedEvent.hpp
            events/ItemRemovedEvent.hpp

    SOURCES items/ItemBase.cpp
            items/BoundingVolume.cpp
            items/AlignedBoundingBox.cpp
            items/DummyItem.cpp
            plugin/ClassLoader.cpp
            events/GraphEventPublisher.cpp
            events/GraphEventDispatcher.cpp
            graph/TransformGraph.cpp            
    DEPS_CMAKE Boost
    DEPS_PKGCONFIG base-types class_loader)


install(FILES
    all
    DESTINATION include/envire_core)

