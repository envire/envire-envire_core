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
