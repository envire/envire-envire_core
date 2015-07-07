#include <boost/test/unit_test.hpp>
#include <envire_core/Item.hpp>
#include <Eigen/Geometry>
#include <class_loader/class_loader.h>

using namespace envire::core;

BOOST_AUTO_TEST_CASE(vector_plugin_test)
{
    if(const char* env_p = std::getenv("LD_LIBRARY_PATH"))
    {
        std::string path_plugin(env_p);
        path_plugin += "/libtest_envire_plugin.so";
        class_loader::ClassLoader loader(path_plugin, false);

        std::vector<std::string> classes = loader.getAvailableClasses<ItemBase>();
        std::cerr << "Available plugins: " << std::endl;
        for(auto &class_name : classes)
            std::cerr << "  " << class_name << std::endl;

        boost::shared_ptr<ItemBase> vector_plugin = loader.createInstance<ItemBase>("VectorPlugin");
        std::cerr << "class name: " << vector_plugin->getClassName() << std::endl;
        std::cerr << "class uuid: " << vector_plugin->getIDString() << std::endl;

        Item<Eigen::Vector3d>* vector_plugin_real = dynamic_cast< Item<Eigen::Vector3d>* >(vector_plugin.get());
        if(vector_plugin_real != NULL)
        {
            vector_plugin_real->setData(Eigen::Vector3d(1.0,2.0,3.0));
            std::cerr << "class data: " << vector_plugin_real->getData().transpose() << std::endl;
        }
    }

}