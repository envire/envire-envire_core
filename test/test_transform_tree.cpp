#include <boost/uuid/uuid_generators.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/lexical_cast.hpp> /** to string conversion when using < C++11 */

#include <envire_core/TransformTreeTypes.hpp>
#include <envire_core/TransformTree.hpp>
#include <envire_core/Item.hpp>
#include <envire_core/GraphViz.hpp>
#include <envire_core/events/TreeEventDispatcher.hpp>

using namespace envire::core;


class Dispatcher : public TreeEventDispatcher {
public:
    boost::optional<FrameAddedEvent> frameAddedEvent;
    boost::optional<VertexAddedEvent> vertexAddedEvent;
    boost::optional<VertexRemovedEvent> vertexRemovedEvent;
    boost::optional<TransformAddedEvent> transformAddedEvent;
    boost::optional<TransformModifiedEvent> transformModifiedEvent;
    boost::optional<TransformRemovedEvent> transformRemovedEvent;
    int callCount = 0;

    virtual void frameAdded(const FrameAddedEvent& e)
    {
      ++callCount;
      frameAddedEvent = e;
    }

    virtual void vertexAdded(const VertexAddedEvent& e)
    {
      ++callCount;
      vertexAddedEvent = e;
    }

    virtual void vertexRemoved(const VertexRemovedEvent& e)
    {
      ++callCount;
      vertexRemovedEvent = e;
    }

    virtual void transformAdded(const TransformAddedEvent& e)
    {
      ++callCount;
      transformAddedEvent = e;
    }

    virtual void transformModified(const TransformModifiedEvent& e)
    {
      ++callCount;
      transformModifiedEvent = e;
    }

    virtual void transformRemoved(const TransformRemovedEvent& e)
    {
      ++callCount;
      transformRemovedEvent = e;
    }
};

BOOST_AUTO_TEST_CASE(add_remove_transform_event_test)
{
  TransformTree tree;
  Frame f1("f1");
  Frame f2("f2");
  vertex_descriptor v1 = tree.add_vertex(f1);
  vertex_descriptor v2 = tree.add_vertex(f2);

  Dispatcher d;
  tree.subscribe(&d);
  //add edge
  Transform tf;
  tf.transform.translation.x() = 42;
  edge_descriptor edge;
  bool added = false;
  boost::tie(edge, added) = tree.add_edge(v1, v2, tf);
  BOOST_CHECK(added);
  BOOST_CHECK(d.callCount == 1);
  BOOST_CHECK(d.transformAddedEvent->edge == edge);
  BOOST_CHECK(d.transformAddedEvent->from == v1);
  BOOST_CHECK(d.transformAddedEvent->to == v2);
  BOOST_CHECK(d.transformAddedEvent->transform.transform.translation.x() == 42);

  //use add_edge to modify existing edge
  added = true;
  tf.transform.translation.x() = 44;
  boost::tie(edge, added) = tree.add_edge(v1, v2, tf);
  BOOST_CHECK(d.callCount == 2);
  BOOST_CHECK(d.transformModifiedEvent->edge == edge);
  BOOST_CHECK(d.transformModifiedEvent->from == v1);
  BOOST_CHECK(d.transformModifiedEvent->to == v2);
  BOOST_CHECK(d.transformModifiedEvent->oldTransform.transform.translation.x() == 42);
  BOOST_CHECK(d.transformModifiedEvent->newTransform.transform.translation.x() == 44);

  //remove the edge
  tree.remove_edge(edge);
  BOOST_CHECK(d.callCount == 3);
  BOOST_CHECK(d.transformRemovedEvent->from == v1);
  BOOST_CHECK(d.transformRemovedEvent->to == v2);
  BOOST_CHECK(d.transformRemovedEvent->transform.transform.translation.x() == 44);

}


BOOST_AUTO_TEST_CASE(add_remove_vertex_event_test)
{
  Dispatcher d;
  TransformTree tree;
  tree.subscribe(&d);
  Frame root("Root");
  vertex_descriptor rootVertex = tree.add_vertex(root);
  BOOST_CHECK(d.callCount == 1);
  BOOST_CHECK(d.vertexAddedEvent->addedVertex == rootVertex);
  tree.remove_vertex(rootVertex);
  BOOST_CHECK(d.callCount == 2);
  BOOST_CHECK(d.vertexRemovedEvent.is_initialized());
}

