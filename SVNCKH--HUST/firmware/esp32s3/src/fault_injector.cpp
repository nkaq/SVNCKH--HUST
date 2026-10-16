#include "fault_injector.h"






void FaultInjector::begin()
{
    reset();
}






void FaultInjector::reset()
{
    portENTER_CRITICAL(
        &mux
    );


    pendingFault =
        InjectedFault::NONE;


    disconnectActive =
        false;


    portEXIT_CRITICAL(
        &mux
    );
}






void FaultInjector::arm(
    InjectedFault fault
)
{
    portENTER_CRITICAL(
        &mux
    );


    pendingFault =
        fault;


    if (
        fault ==
        InjectedFault::SENSOR_DISCONNECT
    )
    {
        disconnectActive =
            true;
    }


    portEXIT_CRITICAL(
        &mux
    );
}






bool FaultInjector::isArmed(
    InjectedFault fault
) const
{
    bool result;


    portENTER_CRITICAL(
        &mux
    );


    result =
        (
            pendingFault ==
            fault
        );


    portEXIT_CRITICAL(
        &mux
    );


    return result;
}






bool FaultInjector::consume(
    InjectedFault fault
)
{
    bool consumed =
        false;


    portENTER_CRITICAL(
        &mux
    );


    if (
        pendingFault ==
        fault
    )
    {
        pendingFault =
            InjectedFault::NONE;


        consumed =
            true;
    }


    portEXIT_CRITICAL(
        &mux
    );


    return consumed;
}






void FaultInjector::setDisconnectActive(
    bool active
)
{
    portENTER_CRITICAL(
        &mux
    );


    disconnectActive =
        active;


    if (!active)
    {
        if (
            pendingFault ==
            InjectedFault::SENSOR_DISCONNECT
        )
        {
            pendingFault =
                InjectedFault::NONE;
        }
    }


    portEXIT_CRITICAL(
        &mux
    );
}






bool FaultInjector::isDisconnectActive() const
{
    bool result;


    portENTER_CRITICAL(
        &mux
    );


    result =
        disconnectActive;


    portEXIT_CRITICAL(
        &mux
    );


    return result;
}






InjectedFault FaultInjector::getPendingFault() const
{
    InjectedFault result;


    portENTER_CRITICAL(
        &mux
    );


    result =
        pendingFault;


    portEXIT_CRITICAL(
        &mux
    );


    return result;
}






const char* FaultInjector::faultName(
    InjectedFault fault
) const
{
    switch (fault)
    {
        case InjectedFault::NONE:
            return "NONE";


        case InjectedFault::I2C_ERROR:
            return "I2C_ERROR";


        case InjectedFault::TIMEOUT:
            return "TIMEOUT";


        case InjectedFault::SENSOR_DISCONNECT:
            return "SENSOR_DISCONNECT";


        case InjectedFault::BUFFER_OVERFLOW:
            return "BUFFER_OVERFLOW";


        case InjectedFault::FIFO_OVERRUN:
            return "FIFO_OVERRUN";
    }


    return "UNKNOWN";
}