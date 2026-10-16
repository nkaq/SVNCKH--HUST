#pragma once

#include <Arduino.h>


// =====================================================
// FAULT INJECTION TYPES
//
// Chỉ dùng cho engineering validation / software test.
// Không phải lỗi thật từ sensor.
// =====================================================

enum class InjectedFault : uint8_t
{
    NONE = 0,

    I2C_ERROR,

    TIMEOUT,

    SENSOR_DISCONNECT,

    BUFFER_OVERFLOW,

    FIFO_OVERRUN
};


// =====================================================
// FAULT INJECTOR
// =====================================================

class FaultInjector
{
public:

    void begin();

    void reset();


    // Arm một lỗi.
    // Lỗi sẽ được consume đúng 1 lần,
    // trừ SENSOR_DISCONNECT có thể giữ active.
    void arm(
        InjectedFault fault
    );


    // Kiểm tra lỗi đang chờ.
    bool isArmed(
        InjectedFault fault
    ) const;


    // Consume lỗi một lần.
    bool consume(
        InjectedFault fault
    );


    // SENSOR_DISCONNECT có thể giữ liên tục.
    void setDisconnectActive(
        bool active
    );


    bool isDisconnectActive() const;


    InjectedFault getPendingFault() const;


    const char* faultName(
        InjectedFault fault
    ) const;


private:

    mutable portMUX_TYPE mux =
        portMUX_INITIALIZER_UNLOCKED;


    InjectedFault pendingFault =
        InjectedFault::NONE;


    bool disconnectActive =
        false;
};