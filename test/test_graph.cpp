#define protected public
#include <boost/test/unit_test.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/archive/polymorphic_text_iarchive.hpp>
#include <boost/archive/polymorphic_text_oarchive.hpp>
#include <boost/archive/polymorphic_binary_iarchive.hpp>
#include <boost/archive/polymorphic_binary_oarchive.hpp>
#include <envire_core/graph/Graph.hpp>
#include <envire_core/events/GraphEventDispatcher.hpp>
#include <envire_core/graph/GraphViz.hpp>
#include <envire_core/events/GraphEventQueue.hpp>
#include <vector>
#include <string>
 
using namespace envire::core;
using namespace std;


class FrameProp
{
  FrameId id;
public:
  const FrameId& getId() const
  {
    return id;
  }
  void setId(const FrameId& _id) 
  {
      id = _id;
  }
  
  const string toGraphviz() const
  {
      return "[label=\"" + id + "\"]";
  }
  
  template<class Archive>
  void serialize(Archive &ar, const unsigned int version)
  {
    ar & id;
  }
  
};

struct EdgeProp
{
  bool inverted = false;
  int value = 42;
  EdgeProp inverse() const
  {
    EdgeProp other;
    other.inverted = !inverted;
    other.value = -value;
    return other;
  }
  
  const string toGraphviz() const
  {
      return "[label=\" inverted: " + boost::lexical_cast<string>(inverted) +
             ", value: " + boost::lexical_cast<string>(value) + "\"]";
  }
  
  template<class Archive>
  void serialize(Archive &ar, const unsigned int version)
  {
    ar & inverted;
    ar & value;
  }
};

using Gra = envire::core::Graph<FrameProp, EdgeProp>;

class Dispatcher : public GraphEventDispatcher {
public:
    vector<EdgeAddedEvent> edgeAddedEvents;
    vector<EdgeModifiedEvent> edgeModifiedEvents;
    vector<EdgeRemovedEvent> edgeRemovedEvents;
    vector<FrameAddedEvent> frameAddedEvents;
    vector<FrameRemovedEvent> frameRemovedEvents;
    
    Dispatcher(Gra& graph) : GraphEventDispatcher(&graph) {}
    Dispatcher() : GraphEventDispatcher() {}
    virtual ~Dispatcher() {}
    
    virtual void edgeAdded(const EdgeAddedEvent& e)
    {
        edgeAddedEvents.push_back(e);
    }

    virtual void edgeModified(const EdgeModifiedEvent& e)
    {
        edgeModifiedEvents.push_back(e);
    }

    virtual void edgeRemoved(const EdgeRemovedEvent& e)
    {
        edgeRemovedEvents.push_back(e);
    }
    virtual void frameAdded(const FrameAddedEvent& e)
    {
        frameAddedEvents.push_back(e);
    }
    virtual void frameRemoved(const FrameRemovedEvent& e)
    {
        frameRemovedEvents.push_back(e);
    }
    
    void itemRemoved(const ItemRemovedEvent& e) override
    {
    }
};

class EventQueue : public GraphEventQueue
{

public:
    EventQueue(Gra& graph) : GraphEventQueue(&graph) {}
    EventQueue() : GraphEventQueue() {}
    virtual ~EventQueue() {}

    virtual void process( const GraphEvent& event )
    {
        dispatcher.notifyGraphEvent(event);
        //events.push_back(event);
    }

    Dispatcher dispatcher;
    vector<GraphEvent> events;
};

BOOST_AUTO_TEST_CASE(simple_add_remove_edge_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";   
    Gra g;
    EdgeProp ep;
    
    g.addFrame(a);
    g.addFrame(b);
     
    g.add_edge(a, b, ep);
    BOOST_CHECK(g.num_edges() == 2); //2 because the graph also adds the inverse
    BOOST_CHECK_THROW(g.add_edge(a, b, ep), EdgeAlreadyExistsException);
    BOOST_CHECK_THROW(g.add_edge(b, a, ep), EdgeAlreadyExistsException);
    BOOST_CHECK(g.num_edges() == 2);
    
    g.remove_edge(a, b);
    BOOST_CHECK(g.num_edges() == 0);
    BOOST_CHECK_THROW(g.getEdge(b, a), UnknownEdgeException);
    BOOST_CHECK_THROW(g.getEdge(a, b), UnknownEdgeException);
    BOOST_CHECK_THROW(g.remove_edge(b, a), UnknownEdgeException);
    BOOST_CHECK_THROW(g.remove_edge(a, b), UnknownEdgeException);

    BOOST_CHECK(g.num_vertices() == 2); //vertices are still there, just not connected anymore
}

BOOST_AUTO_TEST_CASE(complex_remove_transform_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    FrameId c = "frame_c";
    Gra graph;
    EdgeProp ep;

    graph.add_edge(a, b, ep);
    graph.add_edge(a, c, ep);

    graph.remove_edge(b, a);
    BOOST_CHECK(graph.num_edges() == 2);
    BOOST_CHECK(graph.num_vertices() == 3);
    BOOST_CHECK_NO_THROW(graph.getVertex(a));
    BOOST_CHECK_NO_THROW(graph.getVertex(b));
    BOOST_CHECK_NO_THROW(graph.getVertex(c));
}

BOOST_AUTO_TEST_CASE(add_edge_inverse_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    Gra graph;
    EdgeProp ep;

    graph.add_edge(a, b, ep);
    EdgeProp inverted = graph.getEdgeProperty(b, a);
    BOOST_CHECK(ep.inverted != inverted.inverted);
}


BOOST_AUTO_TEST_CASE(add_frame_already_exists_test)
{
    FrameId a = "frame_a";
    Gra g;
    g.addFrame(a);
    BOOST_CHECK_THROW(g.addFrame(a), FrameAlreadyExistsException);
}

BOOST_AUTO_TEST_CASE(get_vertex_unknown_frame_test)
{
    const FrameId a = "frame_a";
    Gra g;
    BOOST_CHECK_THROW(g.getVertex(a), UnknownFrameException);
}


