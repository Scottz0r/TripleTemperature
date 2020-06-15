#include "Wire.h"
#include <exception>

static constexpr auto IMPL_IS_NULL = "Wire impl is null";

// Extern declared in Wire.h
TwoWire Wire;
TwoWireImpl *wire_impl;

int TwoWire::available()
{
    if (wire_impl)
    {
        return wire_impl->available();
    }
    else
    {
        throw std::exception(IMPL_IS_NULL);
    }
}

void TwoWire::begin()
{
    if (wire_impl)
    {
        wire_impl->begin();
    }
    else
    {
        throw std::exception(IMPL_IS_NULL);
    }
}

void TwoWire::beginTransmission(uint8_t val)
{
    if (wire_impl)
    {
        wire_impl->beginTransmission(val);
    }
    else
    {
        throw std::exception(IMPL_IS_NULL);
    }
}

uint8_t TwoWire::endTransmission()
{
    if (wire_impl)
    {
        return wire_impl->endTransmission();
    }
    else
    {
        throw std::exception(IMPL_IS_NULL);
    }
}

int TwoWire::read()
{
    if (wire_impl)
    {
        return wire_impl->read();
    }
    else
    {
        throw std::exception(IMPL_IS_NULL);
    }
}

uint8_t TwoWire::requestFrom(uint8_t addr, uint8_t cmd)
{
    if (wire_impl)
    {
        return wire_impl->requestFrom(addr, cmd);
    }
    else
    {
        throw std::exception(IMPL_IS_NULL);
    }
}

size_t TwoWire::write(uint8_t val)
{
    if (wire_impl)
    {
        return wire_impl->write(val);
    }
    else
    {
        throw std::exception(IMPL_IS_NULL);
    }
}
