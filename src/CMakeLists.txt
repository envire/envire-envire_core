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
            graph/TransformGraph.hpp
            graph/TransformGraphTypes.hpp
            graph/TransformGraphExceptions.hpp
            graph/TransformGraphVisitors.hpp
            graph/GraphViz.hpp
            graph/TreeView.hpp
            events/GraphEvent.hpp
            events/GraphEventSubscriber.hpp
            events/GraphEventDispatcher.hpp
            events/GraphEventPublisher.hpp
            events/TransformAddedEvent.hpp
            events/TransformModifiedEvent.hpp
            events/TransformRemovedEvent.hpp
            events/ItemAddedEvent.hpp
            events/ItemRemovedEvent.hpp
            events/FrameAddedEvent.hpp
            events/FrameRemovedEvent.hpp
            events/GraphItemEventDispatcher.hpp
            serialization/Serialization.hpp
            serialization/SerializationHandle.hpp
            serialization/SerializationRegistration.hpp
            serialization/ItemHeader.hpp
            serialization/EigenTypes.hpp
            serialization/BoostTypes.hpp
            serialization/BaseTypes.hpp
            serialization/BinaryBufferHelper.hpp
            util/MetaProgramming.hpp)
            
set(sources items/ItemBase.cpp
            items/AlignedBoundingBox.cpp
            events/GraphEventPublisher.cpp
            events/GraphEventDispatcher.cpp
            events/GraphEventSubscriber.cpp
            graph/TransformGraph.cpp
            serialization/Serialization.cpp)
            
set(deps_pkg_config base-types)

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
