#include <boost/test/unit_test.hpp>
#include <envire_core/all>
#include <vector>

using namespace envire::core;
using namespace std;

bool compareTransform(const Transform& a, const Transform& b)
{
    return a.transform.translation.x() == b.transform.translation.x() &&
           a.transform.translation.y() == b.transform.translation.y() &&
           a.transform.translation.z() == b.transform.translation.z() &&
           a.transform.orientation.x() == b.transform.orientation.x() &&
           a.transform.orientation.y() == b.transform.orientation.y() &&
           a.transform.orientation.z() == b.transform.orientation.z() &&
           a.transform.orientation.w() == b.transform.orientation.w();
}

class Dispatcher : public GraphEventDispatcher {
public:
    vector<TransformAddedEvent> transformAddedEvent;
    vector<TransformModifiedEvent> transformModifiedEvent;
    vector<TransformRemovedEvent> transformRemovedEvent;

    virtual void transformAdded(const TransformAddedEvent& e)
    {
        transformAddedEvent.push_back(e);
    }

    virtual void transformModified(const TransformModifiedEvent& e)
    {
        transformModifiedEvent.push_back(e);
    }

    virtual void transformRemoved(const TransformRemovedEvent& e)
    {
        transformRemovedEvent.push_back(e);
    }
};

BOOST_AUTO_TEST_CASE(remove_transform_event_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    TransformGraph tree;
    Transform tf;
    tf.transform.translation << 42, 21, -42;
    tf.transform.orientation = base::AngleAxisd(0.25, base::Vector3d::UnitX());
    tree.addTransform(a, b, tf);
    std::shared_ptr<Dispatcher> d(new Dispatcher());
    tree.subscribe(d);  
    tree.removeTransform(a, b);
    BOOST_CHECK(d->transformRemovedEvent.size() == 2);
    BOOST_CHECK(d->transformRemovedEvent[0].origin == a);
    BOOST_CHECK(d->transformRemovedEvent[0].target == b);
    BOOST_CHECK(d->transformRemovedEvent[1].origin == b);
    BOOST_CHECK(d->transformRemovedEvent[1].target == a);
}

BOOST_AUTO_TEST_CASE(simple_remove_transform_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    TransformGraph tree;
    Transform tf;
    tf.transform.translation << 42, 21, -42;
    tf.transform.orientation = base::AngleAxisd(0.25, base::Vector3d::UnitX());
    tree.addTransform(a, b, tf);

    tree.removeTransform(b, a);
    BOOST_CHECK(tree.num_edges() == 0);
    BOOST_CHECK(tree.num_vertices() == 0);
}

BOOST_AUTO_TEST_CASE(remove_existing_but_unconnected_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    FrameId c = "frame_c";
    TransformGraph tree;
    Transform tf;
    tree.addTransform(a, b, tf);
    tree.addTransform(a, c, tf);
    BOOST_CHECK_THROW(tree.removeTransform(b, c), UnknownTransformException);
}

BOOST_AUTO_TEST_CASE(complex_remove_transform_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    FrameId c = "frame_c";
    TransformGraph tree;
    Transform tf;
    tf.transform.translation << 42, 21, -42;
    tf.transform.orientation = base::AngleAxisd(0.25, base::Vector3d::UnitX());
    tree.addTransform(a, b, tf);
    tree.addTransform(a, c, tf);

    tree.removeTransform(b, a);
    BOOST_CHECK(tree.num_edges() == 2);
    BOOST_CHECK(tree.num_vertices() == 2);
    BOOST_CHECK_NO_THROW(tree.getFrame(a));
    BOOST_CHECK_NO_THROW(tree.getFrame(c));
    BOOST_CHECK_THROW(tree.getFrame(b), UnknownFrameException);
}

BOOST_AUTO_TEST_CASE(simple_modify_transform_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    TransformGraph tree;
    Transform tf;
    tf.transform.translation << 42, 21, -42;
    tf.transform.orientation = base::AngleAxisd(0.25, base::Vector3d::UnitX());
    BOOST_CHECK_NO_THROW(tree.addTransform(a, b, tf));

    Transform tf2;
    tf2.transform.translation << 0, 1, 2;
    tf2.transform.orientation = base::AngleAxisd(0.42, base::Vector3d::UnitY());
    tree.updateTransform(b, a, tf2);
    Transform readBa = tree.getTransform(b, a);
    Transform readAb = tree.getTransform(a, b);
    BOOST_CHECK(compareTransform(readBa, tf2));

    Transform tf2Inv = tf2;
    tf2Inv.setTransform(tf2.transform.inverse());
    BOOST_CHECK(compareTransform(readAb, tf2Inv));
}

