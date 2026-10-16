#include "error_manager.h"






void ErrorManager::reset()
{
    portENTER_CRITICAL(&mux);


    counters =
        ErrorCounters{};


    lastError =
        ErrorCode::NONE;


    portEXIT_CRITICAL(&mux);
}






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




        case ErrorCode::I2C_ERROR:

            counters.i2c_errors++;

            break;






        case ErrorCode::TIMEOUT:

            counters.timeouts++;

            break;






        case ErrorCode::SENSOR_DISCONNECT:

            counters.disconnects++;

            break;









        case ErrorCode::BUFFER_OVERFLOW:

            counters.buffer_overflows++;

            counters.dropped_samples++;

            break;






        case ErrorCode::FIFO_OVERRUN:

            counters.fifo_overruns++;

            break;






        case ErrorCode::INVALID_STATE:

            counters.invalid_state++;

            break;






        case ErrorCode::INVALID_CONFIG:

            counters.invalid_config++;

            break;


        case ErrorCode::NONE:
        default:

            break;
    }


    portEXIT_CRITICAL(&mux);
}









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






void ErrorManager::markRecoveryAttempt()
{
    portENTER_CRITICAL(&mux);


    counters.recovery_attempts++;


    portEXIT_CRITICAL(&mux);
}






void ErrorManager::markRecoverySuccess()
{
    portENTER_CRITICAL(&mux);


    counters.recovery_successes++;


    portEXIT_CRITICAL(&mux);
}






void ErrorManager::markRecoveryFailure()
{
    portENTER_CRITICAL(&mux);


    counters.recovery_failures++;


    portEXIT_CRITICAL(&mux);
}






ErrorCode ErrorManager::getLastError() const
{
    ErrorCode result;


    portENTER_CRITICAL(&mux);


    result =
        lastError;


    portEXIT_CRITICAL(&mux);


    return result;
}






ErrorCounters ErrorManager::getCounters() const
{
    ErrorCounters result;


    portENTER_CRITICAL(&mux);


    result =
        counters;


    portEXIT_CRITICAL(&mux);


    return result;
}






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