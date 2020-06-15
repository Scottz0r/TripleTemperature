#ifndef _SCOTTZ0R_MOCKS_HARDWARE_SERIAL_INCLUDE_GUARD
#define _SCOTTZ0R_MOCKS_HARDWARE_SERIAL_INCLUDE_GUARD

#include <inttypes.h>

class HardwareSerial
{
public:
    void begin(unsigned long baud);

    int available();

    int read();

    void write(uint8_t* buf, size_t size);

    operator bool()
    {
        return true;
    }
};

extern HardwareSerial Serial;

#endif // _SCOTTZ0R_MOCKS_HARDWARE_SERIAL_INCLUDE_GUARD