BOOST_AUTO_TEST_CASE(update_transform_on_empty_tree_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    TransformGraph tree;
    Transform tf;
    BOOST_CHECK_THROW(tree.updateTransform(a, b, tf), UnknownTransformException);
}

BOOST_AUTO_TEST_CASE(update_transform_invalid_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    FrameId c = "frame_c";
    TransformGraph tree;
    Transform tf;
    tree.addTransform(a, b, tf);
    BOOST_CHECK_THROW(tree.updateTransform(a, c, tf), UnknownTransformException);
}

BOOST_AUTO_TEST_CASE(remove_transform_on_empty_tree_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    TransformGraph tree;
    BOOST_CHECK_THROW(tree.removeTransform(a, b), UnknownTransformException);
}

BOOST_AUTO_TEST_CASE(remove_transform_invalid_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    FrameId c = "frame_c";
    TransformGraph tree;
    Transform tf;
    tree.addTransform(a, b, tf);
    BOOST_CHECK_THROW(tree.removeTransform(a, c), UnknownTransformException);
}

BOOST_AUTO_TEST_CASE(get_invalid_transform_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    FrameId c = "frame_c";
    TransformGraph tree;
    Transform tf;
    tf.transform.translation << 42, 21, -42;
    tf.transform.orientation = base::AngleAxisd(0.25, base::Vector3d::UnitX());
    BOOST_CHECK_NO_THROW(tree.addTransform(a, b, tf));
    BOOST_CHECK_THROW(tree.getTransform(a, c), UnknownTransformException);
}

BOOST_AUTO_TEST_CASE(get_edge_on_empty_tree_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    TransformGraph tree;
    BOOST_CHECK_THROW(tree.getEdge(a, b), UnknownTransformException);
}

BOOST_AUTO_TEST_CASE(get_edge_invalid_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    FrameId c = "frame_c";
    TransformGraph tree;
    Transform tf;
    tree.addTransform(a, b, tf);
    tree.addTransform(a, c, tf);
    BOOST_CHECK_THROW(tree.getEdge(b, c), UnknownTransformException);
}


BOOST_AUTO_TEST_CASE(modify_transform_event_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    TransformGraph tree;
    Transform tf;
    tf.transform.translation << 42, 21, -42;
    tf.transform.orientation = base::AngleAxisd(0.25, base::Vector3d::UnitX());
    tree.addTransform(a, b, tf);

    std::shared_ptr<Dispatcher> d(new Dispatcher());
    tree.subscribe(d);
    Transform tf2;
    tf2.transform.translation << 0, 1, 2; 
    tf2.transform.orientation = base::AngleAxisd(0.42, base::Vector3d::UnitY());
    tree.updateTransform(a, b, tf2);
    BOOST_CHECK(d->transformModifiedEvent.size() == 1);
    BOOST_CHECK(compareTransform(d->transformModifiedEvent[0].newTransform, tf2));
    BOOST_CHECK(compareTransform(d->transformModifiedEvent[0].oldTransform, tf));
    Transform tf2Inv = tf2;
    tf2Inv.setTransform(tf2.transform.inverse());
    BOOST_CHECK(compareTransform(d->transformModifiedEvent[0].newInverseTransform, tf2Inv));
    Transform tfInv = tf;
    tfInv.setTransform(tf.transform.inverse());
    BOOST_CHECK(compareTransform(d->transformModifiedEvent[0].oldInverseTransform, tfInv));
    
    BOOST_CHECK(d->transformModifiedEvent[0].origin == a);
    BOOST_CHECK(d->transformModifiedEvent[0].target == b);
    BOOST_CHECK(d->transformModifiedEvent[0].edge == tree.getEdge(a, b));
    BOOST_CHECK(d->transformModifiedEvent[0].inverseEdge == tree.getEdge(b, a));
}

