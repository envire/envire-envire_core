#include <boost/test/unit_test.hpp>
#include <envire_core/Frame.hpp>

using envire::core::Frame;
using boost::uuids::uuid;

BOOST_AUTO_TEST_CASE(uuid_test)
{
    Frame f;
    BOOST_ASSERT(f.uuid.version() == uuid::version_random_number_based);
    BOOST_ASSERT(!f.uuid.is_nil());
}