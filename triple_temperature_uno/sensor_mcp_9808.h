/**
    MCP9808 sensor class. Reads temperature from a MCP9808 over I2C.
 */
#ifndef _SCOTTZ0R_TEMPERATURE_SENSOR_MCP9808_INCLUDE_GUARD
#define _SCOTTZ0R_TEMPERATURE_SENSOR_MCP9808_INCLUDE_GUARD

#include <inttypes.h>

namespace scottz0r
{
namespace temperature
{

    /// @brief This class interfaces with a MCP9808 temperature sensor. This device communicates over I2C.
    class SensorMcp9808
    {
        static constexpr uint8_t MCP9808_I2CADDR_DEFAULT = 0x18;
        static constexpr uint8_t MCP9808_I2CADDR_LOW = 0x18;
        static constexpr uint8_t MCP9808_I2CADDR_HIGH = 0x1F;

        static constexpr uint8_t MCP9808_REG_AMBIENT_TEMP = 0x05;
        static constexpr uint8_t MCP9808_REG_MANUF_ID = 0x06;
        static constexpr uint8_t MCP9808_REG_DEVICE_ID = 0x07;
        static constexpr uint8_t MCP9808_REG_CONFIG = 0x01;

        static constexpr uint16_t MCP9808_MANUFACTURER_ID = 0x0054;
        static constexpr uint16_t MCP9808_DEVICE_ID = 0x0400;

    public:
        SensorMcp9808();

        bool begin(uint8_t addr);

        bool read_temp(int16_t &result);

        bool good() const
        {
            return m_good;
        }

        bool bad() const
        {
            return !m_good;
        }

    private:
        bool read16(uint8_t reg, uint16_t &result);

        uint8_t m_addr;
        bool m_good;
    };

} // namespace temperature
} // namespace scottz0r

#endif // _SCOTTZ0R_TEMPERATURE_SENSOR_MCP9808_INCLUDE_GUARD
