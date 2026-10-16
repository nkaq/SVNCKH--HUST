#include "error_manager.h"


// =====================================================
// RESET
// =====================================================

void ErrorManager::reset()
{
    portENTER_CRITICAL(&mux);


    counters =
        ErrorCounters{};


    lastError =
        ErrorCode::NONE;


    portEXIT_CRITICAL(&mux);
}


// =====================================================
// REPORT ERROR
// =====================================================

void ErrorManager::report(
    ErrorCode error
)
{
    if (error == ErrorCode::NONE)
    {
        return;
    }


    portENTER_CRITICAL(&mux);


    lastError =
        error;


    counters.total_errors++;


    switch (error)
    {
        // ---------------------------------------------
        // I2C
        // ---------------------------------------------

        case ErrorCode::I2C_ERROR:

            counters.i2c_errors++;

            break;


        // ---------------------------------------------
        // TIMEOUT
        // ---------------------------------------------

        case ErrorCode::TIMEOUT:

            counters.timeouts++;

            break;


        // ---------------------------------------------
        // SENSOR LOST
        // ---------------------------------------------

        case ErrorCode::SENSOR_DISCONNECT:

            counters.disconnects++;

            break;


        // ---------------------------------------------
        // SAMPLE RING BUFFER FULL
        //
        // Policy:
        // DROP NEWEST.
        // ---------------------------------------------

        case ErrorCode::BUFFER_OVERFLOW:

            counters.buffer_overflows++;

            counters.dropped_samples++;

            break;


        // ---------------------------------------------
        // ADXL FIFO OVERRUN
        // ---------------------------------------------

        case ErrorCode::FIFO_OVERRUN:

            counters.fifo_overruns++;

            break;


        // ---------------------------------------------
        // INVALID STATE / COMMAND
        // ---------------------------------------------

        case ErrorCode::INVALID_STATE:

            counters.invalid_state++;

            break;


        // ---------------------------------------------
        // INVALID CONFIG
        // ---------------------------------------------

        case ErrorCode::INVALID_CONFIG:

            counters.invalid_config++;

            break;


        case ErrorCode::NONE:
        default:

            break;
    }


    portEXIT_CRITICAL(&mux);
}


// =====================================================
// ADD DROPPED SAMPLES
//
// Dùng khi biết có nhiều sample bị mất,
// ví dụ FIFO overrun thực tế.
// =====================================================

void ErrorManager::addDroppedSamples(
    uint32_t count
)
{
    if (count == 0)
    {
        return;
    }


    portENTER_CRITICAL(&mux);


    counters.dropped_samples +=
        count;


    portEXIT_CRITICAL(&mux);
}


// =====================================================
// RECOVERY ATTEMPT
// =====================================================

void ErrorManager::markRecoveryAttempt()
{
    portENTER_CRITICAL(&mux);


    counters.recovery_attempts++;


    portEXIT_CRITICAL(&mux);
}


// =====================================================
// RECOVERY SUCCESS
// =====================================================

void ErrorManager::markRecoverySuccess()
{
    portENTER_CRITICAL(&mux);


    counters.recovery_successes++;


    portEXIT_CRITICAL(&mux);
}


// =====================================================
// RECOVERY FAILURE
// =====================================================

void ErrorManager::markRecoveryFailure()
{
    portENTER_CRITICAL(&mux);


    counters.recovery_failures++;


    portEXIT_CRITICAL(&mux);
}


// =====================================================
// GET LAST ERROR
// =====================================================

ErrorCode ErrorManager::getLastError() const
{
    ErrorCode result;


    portENTER_CRITICAL(&mux);


    result =
        lastError;


    portEXIT_CRITICAL(&mux);


    return result;
}


// =====================================================
// GET COUNTERS
// =====================================================

ErrorCounters ErrorManager::getCounters() const
{
    ErrorCounters result;


    portENTER_CRITICAL(&mux);


    result =
        counters;


    portEXIT_CRITICAL(&mux);


    return result;
}


// =====================================================
// ERROR NAME
// =====================================================

const char* ErrorManager::errorName(
    ErrorCode error
) const
{
    switch (error)
    {
        case ErrorCode::NONE:
            return "NONE";

        case ErrorCode::I2C_ERROR:
            return "I2C_ERROR";

        case ErrorCode::TIMEOUT:
            return "TIMEOUT";

        case ErrorCode::SENSOR_DISCONNECT:
            return "SENSOR_DISCONNECT";

        case ErrorCode::BUFFER_OVERFLOW:
            return "BUFFER_OVERFLOW";

        case ErrorCode::FIFO_OVERRUN:
            return "FIFO_OVERRUN";

        case ErrorCode::INVALID_STATE:
            return "INVALID_STATE";

        case ErrorCode::INVALID_CONFIG:
            return "INVALID_CONFIG";
    }


    return "UNKNOWN";
}