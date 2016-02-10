#include <boost/test/unit_test.hpp>
#include <envire_core/items/Item.hpp>
#include <Eigen/Geometry>
#include <class_loader/class_loader.h>
#include <envire_core/plugin/ClassLoader.hpp>
#include <envire_core/plugin/PluginManager.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <vector>

using namespace envire::core;
using namespace envire;
using namespace std;

BOOST_AUTO_TEST_CASE(test_copy_operators)
{
    class StringItem : public envire::core::Item<std::string>
    {
        virtual std::string getClassName() const { return "StringItem"; }
    };

    StringItem string_item;
    string_item.setData("In-A-Gadda-Da-Vida");
    string_item.setTime(base::Time::now());
    string_item.setFrame("map_frame");

    // copy to new item
    StringItem string_item_copy;
    string_item_copy = string_item;
    BOOST_CHECK(string_item.getData() == string_item_copy.getData());
    BOOST_CHECK(string_item.getFrame() == string_item_copy.getFrame());
    BOOST_CHECK(string_item.getTime() == string_item_copy.getTime());
    BOOST_CHECK(string_item.getID() == string_item_copy.getID());

    // copy constructor
    StringItem string_item_copy_2(string_item);
    BOOST_CHECK(string_item.getData() == string_item_copy_2.getData());
    BOOST_CHECK(string_item.getFrame() == string_item_copy_2.getFrame());
    BOOST_CHECK(string_item.getTime() == string_item_copy_2.getTime());
    BOOST_CHECK(string_item.getID() == string_item_copy_2.getID());

    StringItem string_item_move;
    string_item_move = std::move(string_item_copy);
    BOOST_CHECK(string_item.getData() == string_item_move.getData());
    BOOST_CHECK(string_item.getFrame() == string_item_move.getFrame());
    BOOST_CHECK(string_item.getTime() == string_item_move.getTime());
    BOOST_CHECK(string_item.getID() == string_item_move.getID());

    StringItem string_item_move_2(std::move(string_item_copy_2));
    BOOST_CHECK(string_item.getData() == string_item_move_2.getData());
    BOOST_CHECK(string_item.getFrame() == string_item_move_2.getFrame());
    BOOST_CHECK(string_item.getTime() == string_item_move_2.getTime());
    BOOST_CHECK(string_item.getID() == string_item_move_2.getID());
}

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
    BOOST_CHECK(classes.size() == 1);
    for(auto &class_name : classes)
        BOOST_CHECK(class_name == "VectorPlugin");

    // create instance
    boost::shared_ptr<ItemBase> vector_plugin = loader.createInstance<ItemBase>("VectorPlugin");
    BOOST_CHECK(vector_plugin->getClassName() == "VectorPlugin");

    // create another instance
    boost::shared_ptr<ItemBase> vector_plugin_2 = loader.createInstance<ItemBase>("VectorPlugin");
    BOOST_CHECK(vector_plugin->getID() != vector_plugin_2->getID());

    Item<Eigen::Vector3d>* vector_plugin_item = dynamic_cast< Item<Eigen::Vector3d>* >(vector_plugin.get());
    BOOST_CHECK(vector_plugin_item);

    Eigen::Vector3d data(1.0,2.0,3.0);
    vector_plugin_item->setData(data);
    BOOST_CHECK(vector_plugin_item->getData() == data);
}


BOOST_AUTO_TEST_CASE(class_loader_test)
{
    BOOST_CHECK(envire::ClassLoader::getInstance()->hasValidPluginPath());
    BOOST_CHECK(!envire::ClassLoader::getInstance()->hasItem("VectorPlugin"));

    ItemBase::Ptr vector_plugin = envire::ClassLoader::getInstance()->createItem("VectorPlugin");
    BOOST_CHECK(envire::ClassLoader::getInstance()->hasItem("VectorPlugin"));
    BOOST_CHECK(vector_plugin->getClassName() == "VectorPlugin");
    BOOST_CHECK(vector_plugin.use_count() == 1);
    ItemBase::Ptr vector_plugin_2 = vector_plugin;
    BOOST_CHECK(vector_plugin.use_count() == 2);
    BOOST_CHECK(vector_plugin->getID() == vector_plugin_2->getID());

    Item<Eigen::Vector3d>::Ptr vector_plugin_3 = envire::ClassLoader::getInstance()->createItem< Item<Eigen::Vector3d> >("VectorPlugin");
    BOOST_CHECK(vector_plugin_3->getClassName() == "VectorPlugin");
    BOOST_CHECK(vector_plugin_2->getID() != vector_plugin_3->getID());
}

BOOST_AUTO_TEST_CASE(plugin_manager_test)
{
    // load xml files
    std::vector<std::string> xml_paths;
    const char* root_folder = std::getenv("AUTOPROJ_CURRENT_ROOT");
    BOOST_CHECK(root_folder != NULL);
    std::string root_folder_str(root_folder);
    root_folder_str += "/envire/envire_core/test/";
    xml_paths.push_back(root_folder_str);
    PluginManager plugin_manager(xml_paths, false);

    // check available classes
    std::vector<std::string> available_classes = plugin_manager.getAvailableClasses();
    BOOST_CHECK(available_classes.size() == 2);
    BOOST_CHECK(available_classes.back() == "envire::VectorPlugin");
    BOOST_CHECK(available_classes.front() == "envire::FakePlugin");

    // check if they are both of the same type
    available_classes = plugin_manager.getAvailableClasses("envire::core::ItemBase");
    BOOST_CHECK(available_classes.size() == 2);

    // get base class with full type name
    std::string base_class;
    BOOST_CHECK(plugin_manager.getBaseClass("envire::VectorPlugin", base_class));
    BOOST_CHECK(base_class == "envire::core::ItemBase");

    // get base class with class name only
    base_class.clear();
    BOOST_CHECK(plugin_manager.getBaseClass("VectorPlugin", base_class));
    BOOST_CHECK(base_class == "envire::core::ItemBase");

    // get base class of not existing class
    base_class.clear();
    BOOST_CHECK(plugin_manager.getBaseClass("UnknownPlugin", base_class) == false);

    // get library path
    std::string library_path;
    BOOST_CHECK(plugin_manager.getClassLibraryPath("envire::VectorPlugin", library_path));
    BOOST_CHECK(library_path == "lib/VectorPlugin_envire_plugin");

    // get all registered libraries
    std::set<std::string> libs = plugin_manager.getRegisteredLibraries();
    BOOST_CHECK(libs.size() == 1);
    BOOST_CHECK(*libs.begin() == "lib/VectorPlugin_envire_plugin");

    // remove one of the classes
    BOOST_CHECK(plugin_manager.removeClassInfo("envire::FakePlugin"));

    available_classes = plugin_manager.getAvailableClasses();
    BOOST_CHECK(available_classes.size() == 1);

    // remove all classes
    plugin_manager.clear();
    available_classes = plugin_manager.getAvailableClasses();
    BOOST_CHECK(available_classes.size() == 0);

    // reload class info
    plugin_manager.reloadXMLPluginFiles();
    available_classes = plugin_manager.getAvailableClasses();
    BOOST_CHECK(available_classes.size() == 2);
}