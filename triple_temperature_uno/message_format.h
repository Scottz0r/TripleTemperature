#ifndef _SCOTTZ0R_TEMPERATURE_MESSAGE_FORMAT_INCLUDE_GUARD
#define _SCOTTZ0R_TEMPERATURE_MESSAGE_FORMAT_INCLUDE_GUARD

#include "temperature_types.h"

namespace scottz0r
{
namespace temperature
{
    struct MessageBuffer
    {
        uint8_t buffer[12];
        unsigned message_size;
    };

    void format_msg_temperature(MessageBuffer &dest, const TemperatureVoteResult &data);

    void format_msg_system_status(MessageBuffer &dest, const SystemSensorStatus &status);

    void format_msg_error(MessageBuffer &dest, ErrorCode error_code);
} // namespace temperature
} // namespace scottz0r

#endif // _SCOTTZ0R_TEMPERATURE_MESSAGE_FORMAT_INCLUDE_GUARD
