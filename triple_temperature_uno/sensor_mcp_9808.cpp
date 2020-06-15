#include "sensor_mcp_9808.h"
#include <Wire.h>

namespace scottz0r
{
namespace temperature
{
    SensorMcp9808::SensorMcp9808() : m_addr(0), m_good(false)
    {
    }

    bool SensorMcp9808::begin(uint8_t addr)
    {
        // Do not allow begin to be called multiple times.
        if (m_good)
        {
            return false;
        }

        m_good = false;

        // Must be within acceptable I2C address range, as determined by A0-2 pins. Pins add to the I2C address, with A2
        // as high bit.
        if (addr < MCP9808_I2CADDR_LOW || addr > MCP9808_I2CADDR_HIGH)
        {
            return false;
        }

        m_addr = addr;

        // Read device info to ensure this is the right I2C device.
        uint16_t dev_info;

        // Ensure the device is the one expected by checking Manufacturer ID and Device ID.
        if (!read16(MCP9808_REG_MANUF_ID, dev_info) || dev_info != MCP9808_MANUFACTURER_ID)
        {
            return false;
        }

        if (!read16(MCP9808_REG_DEVICE_ID, dev_info) || dev_info != MCP9808_DEVICE_ID)
        {
            return false;
        }

        // Write zero/default settings to device.
        Wire.beginTransmission(m_addr);
        Wire.write(MCP9808_REG_CONFIG);
        Wire.write(0);
        Wire.write(0);
        if (Wire.endTransmission() != 0)
        {
            return false;
        }

        m_good = true;
        return true;
    }

    bool SensorMcp9808::read_temp(int16_t &result)
    {
        // Do not attempt to read if sensor is in a failed initialization state.
        if (!m_good)
        {
            result = 0;
            return false;
        }

        uint16_t temp_raw;
        if (read16(MCP9808_REG_AMBIENT_TEMP, temp_raw))
        {
            // Need at least 4 bytes for larger computation.
            int32_t temp_signed = temp_raw & 0x0FFF;

            // If raw high bit set, then number is negative. Subtract number from 2^12 to make into a negative number
            if (temp_raw & 0x1000)
            {
                temp_signed = -1 * (0x1000L - temp_signed);
            }

            // Convert raw number into degrees in Celsius. The result will be a number of XXX.XX, but with the decimal
            // suppressed. Narrow after conversion. This should be in range of -4,000, 12,500 (-40.00, 125.00)
            // Spec examples have / 16, but right shift 4 is same result as integer division of 16.
            result = (int16_t)((temp_signed * 100L) >> 4L);
            return true;
        }

        return false;
    }

    bool SensorMcp9808::read16(uint8_t reg, uint16_t &result)
    {
        result = 0;

        Wire.beginTransmission(m_addr);
        Wire.write(reg);
        uint8_t xmit_status = Wire.endTransmission();

        if (xmit_status != 0)
        {
            return false;
        }

        Wire.requestFrom(m_addr, (uint8_t)2);
        if (Wire.available() == 2)
        {
            // MCP9808 returns data in big endian.
            uint8_t msb = Wire.read();
            uint8_t lsb = Wire.read();

            result = (msb << 8) | lsb;

            return true;
        }
        else
        {
            return false;
        }
    }
} // namespace temperature
} // namespace scottz0r
