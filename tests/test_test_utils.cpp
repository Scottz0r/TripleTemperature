#include <boost/test/unit_test.hpp>

// File being tested:
#include "test_utils.h"

BOOST_AUTO_TEST_SUITE(test_utils)

BOOST_AUTO_TEST_CASE(it_should_do_always_when_scope_ends)
{
    int some_number = 123;

    {
        auto always = make_always([&]() { some_number = 456; });
        BOOST_TEST(some_number == 123);
    }

    BOOST_TEST(some_number == 456);
}

BOOST_AUTO_TEST_SUITE_END()
