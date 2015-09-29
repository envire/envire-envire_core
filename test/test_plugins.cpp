#include <boost/test/unit_test.hpp>
#include <envire_core/items/Item.hpp>
#include <Eigen/Geometry>
#include <class_loader/class_loader.h>
#include <envire_core/plugin/ClassLoader.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <vector>

using namespace envire::core;
using namespace envire;
using namespace std;

BOOST_AUTO_TEST_CASE(vector_plugin_test)
{
    vector<string> paths = ClassLoader::loadLibraryPath();
    //find correct lib path
    string path_plugin = "";
    for(string path : paths)
    {
        path += "/libVectorPlugin_envire_plugin.so";
        if(boost::filesystem::exists(path))
        {
            path_plugin = path;
        }
    }
    BOOST_CHECK(path_plugin.length() > 0);
    class_loader::ClassLoader loader(path_plugin, false);

    // check available classes
    vector<string> classes = loader.getAvailableClasses<ItemBase>();
    BOOST_ASSERT(classes.size() == 1);
    for(auto &class_name : classes)
        BOOST_ASSERT(class_name == "VectorPlugin");

    // create instance
    boost::shared_ptr<ItemBase> vector_plugin = loader.createInstance<ItemBase>("VectorPlugin");
    BOOST_ASSERT(vector_plugin->getClassName() == "VectorPlugin");

    // create another instance
    boost::shared_ptr<ItemBase> vector_plugin_2 = loader.createInstance<ItemBase>("VectorPlugin");
    BOOST_ASSERT(vector_plugin->getID() != vector_plugin_2->getID());

    Item<Eigen::Vector3d>* vector_plugin_item = dynamic_cast< Item<Eigen::Vector3d>* >(vector_plugin.get());
    BOOST_ASSERT(vector_plugin_item);

    Eigen::Vector3d data(1.0,2.0,3.0);
    vector_plugin_item->setData(data);
    BOOST_ASSERT(vector_plugin_item->getData() == data);
}


BOOST_AUTO_TEST_CASE(class_loader_test)
{
    BOOST_ASSERT(envire::ClassLoader::getInstance()->hasValidPluginPath());
    BOOST_ASSERT(!envire::ClassLoader::getInstance()->hasItem("VectorPlugin"));

    ItemBase::Ptr vector_plugin = envire::ClassLoader::getInstance()->createItem("VectorPlugin");
    BOOST_ASSERT(envire::ClassLoader::getInstance()->hasItem("VectorPlugin"));
    BOOST_ASSERT(vector_plugin->getClassName() == "VectorPlugin");
    BOOST_ASSERT(vector_plugin->getRefCount() == 1);
    ItemBase::Ptr vector_plugin_2 = vector_plugin;
    BOOST_ASSERT(vector_plugin->getRefCount() == 2);
    BOOST_ASSERT(vector_plugin->getID() == vector_plugin_2->getID());

    Item<Eigen::Vector3d>::Ptr vector_plugin_3 = envire::ClassLoader::getInstance()->createItem< Item<Eigen::Vector3d> >("VectorPlugin");
    BOOST_ASSERT(vector_plugin_3->getClassName() == "VectorPlugin");
    BOOST_ASSERT(vector_plugin_2->getID() != vector_plugin_3->getID());
}
