#include "message_format.h"

#define TEMPERATURE_MSG_SIZE 12
#define SYSTEM_STATUS_MSG_SIZE 4
#define REQUEST_ERROR_MSG_SIZE 3

namespace scottz0r
{
namespace temperature
{
    union Uint16Splitter {
        uint16_t num;
        uint8_t split[2];
    };

    void format_msg_temperature(MessageBuffer &dest, const TemperatureVoteResult &data)
    {
        Uint16Splitter splitter;

        dest.buffer[0] = static_cast<uint8_t>(MessageIdentifier::Temperature);

        // Reading status
        if (data.status < TemperatureVoteStatus::_Unknown)
        {
            dest.buffer[1] = static_cast<uint8_t>(data.status);
        }
        else
        {
            dest.buffer[1] = static_cast<uint8_t>(TemperatureVoteStatus::_Unknown);
        }

        // Temperature 0
        splitter.num = data.temp0;
        dest.buffer[2] = splitter.split[0];
        dest.buffer[3] = splitter.split[1];

        // Temperature 1
        splitter.num = data.temp1;
        dest.buffer[4] = splitter.split[0];
        dest.buffer[5] = splitter.split[1];

        // Temperature 2
        splitter.num = data.temp2;
        dest.buffer[6] = splitter.split[0];
        dest.buffer[7] = splitter.split[1];

        // Agreement bits: Pack into a single byte.
        uint8_t agreement_bits = 0;

        if (data.is_temp0_agree)
        {
            agreement_bits |= 0x01;
        }

        if (data.is_temp1_agree)
        {
            agreement_bits |= 0x02;
        }

        if (data.is_temp2_agree)
        {
            agreement_bits |= 0x04;
        }

        dest.buffer[8] = agreement_bits;

        // Average Temperature
        splitter.num = data.average;
        dest.buffer[9] = splitter.split[0];
        dest.buffer[10] = splitter.split[1];

        // Checksum: XOR all bytes. Unwind loop because message is constant size known at compile time.
        uint8_t checksum = 0;
        checksum ^= dest.buffer[0];
        checksum ^= dest.buffer[1];
        checksum ^= dest.buffer[2];
        checksum ^= dest.buffer[3];
        checksum ^= dest.buffer[4];
        checksum ^= dest.buffer[5];
        checksum ^= dest.buffer[6];
        checksum ^= dest.buffer[7];
        checksum ^= dest.buffer[8];
        checksum ^= dest.buffer[9];
        checksum ^= dest.buffer[10];

        dest.buffer[11] = checksum;
        dest.message_size = TEMPERATURE_MSG_SIZE;
    }

    void format_msg_system_status(MessageBuffer &dest, const SystemSensorStatus &status)
    {
        dest.buffer[0] = static_cast<uint8_t>(MessageIdentifier::SystemStatus);

        if (status.system_status < SystemStatus::_Unknown)
        {
            dest.buffer[1] = static_cast<uint8_t>(status.system_status);
        }
        else
        {
            dest.buffer[1] = static_cast<uint8_t>(SystemStatus::_Unknown);
        }

        // Combine three sensor flags into a single byte for communication.
        uint8_t sensor_status_byte = 0;
        if (status.is_sensor_0_good)
        {
            sensor_status_byte |= 0x01;
        }

        if (status.is_sensor_1_good)
        {
            sensor_status_byte |= 0x02;
        }

        if (status.is_sensor_2_good)
        {
            sensor_status_byte |= 0x04;
        }

        dest.buffer[2] = sensor_status_byte;

        uint8_t checksum = 0;
        checksum ^= dest.buffer[0];
        checksum ^= dest.buffer[1];
        checksum ^= dest.buffer[2];

        dest.buffer[3] = checksum;
        dest.message_size = SYSTEM_STATUS_MSG_SIZE;
    }

    void format_msg_error(MessageBuffer &dest, ErrorCode error_code)
    {
        dest.buffer[0] = static_cast<uint8_t>(MessageIdentifier::Error);

        if (error_code < ErrorCode::_Unknown)
        {
            dest.buffer[1] = static_cast<uint8_t>(error_code);
        }
        else
        {
            dest.buffer[1] = static_cast<uint8_t>(ErrorCode::_Unknown);
        }

        uint8_t checksum = 0;
        checksum ^= dest.buffer[0];
        checksum ^= dest.buffer[1];

        dest.buffer[2] = checksum;
        dest.message_size = REQUEST_ERROR_MSG_SIZE;
    }
} // namespace temperature
} // namespace scottz0r
