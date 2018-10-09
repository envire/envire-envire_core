Build and test status: [![CircleCI](https://circleci.com/gh/envire/envire-envire_core.svg?style=svg)](https://circleci.com/gh/envire/envire-envire_core)


EnviRe Core
=============
Core library for Environment Representation. The goal of this core part is
to deal with the representation commonalities among plugins.

Environment Representation (EnviRe) technologies are meant to close the gap and
provide techniques to store, operate and interchange information within a
robotic system. The application of EnviRe mainly focus to support navigation,
simulation and operations and simplify the interchange of algorithms among software components.

Installation
------------
The easiest way to build and install this package is to use Rock's build system.
See [this page](http://rock-robotics.org/documentation/installation.html)
on how to install Rock.

However, if you feel that it's too heavy for your needs, Rock aims at having
most of its "library" packages (such as this one) to follow best practices. See
[this page](http://rock-robotics.org/documentation/packages/outside_of_rock.html)
for installation instructions outside of Rock.

Rock CMake Macros
-----------------

This package uses a set of CMake helper shipped as the Rock CMake macros.
Documentations is available on [this page](http://rock-robotics.org/documentation/packages/cmake_macros.html).

Rock Standard Layout
--------------------

This directory structure follows some simple rules, to allow for generic build
processes and simplify reuse of this project. Following these rules ensures that
the Rock CMake macros automatically handle the project's build process and
install setup properly.

STRUCTURE
-- src/
	Contains all header (*.h/*.hpp) and source files
-- build/
	The target directory for the build process, temporary content
-- bindings/
	Language bindings for this package, e.g. put into subfolders such as
   |-- ruby/
        Ruby language bindings
-- viz/
        Source files for a vizkit plugin / widget related to this library
-- resources/
	General resources such as images that are needed by the program
-- configuration/
	Configuration files for running the program
-- external/
	When including software that needs a non standard installation process, or one that can be
	easily embedded include the external software directly here
-- doc/
	should contain the existing doxygen file: doxygen.conf

Graph Usage Examples
--------------------
This section contains a few simple usage examples that showcase some of the graph's features.

#### Adding Frames
Frames can be added either explicitly by calling ``addFrame()``
```
EnvireGraph g;
const FrameId frame = "frame_a";
g.addFrame(frame);
```
or implicitly by using a unknown frame id in ``addTransform()``.
```
EnvireGraph g;
const FrameId frameA = "frame_a";
const FrameId frameB = "frame_b";
Transform tf;
g.addTransform(frameA, frameB, tf);
```
Frames cannot be added twice. If a frame with the given name already exists,
an exception will be thrown.

The above examples will create the frame property using the default constructor.
Another constructor can be used by calling ``emplaceFrame()``. Calling
``emplaceFrame()`` does only make sense, if the frame property has non-default
constructors.

#### Removing Frames
Frames can be removed by calling ``removeFrame()``:
```
EnvireGraph g;
const FrameId frame = "frame_a";
g.addFrame(frame);
g.disconnectFrame(frame);
g.removeFrame(frame);
```

``disconnectFrame()`` removes all transforms that are connected to the given frame.
Frames can only be removed, if they are not connected to the graph. I.e. if no
edges are connected to the frame. An exception will be thrown, if the frame is
still connected. This is an artificial restriction, technically it would be
possible to remove frames while they are still connected. The intention of this
restriction is, to make the user aware of the consequences that removing a frame
might have for the graph structure as a whole.

#### Creating Items
Before an item can be added to a frame, it has to be loaded using the ``ClassLoader``.
```
#include <envire_core/plugin/ClassLoader.hpp>
#include <envire_core/items/Item.hpp>
#include <octomap/AbstractOcTree.h>
```
```
TODO THIS CODE IS BROKEN!!! FIX IT
//   envire::core::Item<boost::shared_ptr<octomap::AbstractOcTree>> octree;
//   ClassLoader* loader = ClassLoader::getInstance();
//   if(!loader->createEnvireItem("envire::core::Item<boost::shared_ptr<octomap::AbstractOcTree>>", octree))
//   {
//     std::cerr << "Unabled to load envire::octomap::OcTree" << std::endl;
//     return -1;
//   }
```

It is also possible to instantiate items directly, however this is only
recommended for testing because visualization and serialization only work if
the ``ClassLoader`` was used to load the item.

#### Adding Item
Once the item is loaded, there are two ways to add it to the graph.
The common way is to add it using ``addItemToFrame()``:
```
TODO BROKEN!!
//   g.addItemToFrame(frame, octree);
```
The item will remember the frame that it was added to. I.e. an item cannot be part of two frames at the same time.

It is also possible to set the frame id beforehand and add the item using
``addItem()``.
```
TODO BROKEN!!!!
//   octree->setFrame(frame);
//   g.addItem(octree);
```

The item type can be a pointer to any subclass of ``ItemBase``.

#### Accessing Items
When working with items, the user needs to know the item type. The type can
either be provided at compile time using template parameters or at runtime using
``std::type_index``.

#### Checking Whether a Frame Contains Items of a Specific Type
``containsItems()`` is used to check for the existence of items of a given type
in a given frame.
```
TODO BROKEN!!!
//   const bool contains = g.containsItems<envire::core::Item<boost::shared_ptr<octomap::AbstractOcTree>>>(frame);
```
If the type is not known at compile time, there is also an overload that
accepts ``std::type_index``:
```
TODO BROKEN!!!
//   const std::type_index index(octree->getTypeIndex());
//   const bool contains2 = g.containsItems(frame, index);
```


#### Accessing Items with Iterators

The ``ItemIterator`` can be used to iterate over all items of a specific type
in a frame. The iterator internally takes care of the necessary type casting
and type checks.
```
TODO BROKEN!!!
//   using Iterator = EnvireGraph::ItemIterator<envire::core::Item<boost::shared_ptr<octomap::AbstractOcTree>>>;
//   Iterator it, end;
//   std::tie(it, end) = g.getItems<envire::core::Item<boost::shared_ptr<octomap::AbstractOcTree>>>(frame);
//   for(; it != end; ++it)
//   {
//     std::cout << "Item uuid: " << it->getIDString() << std::endl;
//   }
```

A convenience method exist to get an ``ItemIterator`` of the i'th item:
```
TODO BROKEN!!!
//   Iterator itemIt = g.getItem<envire::core::Item<boost::shared_ptr<octomap::AbstractOcTree>>>(frame, 42);
```

#### Accessing Items without Iterators
If type information is not available at compile time, ``getItems()`` can also
be used with ``std::type_index``:
```
TODO BROKEN!!!
//   const std::type_index index2(octree->getTypeIndex());
//   const Frame::ItemList& items = g.getItems(frame, index2);
```

The returned reference points directly to graph internal memory.

#### Removing Items

Items can be removed by calling ``removeItemFromFrame()``. Removing items invalidates
all iterators of the same type. To be able to iteratively remove items, the
method returns a new pair of iterators.
```
TODO BROKEN!!!
//   Iterator i, endI;
//   std::tie(i, endI) = g.getItems<envire::core::Item<boost::shared_ptr<octomap::AbstractOcTree>>>(frame);
//   for(; i != endI;)
//   {
//       std::tie(i, endI) = g.removeItemFromFrame(frame, i);
//   }
```

All items can be removed at once using ``clearFrame()``.
```
TODO BROKEN!!!
//   g.clearFrame(frame);
```


Maintenance and development
--------------------
DFKI GmbH - Robotics Innovation Center

![alt tag](https://github.com/envire/envire.github.io/raw/master/images/dfki_logo.jpg)
