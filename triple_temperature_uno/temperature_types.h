#ifndef _SCOTTZ0R_TEMPERATURE_TYPES_INCLUDE_GUARD
#define _SCOTTZ0R_TEMPERATURE_TYPES_INCLUDE_GUARD

#include <inttypes.h>

namespace scottz0r
{
namespace temperature
{
    using size_type = unsigned int;
    using time_type = unsigned long;
    using temperature_type = int16_t;

    enum class SystemStatus : uint8_t
    {
        OK = 0,
        SetupError = 1,
        UnknownFailure = 2,
        _Unknown = 3
    };

    enum class ErrorCode : uint8_t
    {
        BadRequest = 0,
        SystemFailure = 1,
        _Unknown = 2
    };

    enum class MessageIdentifier : uint8_t
    {
        Temperature = 1,
        SystemStatus = 2,
        Error = 3,
        Request = 4,
        _Unknown = 5
    };

    struct TemperatureReading
    {
        bool is_valid;
        temperature_type temperature;
    };

    enum class TemperatureVoteStatus : uint8_t
    {
        OK = 0,
        SensorError = 1,
        Disagree = 2,
        _Unknown = 3
    };

    struct TemperatureVoteResult
    {
        TemperatureVoteStatus status;
        bool is_temp0_agree;
        bool is_temp1_agree;
        bool is_temp2_agree;

        temperature_type temp0;
        temperature_type temp1;
        temperature_type temp2;
        temperature_type average;
    };

    struct SystemSensorStatus
    {
        bool is_sensor_0_good;
        bool is_sensor_1_good;
        bool is_sensor_2_good;
        SystemStatus system_status;
    };
} // namespace temperature
} // namespace scottz0r

#endif // _SCOTTZ0R_TEMPERATURE_TYPES_INCLUDE_GUARD
