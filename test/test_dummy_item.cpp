#include <boost/test/unit_test.hpp>
#include <iostream>
#include <envire_core/DummyItem.hpp>
#include <assert.h>

using namespace envire::core;

BOOST_AUTO_TEST_CASE(contructors)
{
    std::cout << "TEST: Constructor With Parameters Instantiation Test" << std::endl;
    Item<DummyClass> item(1);
    //envire::core::Item<DummyClass> item;
    //envire::core::DummyItem item(1);
    std::cout << "Constructor Test Done" << std::endl;
}

BOOST_AUTO_TEST_CASE(contructor_parameters_forwarding)
{
    std::cout << "TEST: Constructor With Parameters Forwarding Test" << std::endl;
    int id = 1;
    DummyItem item(id);
    DummyClass dummy_object = item.getData();
    assert(id==dummy_object.getId());
}