BOOST_AUTO_TEST_CASE(simple_get_tree_test)
{
    Gra graph;
    EdgeProp ep;
    
    /*       a
     *      / \
     *     c   b
     *   /  \
     *  d   e
     *    /  \
     *   f   g
     */

    FrameId a = "frame_a";
    FrameId b = "frame_b";
    FrameId c = "frame_c";
    FrameId d = "frame_d";
    FrameId e = "frame_e";
    FrameId f = "frame_f";
    FrameId g = "frame_g";

    graph.add_edge(a, b, ep);
    graph.add_edge(a, c, ep);
    graph.add_edge(c, d, ep);
    graph.add_edge(c, e, ep);
    graph.add_edge(e, f, ep);
    graph.add_edge(e, g, ep);
    
    //use a as root
    TreeView view = graph.getTree(graph.vertex(a));
    BOOST_CHECK(view.root == graph.vertex(a));
    VertexRelationMap& tree = view.tree;
    BOOST_CHECK(tree.size() == 7);
    BOOST_CHECK(tree[graph.vertex(a)].children.size() == 2);
    BOOST_CHECK(tree[graph.vertex(c)].children.size() == 2);
    BOOST_CHECK(tree[graph.vertex(e)].children.size() == 2);
    BOOST_CHECK(tree[graph.vertex(a)].parent == Gra::null_vertex()); //check parent
    BOOST_CHECK(tree[graph.vertex(a)].parentRelation == nullptr); //check parent
    BOOST_CHECK(tree[graph.vertex(b)].parent == graph.vertex(a));
    BOOST_CHECK(tree[graph.vertex(b)].parentRelation == &tree[graph.vertex(a)]);
    BOOST_CHECK(tree[graph.vertex(d)].parent == graph.vertex(c));
    BOOST_CHECK(tree[graph.vertex(d)].parentRelation == &tree[graph.vertex(c)]);
    BOOST_CHECK(tree[graph.vertex(f)].parent == graph.vertex(e));
    BOOST_CHECK(tree[graph.vertex(f)].parentRelation == &tree[graph.vertex(e)]);
    BOOST_CHECK(tree[graph.vertex(g)].parent == graph.vertex(e));
    BOOST_CHECK(tree[graph.vertex(g)].parentRelation == &tree[graph.vertex(e)]);
    std::unordered_set<GraphTraits::vertex_descriptor>& aChildren = tree[graph.vertex(a)].children;
    BOOST_CHECK(aChildren.find(graph.vertex(b)) != aChildren.end());
    BOOST_CHECK(aChildren.find(graph.vertex(c)) != aChildren.end());
    std::unordered_set<GraphTraits::vertex_descriptor>& cChildren = tree[graph.vertex(c)].children;
    BOOST_CHECK(cChildren.find(graph.vertex(d)) != cChildren.end());
    BOOST_CHECK(cChildren.find(graph.vertex(e)) != cChildren.end());
    std::unordered_set<GraphTraits::vertex_descriptor>& eChildren = tree[graph.vertex(e)].children;
    BOOST_CHECK(eChildren.find(graph.vertex(f)) != eChildren.end());
    BOOST_CHECK(eChildren.find(graph.vertex(g)) != eChildren.end());


    BOOST_CHECK(view.isRoot(graph.vertex(a)) == true); //check root
    BOOST_CHECK(view.isRoot(graph.vertex(b)) == false); //check root
    BOOST_CHECK(view.isRoot(graph.vertex(c)) == false); //check root
    BOOST_CHECK(view.isRoot(graph.vertex(d)) == false); //check root
    BOOST_CHECK(view.isRoot(graph.vertex(e)) == false); //check root
    BOOST_CHECK(view.isRoot(graph.vertex(f)) == false); //check root
    BOOST_CHECK(view.isRoot(graph.vertex(g)) == false); //check root

   
    
     /*       d
     *        |
     *        c
     *       / \
     *      a   e
     *     /   / \
     *    b   f   g
     * Now use d as root node
     */
     
    view = graph.getTree(graph.getVertex(d));
    BOOST_CHECK(view.root == graph.getVertex(d));
    tree = view.tree;
    BOOST_CHECK(tree.size() == 7);
    BOOST_CHECK(tree[graph.vertex(d)].children.size() == 1);
    BOOST_CHECK(tree[graph.vertex(c)].children.size() == 2);
    BOOST_CHECK(tree[graph.vertex(e)].children.size() == 2);
    BOOST_CHECK(tree[graph.vertex(a)].children.size() == 1);
    BOOST_CHECK(tree[graph.vertex(d)].parent == Gra::null_vertex()); //check parent
    BOOST_CHECK(tree[graph.vertex(b)].parent == graph.vertex(a));
    BOOST_CHECK(tree[graph.vertex(f)].parent == graph.vertex(e));
    BOOST_CHECK(tree[graph.vertex(g)].parent == graph.vertex(e));

    std::unordered_set<GraphTraits::vertex_descriptor>& aChildren2 = tree[graph.vertex(a)].children;
    BOOST_CHECK(aChildren2.find(graph.vertex(b)) != aChildren2.end());
    std::unordered_set<GraphTraits::vertex_descriptor>& cChildren2 = tree[graph.vertex(c)].children;
    BOOST_CHECK(cChildren2.find(graph.vertex(a)) != cChildren2.end());
    BOOST_CHECK(cChildren2.find(graph.vertex(e)) != cChildren2.end());
    std::unordered_set<GraphTraits::vertex_descriptor>& dChildren = tree[graph.vertex(d)].children;
    BOOST_CHECK(dChildren.find(graph.vertex(c)) != aChildren.end());
    std::unordered_set<GraphTraits::vertex_descriptor>& eChildren2 = tree[graph.vertex(e)].children;
    BOOST_CHECK(eChildren2.find(graph.vertex(f)) != eChildren2.end());
    BOOST_CHECK(eChildren2.find(graph.vertex(g)) != eChildren2.end());

    BOOST_CHECK(view.isRoot(graph.vertex(d)) == true); //check root
    BOOST_CHECK(view.isRoot(graph.vertex(c)) == false); //check root
    BOOST_CHECK(view.isRoot(graph.vertex(a)) == false); //check root
    BOOST_CHECK(view.isRoot(graph.vertex(e)) == false); //check root
    BOOST_CHECK(view.isRoot(graph.vertex(b)) == false); //check root
    BOOST_CHECK(view.isRoot(graph.vertex(f)) == false); //check root
    BOOST_CHECK(view.isRoot(graph.vertex(g)) == false); //check root

}

