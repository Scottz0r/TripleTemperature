#include <boost/endian/conversion.hpp>
#include <boost/test/unit_test.hpp>

// File being tested:
#include "message_format.h"

using namespace scottz0r::temperature;

union Int16Splitter {
    int16_t value;
    struct
    {
        uint8_t b0;
        uint8_t b1;
    } split;
};

BOOST_AUTO_TEST_SUITE(message_format)

BOOST_AUTO_TEST_CASE(it_should_format_temperature_positive)
{
    MessageBuffer buffer;
    TemperatureVoteResult vote{};

    // Multi byte numbers are required to be in Little Endian and two's complement.
    Int16Splitter temp0, temp1, temp2, average;
    temp0.value = boost::endian::native_to_little(1000);
    temp1.value = boost::endian::native_to_little(1025);
    temp2.value = boost::endian::native_to_little(995);
    average.value = boost::endian::native_to_little(1007);

    vote.temp0 = temp0.value;
    vote.temp1 = temp1.value;
    vote.temp2 = temp2.value;
    vote.average = average.value;

    vote.is_temp0_agree = true;
    vote.is_temp1_agree = true;
    vote.is_temp2_agree = true;

    vote.status = TemperatureVoteStatus::OK;

    format_msg_temperature(buffer, vote);

    BOOST_TEST(buffer.buffer[0] == 1);
    BOOST_TEST(buffer.buffer[1] == 0);
    BOOST_TEST(buffer.buffer[2] == temp0.split.b0);
    BOOST_TEST(buffer.buffer[3] == temp0.split.b1);
    BOOST_TEST(buffer.buffer[4] == temp1.split.b0);
    BOOST_TEST(buffer.buffer[5] == temp1.split.b1);
    BOOST_TEST(buffer.buffer[6] == temp2.split.b0);
    BOOST_TEST(buffer.buffer[7] == temp2.split.b1);
    BOOST_TEST(buffer.buffer[8] == 7);
    BOOST_TEST(buffer.buffer[9] == average.split.b0);
    BOOST_TEST(buffer.buffer[10] == average.split.b1);

    uint8_t checksum = 0;
    checksum ^= 1;
    checksum ^= 0;
    checksum ^= temp0.split.b0;
    checksum ^= temp0.split.b1;
    checksum ^= temp1.split.b0;
    checksum ^= temp1.split.b1;
    checksum ^= temp2.split.b0;
    checksum ^= temp2.split.b1;
    checksum ^= 7;
    checksum ^= average.split.b0;
    checksum ^= average.split.b1;

    BOOST_TEST(buffer.buffer[11] == checksum);
}

BOOST_AUTO_TEST_CASE(it_should_format_temperature_negative)
{
    MessageBuffer buffer;
    TemperatureVoteResult vote{};

    Int16Splitter temp0, temp1, temp2, average;
    temp0.value = boost::endian::native_to_little(-3850);
    temp1.value = boost::endian::native_to_little(-3800);
    temp2.value = boost::endian::native_to_little(1000);
    average.value = boost::endian::native_to_little(-3825);

    vote.temp0 = temp0.value;
    vote.temp1 = temp1.value;
    vote.temp2 = temp2.value;
    vote.average = average.value;

    vote.is_temp0_agree = true;
    vote.is_temp1_agree = true;
    vote.is_temp2_agree = false;

    vote.status = TemperatureVoteStatus::Disagree;

    format_msg_temperature(buffer, vote);

    BOOST_TEST(buffer.buffer[0] == 1);
    BOOST_TEST(buffer.buffer[1] == 2);
    BOOST_TEST(buffer.buffer[2] == temp0.split.b0);
    BOOST_TEST(buffer.buffer[3] == temp0.split.b1);
    BOOST_TEST(buffer.buffer[4] == temp1.split.b0);
    BOOST_TEST(buffer.buffer[5] == temp1.split.b1);
    BOOST_TEST(buffer.buffer[6] == temp2.split.b0);
    BOOST_TEST(buffer.buffer[7] == temp2.split.b1);
    BOOST_TEST(buffer.buffer[8] == 3);
    BOOST_TEST(buffer.buffer[9] == average.split.b0);
    BOOST_TEST(buffer.buffer[10] == average.split.b1);

    uint8_t checksum = 0;
    checksum ^= 1;
    checksum ^= 2;
    checksum ^= temp0.split.b0;
    checksum ^= temp0.split.b1;
    checksum ^= temp1.split.b0;
    checksum ^= temp1.split.b1;
    checksum ^= temp2.split.b0;
    checksum ^= temp2.split.b1;
    checksum ^= 3;
    checksum ^= average.split.b0;
    checksum ^= average.split.b1;

    BOOST_TEST(buffer.buffer[11] == checksum);
}

