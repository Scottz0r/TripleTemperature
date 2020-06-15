///
/// @file
///
/// Temperature agreement engine. Takes three measurements of temperatures and votes on agreement.
#ifndef _SCOTTZ0R_TEMPERATURE_TEMPERATURE_ENGINE_INCLUDE_GUARD
#define _SCOTTZ0R_TEMPERATURE_TEMPERATURE_ENGINE_INCLUDE_GUARD

#include "temperature_types.h"

namespace scottz0r
{
namespace temperature
{
    class TemperatureVoteEngine
    {
    public:
        TemperatureVoteEngine(int16_t temperature_tolerance);

        void vote_temperature(
            const TemperatureReading &temp0, const TemperatureReading &temp1, const TemperatureReading &temp2,
            TemperatureVoteResult &out_result);

    private:
        bool is_within_tolerance(const TemperatureReading &a, const TemperatureReading &b, const TemperatureReading &c);

        int16_t m_temperature_tolerance;
    };

} // namespace temperature
} // namespace scottz0r

#endif // _SCOTTZ0R_TEMPERATURE_TEMPERATURE_ENGINE_INCLUDE_GUARD
