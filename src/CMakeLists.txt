find_package(Boost COMPONENTS serialization)


set(headers items/ItemBase.hpp
            items/Item.hpp
            items/Frame.hpp
            items/Transform.hpp
            items/Environment.hpp
            items/AlignedBoundingBox.hpp
            items/RandomGenerator.hpp
            items/SpatialItem.hpp
            items/BoundingVolume.hpp
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
            events/EdgeAddedEvent.hpp
            events/EdgeModifiedEvent.hpp
            events/EdgeRemovedEvent.hpp
            events/ItemAddedEvent.hpp
            events/ItemRemovedEvent.hpp
            events/FrameAddedEvent.hpp
            events/FrameRemovedEvent.hpp
            events/GraphItemEventDispatcher.hpp
            serialization/Serialization.hpp
            serialization/SerializationHandle.hpp
            serialization/SerializationRegistration.hpp
            serialization/ItemHeader.hpp
            serialization/BinaryBufferHelper.hpp
            util/Demangle.hpp)

            
set(sources items/ItemBase.cpp
            items/AlignedBoundingBox.cpp
            events/GraphEventPublisher.cpp
            events/GraphEventDispatcher.cpp
            events/GraphEventSubscriber.cpp
            graph/EnvireGraph.cpp
            serialization/Serialization.cpp)
            
set(deps_pkg_config base-types boost_serialization)

if(ENABLE_PLUGINS)

  set(headers ${headers}
              plugin/ClassLoader.hpp
              plugin/Plugin.hpp)          
  set(sources ${sources}
              plugin/ClassLoader.cpp)
  set(deps_pkg_config ${deps_pkg_config} class_loader)
  
endif()

             

rock_library(envire_core
    HEADERS ${headers}
    SOURCES ${sources}
    DEPS_CMAKE Boost
    DEPS_PKGCONFIG ${deps_pkg_config})


install(FILES
    all
    DESTINATION include/envire_core)
