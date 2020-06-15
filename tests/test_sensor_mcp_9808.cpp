#include "fakeit.hpp"
#include "mocks/Wire.h"
#include "test_utils.h"
#include <boost/endian/conversion.hpp>
#include <boost/test/unit_test.hpp>

// File being tested:
#include "sensor_mcp_9808.h"

using namespace scottz0r::temperature;
using namespace fakeit;

/// @brief Configure the given mock with a happy path i2c setup sequence for the MCP 9808 sensor.
/// @param mock Mock object to configure.
void configure_mock_begin_happy(Mock<TwoWireImpl> &mock)
{
    // Two 2-byte available reads for device info
    When(Method(mock, available)).Return(2, 2);

    Fake(Method(mock, begin));
    Fake(Method(mock, beginTransmission));

    // Three "good" calls to endTransmission. Two for reading device info, one for writing config.
    When(Method(mock, endTransmission)).Return(0, 0, 0);

    // Four bytes read for Manufacture id 84 and device id 1024 (big endian).
    When(Method(mock, read)).Return(0x00, 0x54, 0x04, 0x00);

    Fake(Method(mock, requestFrom));

    // Always return that byte was written.
    When(Method(mock, write)).AlwaysReturn(1);
}

BOOST_AUTO_TEST_SUITE(sensor_mcp_9808)

BOOST_AUTO_TEST_CASE(it_should_construct_default_state)
{
    SensorMcp9808 sensor;

    BOOST_TEST(sensor.bad());
    BOOST_TEST(!sensor.good());
}

BOOST_AUTO_TEST_CASE(it_should_begin_good_address)
{
    auto always = make_always([&]() { wire_impl = nullptr; });

    // Setup mock to get information from I2C device successfully.
    Mock<TwoWireImpl> mock;
    configure_mock_begin_happy(mock);
    wire_impl = &mock.get();

    SensorMcp9808 sensor;
    bool rc = false;

    rc = sensor.begin(0x18);
    BOOST_TEST(rc);
    BOOST_TEST(sensor.good());
    BOOST_TEST(!sensor.bad());
}

BOOST_AUTO_TEST_CASE(it_should_not_begin_setup_i2c_failure)
{
    // This test is for code coverage in read16() function to test bad transmit status and unexpected available().

    auto always = make_always([&]() { wire_impl = nullptr; });

    // Setup mock to fail on I2C register transmission.
    Mock<TwoWireImpl> mock;
    Fake(Method(mock, beginTransmission));
    Fake(Method(mock, write));
    When(Method(mock, endTransmission)).Return(1);
    When(Method(mock, requestFrom)).Return(2);
    When(Method(mock, available)).Return(2);
    When(Method(mock, read)).Return(0x00, 0x54, 0x04, 0x00);
    wire_impl = &mock.get();

    SensorMcp9808 sensor;
    bool rc = false;

    rc = sensor.begin(0x18);
    BOOST_TEST(!rc);
    BOOST_TEST(!sensor.good());
    BOOST_TEST(sensor.bad());

    // Similar test, but setup mock to return an incorrect number of bytes for 16 bit register read.
    mock.Reset();
    Fake(Method(mock, beginTransmission));
    Fake(Method(mock, write));
    When(Method(mock, endTransmission)).Return(0);
    When(Method(mock, requestFrom)).Return(1);
    When(Method(mock, available)).Return(1);
    When(Method(mock, read)).Return(0x00, 0x00);
    wire_impl = &mock.get();

    rc = sensor.begin(0x18);
    BOOST_TEST(!rc);
    BOOST_TEST(!sensor.good());
    BOOST_TEST(sensor.bad());
}

BOOST_AUTO_TEST_CASE(it_should_not_begin_bad_address)
{
    auto always = make_always([&]() { wire_impl = nullptr; });

    SensorMcp9808 sensor;
    bool rc = false;

    // Setup mock to get information from I2C device successfully.
    Mock<TwoWireImpl> mock;
    configure_mock_begin_happy(mock);
    wire_impl = &mock.get();

    // Address too small.
    rc = sensor.begin(0x17);
    BOOST_TEST(!rc);
    BOOST_TEST(!sensor.good());
    BOOST_TEST(sensor.bad());

    // Address too large.
    rc = sensor.begin(0x20);
    BOOST_TEST(!rc);
    BOOST_TEST(!sensor.good());
    BOOST_TEST(sensor.bad());
}

BOOST_AUTO_TEST_CASE(it_should_not_allow_begin_multiple_calls)
{
    auto always = make_always([&]() { wire_impl = nullptr; });

    SensorMcp9808 sensor;

    // Setup mock to get information from I2C device successfully.
    Mock<TwoWireImpl> mock;
    configure_mock_begin_happy(mock);
    wire_impl = &mock.get();

    bool rc = false;

    rc = sensor.begin(0x18);
    BOOST_TEST(rc);

    // Reset mock into a state that would start successfully, again.
    mock.Reset();
    configure_mock_begin_happy(mock);

    rc = sensor.begin(0x18);
    BOOST_TEST(!rc);
}

BOOST_AUTO_TEST_CASE(it_should_not_begin_bad_manuf_id)
{
    auto always = make_always([&]() { wire_impl = nullptr; });

    SensorMcp9808 sensor;

    // Setup a setup happy path, except the read() return values from the i2c device.
    Mock<TwoWireImpl> mock;
    When(Method(mock, available)).Return(2, 2);
    Fake(Method(mock, begin));
    Fake(Method(mock, beginTransmission));
    When(Method(mock, endTransmission)).Return(0, 0, 0);
    Fake(Method(mock, requestFrom));
    When(Method(mock, write)).AlwaysReturn(1);

    // Return bad manufacture id, but good device id.
    When(Method(mock, read)).Return(0xC3, 0x70, 0x04, 0x00);

    wire_impl = &mock.get();

    bool rc = sensor.begin(0x18);
    BOOST_TEST(!rc);
    BOOST_TEST(!sensor.good());
    BOOST_TEST(sensor.bad());
}

