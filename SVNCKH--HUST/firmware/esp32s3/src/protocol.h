#pragma once

#include <Arduino.h>

#include "config.h"
#include "daq_types.h"
#include "state_machine.h"
#include "error_manager.h"






enum class ProtocolCommandType : uint8_t
{
    NONE = 0,

    HELP,
    STATUS,

    START,
    STOP,

    SET_RECORD_ID,
    SET_CONDITION,
    SET_MODE,
    SET_STREAM,

    STRESS,





    FAULT_I2C,
    FAULT_TIMEOUT,
    FAULT_DISCONNECT,
    FAULT_BUFFER_OVERFLOW,
    FAULT_FIFO_OVERRUN,

    RECOVER,
    CLEAR_FAULT,

    INVALID
};






struct ProtocolCommand
{
    ProtocolCommandType type =
        ProtocolCommandType::NONE;


    uint32_t arg1 =
        0;


    uint32_t arg2 =
        0;


    AcquisitionMode mode =
        AcquisitionMode::DATA_READY;


    bool boolValue =
        false;


    char text[48] =
        {0};
};






class Protocol
{
public:

    void begin(
        StateMachine* stateMachinePtr,
        ErrorManager* errorManagerPtr
    );


    void poll();


    bool commandAvailable() const;


    ProtocolCommand getCommand();


    void printStatus() const;


    void printHelp() const;


    static const char* commandName(
        ProtocolCommandType type
    );


    static const char* modeName(
        AcquisitionMode mode
    );


private:

    static constexpr size_t RX_BUFFER_SIZE =
        128;


    StateMachine* stateMachine =
        nullptr;


    ErrorManager* errorManager =
        nullptr;


    char rxBuffer[RX_BUFFER_SIZE] =
        {0};


    size_t rxLength =
        0;


    bool pendingCommandAvailable =
        false;


    ProtocolCommand pendingCommand;


    void parseLine(
        char* line
    );


    void setInvalidCommand();


    static char* trim(
        char* text
    );


    static void toUpper(
        char* text
    );


    static bool parseUnsigned(
        const char* text,
        uint32_t& value
    );


    static bool copyText(
        char* destination,
        size_t destinationSize,
        const char* source
    );
};