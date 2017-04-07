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

#include <envire_core/graph/TreeView.hpp>

namespace envire { namespace core
{
    using vertex_descriptor = GraphTraits::vertex_descriptor;
    using edge_descriptor = GraphTraits::edge_descriptor;
    
    TreeView& TreeView::operator=(const TreeView& other)
    {
        //this operator has to be here because the default
        //operator= wants to copy treeUpdated, which is is not possible.
        
        //WARNING If you add members to this class, make sure
        //        to copy them!
        publisher = other.publisher;
        tree = other.tree;
        crossEdges = other.crossEdges;
        root = other.root;
        return *this;
    }
    
TreeView::TreeView(TreeView&& other) noexcept : crossEdges(std::move(other.crossEdges)),
                                                root(std::move(other.root)),
                                                tree(std::move(other.tree))
{
    //if the other TreeView was subscribed, unsubscribe it and 
    //subscribe this instead
    if(other.publisher != nullptr)
    {
      other.publisher->unsubscribeTreeView(&other);
      other.publisher->subscribeTreeView(this); //sets this.publisher
      other.publisher = nullptr;
    }
    crossEdgeAdded.swap(other.crossEdgeAdded);
    edgeAdded.swap(other.edgeAdded);
}


TreeView::~TreeView()
{
    if(nullptr != publisher)
    {
        //automatic unsubscribe
        publisher->unsubscribeTreeView(this);
        publisher = nullptr;
    }
}

bool TreeView::hasRoot()
{
    if (root == GraphTraits::null_vertex())
    {
        return false;
    }
    else {
        return true;
    }
}

void TreeView::setPublisher(TreeUpdatePublisher* pub)
{
    assert(publisher == nullptr);
    publisher = pub;
}

bool TreeView::isRoot(const vertex_descriptor vd) const
{
    return vd == root;
}

void TreeView::clear()
{
    tree.clear();
    crossEdges.clear();
    root = GraphTraits::null_vertex();
}

void TreeView::unsubscribe()
{
    if(publisher != nullptr)
    {
        publisher->unsubscribeTreeView(this);
        publisher = nullptr;
    }
}

bool TreeView::edgeExists(const vertex_descriptor a, const vertex_descriptor b) const
{
    //an edge exists if either a is the parent of b and aChildren contains b
    //or the other way around.
    if(tree.find(a) == tree.end() || tree.find(b) == tree.end())
    {
        return false;
    }
    const VertexRelation& aRelation = tree.at(a);
    const VertexRelation& bRelation = tree.at(b);

    //If we assume that we made no mistake when populating the tree we could just 
    //return (aRelation.parent == b || bRelation.parent == a)
    //but using asserts is always better :D

    //the if will be optimized out if asserts are disabled
    if(aRelation.parent == b) //b is parent of a
    {
      assert(bRelation.children.find(a) != bRelation.children.end());
    }
    else if(bRelation.parent == a) //a is parent of b
    {
      assert(aRelation.children.find(b) != aRelation.children.end());
    }
    return aRelation.parent == b || bRelation.parent == a;
}

bool TreeView::vertexExists(const vertex_descriptor vd) const
{
  return tree.find(vd) != tree.end();
}

void TreeView::addCrossEdge(const GraphTraits::vertex_descriptor origin,
                            const GraphTraits::vertex_descriptor target,
                            const GraphTraits::edge_descriptor edge)
{
    crossEdges.emplace_back(origin, target, edge);
    crossEdgeAdded(crossEdges.back());
}
void TreeView::addEdge(vertex_descriptor origin, vertex_descriptor target)
{
    tree[origin].children.insert(target);
    tree[target].parent = origin;
    edgeAdded(origin, target);
}



void TreeView::removeEdge(vertex_descriptor origin, vertex_descriptor target)
{
  

  /**Algorithm:
   * (1) bfs visit the sub-tree that will be removed. For each visited vertex:
   * (2.1) Check if a cross-edge is connected to the vertex.
   * (2.2) If yes, check if the cross-edge is already part of S. If it is, 
   *       remove it from S. otherwise add it.
   * (3) S now contains all cross-edges that connect the sub-tree to the
   *     graph.
   * (4) Remove the sub-tree from the graph
   * (5) If S is not empty, generate a new sub-tree with S[0] as root and
   *     add it to the tree. 
   * */
  
  
  //a map to map from vertex_descriptor to edge_descriptor.
  //It is a multimap because multiple cross-edges might be connected
  //to the same vertex.
  std::multimap<vertex_descriptor, std::vector<CrossEdge>::iterator> vertexToCrossEdge;
  for(std::vector<CrossEdge>::iterator edge = crossEdges.begin(); edge != crossEdges.end(); ++edge)
  {
      vertexToCrossEdge.emplace(edge->origin, edge);
      vertexToCrossEdge.emplace(edge->target, edge);
  }
  
  vertex_descriptor realTarget;
  //figure out which of the vertices is acutally the origin in the tree
  if(tree[target].parent == origin)
  {
      realTarget = target;
  }
  else if(tree[origin].parent == target)
  {
      realTarget = origin;
  }
  else
  {
      //happens if one of the vertices isn't part of the tree, in that case this 
      //method should have never been called in the first place.
      assert(false);
  }
  
  //will contain the list of cross-edges that are connected to the sub-tree
  std::set<std::vector<CrossEdge>::iterator> treeLeavingCrossEdges;
  //stores all visited vertices that should be removed later
  std::vector<vertex_descriptor> vertices;
  
  visitBfs(realTarget, [&](vertex_descriptor node, vertex_descriptor parent)
  {
      vertices.push_back(node);
      auto range = vertexToCrossEdge.equal_range(node);
      for (auto it = range.first; it != range.second; ++it)
      {
          std::vector<CrossEdge>::iterator crossEdge = it->second;
          if(treeLeavingCrossEdges.find(crossEdge) == treeLeavingCrossEdges.end())
          {
              treeLeavingCrossEdges.insert(crossEdge);
          } 
          else
          {
              //if we found an edge for the second time it is internal
              //in the sub-tree and should be removed as well.
              treeLeavingCrossEdges.erase(crossEdge);
              crossEdges.erase(crossEdge);
          }
      }
  });
  
  //remove vertices in reverse order to ensure that the parent is still in the
  //tree when the event is emitted.
  while(vertices.size() > 0)
  {
      const vertex_descriptor node = vertices.back();
      vertices.pop_back();
      VertexRelation& relation = tree[node];
      const vertex_descriptor parent = relation.parent;
      tree[parent].children.erase(node);
      //since we are removing bottom-up, all children should have been removed already.
      assert(relation.children.size() == 0);

      tree.erase(node);
      edgeRemoved(parent, node);
  }
  
  //TODO if cross-edge is present, readd new tree
  if(treeLeavingCrossEdges.size() > 0)
  {
      throw std::runtime_error("NOT IMPLEMENTED");
  }

}

void TreeView::addRoot(vertex_descriptor root)
{
    tree[root].parent = GraphTraits::null_vertex();
    this->root = root;
}

vertex_descriptor TreeView::getParent(vertex_descriptor node) const
{
    if (node == GraphTraits::null_vertex())
    {
      throw std::runtime_error("envire_core:TreeView::getParent: Node is null vertex.");
    }
    if(tree.find(node) == tree.end())
    {
      throw std::runtime_error("envire_core:TreeView::getParent: Node is not in the tree.");
    }
    return tree.at(node).parent;
}

bool TreeView::isParent(const vertex_descriptor parent, const vertex_descriptor child) const
{
  return tree.at(child).parent == parent;
}


}}