BOOST_AUTO_TEST_CASE(simple_get_tree_with_frameId_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    FrameId c = "frame_c";

    Gra graph;
    EdgeProp ep;
    
    
    graph.add_edge(a, b, ep);
    graph.add_edge(a, c, ep);

    TreeView view = graph.getTree(a);
    BOOST_CHECK(view.root == graph.getVertex(a));
    
    VertexRelationMap& tree = view.tree;
    BOOST_CHECK(tree.size() == 3);
    BOOST_CHECK(tree[graph.vertex(a)].children.size() == 2);
    BOOST_CHECK(tree[graph.vertex(b)].parent == graph.vertex(a));
    BOOST_CHECK(tree[graph.vertex(c)].parent == graph.vertex(a));
    BOOST_CHECK(tree[graph.vertex(c)].parentRelation == &tree[graph.vertex(a)]);
    BOOST_CHECK(tree[graph.vertex(a)].parent == Gra::null_vertex());
    BOOST_CHECK(tree[graph.vertex(a)].parentRelation == nullptr);
}

BOOST_AUTO_TEST_CASE(simple_get_tree_with_invalid_frameId_test)
{
    FrameId a = "frame_a";
    Gra graph;
    BOOST_CHECK_THROW(graph.getTree(a), UnknownFrameException);
}


BOOST_AUTO_TEST_CASE(non_tree_edges_test)
{
  
  /*            A
   *           /  \
   *          B    D
   *          |    |
   *          C    |
   *           \  /
   *            E   
   */ 
  
    FrameId a("A");
    FrameId b("B");
    FrameId c("C");
    FrameId d("D");
    FrameId e("E");
    
    Gra graph;
    EdgeProp ep;
    
    
    graph.add_edge(a, b, ep);
    graph.add_edge(a, d, ep);
    graph.add_edge(b, c, ep);
    graph.add_edge(c, e, ep);
    graph.add_edge(d, e, ep);
    
    TreeView view = graph.getTree(a);
    BOOST_CHECK(view.root == graph.getVertex(a));
    BOOST_CHECK(view.crossEdges.size() == 1);
    
    TreeView::CrossEdge edge = view.crossEdges[0];
    GraphTraits::vertex_descriptor source = edge.origin;
    GraphTraits::vertex_descriptor target = edge.target;
    BOOST_CHECK(graph.vertex(c) == source);
    BOOST_CHECK(graph.vertex(e) == target);
}

BOOST_AUTO_TEST_CASE(tree_view_automatic_update_simple_test)
{
    Gra graph;
    EdgeProp ep;
    
    
    FrameId A("A");
    FrameId B("B");
    FrameId C("C");
    
    graph.addFrame(A);
    graph.addFrame(B);
    graph.addFrame(C);
     
    graph.add_edge(A, B, ep);

    TreeView view;
    TreeView bView; //view with root B
    graph.getTree(A, true, &view);
    graph.getTree(B, true, &bView);
    
    BOOST_CHECK(view.root == graph.getVertex(A));
    BOOST_CHECK(bView.root == graph.getVertex(B));
    
    graph.add_edge(A, C, ep);
    
    GraphTraits::vertex_descriptor vA = graph.getVertex(A);
    GraphTraits::vertex_descriptor vB = graph.getVertex(B);
    GraphTraits::vertex_descriptor vC = graph.getVertex(C);
    
    BOOST_CHECK(view.tree[vA].children.size() == 2);
    //vB is child of vA
    BOOST_CHECK(view.tree[vA].children.find(vB) != view.tree[vA].children.end());
    //vC is child of vA
    BOOST_CHECK(view.tree[vA].children.find(vC) != view.tree[vA].children.end());
    //vC has no children and her parent is vA
    BOOST_CHECK(view.tree[vC].children.size() == 0);
    BOOST_CHECK(view.tree[vC].parent == vA);
    BOOST_CHECK(view.tree[vC].parentRelation == &view.tree[vA]);
    
    BOOST_CHECK(bView.tree[vA].children.size() == 1);
    BOOST_CHECK(bView.tree[vA].children.find(vC) != view.tree[vA].children.end());
    
    //unsubscribe and add another transform, check that the view doesn't update
    graph.unsubscribeTreeView(&view);
    
    const FrameId D("D");
    graph.add_edge(C, D, ep);
    
    const GraphTraits::vertex_descriptor vD = graph.getVertex(D);
    BOOST_CHECK(view.tree.find(vD) == view.tree.end());
    BOOST_CHECK(view.tree[vC].children.size() == 0);
    
    //but bView should update since it is still subscribed
    BOOST_CHECK(bView.tree[vC].children.size() == 1);
    BOOST_CHECK(bView.tree.find(vD) != bView.tree.end());
    BOOST_CHECK(bView.tree[vD].parent == vC);
    BOOST_CHECK(bView.tree[vD].parentRelation == &bView.tree[vC]);
    BOOST_CHECK(bView.tree[vD].children.size() == 0);
}

BOOST_AUTO_TEST_CASE(tree_view_cross_edge_test)
{
    Gra graph;
    EdgeProp ep;
    
    FrameId A("A");
    FrameId B("B");
    FrameId C("C");
    FrameId D("D");
     
    /**      A
     *      / \
     *     B   C
     *      \ /
     *       D
     */
    
    graph.addFrame(A);
    graph.addFrame(B);
    graph.addFrame(C);
    graph.addFrame(D);
    
    graph.add_edge(A, B, ep);
    
    TreeView view;
    graph.getTree(A, true, &view);
    
    BOOST_CHECK(view.root == graph.getVertex(A));
    
    graph.add_edge(A, C, ep);
    graph.add_edge(B, D, ep);
    graph.add_edge(C, D, ep);
    
    GraphTraits::vertex_descriptor vB = graph.getVertex(B);
    GraphTraits::vertex_descriptor vC = graph.getVertex(C);
    GraphTraits::vertex_descriptor vD = graph.getVertex(D);
    
    //D should be a child of B but not of C because c->d is a cross-edge
    BOOST_CHECK(view.tree[vB].children.size() == 1);
    BOOST_CHECK(view.tree[vB].children.find(vD) != view.tree[vB].children.end());
    BOOST_CHECK(view.tree.find(vC) != view.tree.end());
    BOOST_CHECK(view.tree[vC].children.size() == 0);
    BOOST_CHECK(view.tree[vD].parent == vB);
    BOOST_CHECK(view.tree[vD].parentRelation == &view.tree[vB]);
    BOOST_CHECK(view.tree[vD].children.size() == 0);
    //C -> D or D -> C should be part of the cross edges
    BOOST_CHECK(view.crossEdges.size() == 1);
    const GraphTraits::vertex_descriptor src = view.crossEdges[0].origin;
    const GraphTraits::vertex_descriptor tar = view.crossEdges[0].target;
    //note: it is not specified whether src->tar or tar->src ends up in the cross
    //      edges. The current implementation adds src->tar but that can change.
    BOOST_CHECK(tar == vD);
    BOOST_CHECK(src == vC);
}

