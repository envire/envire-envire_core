#include "TreeView.hpp"
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

void TreeView::addCrossEdge(const edge_descriptor edge)
{
    crossEdges.push_back(edge);
    crossEdgeAdded(edge);
}

void TreeView::addEdge(vertex_descriptor origin, vertex_descriptor target)
{
    tree[origin].children.insert(target);
    tree[target].parent = origin;
    tree[target].parentRelation = &tree[origin];
    edgeAdded(origin, target);
}

void TreeView::addRoot(vertex_descriptor root)
{
    tree[root].parent = GraphTraits::null_vertex();
    tree[root].parentRelation = nullptr;
    this->root = root;
}

vertex_descriptor TreeView::getParent(vertex_descriptor node) const
{
    return tree.at(node).parent;
}


}}