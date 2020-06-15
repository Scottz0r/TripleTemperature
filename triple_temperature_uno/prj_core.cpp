/// @file Main project file for Arduino Uno.
///
#include <Arduino.h>
#include <HardwareSerial.h>
#include <Wire.h>
#include <avr/wdt.h>

#include "message_format.h"
#include "message_reader.h"
#include "prj_config.h"
#include "sensor_mcp_9808.h"
#include "temperature_engine.h"

using namespace scottz0r::temperature;

SystemStatus system_status;

SensorMcp9808 temp_0;
SensorMcp9808 temp_1;
SensorMcp9808 temp_2;

TemperatureVoteEngine temperature_vote_engine(CFG_TEMPERATURE_TOLERANCE);
TemperatureVoteResult temp_vote_result;

MessageReader message_reader(CFG_SERIAL_MESSAGE_TIMEOUT);
MessageBuffer message_buffer;

void collect_send_temperature();
void collect_send_system_status();
void handle_request();
void send_error(ErrorCode error_code);

/// @brief Main program setup function.
void setup()
{
    system_status = SystemStatus::SetupError;

    Serial.begin(CFG_SERIAL_BAUD_RATE);
    Wire.begin();

    // Initialize sensors.
    temp_0.begin(CFG_SENSOR_0_ADDR);
    temp_1.begin(CFG_SENSOR_1_ADDR);
    temp_2.begin(CFG_SENSOR_2_ADDR);

    system_status = SystemStatus::OK;

    wdt_enable(WDTO_60MS);
}

/// @brief Main program loop.
void loop()
{
    if (Serial.available())
    {
        int c = Serial.read();
        bool has_request = message_reader.process(c);

        if (has_request)
        {
            handle_request();
        }
    }

    wdt_reset();
}

void collect_send_temperature()
{
    TemperatureReading temp_0_value;
    TemperatureReading temp_1_value;
    TemperatureReading temp_2_value;

    temp_0_value.is_valid = temp_0.read_temp(temp_0_value.temperature);
    temp_1_value.is_valid = temp_1.read_temp(temp_1_value.temperature);
    temp_2_value.is_valid = temp_2.read_temp(temp_2_value.temperature);

    temperature_vote_engine.vote_temperature(temp_0_value, temp_1_value, temp_2_value, temp_vote_result);

    format_msg_temperature(message_buffer, temp_vote_result);

    // TODO: Serial available so not blocking wdt if full.
    Serial.write(message_buffer.buffer, message_buffer.message_size);
}

void collect_send_system_status()
{
    SystemSensorStatus status;
    status.is_sensor_0_good = temp_0.good();
    status.is_sensor_1_good = temp_1.good();
    status.is_sensor_2_good = temp_2.good();
    status.system_status = system_status;

    format_msg_system_status(message_buffer, status);

    Serial.write(message_buffer.buffer, message_buffer.message_size);
}

void send_error(ErrorCode error_code)
{
    format_msg_error(message_buffer, error_code);
    Serial.write(message_buffer.buffer, message_buffer.message_size);
}

void handle_request()
{
    // Do not try to do anything with request if system is not OK.
    if (system_status != SystemStatus::OK)
    {
        send_error(ErrorCode::SystemFailure);
        return;
    }

    RequestType request_type;
    if (!message_reader.get_data(request_type))
    {
        send_error(ErrorCode::BadRequest);
        return;
    }

    switch (request_type)
    {
    case RequestType::Temperature:
        collect_send_temperature();
        break;
    case RequestType::SystemStatus:
        collect_send_system_status();
        break;
    default:
        send_error(ErrorCode::BadRequest);
        break;
    }
}
