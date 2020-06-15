#ifndef _SCOTTZ0R_TEMPERATURE_MESSAGE_READER_INCLUDE_GUARD
#define _SCOTTZ0R_TEMPERATURE_MESSAGE_READER_INCLUDE_GUARD

#include "temperature_types.h"

namespace scottz0r
{
namespace temperature
{
    enum class RequestType : uint8_t
    {
        Temperature = 0,
        SystemStatus = 1,
        _Unknown = 2
    };

    class MessageReader
    {
        enum class State
        {
            Start,
            Collect,
            Done
        };

    public:
        MessageReader(time_type receive_timeout);

        static constexpr size_type buffer_size = 8;

        bool process(int c);

        void reset();

        bool get_data(RequestType &dest);

    private:
        bool decode_request_message(RequestType &dest);

        uint8_t m_buffer[buffer_size];
        size_type m_buffer_index;

        time_type m_start_receive;
        time_type m_receive_timeout;
        State m_state;
    };
} // namespace temperature
} // namespace scottz0r

#endif // _SCOTTZ0R_TEMPERATURE_MESSAGE_READER_INCLUDE_GUARD