BOOST_AUTO_TEST_CASE(frame_add_event_test)
{
    Dispatcher d;
    TransformTree tree;
    tree.subscribe(&d);
    Frame root("Root");
    vertex_descriptor rootVertex = tree.add_vertex(root);

    Frame child("child");
    Frame childchild("childchild");

    Transform tf;
    tf.transform.translation.x() = 42;

    BOOST_CHECK(!d.frameAddedEvent.is_initialized());
    BOOST_CHECK(d.callCount == 1);
    vertex_descriptor childVertex = tree.addFrame(child, rootVertex, tf);
    BOOST_CHECK(d.frameAddedEvent.is_initialized());
    BOOST_CHECK(d.callCount == 2);
    BOOST_CHECK(d.frameAddedEvent->parent == rootVertex);
    BOOST_CHECK(d.frameAddedEvent->addedVertex == childVertex);
    BOOST_CHECK(d.frameAddedEvent->transform.transform.translation.x() == 42);

    d.frameAddedEvent.reset();
    BOOST_CHECK(!d.frameAddedEvent.is_initialized());
    Transform tf2;
    tf2.transform.translation.x() = 84;
    vertex_descriptor childChildVertex = tree.addFrame(childchild, childVertex, tf2);
    BOOST_CHECK(d.frameAddedEvent.is_initialized());
    BOOST_CHECK(d.callCount == 3);
    BOOST_CHECK(d.frameAddedEvent->parent == childVertex);
    BOOST_CHECK(d.frameAddedEvent->addedVertex == childChildVertex);
    BOOST_CHECK(d.frameAddedEvent->transform.transform.translation.x() == 84);
}

BOOST_AUTO_TEST_CASE(add_and_remove_vertex_test)
{
    unsigned int max_vertices = 100;

    BOOST_TEST_MESSAGE("ADD VERTEX TEST...");
    envire::core::TransformTree tree;

    unsigned int i = 0;
    for (i = 0; i<max_vertices; ++i)
    {
        envire::core::Frame frame("frame_"+boost::lexical_cast<std::string>(i));
        tree.add_vertex(frame);
    }

    BOOST_CHECK(tree.num_vertices() == i);
    BOOST_TEST_MESSAGE("DONE\n");

    BOOST_TEST_MESSAGE("REMOVE VERTEX TEST...");
    envire::core::vertex_iterator vi, vi_end, next;
    boost::tie(vi, vi_end) = tree.vertices();
    for (next = vi; vi != vi_end; vi = next)
    {
        ++next;
        tree.remove_vertex(*vi);
    }

    BOOST_CHECK(tree.num_vertices() == 0);
    BOOST_TEST_MESSAGE("DONE\n");
}

BOOST_AUTO_TEST_CASE(add_and_remove_edge_test)
{

    unsigned int max_vertices = 100;

    BOOST_TEST_MESSAGE("ADD EDGES TEST...");
    envire::core::TransformTree tree;

    unsigned int i = 0;
    for (i = 0; i<max_vertices; ++i)
    {
        envire::core::Frame frame("frame_"+boost::lexical_cast<std::string>(i));
        tree.add_vertex(frame);
    }

    /** get root node **/
    envire::core::vertex_descriptor root = tree.vertex(0);
    for (envire::core::vertices_size_type iv = 1; iv < tree.num_vertices(); ++iv)
    {
        envire::core::edge_descriptor edge; bool b;
        envire::core::vertex_descriptor node_to = tree.vertex(iv);
        try
        {
            boost::tie(edge, b) = tree.add_edge(root, node_to);
            BOOST_CHECK(b == true);
        }catch (std::logic_error e)
        {
            std::cerr<<e.what();
        }
    }

    BOOST_CHECK(tree.num_edges() == tree.num_vertices() - 1);
    BOOST_TEST_MESSAGE("DONE\n");

    BOOST_TEST_MESSAGE("REMOVE EDGES TEST...");
    envire::core::edge_iterator ei, ei_end, next;
    boost::tie(ei, ei_end) = tree.edges();
    for (next = ei; ei != ei_end; ei = next)
    {
        ++next;
        tree.remove_edge(*ei);
    }

    BOOST_CHECK(tree.num_edges() == 0);
    BOOST_TEST_MESSAGE("DONE\n");
}

