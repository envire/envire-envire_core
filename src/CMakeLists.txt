find_package(Boost COMPONENTS serialization filesystem system)


set(headers items/ItemBase.hpp
            items/Item.hpp
            items/Frame.hpp
            items/Transform.hpp
            items/Environment.hpp
            items/AlignedBoundingBox.hpp
            items/RandomGenerator.hpp
            items/SpatialItem.hpp
            items/BoundingVolume.hpp
            items/ItemMetadata.hpp
            graph/GraphExceptions.hpp
            graph/GraphVisitors.hpp
            graph/GraphViz.hpp
            graph/TreeView.hpp
            graph/GraphTypes.hpp
            graph/Graph.hpp
            graph/TransformGraph.hpp
            graph/EnvireGraph.hpp
            events/GraphEvent.hpp
            events/GraphEventSubscriber.hpp
            events/GraphEventDispatcher.hpp
            events/GraphEventPublisher.hpp
            events/GraphEventQueue.hpp
            events/EdgeEvents.hpp
            events/ItemAddedEvent.hpp
            events/ItemRemovedEvent.hpp
            events/FrameEvents.hpp
            events/GraphItemEventDispatcher.hpp
            events/GraphEventExceptions.hpp
            serialization/Serialization.hpp
            serialization/SerializationHandle.hpp
            serialization/SerializationRegistration.hpp
            serialization/ItemHeader.hpp
            serialization/BinaryBufferHelper.hpp
            serialization/SerializableConcept.hpp
            util/Demangle.hpp
            util/Exceptions.hpp)

            
set(sources items/ItemBase.cpp
            items/AlignedBoundingBox.cpp
            items/ItemMetadata.cpp
            events/GraphEvent.cpp
            events/GraphEventPublisher.cpp
            events/GraphEventDispatcher.cpp
            events/GraphEventSubscriber.cpp
            events/GraphEventQueue.cpp
            graph/EnvireGraph.cpp
            graph/TreeView.cpp
            serialization/Serialization.cpp
            util/Demangle.cpp)
            
set(deps_pkg_config base-types)

if(ENABLE_PLUGINS)

  set(headers ${headers}
              plugin/ClassLoader.hpp
              plugin/Plugin.hpp)
  set(sources ${sources}
              plugin/ClassLoader.cpp)
  set(deps_pkg_config ${deps_pkg_config} class_loader plugin_manager)
  
endif()

             

rock_library(envire_core
    HEADERS ${headers}
    SOURCES ${sources}
    DEPS_CMAKE Boost Glog
    DEPS_PKGCONFIG ${deps_pkg_config})


install(FILES
    all
    DESTINATION include/envire_core)
