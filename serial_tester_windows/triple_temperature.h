#pragma once

#include <cstdint>
#include <iostream>
#include <string>

struct TemperatureResult
{
    double average;
    double temp0;
    double temp1;
    double temp2;
    bool temp0_ok;
    bool temp1_ok;
    bool temp2_ok;
};

struct StatusResult
{
    int system_status;
    bool sensor_0_ok;
    bool sensor_1_ok;
    bool sensor_2_ok;
};

class TripleTemperature
{
    struct Impl;

public:
    TripleTemperature();

    ~TripleTemperature();

    bool close();

    bool connect(const std::wstring &port);

    bool get_temperature(TemperatureResult &dest);

    bool get_status(StatusResult &dest);

    bool is_open();

private:
    Impl *p_impl;
};

std::wostream &operator<<(std::wostream &os, const StatusResult &status);

std::wostream &operator<<(std::wostream &os, const TemperatureResult &temperature);
