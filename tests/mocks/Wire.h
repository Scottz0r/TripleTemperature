#ifndef _SCOTTZ0R_MOCKS_WIRE_INCLUDE_GUARD
#define _SCOTTZ0R_MOCKS_WIRE_INCLUDE_GUARD

#include <inttypes.h>

class TwoWire
{
public:
    int available();

    void begin();

    void beginTransmission(uint8_t);

    uint8_t endTransmission();

    int read();

    uint8_t requestFrom(uint8_t, uint8_t);

    size_t write(uint8_t);
};

class TwoWireImpl
{
public:
    virtual int available() = 0;

    virtual void begin() = 0;

    virtual void beginTransmission(uint8_t) = 0;

    virtual uint8_t endTransmission() = 0;

    virtual int read() = 0;

    virtual uint8_t requestFrom(uint8_t, uint8_t) = 0;

    virtual size_t write(uint8_t) = 0;
};

extern TwoWire Wire;
extern TwoWireImpl *wire_impl;

#endif // _SCOTTZ0R_MOCKS_WIRE_INCLUDE_GUARD