BOOST_AUTO_TEST_CASE(it_should_format_temperature_bad_status_enum)
{
    MessageBuffer buffer;
    TemperatureVoteResult vote{};

    vote.status = static_cast<TemperatureVoteStatus>(120);

    format_msg_temperature(buffer, vote);

    BOOST_TEST(buffer.buffer[0] == 1);
    BOOST_TEST(buffer.buffer[1] == 3);
    BOOST_TEST(buffer.buffer[2] == 0);
    BOOST_TEST(buffer.buffer[3] == 0);
    BOOST_TEST(buffer.buffer[4] == 0);
    BOOST_TEST(buffer.buffer[5] == 0);
    BOOST_TEST(buffer.buffer[6] == 0);
    BOOST_TEST(buffer.buffer[7] == 0);
    BOOST_TEST(buffer.buffer[8] == 0);
    BOOST_TEST(buffer.buffer[9] == 0);
    BOOST_TEST(buffer.buffer[10] == 0);
    BOOST_TEST(buffer.buffer[11] == (1 ^ 3));
}

BOOST_AUTO_TEST_CASE(it_should_format_system_status)
{
    MessageBuffer buffer;
    SystemSensorStatus status;

    status.is_sensor_0_good = true;
    status.is_sensor_1_good = false;
    status.is_sensor_2_good = true;
    status.system_status = SystemStatus::OK;

    format_msg_system_status(buffer, status);

    BOOST_TEST(buffer.message_size == 4);
    BOOST_TEST(buffer.buffer[0] == 2);
    BOOST_TEST(buffer.buffer[1] == 0);
    BOOST_TEST(buffer.buffer[2] == 5);
    BOOST_TEST(buffer.buffer[3] == (2 ^ 0 ^ 5));

    status.is_sensor_0_good = false;
    status.is_sensor_1_good = true;
    status.is_sensor_2_good = false;
    status.system_status = SystemStatus::SetupError;

    format_msg_system_status(buffer, status);

    BOOST_TEST(buffer.message_size == 4);
    BOOST_TEST(buffer.buffer[0] == 2);
    BOOST_TEST(buffer.buffer[1] == 1);
    BOOST_TEST(buffer.buffer[2] == 2);
    BOOST_TEST(buffer.buffer[3] == (2 ^ 1 ^ 2));
}

BOOST_AUTO_TEST_CASE(it_should_format_system_status_bad_status_enum)
{
    MessageBuffer buffer;
    SystemSensorStatus status;

    status.is_sensor_0_good = true;
    status.is_sensor_1_good = true;
    status.is_sensor_2_good = true;

    // Forced bad enumeration.
    status.system_status = static_cast<SystemStatus>(42);

    format_msg_system_status(buffer, status);

    BOOST_TEST(buffer.message_size == 4);
    BOOST_TEST(buffer.buffer[0] == 2);
    BOOST_TEST(buffer.buffer[1] == 3);
    BOOST_TEST(buffer.buffer[2] == 7);
    BOOST_TEST(buffer.buffer[3] == (2 ^ 3 ^ 7));
}

BOOST_AUTO_TEST_CASE(it_should_format_error_msg)
{
    MessageBuffer buffer;
    ErrorCode error_code;

    error_code = ErrorCode::SystemFailure;

    format_msg_error(buffer, error_code);

    BOOST_TEST(buffer.message_size == 3);
    BOOST_TEST(buffer.buffer[0] == 3);
    BOOST_TEST(buffer.buffer[1] == 1);
    BOOST_TEST(buffer.buffer[2] == (3 ^ 1));

    error_code = ErrorCode::BadRequest;

    format_msg_error(buffer, error_code);

    BOOST_TEST(buffer.message_size == 3);
    BOOST_TEST(buffer.buffer[0] == 3);
    BOOST_TEST(buffer.buffer[1] == 0);
    BOOST_TEST(buffer.buffer[2] == (3 ^ 0));
}

BOOST_AUTO_TEST_CASE(it_should_format_error_msg_bad_error_enum)
{
    MessageBuffer buffer;
    ErrorCode error_code;

    error_code = static_cast<ErrorCode>(42);

    format_msg_error(buffer, error_code);

    BOOST_TEST(buffer.message_size == 3);
    BOOST_TEST(buffer.buffer[0] == 3);
    BOOST_TEST(buffer.buffer[1] == 2);
    BOOST_TEST(buffer.buffer[2] == (3 ^ 2));
}

BOOST_AUTO_TEST_SUITE_END()