BOOST_AUTO_TEST_CASE(operations_with_transform)
{
    Transform tf1, tf2;
    tf1.transform.translation << 42, 21, -42;
    tf1.transform.orientation = base::AngleAxisd(0.25, base::Vector3d::UnitX());
    tf2.transform.translation << 42, 21, -42;
    tf2.transform.orientation = base::AngleAxisd(0.25, base::Vector3d::UnitX());

    Transform tf_manual;
    tf_manual.setTransform(tf1.transform * tf2.transform);
    Transform tf_operator(tf1 * tf2);
    BOOST_CHECK(compareTransform(tf_operator, tf_manual));
}

BOOST_AUTO_TEST_CASE(simple_add_get_transform_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    TransformGraph tree;
    Transform tf;
    tf.transform.translation << 42, 21, -42;
    tf.transform.orientation = base::AngleAxisd(0.25, base::Vector3d::UnitX());
    BOOST_CHECK_NO_THROW(tree.addTransform(a, b, tf));
    BOOST_CHECK(tree.num_edges() == 2);
    BOOST_CHECK(tree.num_vertices() == 2);
    Transform readTf = tree.getTransform(a, b);
    BOOST_CHECK(compareTransform(readTf, tf));
    Transform invTf = tf;
    invTf.setTransform(tf.transform.inverse());
    Transform readTfInv = tree.getTransform(b, a);
    BOOST_CHECK(compareTransform(readTfInv, invTf));
}

BOOST_AUTO_TEST_CASE(complex_add_get_transform_test)
{

    FrameId a = "frame_a";
    FrameId b = "frame_b";
    FrameId c = "frame_c";
    FrameId d = "frame_d";
    TransformGraph tree;
    Transform tf;
    tf.transform.translation << 42, 21, -42;
    tf.transform.orientation = base::AngleAxisd(0.25, base::Vector3d::UnitX());
    BOOST_CHECK_NO_THROW(tree.addTransform(a, b, tf));
    BOOST_CHECK_NO_THROW(tree.addTransform(b, c, tf));
    BOOST_CHECK_NO_THROW(tree.addTransform(a, d, tf));
    BOOST_CHECK(tree.num_edges() == 6);
    BOOST_CHECK(tree.num_vertices() == 4);

    /* a -> c **/
    BOOST_TEST_MESSAGE( "a - > c" );
    Transform readTf;
    BOOST_CHECK_NO_THROW(readTf = tree.getTransform(a, c));
    BOOST_CHECK(compareTransform(readTf, tf*tf));

    /* c -> a **/
    BOOST_TEST_MESSAGE( "c - > a" );
    Transform invTf;
    invTf.setTransform(tf.transform.inverse() * tf.transform.inverse());
    Transform readTfInv;
    BOOST_CHECK_NO_THROW(readTfInv = tree.getTransform(c, a));
    BOOST_CHECK(compareTransform(readTfInv, invTf));

    /* a -> d **/
    BOOST_TEST_MESSAGE( "a - > d" );
    BOOST_CHECK_NO_THROW(readTf = tree.getTransform(a, d));
    BOOST_CHECK(compareTransform(readTf, tf));

    /* c -> d **/
    BOOST_TEST_MESSAGE( "c - > d" );
    Transform complexTf;
    complexTf.setTransform(tf.transform.inverse()*tf.transform.inverse()*tf.transform);
    BOOST_CHECK_NO_THROW(readTf = tree.getTransform(c, d));
    BOOST_CHECK(compareTransform(readTf, complexTf));

    /* d -> c **/
    BOOST_TEST_MESSAGE( "d - > c" );
    complexTf.setTransform(tf.transform.inverse()*tf.transform*tf.transform);
    BOOST_CHECK_NO_THROW(readTf = tree.getTransform(d, c));
    BOOST_CHECK(compareTransform(readTf, complexTf));

    //Close a cycle with an extra frame.
    //In practice it should not happen in envire
    FrameId e = "frame_e";
    BOOST_CHECK_NO_THROW(tree.addTransform(d, e, tf));
    BOOST_CHECK_NO_THROW(tree.addTransform(e, c, tf));

    /* d -> c **/
    BOOST_TEST_MESSAGE( "d - > c" );
    BOOST_CHECK_NO_THROW(readTf = tree.getTransform(d, c));
    BOOST_CHECK(compareTransform(readTf, tf*tf));

    /* a-> e **/
    BOOST_TEST_MESSAGE( "a - > e" );
    BOOST_CHECK_NO_THROW(readTf = tree.getTransform(a, e));
    BOOST_CHECK(compareTransform(readTf, tf*tf));

    FrameId f = "frame_f";
    BOOST_CHECK_NO_THROW(tree.addTransform(d, f, tf));

    /* c -> f **/
    BOOST_TEST_MESSAGE( "c - > f" );
    BOOST_CHECK_NO_THROW(readTf = tree.getTransform(c, f));
    complexTf.setTransform(tf.transform.inverse()*tf.transform.inverse()*tf.transform);
    BOOST_CHECK(compareTransform(readTf, complexTf));

}


