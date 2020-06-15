#include "Arduino.h"
#include <exception>

ArduinoImpl *arduino_impl;

unsigned long millis()
{
    if (arduino_impl)
    {
        return arduino_impl->millis();
    }

    return 0;
}
