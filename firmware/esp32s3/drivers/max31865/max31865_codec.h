
#pragma once

#include <stdint.h>
#include <math.h>

struct Max31865RtdReading
{
    uint16_t adc_code = 0;
    bool fault_flag = false;
    float resistance_ohms = NAN;
    bool valid = false;
};

class Max31865Codec
{
public:
    // Convert the two RTD register bytes
    // into a 15-bit ADC code and a fault flag.
    //
    // rref_ohms must be measured or verified
    // for the actual MAX31865 module.
    static Max31865RtdReading decode(
        uint8_t rtd_msb,
        uint8_t rtd_lsb,
        float rref_ohms
    )
    {
        Max31865RtdReading result = {};

        const uint16_t raw =
            (static_cast<uint16_t>(rtd_msb) << 8) |
            static_cast<uint16_t>(rtd_lsb);

        result.adc_code = raw >> 1;
        result.fault_flag = (raw & 0x0001U) != 0;

        if (
            !isfinite(rref_ohms) ||
            rref_ohms <= 0.0f ||
            result.fault_flag
        )
        {
            return result;
        }

        result.resistance_ohms =
            static_cast<float>(result.adc_code) *
            rref_ohms / 32768.0f;

        result.valid = true;

        return result;
    }
};
