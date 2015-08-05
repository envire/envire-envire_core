#include <boost/test/unit_test.hpp>
#include <iostream>
#include <envire_core/DummyItem.hpp>

BOOST_AUTO_TEST_CASE(contructors)
{
    envire::core::DummyItem item;
    std::cout << "Constructor Test Done" << std::endl;
}

BOOST_AUTO_TEST_CASE(welcome)
{
    envire::core::DummyClass dummy_object;
    dummy_object.welcome();
    envire::core::DummyItem item;
    item.setData(dummy_object);
    std::cout << "Set data Test Done" << std::endl;
}
