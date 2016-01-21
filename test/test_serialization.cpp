#include <boost/test/unit_test.hpp>

#include <Eigen/Geometry>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/archive/polymorphic_text_iarchive.hpp>
#include <boost/archive/polymorphic_text_oarchive.hpp>
#include <boost/archive/polymorphic_binary_iarchive.hpp>
#include <boost/archive/polymorphic_binary_oarchive.hpp>

#include <envire_core/serialization/Serialization.hpp>
#include <envire_core/items/Item.hpp>
#include <envire_core/plugin/ClassLoader.hpp>
#include <envire_core/graph/TransformGraph.hpp>
#include <envire_core/graph/GraphViz.hpp>

using namespace envire::core;

BOOST_AUTO_TEST_CASE(vector_plugin_serialization_text)
{
    BOOST_CHECK(envire::ClassLoader::getInstance()->hasValidPluginPath());

    // create vector plugin
    auto base_plugin = envire::ClassLoader::getInstance()->createItem("VectorPlugin");
    BOOST_CHECK(base_plugin != NULL);

    Item<Eigen::Vector3d>::Ptr vector_plugin;
    vector_plugin = boost::dynamic_pointer_cast< Item<Eigen::Vector3d> >(base_plugin);
    BOOST_CHECK(vector_plugin != NULL);

    // set values
    vector_plugin->setFrame("body");
    vector_plugin->setTime(base::Time::now());
    vector_plugin->getData().x() = 2.0;
    vector_plugin->getData().y() = 3.0;
    vector_plugin->getData().z() = -5.0;

    // serialize to string stream
    std::stringstream stream;
    boost::archive::polymorphic_text_oarchive oa(stream);
    BOOST_CHECK(Serialization::save(oa, base_plugin));

    // deserialize from string stream
    boost::archive::polymorphic_text_iarchive ia(stream);
    envire::core::ItemBase::Ptr base_plugin_2 = NULL;
    BOOST_CHECK(Serialization::load(ia, base_plugin_2));

    // check if internal state is identical
    Item<Eigen::Vector3d>::Ptr vector_plugin_2 = boost::dynamic_pointer_cast< Item<Eigen::Vector3d> >(base_plugin_2);
    BOOST_CHECK(vector_plugin_2->getFrame() == vector_plugin->getFrame());
    BOOST_CHECK(vector_plugin_2->getData() == vector_plugin->getData());
    BOOST_CHECK(vector_plugin_2->getID() == vector_plugin->getID());
    BOOST_CHECK(vector_plugin_2->getTime() == vector_plugin->getTime());
}

BOOST_AUTO_TEST_CASE(vector_plugin_serialization_binary)
{
    BOOST_CHECK(envire::ClassLoader::getInstance()->hasValidPluginPath());

    // create vector plugin
    auto base_plugin = envire::ClassLoader::getInstance()->createItem("VectorPlugin");
    BOOST_CHECK(base_plugin != NULL);
    Item<Eigen::Vector3d>::Ptr vector_plugin;
    vector_plugin = boost::dynamic_pointer_cast< Item<Eigen::Vector3d> >(base_plugin);
    BOOST_CHECK(vector_plugin != NULL);

    // set values
    vector_plugin->setFrame("body");
    vector_plugin->setTime(base::Time::now());
    vector_plugin->getData().x() = 2.0;
    vector_plugin->getData().y() = 3.0;
    vector_plugin->getData().z() = -5.0;

    // serialize to string stream
    std::stringstream stream;
    boost::archive::polymorphic_binary_oarchive oa(stream);
    BOOST_CHECK(Serialization::save(oa, base_plugin));

    // deserialize from string stream
    boost::archive::polymorphic_binary_iarchive ia(stream);
    envire::core::ItemBase::Ptr base_plugin_2 = NULL;
    BOOST_CHECK(Serialization::load(ia, base_plugin_2));

    // check if internal state is identical
    Item<Eigen::Vector3d>::Ptr vector_plugin_2 = boost::dynamic_pointer_cast< Item<Eigen::Vector3d> >(base_plugin_2);
    BOOST_CHECK(vector_plugin_2->getFrame() == vector_plugin->getFrame());
    BOOST_CHECK(vector_plugin_2->getData() == vector_plugin->getData());
    BOOST_CHECK(vector_plugin_2->getID() == vector_plugin->getID());
    BOOST_CHECK(vector_plugin_2->getTime() == vector_plugin->getTime());
}