BOOST_AUTO_TEST_CASE(tree_view_move_semantics_test)
{
    Gra graph;
    EdgeProp ep;
    
    FrameId A("A");
    FrameId B("B");
    FrameId C("C");

    graph.addFrame(A);
    graph.addFrame(B);
    graph.addFrame(C);
    
    graph.add_edge(A, B, ep);
    TreeView view;
    graph.getTree(A, true, &view);
    bool edgeCallback = false;
    view.edgeAdded.connect([&](GraphTraits::vertex_descriptor, GraphTraits::vertex_descriptor)
        {
            edgeCallback = true;
        });
    TreeView newView = std::move(view);
    BOOST_CHECK(view.publisher == nullptr);
    graph.add_edge(B, C, ep);
    BOOST_CHECK(edgeCallback);

}



BOOST_AUTO_TEST_CASE(tree_view_add_sub_tree_test)
{
    //merge two trees
    
    /*     A                         E
     *    / \                       /
     *   B   C                     F
     *        \                   /
     *         D  --merge here-- G
     *                            \ 
     *                             H
     */ 
    
    Gra graph;
    FrameId A("Arthur Dent");
    FrameId B("Prosser");
    FrameId C("Ford Prefect");
    FrameId D("Lady Cynthia Fitzmelton");
    FrameId E("The barman");
    FrameId F("Prostetnic Vogon Jeltz");
    FrameId G("Zaphod Beebleborx");
    FrameId H("Tricial McMillian");
    EdgeProp ep;
    
    //prepare graph (see picture above)
    graph.add_edge(A, B, ep);
    graph.add_edge(A, C, ep);
    graph.add_edge(C, D, ep);
    
    graph.add_edge(E, F, ep);
    graph.add_edge(F, G, ep);
    graph.add_edge(G, H, ep);
    
    TreeView view;
    graph.getTree(A, true, &view);
    
    GraphTraits::vertex_descriptor vA = graph.getVertex(A);
    GraphTraits::vertex_descriptor vB = graph.getVertex(B);
    GraphTraits::vertex_descriptor vC = graph.getVertex(C);
    GraphTraits::vertex_descriptor vD = graph.getVertex(D);
    GraphTraits::vertex_descriptor vE = graph.getVertex(E);
    GraphTraits::vertex_descriptor vF = graph.getVertex(F);
    GraphTraits::vertex_descriptor vG = graph.getVertex(G);
    GraphTraits::vertex_descriptor vH = graph.getVertex(H);
    
    BOOST_CHECK(view.tree.find(vG) == view.tree.end());
    BOOST_CHECK(view.tree.find(vF) == view.tree.end());
    BOOST_CHECK(view.tree.find(vE) == view.tree.end());
    BOOST_CHECK(view.tree.find(vH) == view.tree.end());
    
    //now add the transform that triggers the tree update
    graph.add_edge(D, G, ep);
    
    BOOST_CHECK(view.tree.find(vG) != view.tree.end());
    BOOST_CHECK(view.tree.find(vF) != view.tree.end());
    BOOST_CHECK(view.tree.find(vE) != view.tree.end());
    BOOST_CHECK(view.tree.find(vH) != view.tree.end());
    
    BOOST_CHECK(view.tree[vD].children.size() == 1);
    BOOST_CHECK(view.tree[vD].children.find(vG) != view.tree[vD].children.end());
    BOOST_CHECK(view.tree[vG].parent == vD);
    BOOST_CHECK(view.tree[vG].parentRelation == &view.tree[vD]);
    
    BOOST_CHECK(view.tree[vG].children.size() == 2);
    BOOST_CHECK(view.tree[vG].children.find(vH) != view.tree[vG].children.end());
    BOOST_CHECK(view.tree[vG].children.find(vF) != view.tree[vG].children.end());
    
    BOOST_CHECK(view.tree[vF].parent == vG);
    BOOST_CHECK(view.tree[vF].parentRelation == &view.tree[vG]);
    BOOST_CHECK(view.tree[vH].parent == vG);
    BOOST_CHECK(view.tree[vH].parentRelation == &view.tree[vG]);
    
    BOOST_CHECK(view.tree[vE].parent == vF);
    BOOST_CHECK(view.tree[vE].parentRelation == &view.tree[vF]);
    BOOST_CHECK(view.tree[vE].children.size() == 0);
    
    BOOST_CHECK(view.tree[vA].children.size() == 2);
    BOOST_CHECK(view.tree[vA].children.find(vB) != view.tree[vA].children.end());
    BOOST_CHECK(view.tree[vA].children.find(vC) != view.tree[vA].children.end());
    
    BOOST_CHECK(view.tree[vC].parent == vA);
    BOOST_CHECK(view.tree[vC].parentRelation == &view.tree[vA]);
    BOOST_CHECK(view.crossEdges.size() == 0);
    
}


BOOST_AUTO_TEST_CASE(get_tree_disconnected_graph_test)
{
  Gra g;
  FrameId A("A");
  g.addFrame(A);
  TreeView view;
  g.getTree(A, &view);
  GraphTraits::vertex_descriptor vA = g.getVertex(A);
  BOOST_CHECK(view.tree.find(vA) != view.tree.end());
}