BOOST_AUTO_TEST_CASE(add_an_item)
{
    // Instantiate a tree and vertex
    using namespace envire::core;
    TransformTree tree;
    Frame frame("Example frame");
    tree.add_vertex(frame);
    // Add an item to the frame of the vertex
    boost::intrusive_ptr<Item<std::string>> item(new Item<std::string>());
    item->setData("Contents of the Item");
    boost::intrusive_ptr<ItemBase> itemB(item);

    // Include the item in the vector of the frame (with a vector)
    unsigned int vectorSize = 1;
    std::vector< boost::intrusive_ptr<ItemBase>> itemPVector(vectorSize);
    itemPVector[0] = itemB;
    frame.items = itemPVector;
    // Visualize results
    envire::core::GraphViz gviz;
    gviz.write(tree, "graphviz_boost_test_add_an_item.dot");
}

class Vector: public envire::core::Item<Eigen::Vector3d>
{
};

BOOST_AUTO_TEST_CASE(property_and_grahviz_test)
{

    unsigned int max_vertices = 100;

    envire::core::TransformTree tree;
    unsigned int vector_size = 100;
    unsigned int magic_number = 42;

    /** Create a list of items **/
    std::vector< boost::intrusive_ptr<envire::core::ItemBase> > items_vector(vector_size);
    boost::intrusive_ptr<Vector> my_vector(new Vector());
    my_vector->setData(magic_number * Eigen::Vector3d::Ones());

    for (std::vector<boost::intrusive_ptr<envire::core::ItemBase> >::iterator it = items_vector.begin() ; it != items_vector.end(); ++it)
    {
        /** Pointer to the same object **/
        *it = my_vector;
    }

    for (unsigned int i = 0; i<max_vertices; ++i)
    {
        envire::core::Frame frame("frame_"+boost::lexical_cast<std::string>(i));
        frame.items = items_vector;
        tree.add_vertex(frame);
    }

    BOOST_TEST_MESSAGE("FRAME PROPERTY TEST...");
    for (unsigned int i = 0; i < max_vertices; ++i)
    {
        envire::core::Frame frame = tree.getFrame(tree.vertex(i));
        BOOST_CHECK(frame.name == "frame_"+boost::lexical_cast<std::string>(i));
        BOOST_CHECK(frame.items.size() == vector_size);
        for (std::vector< boost::intrusive_ptr<envire::core::ItemBase> >::const_iterator it = frame.items.begin();
                it != frame.items.end(); ++it)
        {
            BOOST_CHECK(*it == my_vector);
            Vector* pvector = dynamic_cast< Vector* >(it->get());
            BOOST_CHECK(pvector->getData() == magic_number * Eigen::Vector3d::Ones());
        }
    }
    BOOST_TEST_MESSAGE("DONE\n");

    /** get root node and create edges **/
    base::Time now = base::Time::now();
    envire::core::vertex_descriptor root = tree.vertex(0);
    for (envire::core::vertices_size_type iv = 1; iv < tree.num_vertices(); ++iv)
    {
        envire::core::edge_descriptor edge; bool b;
        envire::core::Transform tf_prop(now);
        base::TransformWithCovariance tf(Eigen::AngleAxisd::Identity(), static_cast<base::Position>(my_vector->getData()));
        tf_prop.setTransform(tf);
        envire::core::vertex_descriptor node_to = tree.vertex(iv);
        boost::tie(edge, b) = tree.add_edge(root, node_to, tf_prop);
        BOOST_CHECK(b == true);
    }

    BOOST_TEST_MESSAGE("TRANSFORM PROPERTY TEST...");
    envire::core::edge_iterator it, end;
    for(boost::tie(it, end) = tree.edges(); it != end; ++it)
    {

        const Transform& transform = tree.getTransform(it);
        BOOST_CHECK(transform.time == now);
        BOOST_CHECK(transform.transform.translation == my_vector->getData());
    }
    BOOST_TEST_MESSAGE("DONE\n");

    BOOST_TEST_MESSAGE("GRAPHVIZ TEST...");
    envire::core::GraphViz gviz;

    gviz.write(tree, "graphviz_boost_test_transform_tree.dot");

    //tree.clear();
    BOOST_TEST_MESSAGE("DONE\n");
}

