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

#include <boost/test/unit_test.hpp>
#define private public
#define protected public
#include <Eigen/Geometry>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/filesystem.hpp>

#include <envire_core/serialization/Serialization.hpp>
#include <envire_core/items/Item.hpp>
#include <envire_core/plugin/ClassLoader.hpp>
#include <envire_core/graph/TransformGraph.hpp>
#include <envire_core/graph/GraphViz.hpp>
#include <envire_core/graph/EnvireGraph.hpp>

using namespace envire::core;

struct LoadLocalPlugins {
    LoadLocalPlugins()
    {
        ClassLoader* loader = ClassLoader::getInstance();
        std::vector<std::string> xml_paths;
        const char* root_folder = std::getenv("AUTOPROJ_CURRENT_ROOT");
        std::string root_folder_str(root_folder);
        root_folder_str += "/envire/envire_core/test";
        xml_paths.push_back(root_folder_str);
        loader->clear();
        loader->overridePluginXmlPaths(xml_paths);
        loader->reloadXMLPluginFiles();
    }
    ~LoadLocalPlugins()
    {
        ClassLoader::getInstance()->clear();
    }
};

BOOST_GLOBAL_FIXTURE( LoadLocalPlugins );

BOOST_AUTO_TEST_CASE(item_serialization_expected_failures)
{
    class DummyObject
    {
        std::string s;
    };

    envire::core::Item<DummyObject>::Ptr dummy_item(new envire::core::Item<DummyObject>);
    envire::core::ItemBase::Ptr base_item = boost::dynamic_pointer_cast< envire::core::ItemBase >(dummy_item);
    std::string class_name;
    BOOST_CHECK(base_item->getClassName(class_name) == false);


    std::stringstream stream;
    boost::archive::binary_oarchive oa(stream);
    BOOST_CHECK(Serialization::save(oa, base_item) == false);

    ItemHeader header("dummy_item");
    oa << BOOST_SERIALIZATION_NVP(header);

    boost::archive::binary_iarchive ia(stream);
    BOOST_CHECK(Serialization::load(ia, base_item) == false);
}

BOOST_AUTO_TEST_CASE(vector_plugin_serialization_text)
{
    // create vector plugin
    envire::core::ItemBase::Ptr base_plugin;
    BOOST_CHECK(envire::core::ClassLoader::getInstance()->createEnvireItem("envire::core::Item<Eigen::Vector3d>", base_plugin));
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
    boost::archive::text_oarchive oa(stream);
    BOOST_CHECK(Serialization::save(oa, base_plugin));

    // deserialize from string stream
    boost::archive::text_iarchive ia(stream);
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
    // create vector plugin
    envire::core::ItemBase::Ptr base_plugin;
    BOOST_CHECK(envire::core::ClassLoader::getInstance()->createEnvireItem("envire::core::Item<Eigen::Vector3d>", base_plugin));
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
    boost::archive::binary_oarchive oa(stream);
    BOOST_CHECK(Serialization::save(oa, base_plugin));

    // deserialize from string stream
    boost::archive::binary_iarchive ia(stream);
    envire::core::ItemBase::Ptr base_plugin_2 = NULL;
    BOOST_CHECK(Serialization::load(ia, base_plugin_2));

    // check if internal state is identical
    Item<Eigen::Vector3d>::Ptr vector_plugin_2 = boost::dynamic_pointer_cast< Item<Eigen::Vector3d> >(base_plugin_2);
    BOOST_CHECK(vector_plugin_2->getFrame() == vector_plugin->getFrame());
    BOOST_CHECK(vector_plugin_2->getData() == vector_plugin->getData());
    BOOST_CHECK(vector_plugin_2->getID() == vector_plugin->getID());
    BOOST_CHECK(vector_plugin_2->getTime() == vector_plugin->getTime());

    // serialize to binary blob
    std::vector<uint8_t> bin;
    stream.clear();
    BOOST_CHECK(Serialization::saveToBinary(bin, base_plugin));
    envire::core::ItemBase::Ptr base_plugin_3 = NULL;
    BOOST_CHECK(Serialization::loadFromBinary(bin, base_plugin_3));
    Item<Eigen::Vector3d>::Ptr vector_plugin_3 = boost::dynamic_pointer_cast< Item<Eigen::Vector3d> >(base_plugin_3);
    BOOST_CHECK(vector_plugin_3->getFrame() == vector_plugin->getFrame());
    BOOST_CHECK(vector_plugin_3->getData() == vector_plugin->getData());
    BOOST_CHECK(vector_plugin_3->getID() == vector_plugin->getID());
    BOOST_CHECK(vector_plugin_3->getTime() == vector_plugin->getTime());
}