BOOST_AUTO_TEST_CASE(tree_view_automatic_update_remove_test)
{
    Gra graph;
    FrameId A("A");
    FrameId B("B");
    FrameId C("C");
    EdgeProp ep;
     
    graph.add_edge(A, B, ep);
    graph.add_edge(B, C, ep);

    TreeView view;
    bool edgeAddedCalled = false;
    graph.getTree(A, true, &view);
    view.edgeAdded.connect([&edgeAddedCalled] (GraphTraits::vertex_descriptor,
                                               GraphTraits::vertex_descriptor) 
    {
        edgeAddedCalled = true;
    });
    
    graph.remove_edge(B, C);
    
    GraphTraits::vertex_descriptor vA = graph.getVertex(A);
    GraphTraits::vertex_descriptor vB = graph.getVertex(B);
    GraphTraits::vertex_descriptor vC = graph.getVertex(C);
    
    BOOST_CHECK(view.crossEdges.size() == 0);
    
    BOOST_CHECK(view.tree.find(vA) != view.tree.end());
    BOOST_CHECK(view.tree.find(vB) != view.tree.end());
    BOOST_CHECK(view.tree.find(vC) == view.tree.end());
    BOOST_CHECK(view.tree[vA].children.size() == 1);
    BOOST_CHECK(view.tree[vA].parent == graph.null_vertex());
    BOOST_CHECK(view.tree[vA].parentRelation == nullptr);
    BOOST_CHECK(edgeAddedCalled);
}

BOOST_AUTO_TEST_CASE(tree_edge_exists_test)
{
    Gra graph;
    FrameId A("A");
    FrameId B("B");
    FrameId C("C");
    EdgeProp ep;
    graph.add_edge(A, B, ep);
    graph.add_edge(B, C, ep);
    TreeView view = graph.getTree(A);
    GraphTraits::vertex_descriptor vA = graph.getVertex(A);
    GraphTraits::vertex_descriptor vB = graph.getVertex(B);
    GraphTraits::vertex_descriptor vC = graph.getVertex(C);
    BOOST_CHECK(view.edgeExists(vA, vB));
    BOOST_CHECK(view.edgeExists(vB, vA));
    BOOST_CHECK(!view.edgeExists(vA, vC));
    BOOST_CHECK(!view.edgeExists(vC, vA));
}

BOOST_AUTO_TEST_CASE(simple_modify_edge_prop_test_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    Gra graph;
    EdgeProp ep;
    graph.add_edge(a, b, ep);
    
    ep.value = 21;
    GraphTraits::vertex_descriptor aDesc = graph.getVertex(a);
    GraphTraits::vertex_descriptor bDesc = graph.getVertex(b);
    
    graph.setEdgeProperty(aDesc, bDesc, ep);
    BOOST_CHECK(graph.getEdgeProperty(aDesc, bDesc).value == 21);
    BOOST_CHECK(graph.getEdgeProperty(bDesc, aDesc).value == -21);
}

BOOST_AUTO_TEST_CASE(modify_edge_on_empty_graph_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    Gra graph;
    EdgeProp ep;
    BOOST_CHECK_THROW(graph.setEdgeProperty(a, b, ep), UnknownFrameException);
}


BOOST_AUTO_TEST_CASE(modify_invalid_edge_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    FrameId c = "frame_c";
    Gra graph;
    EdgeProp ep;
    graph.add_edge(a, b, ep);
    graph.addFrame(c);
    BOOST_CHECK_THROW(graph.setEdgeProperty(a, c, ep), UnknownEdgeException);
}

BOOST_AUTO_TEST_CASE(remove_edge_from_empty_graph_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    Gra graph;
    graph.addFrame(a);
    graph.addFrame(b);
    BOOST_CHECK_THROW(graph.remove_edge(a, b), UnknownEdgeException);
}

BOOST_AUTO_TEST_CASE(remove_non_existing_edge_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    FrameId c = "frame_c";
    Gra graph;
    EdgeProp ep;
    graph.add_edge(a, b, ep);
    BOOST_CHECK_THROW(graph.remove_edge(a, c), UnknownFrameException);
}

BOOST_AUTO_TEST_CASE(get_invalid_edge_property_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    FrameId c = "frame_c";
    Gra graph;
    EdgeProp ep;
    BOOST_CHECK_NO_THROW(graph.add_edge(a, b, ep));
    BOOST_CHECK_THROW(graph.getEdgeProperty(a, c), UnknownFrameException);
}

BOOST_AUTO_TEST_CASE(remove_transform_event_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    Gra graph;
    EdgeProp ep;
    graph.add_edge(a, b, ep);
    Dispatcher d(graph);
    graph.remove_edge(a, b);
    BOOST_CHECK(d.edgeRemovedEvents.size() == 1);
    BOOST_CHECK(d.edgeRemovedEvents[0].origin == a);
    BOOST_CHECK(d.edgeRemovedEvents[0].target == b);
}

BOOST_AUTO_TEST_CASE(frame_added_event_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    Gra graph;
    EdgeProp ep;
    Dispatcher d(graph);
    graph.addFrame(a);
    BOOST_CHECK(d.frameAddedEvents.size() == 1);
    BOOST_CHECK(d.frameAddedEvents[0].frame == a);
    
    graph.add_edge(a, b, ep);
    BOOST_CHECK(d.frameAddedEvents.size() == 2);
    BOOST_CHECK(d.frameAddedEvents[1].frame == b);

    FrameId c = "frame_c";
    FrameId e = "frame_d";
    graph.add_edge(c, e, ep);
    BOOST_CHECK(d.frameAddedEvents.size() == 4);
    BOOST_CHECK(d.frameAddedEvents[2].frame == c);
    BOOST_CHECK(d.frameAddedEvents[3].frame == e);
}


BOOST_AUTO_TEST_CASE(modify_edge_property_event_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    Gra graph;
    EdgeProp ep;
    graph.add_edge(a, b, ep);

    Dispatcher d(graph);
    graph.setEdgeProperty(b, a, ep);
    
    BOOST_CHECK(d.edgeModifiedEvents.size() == 1);
    BOOST_CHECK(d.edgeModifiedEvents[0].origin == b);
    BOOST_CHECK(d.edgeModifiedEvents[0].target == a);
    BOOST_CHECK(d.edgeModifiedEvents[0].edge == graph.getEdge(b, a));
    BOOST_CHECK(d.edgeModifiedEvents[0].inverseEdge == graph.getEdge(a, b));
}

