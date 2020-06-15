#ifndef _SCOTTZ0R_MOCKS_ARDUINO_INCLUDE_GUARD
#define _SCOTTZ0R_MOCKS_ARDUINO_INCLUDE_GUARD

#ifdef __cplusplus
extern "C"
{
#endif

    unsigned long millis();

#ifdef __cplusplus
}
#endif

class ArduinoImpl
{
public:
    virtual unsigned long millis() = 0;
};

extern ArduinoImpl *arduino_impl;

#endif // _SCOTTZ0R_MOCKS_ARDUINO_INCLUDE_GUARD
