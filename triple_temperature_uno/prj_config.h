#ifndef _SCOTTZ0R_TEMPERATURE_PRJ_CONFIG_INCLUDE_GUARD
#define _SCOTTZ0R_TEMPERATURE_PRJ_CONFIG_INCLUDE_GUARD

// Timeout for a single message received over serial communication. Milliseconds.
#define CFG_SERIAL_MESSAGE_TIMEOUT 10

// Tolerance to use when voting on temperature agreement. In 100s of Celsius (100 = 1.00 C).
#define CFG_TEMPERATURE_TOLERANCE 50

// I2C addresses for MCP 9808 sensors.
#define CFG_SENSOR_0_ADDR 0x18
#define CFG_SENSOR_1_ADDR 0x19
#define CFG_SENSOR_2_ADDR 0x1A

// Serial baud rate for message communication.
#define CFG_SERIAL_BAUD_RATE 115200

#endif // _SCOTTZ0R_TEMPERATURE_PRJ_CONFIG_INCLUDE_GUARD
