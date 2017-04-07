//
// Copyright (c) 2015, Deutsches Forschungszentrum für Künstliche Intelligenz GmbH.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#pragma once
#include "GraphTypes.hpp"
#include <envire_core/items/Transform.hpp>
#include <envire_core/events/GraphEventDispatcher.hpp>
#include <envire_core/graph/GraphTypes.hpp>
#include <memory>

namespace envire { namespace core
{
  
  class EmptyPathException : public std::exception
  {
  public:
      explicit EmptyPathException() :
        msg("Path is empty") {}
      virtual char const * what() const throw() { return msg.c_str(); }
      const std::string msg;
  };
  
  
  /** Represents a path inside a Graph. I.e. a series of frames that are 
   *  connected by edges.
   *  
   *  Paths can only be instantiated by the Graph. They are always connected to
   *  the graph that created them. Do not use paths on any other graph than the
   *  one that created the path.
   * 
   *  Paths may be auto updating (depending on what you specify on creation).
   *  If a path is auto updating, it will notice when an edge on the path is 
   *  removed from the graph and mark itself as dirty. The next time a dirty path
   *  is used, it will try to update itself and find a new valid path from origin
   *  to target.
   */
  class Path : public GraphEventDispatcher
  {
    //every template specialization of Graph is a friend
    template <class FRAME_PROP, class EDGE_PROP>
    friend class Graph;
    
    template <class FRAME_PROP>
    friend class TransformGraph;
    
  public:
    
    using Ptr = std::shared_ptr<Path>;
      
    /**Returns the origin of this path.
     * @throw EmptyPathException if the path is empty*/
    const FrameId& getOrigin() const;
    
    /**Returns the target of this path.
     * @throw EmptyPathException if the path is empty*/
    const FrameId& getTarget() const;
    
    /**Returns all frames on this path from origin to target.
     * Returns an empty vector if the path is empty.
     * @warning If the path is dirty this method will return an outdated path*/
    const std::vector<FrameId>& getFrames() const;
    
    /**Returns true if the path is dirty. I.e. if an edge on the path has been removed.
     * In this case the path will be recalculated the next time it is used.*/
    bool isDirty() const;
    
    /**Returns true if the path is empty. False otherwise. */
    bool isEmpty() const;
  
    /**Returns true if the path is subscribed to a graph and is autoupdating. False otherwise. */
    bool isAutoUpdating() const;
    
    /** Returns the number of frames in this path*/
    std::size_t getSize() const;
    
    /**returns the i'th frame in the path.
     * @warning crashes if @p i is out of range*/
    const FrameId& operator[](const int i) const;
    
    /**returns the i'th frame in the path.
     * @throw std::out_of_range if @p i is out of range*/
    const FrameId& at(const int i) const;
    
  protected:
    //only the graph may create paths.
    /**Creates a path containing @p frames.
     * The path is not subscribed to any graph and does not auto update.*/
    Path(const std::vector<FrameId>& frames);
    
    /**Creates a path containing @p frames.
     * The path is subscribed to @p graph and auto updates if the graph changes.*/
    Path(const std::vector<FrameId>& frames, GraphEventPublisher* graph);
    
    /**overridden to reset pGraph when unsubscribed. Otherwise we might end up
     * trying to update the path using a deleted graph.*/
    virtual void unsubscribe() override;
    
    /**Checks whether the removed edge is part of this path, if yes dirties the path  */
    virtual void edgeRemoved(const EdgeRemovedEvent& e) override;
    
    void setDirty(const bool value);
    
    void setEdges(const std::set<GraphTraits::edge_descriptor>& edges);
    void setFrames(const std::vector<FrameId>& frames);
    
  private:
    
    /*clear and re-create the edge set */
    void createEdges(const std::vector<FrameId>& frames);
    
    std::vector<FrameId> frames; // Index 0 is the origin, index n the target of the path.
    
    //all edges on the path. Used to quickly check if the path is affected when edges change.
    //is empty when not subscribed to a graph.
    //NOTE we can't use edge_descriptor here because it becomes invalid when the edge is removed
    //     std::pair<FrameId, FrameId> is much slower to hash but is the only choice
    std::unordered_set<std::pair<FrameId, FrameId>> edges;
    bool dirty; //If true, some edge on the path was removed and the path needs to be re-calculated
    bool autoUpdating;
  };
  
  
}}
