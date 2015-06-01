#include <boost/test/unit_test.hpp>
#include <envire-core/Dummy.hpp>

using namespace envire-core;

BOOST_AUTO_TEST_CASE(it_should_not_crash_when_welcome_is_called)
{
    envire-core::DummyClass dummy;
    dummy.welcome();
}
