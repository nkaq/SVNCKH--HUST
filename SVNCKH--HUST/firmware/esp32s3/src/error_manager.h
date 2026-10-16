#pragma once

#include <Arduino.h>

#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"

#include "daq_types.h"


class ErrorManager
{
public:

    // Reset toàn bộ error/recovery counters
    void reset();


    // Báo một error event
    void report(
        ErrorCode error
    );


    // Khi biết chính xác có thêm N sample bị mất
    void addDroppedSamples(
        uint32_t count
    );


    // =================================================
    // RECOVERY COUNTERS
    // =================================================

    void markRecoveryAttempt();

    void markRecoverySuccess();

    void markRecoveryFailure();


    // =================================================
    // READ STATUS
    // =================================================

    ErrorCode getLastError() const;


    // Return COPY để an toàn khi nhiều task cùng chạy.
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