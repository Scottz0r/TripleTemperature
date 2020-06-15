#include "message_reader.h"

#include <Arduino.h>

static constexpr auto REQUEST_MESSAGE_ID = 4; // TODO: Should probably have this in one place for all messages.
static constexpr auto REQUEST_MESSAGE_SIZE = 3;

namespace scottz0r
{
namespace temperature
{
    MessageReader::MessageReader(time_type receive_timeout)
        : m_receive_timeout(receive_timeout), m_buffer_index(0), m_start_receive(0), m_state(State::Start)
    {
    }

    bool MessageReader::process(int c)
    {
        // Message timeout from previous collect. Reset collection state and assume this is a new message.
        if (m_state == State::Collect)
        {
            time_type elapsed = millis() - m_start_receive;
            if (elapsed >= m_receive_timeout)
            {
                m_state = State::Start;
            }
        }

        if (m_state == State::Done || m_state == State::Start)
        {
            m_buffer_index = 0;
            m_state = State::Collect;

            m_start_receive = millis();

            // When starting to collect, if the first byte is not the correct message identifier, do not go to
            // collection state. This means that following bytes could trigger a collect if it contains the start
            // byte.
            if (c != REQUEST_MESSAGE_ID)
            {
                return false;
            }
        }

        // Assert buffer collection state before adding byte.
        if (m_buffer_index < buffer_size)
        {
            m_buffer[m_buffer_index] = (uint8_t)c;
            ++m_buffer_index;
        }
        else
        {
            // Buffer overflow. Restart collection.
            m_state = State::Start;
            return false;
        }

        if (m_buffer_index == REQUEST_MESSAGE_SIZE)
        {
            m_state = State::Done;
            return true;
        }

        return false;
    }

    void MessageReader::reset()
    {
        m_buffer_index = 0;
        m_state = State::Start;
    }

    bool MessageReader::get_data(RequestType &dest)
    {
        if (m_state != State::Done)
        {
            return false;
        }

        return decode_request_message(dest);
    }

    bool MessageReader::decode_request_message(RequestType &dest)
    {
        // Set output parameter to a default state.
        dest = RequestType::_Unknown;

        // Assert buffer size is expected size.
        if (m_buffer_index != REQUEST_MESSAGE_SIZE)
        {
            return false;
        }

        // Assert this really is a request message type.
        if (m_buffer[0] != REQUEST_MESSAGE_ID)
        {
            return false;
        }

        // Checksum. Return false if checksums do not match. Do not attempt to decode data if checksum is bad.
        uint8_t checksum = 0;
        checksum ^= m_buffer[0];
        checksum ^= m_buffer[1];

        if (checksum != m_buffer[2])
        {
            return false;
        }

        // Assert message enumeration type is within bounds.
        if (m_buffer[1] >= static_cast<uint8_t>(RequestType::_Unknown))
        {
            return false;
        }

        dest = static_cast<RequestType>(m_buffer[1]);

        return true;
    }
} // namespace temperature
} // namespace scottz0r
