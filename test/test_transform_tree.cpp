#include <boost/test/unit_test.hpp>
#include <envire_core/TransformGraphTypes.hpp>
#include <envire_core/TransformGraph.hpp>
#include <envire_core/Item.hpp>
#include <envire_core/GraphViz.hpp>
#include <envire_core/events/GraphEventDispatcher.hpp>
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
    BOOST_CHECK(d->transformModifiedEvent.size() == 2);
    BOOST_CHECK(compareTransform(d->transformModifiedEvent[0].newTransform, tf2));
    BOOST_CHECK(compareTransform(d->transformModifiedEvent[0].oldTransform, tf));
    Transform tf2Inv = tf2;
    tf2Inv.setTransform(tf2.transform.inverse());
    BOOST_CHECK(compareTransform(d->transformModifiedEvent[1].newTransform, tf2Inv));
    BOOST_CHECK(d->transformModifiedEvent[0].origin == a);
    BOOST_CHECK(d->transformModifiedEvent[0].target == b);
    BOOST_CHECK(d->transformModifiedEvent[1].origin == b);
    BOOST_CHECK(d->transformModifiedEvent[1].target == a);
    BOOST_CHECK(d->transformModifiedEvent[0].edge == tree.getEdge(a, b));
    BOOST_CHECK(d->transformModifiedEvent[1].edge == tree.getEdge(b, a));
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
    boost::intrusive_ptr<Item<std::string>> item(new Item<std::string>());
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
    boost::intrusive_ptr<Item<std::string>> item(new Item<std::string>());
    BOOST_CHECK_THROW(graph.addItemToFrame(c, item), UnknownFrameException);
}

BOOST_AUTO_TEST_CASE(add_item_test)
{
    TransformGraph graph;
    Frame frame("Example frame");
    Transform tf;
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    graph.addTransform(a, b, tf);
    
    boost::intrusive_ptr<Item<std::string>> item(new Item<std::string>());
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
    Frame frame("Example frame");
    Transform tf;
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    FrameId c = "frame_c";
    graph.addTransform(a, b, tf);
    
    boost::intrusive_ptr<Item<std::string>> item(new Item<std::string>());
    BOOST_CHECK_NO_THROW(graph.addItemToFrame(a, item));
    BOOST_CHECK_THROW(graph.getItems(c), UnknownFrameException);
}

//
//class Vector: public envire::core::Item<Eigen::Vector3d>
//{
//};
//
//BOOST_AUTO_TEST_CASE(property_and_grahviz_test)
//{
//
////    unsigned int max_vertices = 100;
////
////    envire::core::TransformTree tree;
////    unsigned int vector_size = 100;
////    unsigned int magic_number = 42;
////
////    /** Create a list of items **/
////    std::vector< boost::intrusive_ptr<envire::core::ItemBase> > items_vector(vector_size);
////    boost::intrusive_ptr<Vector> my_vector(new Vector());
////    my_vector->setData(magic_number * Eigen::Vector3d::Ones());
////
////    for (std::vector<boost::intrusive_ptr<envire::core::ItemBase> >::iterator it = items_vector.begin() ; it != items_vector.end(); ++it)
////    {
////        /** Pointer to the same object **/
////        *it = my_vector;
////    }
////
////    for (unsigned int i = 0; i<max_vertices; ++i)
////    {
////        envire::core::Frame frame("frame_"+boost::lexical_cast<std::string>(i));
////        frame.items = items_vector;
////        tree.add_vertex(frame);
////    }
////
////    BOOST_TEST_MESSAGE("FRAME PROPERTY TEST...");
////    for (unsigned int i = 0; i < max_vertices; ++i)
////    {
////        const std::string label = "frame_"+boost::lexical_cast<std::string>(i);
////        envire::core::Frame frame = tree.getFrame(tree.vertex(label));
////        BOOST_CHECK(frame.name == "frame_"+boost::lexical_cast<std::string>(i));
////        BOOST_CHECK(frame.items.size() == vector_size);
////        for (std::vector< boost::intrusive_ptr<envire::core::ItemBase> >::const_iterator it = frame.items.begin();
////                it != frame.items.end(); ++it)
////        {
////            BOOST_CHECK(*it == my_vector);
////            Vector* pvector = dynamic_cast< Vector* >(it->get());
////            BOOST_CHECK(pvector->getData() == magic_number * Eigen::Vector3d::Ones());
////        }
////    }
////    BOOST_TEST_MESSAGE("DONE\n");
////
////    /** get root node and create edges **/
////    base::Time now = base::Time::now();
////    envire::core::vertex_descriptor root = tree.vertex("frame_0");
////    for (envire::core::vertices_size_type iv = 1; iv < tree.num_vertices(); ++iv)
////    {
////        envire::core::edge_descriptor edge; bool b;
////        envire::core::Transform tf_prop(now);
////        base::TransformWithCovariance tf(Eigen::AngleAxisd::Identity(), static_cast<base::Position>(my_vector->getData()));
////        tf_prop.setTransform(tf);
////        const std::string label = "frame_"+boost::lexical_cast<std::string>(iv);
////        envire::core::vertex_descriptor node_to = tree.vertex(label);
////        boost::tie(edge, b) = tree.add_edge(root, node_to, tf_prop);
////        BOOST_CHECK(b == true);
////    }
////
////    BOOST_TEST_MESSAGE("TRANSFORM PROPERTY TEST...");
////    envire::core::edge_iterator it, end;
////    for(boost::tie(it, end) = tree.edges(); it != end; ++it)
////    {
////
////        const Transform& transform = tree.getTransform(*it);
////        BOOST_CHECK(transform.time == now);
////        BOOST_CHECK(transform.transform.translation == my_vector->getData());
////    }
////    BOOST_TEST_MESSAGE("DONE\n");
////
////    BOOST_TEST_MESSAGE("GRAPHVIZ TEST...");
////    envire::core::GraphViz gviz;
////
////    gviz.write(tree, "graphviz_boost_test_transform_tree.dot");
////
////    //tree.clear();
////    BOOST_TEST_MESSAGE("DONE\n");
//}
//