BOOST_AUTO_TEST_CASE(frame_removed_event_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    Gra graph;
    EdgeProp ep;
    graph.addFrame(a);
    graph.addFrame(b);
    
    Dispatcher d(graph);
    graph.removeFrame(a);
    BOOST_CHECK(d.frameRemovedEvents.size() == 1);
    BOOST_CHECK(d.frameRemovedEvents[0].frame == a);
    graph.removeFrame(b);
    BOOST_CHECK(d.frameRemovedEvents.size() == 2);
    BOOST_CHECK(d.frameRemovedEvents[1].frame == b);
    
}

BOOST_AUTO_TEST_CASE(get_path_test)
{
    Gra graph;
    FrameId A("A");
    FrameId B("B");
    FrameId C("C");
    FrameId D("D");
    FrameId E("E");
    FrameId F("F");
    FrameId G("G");
    
    EdgeProp ep;
    
    graph.add_edge(A, B, ep);
    graph.add_edge(B, C, ep);
    graph.add_edge(B, F, ep);
    graph.add_edge(A, E, ep);
    graph.add_edge(E, C, ep);
    graph.add_edge(C, D, ep);
    graph.add_edge(D, G, ep);
    graph.add_edge(C, F, ep);
    
    const vector<FrameId> path = graph.getPath(A, D);
    
    BOOST_CHECK(path[0] == A);
    BOOST_CHECK(path[1] == B);
    BOOST_CHECK(path[2] == C);
    BOOST_CHECK(path[3] == D);
}

BOOST_AUTO_TEST_CASE(get_path_invalid_frame_test)
{
    Gra graph;
    FrameId A("A");
    FrameId B("B");
    
    graph.addFrame(A);
    BOOST_CHECK_THROW(graph.getPath(A, B), UnknownFrameException);
    BOOST_CHECK_THROW(graph.getPath(B, A), UnknownFrameException);
}

BOOST_AUTO_TEST_CASE(get_empty_path_test)
{
    Gra graph;
    FrameId A("A");
    FrameId B("B");
    FrameId C("C");
    EdgeProp ep;
    
    graph.add_edge(A, B, ep);
    graph.addFrame(C);
    const vector<FrameId> path = graph.getPath(A, C);
    BOOST_CHECK(path.size() == 0);
}

BOOST_AUTO_TEST_CASE(remove_unknown_frame_test)
{
    FrameId a = "frame_a";
    Gra graph;
    BOOST_CHECK_THROW(graph.removeFrame(a), UnknownFrameException);
}


BOOST_AUTO_TEST_CASE(get_edge_invalid_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    FrameId c = "frame_c";
    Gra graph;
    EdgeProp ep;
    graph.add_edge(a, b, ep);
    graph.add_edge(a, c, ep);
    BOOST_CHECK_THROW(graph.getEdge(b, c), UnknownEdgeException);
}

BOOST_AUTO_TEST_CASE(get_edge_on_empty_graph_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    Gra g;
    BOOST_CHECK_THROW(g.getEdge(a, b), UnknownFrameException);
}


BOOST_AUTO_TEST_CASE(disconnect_frame_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    FrameId c = "frame_c";
    Gra graph;
    EdgeProp ep;
    graph.add_edge(a, b, ep);
    graph.add_edge(a, c, ep);
    graph.disconnectFrame(a);
    BOOST_CHECK_THROW(graph.getEdge(a, b), UnknownEdgeException);
    BOOST_CHECK_THROW(graph.getEdge(b, a), UnknownEdgeException);
    BOOST_CHECK_THROW(graph.getEdge(a, c), UnknownEdgeException);
    BOOST_CHECK_THROW(graph.getEdge(c, a), UnknownEdgeException); 
    
    FrameId d = "frame_d";
    BOOST_CHECK_THROW(graph.disconnectFrame(d), UnknownFrameException);
    
}

BOOST_AUTO_TEST_CASE(remove_frame_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    FrameId c = "frame_c";
    Gra graph;
    EdgeProp ep;
    graph.add_edge(a, b, ep);
    graph.add_edge(a, c, ep);
    
    BOOST_CHECK_THROW(graph.removeFrame(a), FrameStillConnectedException);
    graph.disconnectFrame(a);
    graph.removeFrame(a);
    BOOST_CHECK_THROW(graph.getVertex(a), UnknownFrameException);
}


BOOST_AUTO_TEST_CASE(add_edge_existing_vertex_test)
{ 
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    Gra graph;
    graph.addFrame(a);
    graph.addFrame(b);
    
    GraphTraits::vertex_descriptor aDesc = graph.getVertex(a);
    GraphTraits::vertex_descriptor bDesc = graph.getVertex(b);
    
    EdgeProp ep;
    graph.add_edge(aDesc, bDesc, ep);
    BOOST_CHECK_NO_THROW(graph.getEdge(a, b));
    
    
}

BOOST_AUTO_TEST_CASE(graph_graphviz_test)
{
    Gra graph;
    
    FrameId a("Captain Benjamin Sisko");
    FrameId b("Major Kira Nerys");
    FrameId c("Dr. Julian Bashir");
    FrameId d("Lieutenant Commander Jadzia Dax");
    FrameId e(" Lieutenant Ezri Dax");

    EdgeProp ep;
    graph.add_edge(a,b, ep);
    graph.add_edge(a,c, ep);
    graph.add_edge(c,d, ep);
    graph.add_edge(d,e, ep);
    graph.add_edge(e,a, ep);
    
  
    GraphViz viz;
    viz.write(graph, "graph_graphviz_test.dot");
}


struct StringProperty
{ 
    string value;
    FrameId id;
    StringProperty() : value("wrong") {}
    StringProperty(string value) : value(value) {}
    const FrameId& getId() const { return id;}
    void setId(const FrameId& _id){id = _id;}
    const string toGraphviz() const{return "[label=\"bla\"]";}
    template<class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        ar & value;
    }
};