BOOST_AUTO_TEST_CASE(it_should_not_begin_bad_device_id)
{
    auto always = make_always([&]() { wire_impl = nullptr; });

    SensorMcp9808 sensor;

    // Setup a setup happy path, except the read() return values from the i2c device.
    Mock<TwoWireImpl> mock;
    When(Method(mock, available)).Return(2, 2);
    Fake(Method(mock, begin));
    Fake(Method(mock, beginTransmission));
    When(Method(mock, endTransmission)).Return(0, 0, 0);
    Fake(Method(mock, requestFrom));
    When(Method(mock, write)).AlwaysReturn(1);

    // Return good manufacture id, but bad device id.
    When(Method(mock, read)).Return(0x00, 0x54, 0x14, 0xAA);

    wire_impl = &mock.get();

    bool rc = sensor.begin(0x18);
    BOOST_TEST(!rc);
    BOOST_TEST(!sensor.good());
    BOOST_TEST(sensor.bad());
}

BOOST_AUTO_TEST_CASE(it_should_not_begin_bad_config)
{
    auto always = make_always([&]() { wire_impl = nullptr; });

    SensorMcp9808 sensor;

    // Setup a setup happy path, except the endTransmission() return value.
    Mock<TwoWireImpl> mock;
    When(Method(mock, available)).Return(2, 2);
    Fake(Method(mock, begin));
    Fake(Method(mock, beginTransmission));
    Fake(Method(mock, requestFrom));
    When(Method(mock, read)).Return(0x00, 0x54, 0x04, 0x00);
    When(Method(mock, write)).AlwaysReturn(1);

    // Make 3rd call to end transmission fail, which will fail the device configuration step.
    When(Method(mock, endTransmission)).Return(0, 0, 2);

    wire_impl = &mock.get();

    bool rc = sensor.begin(0x18);
    BOOST_TEST(!rc);
    BOOST_TEST(!sensor.good());
    BOOST_TEST(sensor.bad());
}

BOOST_AUTO_TEST_CASE(it_should_read_good_state_positive_temp)
{
    auto always = make_always([&]() { wire_impl = nullptr; });

    // Setup mock to get setup information from I2C device successfully.
    Mock<TwoWireImpl> mock;
    configure_mock_begin_happy(mock);
    wire_impl = &mock.get();

    SensorMcp9808 sensor;
    bool rc = false;

    rc = sensor.begin(0x18);
    BOOST_TEST(rc);

    // Setup mock to return 13.37 degrees, which is 214 in the device's max resolution.
    // Device returns big endian two's complement in 13 bits.
    mock.Reset();
    Fake(Method(mock, beginTransmission));
    Fake(Method(mock, write));
    When(Method(mock, endTransmission)).Return(0);
    When(Method(mock, requestFrom)).Return(2);
    When(Method(mock, available)).Return(2);
    When(Method(mock, read)).Return(0x00, 0xD6);

    int16_t temp = 0;
    rc = sensor.read_temp(temp);

    BOOST_TEST(rc);
    BOOST_TEST(temp == 1337);
}

BOOST_AUTO_TEST_CASE(it_should_read_good_state_negative_temp)
{
    auto always = make_always([&]() { wire_impl = nullptr; });

    // Setup mock to get setup information from I2C device successfully.
    Mock<TwoWireImpl> mock;
    configure_mock_begin_happy(mock);
    wire_impl = &mock.get();

    SensorMcp9808 sensor;
    bool rc = false;

    rc = sensor.begin(0x18);
    BOOST_TEST(rc);

    // Setup mock to return -35.00 degrees. Device returns big endian two's complement in 13 bits.
    mock.Reset();
    Fake(Method(mock, beginTransmission));
    Fake(Method(mock, write));
    When(Method(mock, endTransmission)).Return(0);
    When(Method(mock, requestFrom)).Return(2);
    When(Method(mock, available)).Return(2);
    When(Method(mock, read)).Return(0x1D, 0xD0);

    int16_t temp = 0;
    rc = sensor.read_temp(temp);

    BOOST_TEST(rc);
    BOOST_TEST(temp == -3500);
}

BOOST_AUTO_TEST_CASE(it_should_read_good_state_failure)
{
    auto always = make_always([&]() { wire_impl = nullptr; });

    // Setup mock to get setup information from I2C device successfully.
    Mock<TwoWireImpl> mock;
    configure_mock_begin_happy(mock);
    wire_impl = &mock.get();

    SensorMcp9808 sensor;
    bool rc = false;

    rc = sensor.begin(0x18);
    BOOST_TEST(rc);

    // Setup mock to make next read a failure.
    mock.Reset();
    Fake(Method(mock, beginTransmission));
    Fake(Method(mock, write));
    When(Method(mock, endTransmission)).Return(1); // Failure transmission.

    int16_t temp = 0;
    rc = sensor.read_temp(temp);

    BOOST_TEST(!rc);
}

BOOST_AUTO_TEST_CASE(it_should_return_false_read_bad_state)
{
    SensorMcp9808 sensor;

    // Don't call begin(), which means sensor is in a bad or not configured state.
    int16_t sensor_reading = 123;
    bool rc = sensor.read_temp(sensor_reading);

    BOOST_TEST(sensor.bad());
    BOOST_TEST(!sensor.good());
    BOOST_TEST(!rc);
    BOOST_TEST(sensor_reading == 0);
}

BOOST_AUTO_TEST_SUITE_END()
