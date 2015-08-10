#include <boost/test/unit_test.hpp>
#include <iostream>
#include <envire_core/DummyItem.hpp>

BOOST_AUTO_TEST_CASE(contructors)
{
    envire::core::Item<DummyClass> item(1);
    //envire::core::Item<DummyClass> item;
    //envire::core::DummyItem item(1);
    std::cout << "Constructor Test Done" << std::endl;
}

BOOST_AUTO_TEST_CASE(welcome)
{
    envire::core::DummyClass dummy_object(1);
    dummy_object.welcome();
    envire::core::DummyItem item(1);
    item.setData(dummy_object);
    std::cout << "Set data Test Done" << std::endl;
}
