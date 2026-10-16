#pragma once

#include <Arduino.h>









enum class InjectedFault : uint8_t
{
    NONE = 0,

    I2C_ERROR,

    TIMEOUT,

    SENSOR_DISCONNECT,

    BUFFER_OVERFLOW,

    FIFO_OVERRUN
};






class FaultInjector
{
public:

    void begin();

    void reset();





    void arm(
        InjectedFault fault
    );



    bool isArmed(
        InjectedFault fault
    ) const;



    bool consume(
        InjectedFault fault
    );



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