BOOST_AUTO_TEST_CASE(add_transform_exception_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    TransformGraph tree;
    Transform tf;
    BOOST_CHECK_NO_THROW(tree.addTransform(a, b, tf));
    BOOST_CHECK_THROW(tree.addTransform(b, a, tf), TransformAlreadyExistsException);
}

BOOST_AUTO_TEST_CASE(get_transform_exception_test)
{
    FrameId a = "frame_a";
    FrameId c = "frame_c";
    TransformGraph tree;
    Transform tf;
    BOOST_CHECK_THROW(tree.getTransform(a, c), UnknownTransformException);
}


BOOST_AUTO_TEST_CASE(simple_add_transform_event_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    TransformGraph tree;
    Transform tf;
    tf.transform.translation << 42, 21, -42;
    tf.transform.orientation = base::AngleAxisd(0.25, base::Vector3d::UnitX());

    std::shared_ptr<Dispatcher> d(new Dispatcher());

    tree.subscribe(d);
    tree.addTransform(a, b, tf);
    BOOST_CHECK(d->transformAddedEvent.size() == 2);
    edge_descriptor aToB = tree.getEdge(a, b);
    BOOST_CHECK(d->transformAddedEvent[0].edge == aToB);
    edge_descriptor bToA = tree.getEdge(b, a);
    BOOST_CHECK(d->transformAddedEvent[1].edge == bToA);
    //annoying amount of checks because there is no operator== in eigen
    BOOST_CHECK(compareTransform(d->transformAddedEvent[0].transform, tf));
    Transform invTf = tf;
    invTf.setTransform(tf.transform.inverse());
    BOOST_CHECK(compareTransform(d->transformAddedEvent[1].transform, invTf));
}

BOOST_AUTO_TEST_CASE(add_item_on_empty_graph_test)
{
    TransformGraph graph;
    FrameId a = "frame_a";
    Item<std::string>::Ptr item(new Item<std::string>());
    BOOST_CHECK_THROW(graph.addItemToFrame(a, item), UnknownFrameException);
    
}

BOOST_AUTO_TEST_CASE(add_invalid_item_test)
{
    TransformGraph graph;
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    FrameId c = "frame_c";
    Transform tf;
    graph.addTransform(a, b, tf);
    Item<std::string>::Ptr item(new Item<std::string>());
    BOOST_CHECK_THROW(graph.addItemToFrame(c, item), UnknownFrameException);
}

BOOST_AUTO_TEST_CASE(add_item_test)
{
    TransformGraph graph;
    Transform tf;
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    graph.addTransform(a, b, tf);
    
    Item<std::string>::Ptr item(new Item<std::string>());
    BOOST_CHECK_NO_THROW(graph.addItemToFrame(a, item));

    BOOST_CHECK(graph.getItems(a).size() == 1);
    BOOST_CHECK(graph.getItems(a)[0] == item);
}

BOOST_AUTO_TEST_CASE(get_item_empty_graph_test)
{
    TransformGraph graph;
    FrameId a = "frame_a";
    BOOST_CHECK_THROW(graph.getItems(a), UnknownFrameException);
}

BOOST_AUTO_TEST_CASE(get_invalid_item_test)
{
    TransformGraph graph;
    Transform tf;
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    FrameId c = "frame_c";
    graph.addTransform(a, b, tf);
    
    Item<std::string>::Ptr item(new Item<std::string>());
    BOOST_CHECK_NO_THROW(graph.addItemToFrame(a, item));
    BOOST_CHECK_THROW(graph.getItems(c), UnknownFrameException);
}


class Vector: public envire::core::Item<Eigen::Vector3d>{};