BOOST_AUTO_TEST_CASE(transform_graph_serialization_binary)
{
    // add transformation
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    TransformGraph graph;
    Transform tf;
    tf.transform.translation << 42, 21, -42;
    tf.transform.orientation = base::AngleAxisd(0.25, base::Vector3d::UnitX());
    tf.transform.cov = base::TransformWithCovariance::Covariance::Identity() * 2.0;
    graph.addTransform(a, b, tf);

    // add items to the graph
    auto base_plugin_a = envire::ClassLoader::getInstance()->createItem("VectorPlugin");
    Item<Eigen::Vector3d>::Ptr vector_plugin_a = boost::dynamic_pointer_cast< Item<Eigen::Vector3d> >(base_plugin_a);
    BOOST_CHECK(vector_plugin_a != NULL);
    vector_plugin_a->setFrame(a);
    vector_plugin_a->setTime(base::Time::now());
    vector_plugin_a->getData().x() = 2.0;
    vector_plugin_a->getData().y() = 3.0;
    vector_plugin_a->getData().z() = -5.0;

    auto base_plugin_b = envire::ClassLoader::getInstance()->createItem("VectorPlugin");
    Item<Eigen::Vector3d>::Ptr vector_plugin_b = boost::dynamic_pointer_cast< Item<Eigen::Vector3d> >(base_plugin_b);
    BOOST_CHECK(vector_plugin_b != NULL);
    vector_plugin_b->setFrame(b);
    vector_plugin_b->setTime(base::Time::now());
    vector_plugin_b->getData().x() = -15.0;
    vector_plugin_b->getData().y() = -94.0;
    vector_plugin_b->getData().z() = 68.0;

    graph.addItemToFrame(a, vector_plugin_a);
    graph.addItemToFrame(b, vector_plugin_b);

    // check if it was added correctly
    BOOST_CHECK(graph.getItemCount(a) == 1);
    BOOST_CHECK(graph.getItemCount(b) == 1);

    // serialize graph to string stream
    std::stringstream stream;
    boost::archive::polymorphic_binary_oarchive oa(stream);
    oa << graph;

    // deserialize graph from string stream
    boost::archive::polymorphic_binary_iarchive ia(stream);
    TransformGraph graph_2;
    ia >> graph_2;

    // check vertecies and edges
    BOOST_CHECK(graph.graph().max_vertex_index() == graph_2.graph().max_vertex_index());
    BOOST_CHECK(graph.graph().max_edge_index() == graph_2.graph().max_edge_index());
    BOOST_CHECK(graph.num_edges() == graph_2.num_edges());
    BOOST_CHECK(graph.num_vertices() == graph_2.num_vertices());

    // check environment
    BOOST_CHECK(graph.graph()[boost::graph_bundle].uuid == graph_2.graph()[boost::graph_bundle].uuid);
    BOOST_CHECK(graph.graph()[boost::graph_bundle].name == graph_2.graph()[boost::graph_bundle].name);

    // check if frames are available
    BOOST_CHECK(graph_2.getItemCount(a) == graph.getItemCount(a));
    BOOST_CHECK(graph_2.getItemCount(b) == graph.getItemCount(b));

    // check if items are the same
    using Iterator = TransformGraph::ItemIterator<Item<Eigen::Vector3d>::Ptr>;
    Iterator begin, end;
    boost::tie(begin, end) = graph_2.getItems<Item<Eigen::Vector3d>::Ptr>(a);
    BOOST_CHECK(begin != end);
    BOOST_CHECK((*begin)->getFrame() == vector_plugin_a->getFrame());
    BOOST_CHECK((*begin)->getData() == vector_plugin_a->getData());
    BOOST_CHECK((*begin)->getID() == vector_plugin_a->getID());
    BOOST_CHECK((*begin)->getTime() == vector_plugin_a->getTime());

    boost::tie(begin, end) = graph_2.getItems<Item<Eigen::Vector3d>::Ptr>(b);
    BOOST_CHECK(begin != end);
    BOOST_CHECK((*begin)->getFrame() == vector_plugin_b->getFrame());
    BOOST_CHECK((*begin)->getData() == vector_plugin_b->getData());
    BOOST_CHECK((*begin)->getID() == vector_plugin_b->getID());
    BOOST_CHECK((*begin)->getTime() == vector_plugin_b->getTime());

    // check if transformation is the same
    Transform tf_2 = graph_2.getTransform(a, b);
    BOOST_CHECK(tf.time == tf_2.time);
    BOOST_CHECK(tf.transform.cov == tf_2.transform.cov);
    BOOST_CHECK(tf.transform.translation == tf_2.transform.translation);
    BOOST_CHECK(tf.transform.orientation.matrix() == tf_2.transform.orientation.matrix());
}