BOOST_AUTO_TEST_CASE(emplace_frame_test)
{
    envire::core::Graph<StringProperty, EdgeProp> graph;
    FrameId id("frrrraaaaammmeee");
    
    
    graph.emplaceFrame(id, "blabla");
    
    StringProperty prop = graph.getFrameProperty(id);
    BOOST_CHECK(prop.value.compare("blabla") == 0);
}


BOOST_AUTO_TEST_CASE(disconnect_frame_event_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    FrameId c = "frame_c";
    Gra graph;
    EdgeProp ep;
    graph.add_edge(a, b, ep);
    graph.add_edge(a, c, ep);
    
    Dispatcher d(graph);
    graph.disconnectFrame(a);
    BOOST_CHECK(d.edgeRemovedEvents.size() == 2);
    BOOST_CHECK(d.edgeRemovedEvents[0].origin == a);
    BOOST_CHECK(d.edgeRemovedEvents[0].target == b);
    BOOST_CHECK(d.edgeRemovedEvents[1].origin == a);
    BOOST_CHECK(d.edgeRemovedEvents[1].target == c);
}


BOOST_AUTO_TEST_CASE(serialization_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    FrameId c = "frame_c";
    Gra graph;
    EdgeProp ab;
    ab.value = 22;
    EdgeProp ac;
    ac.value = 44;
    graph.add_edge(a, b, ab);
    graph.add_edge(a, c, ac);
    
    std::stringstream stream;
    boost::archive::polymorphic_binary_oarchive oa(stream);
    oa << graph;
    boost::archive::polymorphic_binary_iarchive ia(stream);
    Gra graph2;
    ia >> graph2;   
    
    BOOST_CHECK(graph2.num_edges() == graph.num_edges());
    BOOST_CHECK(graph2.num_vertices() == graph.num_vertices());
    //check if vertices exist
    BOOST_CHECK_NO_THROW(graph2.getVertex(a));
    BOOST_CHECK_NO_THROW(graph2.getVertex(b));
    BOOST_CHECK_NO_THROW(graph2.getVertex(c));
    //check if edges exist
    BOOST_CHECK_NO_THROW(graph2.getEdge(a, b));
    BOOST_CHECK_NO_THROW(graph2.getEdge(b, a));
    BOOST_CHECK_NO_THROW(graph2.getEdge(a, c));
    BOOST_CHECK_NO_THROW(graph2.getEdge(c, a));
    //check if edge property was loaded correctly
    BOOST_CHECK(graph2.getEdgeProperty(a, b).value == ab.value);
    BOOST_CHECK(graph2.getEdgeProperty(a, c).value == ac.value);
}

BOOST_AUTO_TEST_CASE(copy_ctor_test)
{
    using StringGraph = envire::core::Graph<StringProperty, EdgeProp>;
    StringGraph graph;
    FrameId a("f1");
    FrameId b("f2");
    FrameId c("f3");
    EdgeProp ab;
    ab.value = 22;
    EdgeProp ac;
    ac.value = 44;    

    graph.emplaceFrame(a, "blabla");
    graph.emplaceFrame(b, "muhhh");
    graph.emplaceFrame(c, "meeeh");
    graph.add_edge(a, b, ab);
    graph.add_edge(a, c, ac);

    StringGraph graph2(graph);
    
    BOOST_CHECK(graph2.num_edges() == 4);
    BOOST_CHECK(graph2.num_vertices() == 3);
    BOOST_CHECK(graph2.getEdgeProperty(a, b).value == 22);
    BOOST_CHECK(graph2.getEdgeProperty(a, c).value == 44);
    BOOST_CHECK_NO_THROW(graph2.getVertex(a));
    BOOST_CHECK_NO_THROW(graph2.getVertex(b));
    BOOST_CHECK_NO_THROW(graph2.getVertex(c));
    BOOST_CHECK(graph2.getFrameProperty(a).value == "blabla");
    BOOST_CHECK(graph2.getFrameProperty(b).value == "muhhh");
    BOOST_CHECK(graph2.getFrameProperty(c).value == "meeeh");
}

BOOST_AUTO_TEST_CASE(treeview_dfsVisit_test)
{
    FrameId a = "a";
    FrameId b = "b";
    FrameId c = "c";
    FrameId d = "d";
    FrameId e = "e";
    FrameId f = "f";
    FrameId g = "g";
    
    Gra graph;
    EdgeProp ep;
    graph.add_edge(a, b, ep);
    graph.add_edge(b, c, ep);
    graph.add_edge(b, d, ep);
    graph.add_edge(a, e, ep);
    graph.add_edge(a, f, ep);
    graph.add_edge(f, g, ep);
    
    TreeView tv = graph.getTree(a);
    
    FrameId expectedOrder[] = {"a", "f", "g", "e", "b", "d", "c"};
    FrameId expectedParent[] = {"", "a", "f", "a", "a", "b", "b"};
    
    int i = 0;
    tv.visitDfs(graph.getVertex(a), [&](GraphTraits::vertex_descriptor vd, 
                                        GraphTraits::vertex_descriptor parent)
      { 
        const FrameId id = graph.getFrameId(vd);
        BOOST_CHECK(id == expectedOrder[i]);
        if(expectedParent[i] == "") 
          BOOST_CHECK(parent == GraphTraits::null_vertex());
        else
        {
          const FrameId parentId = graph.getFrameId(parent);
          BOOST_CHECK(expectedParent[i] == parentId);
        }
        ++i;
      });
}


BOOST_AUTO_TEST_CASE(treeview_bfsVisit_test)
{
    FrameId a = "a";
    FrameId b = "b";
    FrameId c = "c";
    FrameId d = "d";
    FrameId e = "e";
    FrameId f = "f";
    FrameId g = "g";
    
    Gra graph;
    EdgeProp ep;
    graph.add_edge(a, b, ep);
    graph.add_edge(a, c, ep);
    graph.add_edge(b, d, ep);
    graph.add_edge(b, e, ep);
    graph.add_edge(e, f, ep);
    graph.add_edge(c, g, ep);

    TreeView tv = graph.getTree(a);
    
    //the order could just as well be a b c d e g f, this is just how it is
    //implemented right now. 
    FrameId expectedOrder[] = {"a", "c", "b", "g", "e", "d", "f"};
    FrameId expectedParent[] = {"", "a", "a", "c", "b", "b", "e"};
    
    int i = 0;
    tv.visitBfs(graph.getVertex(a), [&](GraphTraits::vertex_descriptor vd, 
                                        GraphTraits::vertex_descriptor parent)
      { 
        const FrameId id = graph.getFrameId(vd);
        BOOST_CHECK(id == expectedOrder[i]);
        if(expectedParent[i] == "") 
          BOOST_CHECK(parent == GraphTraits::null_vertex());
        else
        {
          const FrameId parentId = graph.getFrameId(parent);
          BOOST_CHECK(expectedParent[i] == parentId);
        }
        ++i;
      });
}



