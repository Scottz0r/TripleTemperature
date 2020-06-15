#include "triple_temperature.h"

#include <array>
#include <iomanip>
#include <windows.h>

struct TripleTemperature::Impl
{
    enum class RequestType : uint8_t
    {
        Temperature = 0,
        SystemStatus = 1
    };
    static constexpr uint8_t MAX_REQUEST_TYPE = 1;

    enum class MessageType
    {
        Temperature = 1,
        SystemStatus = 2,
        Error = 3,
        Request = 4
    };

    static constexpr auto MSG_SIZE_TEMPERATURE = 12;
    static constexpr auto MSG_SIZE_SYSTEM_STATUS = 4;
    static constexpr auto MSG_SIZE_ERROR = 3;
    static constexpr auto MSG_SIZE_REQUEST = 3;

    Impl()
    {
        m_handle = INVALID_HANDLE_VALUE;
    }

    ~Impl()
    {
        if (m_handle)
        {
            CloseHandle(m_handle);
        }
    }

    bool close()
    {
        if (m_handle != INVALID_HANDLE_VALUE)
        {
            BOOL rc = CloseHandle(m_handle);
            if (rc)
            {
                m_handle = INVALID_HANDLE_VALUE;
                return true;
            }
        }

        return false;
    }

    bool connect(const std::wstring &port)
    {
        BOOL rc;

        m_handle =
            CreateFileW(port.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

        if (m_handle == INVALID_HANDLE_VALUE)
        {
            return false;
        }

        DCB serial_params{};
        serial_params.DCBlength = sizeof(serial_params);

        serial_params.BaudRate = CBR_115200;
        serial_params.ByteSize = 8;
        serial_params.StopBits = ONESTOPBIT;
        serial_params.Parity = NOPARITY;
        serial_params.fBinary = TRUE;

        rc = SetCommState(m_handle, &serial_params);
        if (!rc)
        {
            CloseHandle(m_handle);
            m_handle = INVALID_HANDLE_VALUE;
            return false;
        }

        COMMTIMEOUTS timeout = {0};
        timeout.ReadIntervalTimeout = 0;
        timeout.ReadTotalTimeoutConstant = 500;
        timeout.ReadTotalTimeoutMultiplier = 0;
        timeout.WriteTotalTimeoutConstant = 500;
        timeout.WriteTotalTimeoutMultiplier = 0;

        rc = SetCommTimeouts(m_handle, &timeout);
        if (!rc)
        {
            CloseHandle(m_handle);
            m_handle = INVALID_HANDLE_VALUE;
            return false;
        }

        return true;
    }

    bool get_temperature(TemperatureResult &dest)
    {
        if (m_handle == INVALID_HANDLE_VALUE)
        {
            return false;
        }

        if (!send_request(RequestType::Temperature))
        {
            return false;
        }

        MessageType message_type;
        if (!read_next(message_type) || message_type != MessageType::Temperature)
        {
            return false;
        }

        return decode_temperature(dest);
    }

    bool get_status(StatusResult &dest)
    {
        if (m_handle == INVALID_HANDLE_VALUE)
        {
            return false;
        }

        if (!send_request(RequestType::SystemStatus))
        {
            return false;
        }

        MessageType message_type;
        if (!read_next(message_type) || message_type != MessageType::SystemStatus)
        {
            return false;
        }

        return decode_status(dest);
    }

    bool is_open()
    {
        return m_handle != INVALID_HANDLE_VALUE;
    }

    bool send_request(RequestType request_type)
    {
        if (static_cast<uint8_t>(request_type) > MAX_REQUEST_TYPE)
        {
            return false;
        }

        uint8_t buffer[MSG_SIZE_REQUEST];
        buffer[0] = static_cast<uint8_t>(MessageType::Request);
        buffer[1] = static_cast<uint8_t>(request_type);
        buffer[2] = buffer[0] ^ buffer[1];

        DWORD bytes_written;
        auto rc = WriteFile(m_handle, buffer, MSG_SIZE_REQUEST, &bytes_written, nullptr);
        if (!rc || bytes_written != MSG_SIZE_REQUEST)
        {
            return false;
        }

        return true;
    }

    bool read_next(MessageType &message_type)
    {
        BOOL rc;
        DWORD bytes_read;

        // Read first byte to know which message there is.
        rc = ReadFile(m_handle, m_buffer, 1, &bytes_read, nullptr);
        if (!rc || bytes_read != 1)
        {
            return false;
        }

        DWORD message_bytes = 0;

        switch (m_buffer[0])
        {
        case 1:
            message_bytes = MSG_SIZE_TEMPERATURE;
            message_type = MessageType::Temperature;
            break;
        case 2:
            message_bytes = MSG_SIZE_SYSTEM_STATUS;
            message_type = MessageType::SystemStatus;
            break;
        case 3:
            message_bytes = MSG_SIZE_ERROR;
            message_type = MessageType::Error;
            break;
        default:
            return false;
        }

        // Read next set of bytes and preserve the first spot in the buffer. The bytes to read will be one less than
        // the message size because byte 1 has been read.
        message_bytes -= 1;
        rc = ReadFile(m_handle, m_buffer + 1, message_bytes, &bytes_read, nullptr);
        if (!rc || bytes_read != message_bytes)
        {
            return false;
        }

        return true;
    }

    void decode_error()
    {
        // TODO?
    }

    bool decode_status(StatusResult &dest)
    {
        dest.system_status = (int)m_buffer[1];
        dest.sensor_0_ok = bool(m_buffer[2] & 0x01);
        dest.sensor_1_ok = bool(m_buffer[2] & 0x01);
        dest.sensor_2_ok = bool(m_buffer[2] & 0x01);

        uint8_t checksum = 0;
        checksum ^= m_buffer[0];
        checksum ^= m_buffer[1];
        checksum ^= m_buffer[2];

        return checksum == m_buffer[3];
    }

    bool decode_temperature(TemperatureResult &dest)
    {
        int16_t temp0 = m_buffer[2] | (m_buffer[3] << 8);
        dest.temp0 = temp0 / 100.0;
        dest.temp0_ok = bool(m_buffer[8] & 0x01);

        int16_t temp1 = m_buffer[4] | (m_buffer[5] << 8);
        dest.temp1 = temp1 / 100.0;
        dest.temp1_ok = bool(m_buffer[8] & 0x02);

        int16_t temp2 = m_buffer[6] | (m_buffer[7] << 8);
        dest.temp2 = temp2 / 100.0;
        dest.temp2_ok = bool(m_buffer[8] & 0x04);

        int16_t average_temp = m_buffer[9] | (m_buffer[10] << 8);
        dest.average = average_temp / 100.0;

        uint8_t checksum = 0;
        for (size_t i = 0; i < 11; ++i)
        {
            checksum ^= m_buffer[i];
        }

        return checksum == m_buffer[11];
    }

    uint8_t m_buffer[16];
    HANDLE m_handle;
};

TripleTemperature::TripleTemperature()
{
    p_impl = new TripleTemperature::Impl();
}

TripleTemperature::~TripleTemperature()
{
    delete p_impl;
}

bool TripleTemperature::close()
{
    return p_impl->close();
}

bool TripleTemperature::connect(const std::wstring &port)
{
    return p_impl->connect(port);
}

bool TripleTemperature::get_temperature(TemperatureResult &dest)
{
    return p_impl->get_temperature(dest);
}

bool TripleTemperature::get_status(StatusResult &dest)
{
    return p_impl->get_status(dest);
}

bool TripleTemperature::is_open()
{
    return p_impl->is_open();
}

std::wostream &operator<<(std::wostream &os, const StatusResult &status)
{
    os << "Sensor Status:" << std::endl;
    os << "Sensor 0: " << status.sensor_0_ok << std::endl;
    os << "Sensor 1: " << status.sensor_1_ok << std::endl;
    os << "Sensor 2: " << status.sensor_2_ok << std::endl;
    os << "System: " << status.system_status << " (0 = OK)" << std::endl;

    return os;
}

std::wostream &operator<<(std::wostream &os, const TemperatureResult &temperature)
{
    os << std::fixed << std::setprecision(2);

    os << "Temperature:" << std::endl;

    os << "Temp0: " << temperature.temp0 << std::endl;
    os << "Temp0 Good: " << temperature.temp0_ok << std::endl;

    os << "Temp1: " << temperature.temp1 << std::endl;
    os << "Temp0 Good: " << temperature.temp1_ok << std::endl;

    os << "Temp2: " << temperature.temp2 << std::endl;
    os << "Temp0 Good: " << temperature.temp2_ok << std::endl;

    os << "Average: " << temperature.average << std::endl;

    return os;
}
