#include "temperature_engine.h"

namespace scottz0r
{
namespace temperature
{
    template <class T> inline T t_abs(const T &x)
    {
        return (x < 0) ? -x : x;
    }

    TemperatureVoteEngine::TemperatureVoteEngine(int16_t temperature_tolerance)
        : m_temperature_tolerance(temperature_tolerance)
    {
    }

    void TemperatureVoteEngine::vote_temperature(
        const TemperatureReading &temp0, const TemperatureReading &temp1, const TemperatureReading &temp2,
        TemperatureVoteResult &out_result)
    {
        short count_valid = 0;

        // Requirement X.XX: Invalid temperature readings must be set to 0.
        out_result.status = TemperatureVoteStatus::SensorError;
        out_result.is_temp0_agree = false;
        out_result.is_temp1_agree = false;
        out_result.is_temp2_agree = false;
        out_result.temp0 = 0;
        out_result.temp1 = 0;
        out_result.temp2 = 0;
        out_result.average = 0;

        if (temp0.is_valid)
        {
            out_result.temp0 = temp0.temperature;
            ++count_valid;
        }

        if (temp1.is_valid)
        {
            out_result.temp1 = temp1.temperature;
            ++count_valid;
        }

        if (temp2.is_valid)
        {
            out_result.temp2 = temp2.temperature;
            ++count_valid;
        }

        // Requirement X.XX: At least two sensors must be valid to do voting.
        if (count_valid < 2)
        {
            out_result.status = TemperatureVoteStatus::SensorError;
            return;
        }

        // Requirement X.XX: Valid values must be within a tolerance of each other.
        out_result.is_temp0_agree = is_within_tolerance(temp0, temp1, temp2);
        out_result.is_temp1_agree = is_within_tolerance(temp1, temp0, temp2);
        out_result.is_temp2_agree = is_within_tolerance(temp2, temp0, temp1);

        // Requirement X.XX: At least two sensors must be within tolerance to find sum_agree.
        short count_agree = 0;
        short sum_agree = 0;

        if (out_result.is_temp0_agree)
        {
            sum_agree += temp0.temperature;
            ++count_agree;
        }

        if (out_result.is_temp1_agree)
        {
            sum_agree += temp1.temperature;
            ++count_agree;
        }

        if (out_result.is_temp2_agree)
        {
            sum_agree += temp2.temperature;
            ++count_agree;
        }

        if (count_agree >= 2)
        {
            out_result.average = sum_agree / count_agree;
        }

        // Requirement X.XX: At least two sensors must be within tolerance to return a "good" state.
        if (count_agree >= 2)
        {
            out_result.status = out_result.status = TemperatureVoteStatus::OK;
            return;
        }
        else
        {
            out_result.status = TemperatureVoteStatus::Disagree;
            return;
        }
    }

    bool TemperatureVoteEngine::is_within_tolerance(
        const TemperatureReading &a, const TemperatureReading &b, const TemperatureReading &c)
    {
        int count_agree = 0;

        if (!a.is_valid)
        {
            return false;
        }

        if (b.is_valid)
        {
            short diff = t_abs(a.temperature - b.temperature);
            if (diff <= m_temperature_tolerance)
            {
                ++count_agree;
            }
        }

        if (c.is_valid)
        {
            short diff = t_abs(a.temperature - c.temperature);
            if (diff <= m_temperature_tolerance)
            {
                ++count_agree;
            }
        }

        // Must agree with one or more sensors.
        return count_agree >= 1;
    }
} // namespace temperature
} // namespace scottz0r