BOOST_AUTO_TEST_CASE(test_unkown_plugin_binary_deserialization)
{
    //Note: This code block can be used to recreate the serialized test files if nessecary
//     envire::core::Item<Eigen::Vector3d>::Ptr plugin;
//     BOOST_CHECK(envire::core::ClassLoader::getInstance()->createEnvireItem< envire::core::Item<Eigen::Vector3d> >("envire::core::Item<Eigen::Vector3d>", plugin));
//     plugin->setFrame("body");
//     plugin->setTime(base::Time::now());
//     plugin->getData().x() = -1.0;
//     plugin->getData().y() = 2.0;
//     plugin->getData().z() = -3.0;
//
//     std::cerr << "ID: " << plugin->getIDString() << std::endl;
//     std::cerr << "Time: " << plugin->getTime().microseconds << std::endl;
//
//     std::ofstream ostream("vector_plugin.bin");
//     boost::archive::binary_oarchive oa(ostream);
//     BOOST_CHECK(Serialization::save(oa, plugin));
//     ostream.close();
//
//     std::ofstream ostream_2("vector_plugin.asc");
//     boost::archive::text_oarchive oa_s(ostream_2);
//     BOOST_CHECK(Serialization::save(oa_s, plugin));
//     ostream_2.close();

    const char* root_path = std::getenv("AUTOPROJ_CURRENT_ROOT");
    std::string serialized_file;
    if(root_path != NULL)
    {
        serialized_file = std::string(root_path) + "/envire/envire_core/test/vector_plugin.bin";
    }
    boost::filesystem::path file(serialized_file);
    BOOST_CHECK(boost::filesystem::exists(file));

    // deserialization plugin
    std::ifstream istream(serialized_file.c_str());
    boost::archive::binary_iarchive ia(istream);
    envire::core::ItemBase::Ptr base_plugin;
    BOOST_CHECK(Serialization::load(ia, base_plugin));

    BOOST_CHECK(base_plugin->getFrame() == "body");
    BOOST_CHECK(base_plugin->getIDString() == "f41cb0be-d8de-4165-a60b-7288f0b73945");
    BOOST_CHECK(base_plugin->getTime().microseconds == 1467802240692835);

    Item<Eigen::Vector3d>::Ptr vector_plugin = boost::dynamic_pointer_cast< Item<Eigen::Vector3d> >(base_plugin);
    BOOST_CHECK(vector_plugin.get() != NULL);
    BOOST_CHECK(vector_plugin->getData().x() == -1.0);
    BOOST_CHECK(vector_plugin->getData().y() == 2.0);
    BOOST_CHECK(vector_plugin->getData().z() == -3.0);
}

BOOST_AUTO_TEST_CASE(test_unkown_plugin_text_deserialization)
{
    const char* root_path = std::getenv("AUTOPROJ_CURRENT_ROOT");
    std::string serialized_file;
    if(root_path != NULL)
    {
        serialized_file = std::string(root_path) + "/envire/envire_core/test/vector_plugin.asc";
    }
    boost::filesystem::path file(serialized_file);
    BOOST_CHECK(boost::filesystem::exists(file));

    // deserialization plugin
    std::ifstream istream(serialized_file.c_str());
    boost::archive::text_iarchive ia(istream);
    envire::core::ItemBase::Ptr base_plugin;
    BOOST_CHECK(Serialization::load(ia, base_plugin));

    BOOST_CHECK(base_plugin->getFrame() == "body");
    BOOST_CHECK(base_plugin->getIDString() == "f41cb0be-d8de-4165-a60b-7288f0b73945");
    BOOST_CHECK(base_plugin->getTime().microseconds == 1467802240692835);

    Item<Eigen::Vector3d>::Ptr vector_plugin = boost::dynamic_pointer_cast< Item<Eigen::Vector3d> >(base_plugin);
    BOOST_CHECK(vector_plugin.get() != NULL);
    BOOST_CHECK(vector_plugin->getData().x() == -1.0);
    BOOST_CHECK(vector_plugin->getData().y() == 2.0);
    BOOST_CHECK(vector_plugin->getData().z() == -3.0);
}

