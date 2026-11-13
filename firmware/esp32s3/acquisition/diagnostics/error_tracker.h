#pragma once

#include <Arduino.h>

#include "../common/daq_types.h"

class ErrorTracker
{
public:
    void reset()
    {
        counters_ = {};
        last_error_ = ErrorCode::NONE;
    }

    void record(
        ErrorCode error
    )
    {
        last_error_ = error;

        if (error == ErrorCode::NONE)
        {
            return;
        }

        counters_.total_errors++;

        switch (error)
        {
            case ErrorCode::I2C_ERROR:
                counters_.i2c_errors++;
                break;

            case ErrorCode::TIMEOUT:
                counters_.timeouts++;
                break;

            case ErrorCode::SENSOR_DISCONNECT:
                counters_.disconnects++;
                break;

            case ErrorCode::BUFFER_OVERFLOW:
                counters_.buffer_overflows++;
                break;

            case ErrorCode::FIFO_OVERRUN:
                counters_.fifo_overruns++;
                break;

            case ErrorCode::INVALID_STATE:
                counters_.invalid_state++;
                break;

            case ErrorCode::INVALID_CONFIG:
                counters_.invalid_config++;
                break;

            case ErrorCode::TIMESTAMP_ERROR:
                counters_.timestamp_errors++;
                break;

            case ErrorCode::NONE:
                break;
        }
    }

    void recordDroppedSamples(
        uint32_t count = 1
    )
    {
        counters_.dropped_samples += count;
    }

    void recordRecoveryAttempt()
    {
        counters_.recovery_attempts++;
    }

    void recordRecoverySuccess()
    {
        counters_.recovery_successes++;
    }

    void recordRecoveryFailure()
    {
        counters_.recovery_failures++;
    }

    const ErrorCounters& counters() const
    {
        return counters_;
    }

    ErrorCode lastError() const
    {
        return last_error_;
    }

private:
    ErrorCounters counters_ = {};
    ErrorCode last_error_ = ErrorCode::NONE;
};