BOOST_AUTO_TEST_CASE(graphviz_test)
{
    TransformGraph graph;
    
    for(int i = 0; i < 12; ++i)
    {
        FrameId origin = "frame_" + boost::lexical_cast<std::string>(i);
        FrameId target = "frame_" + boost::lexical_cast<std::string>(i + 1);
        Transform tf;
        graph.addTransform(origin, target, tf);
    }
    GraphViz viz;
    viz.write(graph, "transformGraph_graphviz_test.dot");
}


BOOST_AUTO_TEST_CASE(simple_get_tree_test)
{
    TransformGraph graph;
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
    
    Transform tf;
    
    graph.addTransform(a, b, tf);
    graph.addTransform(a, c, tf);
    graph.addTransform(c, d, tf);
    graph.addTransform(c, e, tf);
    graph.addTransform(e, f, tf);
    graph.addTransform(e, g, tf);
    
    //use a as root
    VertexMap parentToChildren = graph.getTree(graph.vertex(a));
    BOOST_CHECK(parentToChildren.size() == 3); 
    BOOST_CHECK(parentToChildren[graph.vertex(a)].size() == 2);
    BOOST_CHECK(parentToChildren[graph.vertex(c)].size() == 2);
    BOOST_CHECK(parentToChildren[graph.vertex(e)].size() == 2);
    BOOST_CHECK(parentToChildren.find(graph.vertex(b)) == parentToChildren.end());
    BOOST_CHECK(parentToChildren.find(graph.vertex(d)) == parentToChildren.end());
    BOOST_CHECK(parentToChildren.find(graph.vertex(f)) == parentToChildren.end());
    BOOST_CHECK(parentToChildren.find(graph.vertex(g)) == parentToChildren.end());
    std::vector<vertex_descriptor>& aChildren = parentToChildren[graph.vertex(a)];
    BOOST_CHECK(aChildren[0] == graph.vertex(b) || aChildren[0] == graph.vertex(c));
    BOOST_CHECK(aChildren[1] == graph.vertex(b) || aChildren[1] == graph.vertex(c));
    std::vector<vertex_descriptor>& cChildren = parentToChildren[graph.vertex(c)];
    BOOST_CHECK(cChildren[0] == graph.vertex(d) || cChildren[0] == graph.vertex(e));
    BOOST_CHECK(cChildren[1] == graph.vertex(d) || cChildren[1] == graph.vertex(e));    
    std::vector<vertex_descriptor>& eChildren = parentToChildren[graph.vertex(e)];
    BOOST_CHECK(eChildren[0] == graph.vertex(f) || eChildren[0] == graph.vertex(g));
    BOOST_CHECK(eChildren[1] == graph.vertex(f) || eChildren[1] == graph.vertex(g));     
    
     /*       d
     *        |
     *        c
     *       / \
     *      a   e
     *     /   / \
     *    b   f   g
     * Now use d as root node
     */
    parentToChildren = graph.getTree(graph.vertex(d));
    BOOST_CHECK(parentToChildren.size() == 4); 
    BOOST_CHECK(parentToChildren[graph.vertex(d)].size() == 1);
    BOOST_CHECK(parentToChildren[graph.vertex(c)].size() == 2);
    BOOST_CHECK(parentToChildren[graph.vertex(e)].size() == 2);
    BOOST_CHECK(parentToChildren[graph.vertex(a)].size() == 1);
    BOOST_CHECK(parentToChildren.find(graph.vertex(b)) == parentToChildren.end());
    BOOST_CHECK(parentToChildren.find(graph.vertex(f)) == parentToChildren.end());
    BOOST_CHECK(parentToChildren.find(graph.vertex(g)) == parentToChildren.end());
    
    aChildren = parentToChildren[graph.vertex(a)];
    BOOST_CHECK(aChildren[0] == graph.vertex(b));
    cChildren = parentToChildren[graph.vertex(c)];
    BOOST_CHECK(cChildren[0] == graph.vertex(a) || cChildren[0] == graph.vertex(e));
    BOOST_CHECK(cChildren[1] == graph.vertex(a) || cChildren[1] == graph.vertex(e)); 
    std::vector<vertex_descriptor>& dChildren = parentToChildren[graph.vertex(d)];
    BOOST_CHECK(dChildren[0] == graph.vertex(c));
    eChildren = parentToChildren[graph.vertex(e)];
    BOOST_CHECK(eChildren[0] == graph.vertex(f) || eChildren[0] == graph.vertex(g));
    BOOST_CHECK(eChildren[1] == graph.vertex(f) || eChildren[1] == graph.vertex(g));       
}

