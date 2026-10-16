#pragma once

#include <Arduino.h>

#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"

#include "daq_types.h"


class ErrorManager
{
public:


    void reset();



    void report(
        ErrorCode error
    );



    void addDroppedSamples(
        uint32_t count
    );






    void markRecoveryAttempt();

    void markRecoverySuccess();

    void markRecoveryFailure();






    ErrorCode getLastError() const;



    ErrorCounters getCounters() const;


    const char* errorName(
        ErrorCode error
    ) const;


private:

    ErrorCounters counters;

    ErrorCode lastError =
        ErrorCode::NONE;


    mutable portMUX_TYPE mux =
        portMUX_INITIALIZER_UNLOCKED;
};