BOOST_AUTO_TEST_CASE(ctor_copy_test)
{
  Gra* copy;
  {
    Gra graph;
    
    FrameProp f1, f2, f3;
    f1.setId("AAA");
    f2.setId("BBB");
    f3.setId("CCC");
        
    const Gra::vertex_descriptor v1 = graph.add_vertex("AAA", f1);
    const Gra::vertex_descriptor v2 = graph.add_vertex("BBB", f2);
    const Gra::vertex_descriptor v3 = graph.add_vertex("CCC", f3);
    
    copy = new Gra(graph);
    
    //calling setId directly is not something that the user should do.
    //It breaks the internal structure of the graph!
    graph["AAA"].setId("BLA");
    graph["BBB"].setId("BLA");
    graph["CCC"].setId("BLA");
      
    BOOST_CHECK(graph["AAA"].getId() == "BLA");
    BOOST_CHECK(graph["BBB"].getId() == "BLA");
    BOOST_CHECK(graph["CCC"].getId() == "BLA");

    BOOST_CHECK((*copy)["AAA"].getId() == "AAA");
    BOOST_CHECK((*copy)["BBB"].getId() == "BBB");
    BOOST_CHECK((*copy)["CCC"].getId() == "CCC");
  }
  
    BOOST_CHECK((*copy)["AAA"].getId() == "AAA");
    BOOST_CHECK((*copy)["BBB"].getId() == "BBB");
    BOOST_CHECK((*copy)["CCC"].getId() == "CCC");
}

BOOST_AUTO_TEST_CASE(test_tree_view_events_test)
{
    using vertex_descriptor = GraphTraits::vertex_descriptor;
    using edge_descriptor = GraphTraits::edge_descriptor;
    Gra graph;
    EdgeProp ep;
    TreeView tv;
    std::vector<vertex_descriptor> origins;
    std::vector<vertex_descriptor> targets;
    tv.edgeAdded.connect([&](vertex_descriptor origin, vertex_descriptor target) 
        {
            origins.push_back(origin);
            targets.push_back(target);
        });
    
    std::vector<TreeView::CrossEdge> crossEdges;
    tv.crossEdgeAdded.connect([&](const TreeView::CrossEdge& edge)
        {
            crossEdges.push_back(edge);
        });

    FrameId a = "frame_a";
    graph.addFrame(a);
    graph.getTree(a, true, &tv);
    

    FrameId b = "frame_b";
    FrameId c = "frame_c";
    FrameId d = "frame_d";
    FrameId e = "frame_e";

    graph.add_edge(a, b, ep);
    BOOST_CHECK(origins[0] == graph.getVertex(a));
    BOOST_CHECK(targets[0] == graph.getVertex(b));
    
    graph.add_edge(a, c, ep);
    BOOST_CHECK(origins[1] == graph.getVertex(a));
    BOOST_CHECK(targets[1] == graph.getVertex(c));
    
    graph.add_edge(c, d, ep);
    BOOST_CHECK(origins[2] == graph.getVertex(c));
    BOOST_CHECK(targets[2] == graph.getVertex(d)); 

    graph.add_edge(d, e, ep);
    BOOST_CHECK(origins[3] == graph.getVertex(d));
    BOOST_CHECK(targets[3] == graph.getVertex(e));
    
    graph.add_edge(b, e, ep);
    //check that no new edge has been added
    BOOST_CHECK(origins.size() == 4);
    BOOST_CHECK(targets.size() == 4);
    //check that a cross edge has been added instead
    BOOST_CHECK(crossEdges[0].edge == graph.getEdge(b, e) || crossEdges[0].edge == graph.getEdge(e, b));

}

BOOST_AUTO_TEST_CASE(publish_current_state_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    FrameId c = "frame_c";
    Gra graph;
    EdgeProp ep;

    graph.add_edge(a, b, ep);
    graph.add_edge(a, c, ep);

    Dispatcher d;
    graph.subscribe(&d, true);

    BOOST_CHECK(d.frameAddedEvents.size() == 3);
    BOOST_CHECK(d.edgeAddedEvents.size() == 2);
    BOOST_CHECK(d.frameRemovedEvents.size() == 0);
    BOOST_CHECK(d.edgeRemovedEvents.size() == 0);

    graph.unsubscribe(&d, true);

    BOOST_CHECK(d.frameRemovedEvents.size() == 3);
    BOOST_CHECK(d.edgeRemovedEvents.size() == 2);
}

BOOST_AUTO_TEST_CASE(event_queue_test)
{
    Gra graph;
    EventQueue queue(graph);

    FrameId a = "frame_a";
    FrameId b = "frame_b";
    FrameId c = "frame_c";
    EdgeProp ep;
    EdgeProp ep_1;
    EdgeProp ep_2;

    graph.addFrame(a);

    graph.add_edge(a, b, ep);
    graph.add_edge(a, c, ep);

    graph.setEdgeProperty(a,b,ep_1);
    graph.setEdgeProperty(a,b,ep_2);
    graph.setEdgeProperty(a,c,ep_1);
    graph.setEdgeProperty(a,c,ep_2);

    graph.remove_edge(a,c);

    graph.removeFrame(c);

    queue.flush();

    BOOST_CHECK(queue.dispatcher.frameAddedEvents.size() == 2);
    BOOST_CHECK(queue.dispatcher.frameRemovedEvents.size() == 0);
    BOOST_CHECK(queue.dispatcher.edgeAddedEvents.size() == 1);
    BOOST_CHECK(queue.dispatcher.edgeModifiedEvents.size() == 1);
    BOOST_CHECK(queue.dispatcher.edgeRemovedEvents.size() == 0);
}

