#include "Path.hpp"
#include <envire_core/events/EdgeEvents.hpp>
#include <envire_core/events/GraphEventPublisher.hpp>


namespace envire { namespace core
{
  
Path::Path(const std::vector<FrameId>& frames) : frames(frames), dirty(false), autoUpdating(false)
{}

Path::Path(const std::vector<FrameId>& frames, GraphEventPublisher* graph) : GraphEventDispatcher(graph),
  frames(frames), dirty(false), autoUpdating(true)
{
  createEdges(frames);
}

void Path::edgeRemoved(const EdgeRemovedEvent& e)
{
  //we only get this event if we are subscribed to a graph. I.e. the graph manages this path.
  assert(isAutoUpdating());
  
  if(isDirty())
    return;
  
  if(edges.find(make_pair(e.origin, e.target)) != edges.end())
  {
    setDirty(true);
  }
}
  
const std::vector< FrameId >& Path::getFrames() const
{
  return frames;
}

const FrameId& Path::getOrigin() const
{
  if(isEmpty())
    throw EmptyPathException();
  return frames.front();
}

const FrameId& Path::getTarget() const
{
  if(isEmpty())
    throw EmptyPathException();
  return frames.back();
}

bool Path::isDirty() const
{
  return dirty;
}

bool Path::isEmpty() const
{
  return frames.size() == 0;
}

bool Path::isAutoUpdating() const
{
  return autoUpdating;
}

void Path::unsubscribe()
{
  envire::core::GraphEventSubscriber::unsubscribe();
  dirty = false; //dirty can never be true when not subscribed
  edges.clear();
  autoUpdating = false;
}

void Path::setDirty(const bool value)
{
  dirty = value;
}

void Path::setFrames(const std::vector<FrameId>& frames)
{
  this->frames = frames;
  createEdges(frames);
}

void Path::createEdges(const std::vector< FrameId >& frames)
{
  edges.clear();
  
  if(frames.size() == 0) //loop below breakes for size == 0
    return;
  
  for(int i = 0; i < ((int)frames.size()) - 1; ++i)
  {
    edges.emplace(frames[i], frames[i+1]);
  }
}

std::size_t Path::getSize() const
{
  return frames.size();
}

const FrameId& Path::operator[](const int i) const
{
  return frames[i];
}

const FrameId& Path::at(const int i) const
{
  return frames.at(i);
}



  
}}