BOOST_AUTO_TEST_CASE(envire_graph_serialization_binary)
{
    // add transformation
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    EnvireGraph graph;
    Transform tf;
    tf.transform.translation << 42, 21, -42;
    tf.transform.orientation = base::AngleAxisd(0.25, base::Vector3d::UnitX());
    tf.transform.cov = base::TransformWithCovariance::Covariance::Identity() * 2.0;
    graph.addTransform(a, b, tf);

    // add items to the graph
    envire::core::ItemBase::Ptr base_plugin_a;
    BOOST_CHECK(envire::core::ClassLoader::getInstance()->createEnvireItem("envire::core::Item<Eigen::Vector3d>", base_plugin_a));
    Item<Eigen::Vector3d>::Ptr vector_plugin_a = boost::dynamic_pointer_cast< Item<Eigen::Vector3d> >(base_plugin_a);
    BOOST_CHECK(vector_plugin_a != NULL);
    vector_plugin_a->setFrame(a);
    vector_plugin_a->setTime(base::Time::now());
    vector_plugin_a->getData().x() = 2.0;
    vector_plugin_a->getData().y() = 3.0;
    vector_plugin_a->getData().z() = -5.0;

    envire::core::ItemBase::Ptr base_plugin_b;
    BOOST_CHECK(envire::core::ClassLoader::getInstance()->createEnvireItem("envire::core::Item<Eigen::Vector3d>", base_plugin_b));
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
    BOOST_CHECK(graph.getTotalItemCount(a) == 1);
    BOOST_CHECK(graph.getTotalItemCount(b) == 1);

    // serialize graph to string stream
    std::stringstream stream;
    boost::archive::binary_oarchive oa(stream);
    oa << graph;

    // deserialize graph from string stream
    boost::archive::binary_iarchive ia(stream);
    EnvireGraph graph_2;
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
    BOOST_CHECK(graph_2.getTotalItemCount(a) == graph.getTotalItemCount(a));
    BOOST_CHECK(graph_2.getTotalItemCount(b) == graph.getTotalItemCount(b));

    // check if items are the same
    using Iterator = EnvireGraph::ItemIterator<Item<Eigen::Vector3d>>;
    Iterator begin, end;
    boost::tie(begin, end) = graph_2.getItems<Item<Eigen::Vector3d>>(a);
    BOOST_CHECK(begin != end);
    
    BOOST_CHECK(begin->getFrame() == vector_plugin_a->getFrame());
    BOOST_CHECK(begin->getData() == vector_plugin_a->getData());
    BOOST_CHECK(begin->getID() == vector_plugin_a->getID());
    BOOST_CHECK(begin->getTime() == vector_plugin_a->getTime());

    boost::tie(begin, end) = graph_2.getItems<Item<Eigen::Vector3d>>(b);
    BOOST_CHECK(begin != end);
    BOOST_CHECK(begin->getFrame() == vector_plugin_b->getFrame());
    BOOST_CHECK(begin->getData() == vector_plugin_b->getData());
    BOOST_CHECK(begin->getID() == vector_plugin_b->getID());
    BOOST_CHECK(begin->getTime() == vector_plugin_b->getTime());

    // check if transformation is the same
    Transform tf_2 = graph_2.getTransform(a, b);
    BOOST_CHECK(tf.time == tf_2.time);
    BOOST_CHECK(tf.transform.cov == tf_2.transform.cov);
    BOOST_CHECK(tf.transform.translation == tf_2.transform.translation);
    BOOST_CHECK(tf.transform.orientation.matrix() == tf_2.transform.orientation.matrix());
}
