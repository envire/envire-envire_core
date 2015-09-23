#include <boost/test/unit_test.hpp>
#include <envire_core/TransformTreeTypes.hpp>
#include <envire_core/TransformTree.hpp>
#include <envire_core/Item.hpp>
#include <envire_core/GraphViz.hpp>
#include <envire_core/events/TreeEventDispatcher.hpp>
#include <vector>

using namespace envire::core;
using namespace std;

class Dispatcher : public TreeEventDispatcher {
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

BOOST_AUTO_TEST_CASE(simple_add_get_transform_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    TransformTree tree;
    Transform tf;
    tf.transform.translation << 42, 21, -42;
    tf.transform.orientation = base::AngleAxisd(0.25, base::Vector3d::UnitX());
    BOOST_CHECK_NO_THROW(tree.addTransform(a, b, tf));
    BOOST_CHECK(tree.num_edges() == 2);
    BOOST_CHECK(tree.num_vertices() == 2);
    Transform readTf = tree.getTransform(a, b);
    BOOST_CHECK(readTf.transform.translation.x() == tf.transform.translation.x());
    BOOST_CHECK(readTf.transform.translation.y() == tf.transform.translation.y());
    BOOST_CHECK(readTf.transform.translation.z() == tf.transform.translation.z());
    BOOST_CHECK(readTf.transform.orientation.axis().x() == tf.transform.orientation.axis().x());
    BOOST_CHECK(readTf.transform.orientation.axis().y() == tf.transform.orientation.axis().y());
    BOOST_CHECK(readTf.transform.orientation.axis().z() == tf.transform.orientation.axis().z());
    BOOST_CHECK(readTf.transform.orientation.angle() == tf.transform.orientation.angle());
    Transform invTf = tf;
    invTf.setTransform(tf.transform.inverse());
    Transform readTfInv = tree.getTransform(b, a);
    BOOST_CHECK(readTfInv.transform.translation.x() == invTf.transform.translation.x());
    BOOST_CHECK(readTfInv.transform.translation.y() == invTf.transform.translation.y());
    BOOST_CHECK(readTfInv.transform.translation.z() == invTf.transform.translation.z());
    BOOST_CHECK(readTfInv.transform.orientation.axis().x() == invTf.transform.orientation.axis().x());
    BOOST_CHECK(readTfInv.transform.orientation.axis().y() == invTf.transform.orientation.axis().y());
    BOOST_CHECK(readTfInv.transform.orientation.axis().z() == invTf.transform.orientation.axis().z());
    BOOST_CHECK(readTfInv.transform.orientation.angle() == invTf.transform.orientation.angle());
}

BOOST_AUTO_TEST_CASE(simple_add_transform_event_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    TransformTree tree;
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
    BOOST_CHECK(d->transformAddedEvent[0].transform.transform.translation.x() == tf.transform.translation.x());
    BOOST_CHECK(d->transformAddedEvent[0].transform.transform.translation.y() == tf.transform.translation.y());
    BOOST_CHECK(d->transformAddedEvent[0].transform.transform.translation.z() == tf.transform.translation.z());
    BOOST_CHECK(d->transformAddedEvent[0].transform.transform.orientation.axis().x() == tf.transform.orientation.axis().x());
    BOOST_CHECK(d->transformAddedEvent[0].transform.transform.orientation.axis().y() == tf.transform.orientation.axis().y());
    BOOST_CHECK(d->transformAddedEvent[0].transform.transform.orientation.axis().z() == tf.transform.orientation.axis().z());
    BOOST_CHECK(d->transformAddedEvent[0].transform.transform.orientation.angle() == tf.transform.orientation.angle());
    Transform invTf = tf;
    invTf.setTransform(tf.transform.inverse());
    BOOST_CHECK(d->transformAddedEvent[1].transform.transform.translation.x() == invTf.transform.translation.x());
    BOOST_CHECK(d->transformAddedEvent[1].transform.transform.translation.y() == invTf.transform.translation.y());
    BOOST_CHECK(d->transformAddedEvent[1].transform.transform.translation.z() == invTf.transform.translation.z());
    BOOST_CHECK(d->transformAddedEvent[1].transform.transform.orientation.axis().x() == invTf.transform.orientation.axis().x());
    BOOST_CHECK(d->transformAddedEvent[1].transform.transform.orientation.axis().y() == invTf.transform.orientation.axis().y());
    BOOST_CHECK(d->transformAddedEvent[1].transform.transform.orientation.axis().z() == invTf.transform.orientation.axis().z());
    BOOST_CHECK(d->transformAddedEvent[1].transform.transform.orientation.angle() == invTf.transform.orientation.angle());

}


//
//BOOST_AUTO_TEST_CASE(remove_frame_with_children_test)
//{
//  TransformTree tree;
//  Frame root("root");
//  Frame c1("c1");
//  Frame c2("c2");
//  Frame c3("c3");
//
//  Transform tf;
//  tf.transform.translation.x() = 42;
//
//  vertex_descriptor v0 = tree.addRoot(root);
//  using pair = std::pair<vertex_descriptor, edge_descriptor>;
//  pair v1 = tree.addFrame(c1, v0, tf);
//  pair v2 = tree.addFrame(c2, v1.first, tf);
//  pair v3 = tree.addFrame(c3, v1.first, tf);
//
//  BOOST_CHECK(tree.num_vertices() == 4);
//  BOOST_CHECK(tree.num_edges() == 3);
//  BOOST_CHECK()
//
//  tree.removeFrame(v1.first);
//  BOOST_CHECK(tree.num_vertices() == 1);
//  BOOST_CHECK(tree.num_edges() == 0);
//}
//
//BOOST_AUTO_TEST_CASE(set_get_remove_root_test)
//{
//  TransformTree tree;
//  Frame f1("f1");
//  vertex_descriptor e1 = tree.addRoot(f1);
//  vertex_descriptor e2 = tree.getRoot();
//  BOOST_CHECK(e1 == e2);
//  BOOST_CHECK(tree.num_vertices() == 1);
//  BOOST_CHECK(tree.num_edges() == 0);
//  tree.removeFrame(e2);
//  BOOST_CHECK(tree.num_vertices() == 0);
//}
//
//BOOST_AUTO_TEST_CASE(modify_transform_event_test)
//{
//    TransformTree tree;
//    Frame f1("f1");
//    Frame f2("f2");
//    Transform tf;
//    tf.transform.translation.x() = 42;
//    vertex_descriptor v1 = tree.addRoot(f1);
//    vertex_descriptor v2;
//    edge_descriptor edge;
//    boost::tie(v2, edge) = tree.addFrame(f2, v1, tf);
//    Dispatcher d;
//    tree.subscribe(&d);
//    tf.transform.translation.x() = 44;
//    tree.setTransform(edge, tf);
//    BOOST_CHECK(d->callCount == 1);
//    BOOST_CHECK(d->transformModifiedEvent->newTransform.transform.translation.x() == 44);
//    BOOST_CHECK(d->transformModifiedEvent->oldTransform.transform.translation.x() == 42);
//    BOOST_CHECK(d->transformModifiedEvent->edge == edge);
//}
//
//BOOST_AUTO_TEST_CASE(add_remove_transform_event_test)
//{
//    TransformTree tree;
//    Frame f1("f1");
//    Frame f2("f2");
//    vertex_descriptor v1 = tree.addRoot(f1);
//    vertex_descriptor v2;
//    edge_descriptor edge;
//    Transform tf;
//    tf.transform.translation.x() = 42;
//    boost::tie(v2, edge) = tree.addFrame(f2, v1, tf);
//
//    Dispatcher d;
//    tree.subscribe(&d);
//    //use add_edge to modify existing edge
//    bool added = true;
//    tf.transform.translation.x() = 44;
//    boost::tie(edge, added) = tree.add_edge(v1, v2, tf);
//    BOOST_CHECK(d->callCount == 1);
//    BOOST_CHECK(d->transformModifiedEvent->edge == edge);
//    BOOST_CHECK(d->transformModifiedEvent->from == v1);
//    BOOST_CHECK(d->transformModifiedEvent->to == v2);
//    BOOST_CHECK(d->transformModifiedEvent->oldTransform.transform.translation.x() == 42);
//    BOOST_CHECK(d->transformModifiedEvent->newTransform.transform.translation.x() == 44);
//
//    //remove the edge
//    tree.remove_edge(edge);
//    BOOST_CHECK(d->callCount == 2);
//    BOOST_CHECK(d->transformRemovedEvent->from == v1);
//    BOOST_CHECK(d->transformRemovedEvent->to == v2);
//    BOOST_CHECK(d->transformRemovedEvent->transform.transform.translation.x() == 44);
//
//}
//
//
//BOOST_AUTO_TEST_CASE(frame_add_event_test)
//{
//    Dispatcher d;
//    TransformTree tree;
//    tree.subscribe(&d);
//    Frame root("Root");
//    vertex_descriptor rootVertex = tree.addRoot(root);
//
//    Frame child("child");
//    Frame childchild("childchild");
//
//    Transform tf;
//    tf.transform.translation.x() = 42;
//
//    BOOST_CHECK(!d->frameAddedEvent.is_initialized());
//   // BOOST_CHECK(d->callCount == 1);
//    vertex_descriptor childVertex;
//    edge_descriptor childEdge;
//    boost::tie(childVertex, childEdge) = tree.addFrame(child, rootVertex, tf);
//    BOOST_CHECK(d->frameAddedEvent.is_initialized());
//    BOOST_CHECK(d->callCount == 2);
//    BOOST_CHECK(d->frameAddedEvent->parent == rootVertex);
//    BOOST_CHECK(d->frameAddedEvent->addedVertex == childVertex);
//    BOOST_CHECK(d->frameAddedEvent->transform.transform.translation.x() == 42);
//
//    d->frameAddedEvent.reset();
//    BOOST_CHECK(!d->frameAddedEvent.is_initialized());
//    Transform tf2;
//    tf2.transform.translation.x() = 84;
//    vertex_descriptor childChildVertex;
//    edge_descriptor childChildEdge;
//    boost::tie(childChildVertex, childChildEdge)= tree.addFrame(childchild, childVertex, tf2);
//    BOOST_CHECK(d->frameAddedEvent.is_initialized());
//   // BOOST_CHECK(d->callCount == 3);
//    BOOST_CHECK(d->frameAddedEvent->parent == childVertex);
//    BOOST_CHECK(d->frameAddedEvent->addedVertex == childChildVertex);
//    BOOST_CHECK(d->frameAddedEvent->transform.transform.translation.x() == 84);
//}
//
//
//BOOST_AUTO_TEST_CASE(add_and_remove_edge_test)
//{
//
////    unsigned int max_vertices = 100;
////
////    BOOST_TEST_MESSAGE("ADD EDGES TEST...");
////    envire::core::TransformTree tree;
////
////    unsigned int i = 0;
////    for (i = 0; i<max_vertices; ++i)
////    {
////        envire::core::Frame frame("frame_"+boost::lexical_cast<std::string>(i));
////        tree.add_vertex(frame);
////    }
////
////    /** get root node **/
////    envire::core::vertex_descriptor root = tree.vertex("frame_0");
////    for (envire::core::vertices_size_type iv = 1; iv < tree.num_vertices(); ++iv)
////    {
////        envire::core::edge_descriptor edge; bool b;
////        const std::string label = "frame_"+boost::lexical_cast<std::string>(iv);
////        envire::core::vertex_descriptor node_to = tree.vertex(label);
////        try
////        {
////            boost::tie(edge, b) = tree.add_edge(root, node_to);
////            BOOST_CHECK(b == true);
////        }catch (std::logic_error e)
////        {
////            std::cerr<<e.what();
////        }
////    }
////
////    BOOST_CHECK(tree.num_edges() == tree.num_vertices() - 1);
////    BOOST_TEST_MESSAGE("DONE\n");
////
////    BOOST_TEST_MESSAGE("REMOVE EDGES TEST...");
////    envire::core::edge_iterator ei, ei_end, next;
////    boost::tie(ei, ei_end) = tree.edges();
////    for (next = ei; ei != ei_end; ei = next)
////    {
////        ++next;
////        tree.remove_edge(*ei);
////    }
////
////    BOOST_CHECK(tree.num_edges() == 0);
////    BOOST_TEST_MESSAGE("DONE\n");
//}
//
//BOOST_AUTO_TEST_CASE(add_an_item)
//{
////    // Instantiate a tree and vertex
////    using namespace envire::core;
////    TransformTree tree;
////    Frame frame("Example frame");
////    tree.add_vertex(frame);
////    // Add an item to the frame of the vertex
////    boost::intrusive_ptr<Item<std::string>> item(new Item<std::string>());
////    item->setData("Contents of the Item");
////    boost::intrusive_ptr<ItemBase> itemB(item);
////
////    // Include the item in the vector of the frame (with a vector)
////    unsigned int vectorSize = 1;
////    std::vector< boost::intrusive_ptr<ItemBase>> itemPVector(vectorSize);
////    itemPVector[0] = itemB;
////    frame.items = itemPVector;
////    // Visualize results
////    envire::core::GraphViz gviz;
////    gviz.write(tree, "graphviz_boost_test_add_an_item.dot");
//}
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
