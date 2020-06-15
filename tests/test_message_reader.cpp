#include "mocks/Arduino.h"
#include "test_utils.h"
#include <boost/test/unit_test.hpp>
#include <limits>

// File being tested:
#include "message_reader.h"

using namespace scottz0r::temperature;

class MockArduino : public ArduinoImpl
{
public:
    unsigned long millis() override
    {
        return next_millis;
    }

    unsigned long next_millis = 0;
};

/// @brief Sends a good request to a MessageReader object.
/// @param reader MessageReader object to send messages to.
/// @return True if every part of request reading was successful.
bool try_send_good_request(MessageReader &reader)
{
    bool rc;

    // Send message ID byte.
    rc = reader.process(0x04);
    if (rc)
    {
        return false;
    }

    // Send request type of Temperature.
    rc = reader.process(0x00);
    if (rc)
    {
        return false;
    }

    // Send checksum of 0x04 ^ 0x00 = 0x04
    rc = reader.process(0x04);
    if (!rc)
    {
        return false;
    }

    RequestType actual = RequestType::_Unknown;
    rc = reader.get_data(actual);
    if (!rc)
    {
        return false;
    }

    if (actual != RequestType::Temperature)
    {
        return false;
    }

    return true;
}

BOOST_AUTO_TEST_SUITE(message_reader)

BOOST_AUTO_TEST_CASE(it_should_process_good_request)
{
    auto _always = make_always([&]() { arduino_impl = nullptr; });

    MockArduino mock;
    arduino_impl = &mock;

    // Reader with 10 millisecond message timeout.
    MessageReader reader(10);
    bool rc = false;

    // Send message ID byte.
    mock.next_millis = 10;
    rc = reader.process(0x04);
    BOOST_TEST(!rc);

    // Send request type of Temperature.
    mock.next_millis = 11;
    rc = reader.process(0x00);
    BOOST_TEST(!rc);

    // Send checksum of 0x04 ^ 0x00 = 0x04
    mock.next_millis = 13;
    rc = reader.process(0x04);
    BOOST_TEST(rc);

    RequestType actual = RequestType::_Unknown;
    rc = reader.get_data(actual);
    BOOST_CHECK(actual == RequestType::Temperature);
}

BOOST_AUTO_TEST_CASE(it_should_manually_rest)
{
    auto _always = make_always([&]() { arduino_impl = nullptr; });

    MockArduino mock;
    arduino_impl = &mock;
    mock.next_millis = 0;

    // Reader with 10 millisecond message timeout.
    MessageReader reader(10);
    bool rc = false;

    // Send message ID byte.
    mock.next_millis = 10;
    rc = reader.process(0x04);
    BOOST_TEST(!rc);

    // Rest state.
    reader.reset();

    // Resend a good message and make sure it was read correctly.
    bool sent_good_msg = try_send_good_request(reader);
    BOOST_TEST(sent_good_msg);
}

BOOST_AUTO_TEST_CASE(it_should_reset_timeout)
{
    auto _always = make_always([&]() { arduino_impl = nullptr; });

    MockArduino mock;
    arduino_impl = &mock;

    // Reader with 25 millisecond message timeout.
    MessageReader reader(25);
    bool rc = false;

    // Send message ID byte.
    mock.next_millis = 10;
    rc = reader.process(0x04);
    BOOST_TEST(!rc);

    // Send request type of Temperature.
    mock.next_millis = 500;
    rc = reader.process(0x00);
    BOOST_TEST(!rc);

    // Send checksum of 0x04 ^ 0x00 = 0x04. This would end a valid message, but should return false because of
    // the timeout between byte 0 and byte 1.
    mock.next_millis = 501;
    rc = reader.process(0x04);
    BOOST_TEST(!rc);

    // Send another message to make sure collection state is good after a timeout reset with various bytes.
    // Mimic a normal situation where some time would pass between the prior message and the next.
    mock.next_millis = 750;
    bool sent_good_msg = try_send_good_request(reader);
    BOOST_TEST(sent_good_msg);
}

BOOST_AUTO_TEST_CASE(it_should_return_false_get_data_invalid_checksum)
{
    auto _always = make_always([&]() { arduino_impl = nullptr; });

    MockArduino mock;
    arduino_impl = &mock;

    // Reader with 10 millisecond message timeout.
    MessageReader reader(10);
    bool rc = false;

    // Send message ID byte.
    mock.next_millis = 10;
    rc = reader.process(0x04);
    BOOST_TEST(!rc);

    // Send request type of Temperature.
    mock.next_millis = 11;
    rc = reader.process(0x00);
    BOOST_TEST(!rc);

    // Bad checksum byte.
    mock.next_millis = 12;
    rc = reader.process(0x55);
    BOOST_TEST(rc);

    RequestType actual = RequestType::Temperature;
    rc = reader.get_data(actual);
    BOOST_TEST(!rc);
    BOOST_CHECK(actual == RequestType::_Unknown);
}

BOOST_AUTO_TEST_CASE(it_should_return_false_get_data_bad_request_value)
{
    auto _always = make_always([&]() { arduino_impl = nullptr; });

    MockArduino mock;
    arduino_impl = &mock;

    // Reader with 10 millisecond message timeout.
    MessageReader reader(10);
    bool rc = false;

    // Send message ID byte.
    mock.next_millis = 10;
    rc = reader.process(0x04);
    BOOST_TEST(!rc);

    // Send invalid request type.
    mock.next_millis = 11;
    rc = reader.process(0x7A);
    BOOST_TEST(!rc);

    // Good Checksum
    mock.next_millis = 12;
    rc = reader.process((0x04 ^ 0x7A));
    BOOST_TEST(rc);

    RequestType actual = RequestType::Temperature;
    rc = reader.get_data(actual);
    BOOST_TEST(!rc);
    BOOST_CHECK(actual == RequestType::_Unknown);
}

BOOST_AUTO_TEST_CASE(it_should_process_millis_roll)
{
    auto _always = make_always([&]() { arduino_impl = nullptr; });

    MockArduino mock;
    arduino_impl = &mock;

    // Reader with 10 millisecond message timeout.
    MessageReader reader(10);
    bool rc = false;

    // Send message ID byte. Timer is about to rollover.
    mock.next_millis = std::numeric_limits<unsigned long>::max() - 2;
    rc = reader.process(0x04);
    BOOST_TEST(!rc);

    // Send request type of Temperature. This is after millis rolls over.
    mock.next_millis = 1;
    rc = reader.process(0x00);
    BOOST_TEST(!rc);

    // Send checksum of 0x04 ^ 0x00 = 0x04
    mock.next_millis = 2;
    rc = reader.process(0x04);
    BOOST_TEST(rc);

    // Should get valid message even with rollover.
    RequestType actual = RequestType::_Unknown;
    rc = reader.get_data(actual);
    BOOST_CHECK(actual == RequestType::Temperature);
}

BOOST_AUTO_TEST_SUITE_END()
