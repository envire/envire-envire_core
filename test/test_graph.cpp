#define protected public
#include <boost/test/unit_test.hpp>
#include <envire_core/graph/Graph.hpp>
#include <envire_core/events/GraphEventDispatcher.hpp>
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
    
    GraphTraits::edge_descriptor edge = view.crossEdges[0];
    GraphTraits::vertex_descriptor source = graph.source(edge);
    GraphTraits::vertex_descriptor target = graph.target(edge);
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
    const GraphTraits::vertex_descriptor src = graph.source(view.crossEdges[0]);
    const GraphTraits::vertex_descriptor tar = graph.target(view.crossEdges[0]);
    //note: it is not specified whether src->tar or tar->src ends up in the cross
    //      edges. The current implementation adds tar->src but that can change.
    BOOST_CHECK(tar == vC);
    BOOST_CHECK(src == vD);
}

BOOST_AUTO_TEST_CASE(tree_view_update_event_test)
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
    bool updateCalled = false;
    view.treeUpdated.connect([&updateCalled] () 
      {
          updateCalled = true;
      });
    graph.getTree(A, true, &view);
    
    graph.add_edge(B, C, ep);
    BOOST_CHECK(updateCalled);    
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
    bool updateCalled = false;
    view.treeUpdated.connect([&updateCalled] () 
      {
          updateCalled = true;
      });
    graph.getTree(A, true, &view);
    
    TreeView newView = std::move(view);
    BOOST_CHECK(view.publisher == nullptr);
    graph.add_edge(B, C, ep);
    BOOST_CHECK(updateCalled);    
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
    bool updateCalled = false;
    view.treeUpdated.connect([&updateCalled] () 
    {
        updateCalled = true;
    });
    graph.getTree(A, true, &view);
    
    graph.remove_edge(A, B);
    
    GraphTraits::vertex_descriptor vA = graph.getVertex(A);
    GraphTraits::vertex_descriptor vB = graph.getVertex(B);
    GraphTraits::vertex_descriptor vC = graph.getVertex(C);
    
    BOOST_CHECK(view.crossEdges.size() == 0);
    BOOST_CHECK(view.tree.find(vA) != view.tree.end());
    BOOST_CHECK(view.tree.find(vB) == view.tree.end());
    BOOST_CHECK(view.tree.find(vC) == view.tree.end());
    BOOST_CHECK(view.tree[vA].children.size() == 0);
    BOOST_CHECK(view.tree[vA].parent == graph.null_vertex());
    BOOST_CHECK(view.tree[vA].parentRelation == nullptr);
    BOOST_CHECK(updateCalled);
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
    BOOST_CHECK(d.edgeRemovedEvents.size() == 2);
    BOOST_CHECK(d.edgeRemovedEvents[0].origin == a);
    BOOST_CHECK(d.edgeRemovedEvents[0].target == b);
    BOOST_CHECK(d.edgeRemovedEvents[1].origin == b);
    BOOST_CHECK(d.edgeRemovedEvents[1].target == a);
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
    BOOST_CHECK(d.frameAddedEvents[0].addedFrame == a);
    
    graph.add_edge(a, b, ep);
    BOOST_CHECK(d.frameAddedEvents.size() == 2);
    BOOST_CHECK(d.frameAddedEvents[1].addedFrame == b);

    FrameId c = "frame_c";
    FrameId e = "frame_d";
    graph.add_edge(c, e, ep);
    BOOST_CHECK(d.frameAddedEvents.size() == 4);
    BOOST_CHECK(d.frameAddedEvents[2].addedFrame == c);
    BOOST_CHECK(d.frameAddedEvents[3].addedFrame == e);
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
    BOOST_CHECK(d.frameRemovedEvents[0].removedFrame == a);
    graph.removeFrame(b);
    BOOST_CHECK(d.frameRemovedEvents.size() == 2);
    BOOST_CHECK(d.frameRemovedEvents[1].removedFrame == b);
    
}



