#pragma once

#include "GraphTypes.hpp"
namespace envire { namespace core
{
    
    /** Structure to store the parent and children relation for a vertex in a tree.
     */
    struct VertexRelation
    {
        GraphTraits::vertex_descriptor parent; /**<can be null_vertex */
        std::unordered_set<GraphTraits::vertex_descriptor> children;
        
        
        
    };

    /**A map that shows the vertex information (parent and children) of the vertices in a tree.
       The key is the vertex descriptor.*/
    using VertexRelationMap = std::unordered_map<GraphTraits::vertex_descriptor, VertexRelation>;
    
    
    class TreeView;
    /**A class that notifies TreeViews about updates */
    struct TreeUpdatePublisher
    {
        virtual ~TreeUpdatePublisher() {}
        /**Unsubscribe the view from the publisher */
        virtual void unsubscribeTreeView(TreeView* view) = 0;
        /**Subscribe the view to the publisher */
        virtual void subscribeTreeView(TreeView* view) = 0;
    };
    
    /** A TreeView is a tree shaped snapshot of the graph structure.
     *  it is generated by traversing the graph in bfs order, starting from a 
     *  root node. The vertex_descriptors used in the graph are pointers to
     *  vertices in the graph and can be used to manipulate the graph.
     */
    class TreeView
    {
    public:
             
        struct CrossEdge
        {
            GraphTraits::vertex_descriptor origin;
            GraphTraits::vertex_descriptor target;
            GraphTraits::edge_descriptor edge;
            CrossEdge(GraphTraits::vertex_descriptor origin,
                      GraphTraits::vertex_descriptor target,
                      GraphTraits::edge_descriptor edge) :
                origin(origin), target(target), edge(edge) {}
        };
      
        TreeView(GraphTraits::vertex_descriptor root) : root(root) {}
        
        TreeView() : root(GraphTraits::null_vertex()) {}
        
        /**Creates a copy ***without*** retaining the treeUpdated subscribers  */
        TreeView(const TreeView& other) {*this = other;}
        
        /**Creates a copy ***without*** retaining the treeUpdated subscribers  */
        TreeView& operator=(const TreeView& other);
        
        TreeView(TreeView&& other) noexcept;
        
        virtual ~TreeView();
        
        /**If a publisher is set the TreeView will automatically unsubscribe 
        * from the publisher on destruction. */
        void setPublisher(TreeUpdatePublisher* pub);

        bool isRoot(const GraphTraits::vertex_descriptor vd) const;
        
        /**Removes all content from this TreeView */
        void clear();
        
        /**Unsibscribe from the currently subscribed publisher */
        void unsubscribe();
        
        /**Returns true if an edge between a and b exists in @p view 
         * and is not a cross-edge*/
        bool edgeExists(const GraphTraits::vertex_descriptor a, const GraphTraits::vertex_descriptor b) const;
        /**Returns true if @p vd exists in the view. */
        bool vertexExists(const GraphTraits::vertex_descriptor vd) const;
        
        /**visits all vertices in the tree starting at @p node in dfs order.
         * I.e. it first visits node, then all its children.
         * Calls @p f(vertex_descriptor node, vertex_descriptor parent) for each node.*/
        template <class Func>
        void visitDfs(const GraphTraits::vertex_descriptor node, Func f)
        {
            const auto& parent = tree[node].parent;
            const auto& children = tree[node].children;
            f(node, parent);
            for(const GraphTraits::vertex_descriptor child : children)
            {
                visitDfs(child, f);
            }
        }
        
        /**visits all vertices in the tree starting at @p node in bfs order.
         * Calls @p f(vertex_descriptor node, vertex_descriptor parent) for each node.*/
        template <class Func>
        void visitBfs(const GraphTraits::vertex_descriptor node, Func f)
        {
            std::deque<GraphTraits::vertex_descriptor> nodesToVisit;
            nodesToVisit.push_back(node);
            while(nodesToVisit.size() > 0)
            {
                const GraphTraits::vertex_descriptor current = nodesToVisit.front();
                nodesToVisit.pop_front();
                const GraphTraits::vertex_descriptor parent = getParent(current);
                f(current, parent);
                for(GraphTraits::vertex_descriptor child : tree[current].children)
                {
                    nodesToVisit.push_back(child);
                }
            }
        }
        
        /**Add a cross edge to the view.
         * Emits crossEdgeAdded event */
        void addCrossEdge(const GraphTraits::vertex_descriptor origin,
                          const GraphTraits::vertex_descriptor target,
                          const GraphTraits::edge_descriptor edge);
        
        /**Add an edge to the view.
         * Emits edgeAdded*/
        void addEdge(GraphTraits::vertex_descriptor origin, GraphTraits::vertex_descriptor target);
        
        /**Adds the initial root node to the TreeView */
        void addRoot(GraphTraits::vertex_descriptor root);
        
        /**Removes an edge from the view.
         * Also removes the sub-tree below the edge that should be removed.
         * Emits edgeRemoved for each edge that is removed. The events will be 
         * emitted starting from the deeps edge in the tree, i.e. you can be sure
         * that the parent still exists in the tree when handling the event.
         *
         * If cross-edges exist that point into the sub-tree that is beeing removed,
         * the method will return*/
        void removeEdge(GraphTraits::vertex_descriptor origin, GraphTraits::vertex_descriptor target);
        //FIXME comment exception?!
        GraphTraits::vertex_descriptor getParent(GraphTraits::vertex_descriptor node) const;
        
        /**The signals are invoked whenever the tree is updated by the TransformGraph
        * @note This is only the case if you requested an updating TreeView. 
        *       Otherwise they'll never be invoked.
        */
        boost::signals2::signal<void (const CrossEdge&)> crossEdgeAdded;
        boost::signals2::signal<void (GraphTraits::vertex_descriptor origin,
                                      GraphTraits::vertex_descriptor target)> edgeAdded;
        
                                      /**Is emitted when a an edge is removed.
         * When the signal is emitted, @p target is no longer part of the tree.
         * @p origin is still part of the tree.*/
        boost::signals2::signal<void (GraphTraits::vertex_descriptor origin,
                                      GraphTraits::vertex_descriptor target)> edgeRemoved;

        /* The edges, that had to be removed to create the tree.
         * I.e. All edges that lead to a vertex that has already been discovered.
         * This does **not** include back-edges. I.e. edges that lead to a vertex that
         * has already been visited.
         * @note The TransformGraph always contains two edges between connected nodes (the edge and the inverse edge)
         *       However only one of them will be in the crossEdges. The other one automatically becomes a back-edge 
         *       and is ignored. */
        std::vector<CrossEdge> crossEdges;
        
        /**The root node of this TreeView */
        GraphTraits::vertex_descriptor root;
        VertexRelationMap tree;   
    protected:
        TreeUpdatePublisher* publisher = nullptr;/*< Used for automatic unsubscribing in dtor */
    };
}}