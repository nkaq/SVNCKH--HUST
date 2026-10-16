#include "protocol.h"

#include <cstdlib>
#include <cstring>
#include <cctype>






void Protocol::begin(
    StateMachine* stateMachinePtr,
    ErrorManager* errorManagerPtr
)
{
    stateMachine =
        stateMachinePtr;


    errorManager =
        errorManagerPtr;


    rxLength =
        0;


    rxBuffer[0] =
        '\0';


    pendingCommandAvailable =
        false;


    pendingCommand =
        ProtocolCommand{};
}






void Protocol::poll()
{


    if (pendingCommandAvailable)
    {
        return;
    }


    while (Serial.available() > 0)
    {
        char c =
            static_cast<char>(
                Serial.read()
            );



        if (c == '\r')
        {
            continue;
        }






        if (c == '\n')
        {
            if (rxLength == 0)
            {
                continue;
            }


            rxBuffer[rxLength] =
                '\0';


            parseLine(
                rxBuffer
            );


            rxLength =
                0;


            rxBuffer[0] =
                '\0';


            return;
        }






        if (
            rxLength >=
            RX_BUFFER_SIZE - 1
        )
        {
            rxLength =
                0;


            rxBuffer[0] =
                '\0';


            setInvalidCommand();


            Serial.println(
                "ERR: COMMAND_TOO_LONG"
            );


            return;
        }


        rxBuffer[rxLength++] =
            c;
    }
}






bool Protocol::commandAvailable() const
{
    return pendingCommandAvailable;
}






ProtocolCommand Protocol::getCommand()
{
    ProtocolCommand result =
        pendingCommand;


    pendingCommand =
        ProtocolCommand{};


    pendingCommandAvailable =
        false;


    return result;
}






void Protocol::parseLine(
    char* line
)
{
    pendingCommand =
        ProtocolCommand{};


    char* cleaned =
        trim(
            line
        );


    if (
        cleaned == nullptr ||
        cleaned[0] == '\0'
    )
    {
        return;
    }






    char compact[RX_BUFFER_SIZE] =
        {0};


    if (
        !copyText(
            compact,
            sizeof(compact),
            cleaned
        )
    )
    {
        setInvalidCommand();

        return;
    }


    toUpper(
        compact
    );











    if (
        strcmp(
            compact,
            "HELP"
        ) == 0
    )
    {
        pendingCommand.type =
            ProtocolCommandType::HELP;


        pendingCommandAvailable =
            true;


        return;
    }






    if (
        strcmp(
            compact,
            "STATUS"
        ) == 0
    )
    {
        pendingCommand.type =
            ProtocolCommandType::STATUS;


        pendingCommandAvailable =
            true;


        return;
    }






    if (
        strcmp(
            compact,
            "STOP"
        ) == 0
    )
    {
        pendingCommand.type =
            ProtocolCommandType::STOP;


        pendingCommandAvailable =
            true;


        return;
    }











    if (
        strcmp(
            compact,
            "DR"
        ) == 0
    )
    {
        pendingCommand.type =
            ProtocolCommandType::SET_MODE;


        pendingCommand.mode =
            AcquisitionMode::DATA_READY;


        pendingCommandAvailable =
            true;


        return;
    }






    if (
        strcmp(
            compact,
            "FIFO"
        ) == 0
    )
    {
        pendingCommand.type =
            ProtocolCommandType::SET_MODE;


        pendingCommand.mode =
            AcquisitionMode::FIFO;


        pendingCommandAvailable =
            true;


        return;
    }







    if (
        strcmp(
            compact,
            "STREAM0"
        ) == 0
    )
    {
        pendingCommand.type =
            ProtocolCommandType::SET_STREAM;


        pendingCommand.boolValue =
            false;


        pendingCommandAvailable =
            true;


        return;
    }


    if (
        strcmp(
            compact,
            "STREAM1"
        ) == 0
    )
    {
        pendingCommand.type =
            ProtocolCommandType::SET_STREAM;


        pendingCommand.boolValue =
            true;


        pendingCommandAvailable =
            true;


        return;
    }











    if (
        strcmp(
            compact,
            "FAULTI2C"
        ) == 0
    )
    {
        pendingCommand.type =
            ProtocolCommandType::FAULT_I2C;


        pendingCommandAvailable =
            true;


        return;
    }






    if (
        strcmp(
            compact,
            "FAULTTIMEOUT"
        ) == 0
    )
    {
        pendingCommand.type =
            ProtocolCommandType::FAULT_TIMEOUT;


        pendingCommandAvailable =
            true;


        return;
    }






    if (
        strcmp(
            compact,
            "FAULTDISC"
        ) == 0
    )
    {
        pendingCommand.type =
            ProtocolCommandType::FAULT_DISCONNECT;


        pendingCommandAvailable =
            true;


        return;
    }






    if (
        strcmp(
            compact,
            "FAULTBUF"
        ) == 0
    )
    {
        pendingCommand.type =
            ProtocolCommandType::FAULT_BUFFER_OVERFLOW;


        pendingCommandAvailable =
            true;


        return;
    }






    if (
        strcmp(
            compact,
            "FAULTFIFO"
        ) == 0
    )
    {
        pendingCommand.type =
            ProtocolCommandType::FAULT_FIFO_OVERRUN;


        pendingCommandAvailable =
            true;


        return;
    }






    if (
        strcmp(
            compact,
            "RECOVER"
        ) == 0
    )
    {
        pendingCommand.type =
            ProtocolCommandType::RECOVER;


        pendingCommandAvailable =
            true;


        return;
    }






    if (
        strcmp(
            compact,
            "CLEARFAULT"
        ) == 0
    )
    {
        pendingCommand.type =
            ProtocolCommandType::CLEAR_FAULT;


        pendingCommandAvailable =
            true;


        return;
    }









    if (
        strncmp(
            compact,
            "START",
            5
        ) == 0
        &&
        strlen(compact) > 5
    )
    {
        const char* secondsText =
            compact + 5;


        uint32_t seconds =
            0;


        if (
            parseUnsigned(
                secondsText,
                seconds
            )
            &&
            seconds > 0
        )
        {
            pendingCommand.type =
                ProtocolCommandType::START;


            pendingCommand.arg1 =
                seconds;


            pendingCommandAvailable =
                true;


            return;
        }


        setInvalidCommand();

        return;
    }









    if (
        strncmp(
            compact,
            "STRESS",
            6
        ) == 0
        &&
        strlen(compact) > 6
    )
    {
        char stressBuffer[64] =
            {0};


        if (
            !copyText(
                stressBuffer,
                sizeof(stressBuffer),
                compact + 6
            )
        )
        {
            setInvalidCommand();

            return;
        }


        char* xPosition =
            strchr(
                stressBuffer,
                'X'
            );


        if (xPosition == nullptr)
        {
            setInvalidCommand();

            return;
        }


        *xPosition =
            '\0';


        const char* secondsText =
            stressBuffer;


        const char* runsText =
            xPosition + 1;


        uint32_t seconds =
            0;


        uint32_t runs =
            0;


        if (
            parseUnsigned(
                secondsText,
                seconds
            )
            &&
            parseUnsigned(
                runsText,
                runs
            )
            &&
            seconds > 0
            &&
            runs > 0
        )
        {
            pendingCommand.type =
                ProtocolCommandType::STRESS;


            pendingCommand.arg1 =
                seconds;


            pendingCommand.arg2 =
                runs;


            pendingCommandAvailable =
                true;


            return;
        }


        setInvalidCommand();

        return;
    }








    if (
        strncmp(
            compact,
            "RID",
            3
        ) == 0
        &&
        strlen(cleaned) > 3
    )
    {
        pendingCommand.type =
            ProtocolCommandType::SET_RECORD_ID;


        if (
            !copyText(
                pendingCommand.text,
                sizeof(
                    pendingCommand.text
                ),
                cleaned + 3
            )
        )
        {
            setInvalidCommand();

            return;
        }


        pendingCommandAvailable =
            true;


        return;
    }








    if (
        strncmp(
            compact,
            "COND",
            4
        ) == 0
        &&
        strlen(cleaned) > 4
    )
    {
        pendingCommand.type =
            ProtocolCommandType::SET_CONDITION;


        if (
            !copyText(
                pendingCommand.text,
                sizeof(
                    pendingCommand.text
                ),
                cleaned + 4
            )
        )
        {
            setInvalidCommand();

            return;
        }


        pendingCommandAvailable =
            true;


        return;
    }






    char standardBuffer[RX_BUFFER_SIZE] =
        {0};


    if (
        !copyText(
            standardBuffer,
            sizeof(standardBuffer),
            cleaned
        )
    )
    {
        setInvalidCommand();

        return;
    }


    char* command =
        standardBuffer;


    char* arguments =
        nullptr;


    char* space =
        strchr(
            standardBuffer,
            ' '
        );


    if (space != nullptr)
    {
        *space =
            '\0';


        arguments =
            trim(
                space + 1
            );
    }


    toUpper(
        command
    );






    if (
        strcmp(
            command,
            "HELP"
        ) == 0
    )
    {
        pendingCommand.type =
            ProtocolCommandType::HELP;


        pendingCommandAvailable =
            true;


        return;
    }






    if (
        strcmp(
            command,
            "STATUS"
        ) == 0
    )
    {
        pendingCommand.type =
            ProtocolCommandType::STATUS;


        pendingCommandAvailable =
            true;


        return;
    }






    if (
        strcmp(
            command,
            "STOP"
        ) == 0
    )
    {
        pendingCommand.type =
            ProtocolCommandType::STOP;


        pendingCommandAvailable =
            true;


        return;
    }






    if (
        strcmp(
            command,
            "START"
        ) == 0
    )
    {
        pendingCommand.type =
            ProtocolCommandType::START;


        if (
            arguments == nullptr ||
            arguments[0] == '\0'
        )
        {
            pendingCommand.arg1 =
                0;


            pendingCommandAvailable =
                true;


            return;
        }


        uint32_t seconds =
            0;


        if (
            !parseUnsigned(
                arguments,
                seconds
            )
            ||
            seconds == 0
        )
        {
            setInvalidCommand();

            return;
        }


        pendingCommand.arg1 =
            seconds;


        pendingCommandAvailable =
            true;


        return;
    }






    if (
        strcmp(
            command,
            "SET_RECORD_ID"
        ) == 0
    )
    {
        if (
            arguments == nullptr ||
            arguments[0] == '\0'
        )
        {
            setInvalidCommand();

            return;
        }


        pendingCommand.type =
            ProtocolCommandType::SET_RECORD_ID;


        if (
            !copyText(
                pendingCommand.text,
                sizeof(
                    pendingCommand.text
                ),
                arguments
            )
        )
        {
            setInvalidCommand();

            return;
        }


        pendingCommandAvailable =
            true;


        return;
    }






    if (
        strcmp(
            command,
            "SET_CONDITION"
        ) == 0
    )
    {
        if (
            arguments == nullptr ||
            arguments[0] == '\0'
        )
        {
            setInvalidCommand();

            return;
        }


        pendingCommand.type =
            ProtocolCommandType::SET_CONDITION;


        if (
            !copyText(
                pendingCommand.text,
                sizeof(
                    pendingCommand.text
                ),
                arguments
            )
        )
        {
            setInvalidCommand();

            return;
        }


        pendingCommandAvailable =
            true;


        return;
    }






    if (
        strcmp(
            command,
            "SET_MODE"
        ) == 0
    )
    {
        if (
            arguments == nullptr ||
            arguments[0] == '\0'
        )
        {
            setInvalidCommand();

            return;
        }


        char modeBuffer[32] =
            {0};


        if (
            !copyText(
                modeBuffer,
                sizeof(modeBuffer),
                arguments
            )
        )
        {
            setInvalidCommand();

            return;
        }


        toUpper(
            modeBuffer
        );


        pendingCommand.type =
            ProtocolCommandType::SET_MODE;


        if (
            strcmp(
                modeBuffer,
                "DATA_READY"
            ) == 0
        )
        {
            pendingCommand.mode =
                AcquisitionMode::DATA_READY;
        }
        else if (
            strcmp(
                modeBuffer,
                "FIFO"
            ) == 0
        )
        {
            pendingCommand.mode =
                AcquisitionMode::FIFO;
        }
        else
        {
            setInvalidCommand();

            return;
        }


        pendingCommandAvailable =
            true;


        return;
    }






    if (
        strcmp(
            command,
            "SET_STREAM"
        ) == 0
    )
    {
        if (arguments == nullptr)
        {
            setInvalidCommand();

            return;
        }


        if (
            strcmp(
                arguments,
                "0"
            ) == 0
        )
        {
            pendingCommand.type =
                ProtocolCommandType::SET_STREAM;


            pendingCommand.boolValue =
                false;


            pendingCommandAvailable =
                true;


            return;
        }


        if (
            strcmp(
                arguments,
                "1"
            ) == 0
        )
        {
            pendingCommand.type =
                ProtocolCommandType::SET_STREAM;


            pendingCommand.boolValue =
                true;


            pendingCommandAvailable =
                true;


            return;
        }


        setInvalidCommand();

        return;
    }






    if (
        strcmp(
            command,
            "STRESS"
        ) == 0
    )
    {
        pendingCommand.type =
            ProtocolCommandType::STRESS;


        if (
            arguments == nullptr ||
            arguments[0] == '\0'
        )
        {
            pendingCommand.arg1 =
                0;


            pendingCommand.arg2 =
                0;


            pendingCommandAvailable =
                true;


            return;
        }


        char argsBuffer[64] =
            {0};


        if (
            !copyText(
                argsBuffer,
                sizeof(argsBuffer),
                arguments
            )
        )
        {
            setInvalidCommand();

            return;
        }


        char* savePtr =
            nullptr;


        char* first =
            strtok_r(
                argsBuffer,
                " ",
                &savePtr
            );


        char* second =
            strtok_r(
                nullptr,
                " ",
                &savePtr
            );


        char* extra =
            strtok_r(
                nullptr,
                " ",
                &savePtr
            );


        if (
            first == nullptr ||
            extra != nullptr
        )
        {
            setInvalidCommand();

            return;
        }


        uint32_t seconds =
            0;


        if (
            !parseUnsigned(
                first,
                seconds
            )
            ||
            seconds == 0
        )
        {
            setInvalidCommand();

            return;
        }


        pendingCommand.arg1 =
            seconds;


        if (second == nullptr)
        {
            pendingCommand.arg2 =
                0;


            pendingCommandAvailable =
                true;


            return;
        }


        uint32_t runs =
            0;


        if (
            !parseUnsigned(
                second,
                runs
            )
            ||
            runs == 0
        )
        {
            setInvalidCommand();

            return;
        }


        pendingCommand.arg2 =
            runs;


        pendingCommandAvailable =
            true;


        return;
    }






    setInvalidCommand();
}






void Protocol::setInvalidCommand()
{
    pendingCommand =
        ProtocolCommand{};


    pendingCommand.type =
        ProtocolCommandType::INVALID;


    pendingCommandAvailable =
        true;


    if (errorManager != nullptr)
    {
        errorManager->report(
            ErrorCode::INVALID_CONFIG
        );
    }
}






void Protocol::printStatus() const
{
    Serial.print(
        "firmware="
    );

    Serial.println(
        Config::FIRMWARE_VERSION
    );


    Serial.print(
        "protocol="
    );

    Serial.println(
        Config::PROTOCOL_VERSION
    );


    Serial.print(
        "state="
    );


    if (stateMachine != nullptr)
    {
        Serial.println(
            stateMachine->stateName()
        );
    }
    else
    {
        Serial.println(
            "UNKNOWN"
        );
    }


    if (errorManager == nullptr)
    {
        return;
    }


    ErrorCounters counters =
        errorManager->getCounters();


    Serial.print(
        "last_error="
    );

    Serial.println(
        errorManager->errorName(
            errorManager->getLastError()
        )
    );


    Serial.print(
        "total_errors="
    );

    Serial.println(
        counters.total_errors
    );


    Serial.print(
        "i2c_errors="
    );

    Serial.println(
        counters.i2c_errors
    );


    Serial.print(
        "timeouts="
    );

    Serial.println(
        counters.timeouts
    );


    Serial.print(
        "disconnects="
    );

    Serial.println(
        counters.disconnects
    );


    Serial.print(
        "buffer_overflows="
    );

    Serial.println(
        counters.buffer_overflows
    );


    Serial.print(
        "fifo_overruns="
    );

    Serial.println(
        counters.fifo_overruns
    );


    Serial.print(
        "dropped_samples="
    );

    Serial.println(
        counters.dropped_samples
    );


    Serial.print(
        "invalid_state="
    );

    Serial.println(
        counters.invalid_state
    );


    Serial.print(
        "invalid_config="
    );

    Serial.println(
        counters.invalid_config
    );


    Serial.print(
        "recovery_attempts="
    );

    Serial.println(
        counters.recovery_attempts
    );


    Serial.print(
        "recovery_successes="
    );

    Serial.println(
        counters.recovery_successes
    );


    Serial.print(
        "recovery_failures="
    );

    Serial.println(
        counters.recovery_failures
    );
}






void Protocol::printHelp() const
{
    Serial.println(
        "===== EE2 COMMANDS ====="
    );


    Serial.println(
        "HELP"
    );


    Serial.println(
        "STATUS"
    );


    Serial.println(
        "DR"
    );


    Serial.println(
        "FIFO"
    );


    Serial.println(
        "START5"
    );


    Serial.println(
        "STOP"
    );


    Serial.println(
        "STREAM0"
    );


    Serial.println(
        "STREAM1"
    );


    Serial.println(
        "RIDDAY2001"
    );


    Serial.println(
        "CONDMOCK"
    );


    Serial.println(
        "STRESS5X2"
    );


    Serial.println(
        "STRESS60X3"
    );


    Serial.println();


    Serial.println(
        "===== DAY2 FAULT TEST ====="
    );


    Serial.println(
        "FAULTI2C"
    );


    Serial.println(
        "FAULTTIMEOUT"
    );


    Serial.println(
        "FAULTDISC"
    );


    Serial.println(
        "FAULTBUF"
    );


    Serial.println(
        "FAULTFIFO"
    );


    Serial.println(
        "RECOVER"
    );


    Serial.println(
        "CLEARFAULT"
    );


    Serial.println(
        "==========================="
    );
}






const char* Protocol::commandName(
    ProtocolCommandType type
)
{
    switch (type)
    {
        case ProtocolCommandType::NONE:
            return "NONE";


        case ProtocolCommandType::HELP:
            return "HELP";


        case ProtocolCommandType::STATUS:
            return "STATUS";


        case ProtocolCommandType::START:
            return "START";


        case ProtocolCommandType::STOP:
            return "STOP";


        case ProtocolCommandType::SET_RECORD_ID:
            return "SET_RECORD_ID";


        case ProtocolCommandType::SET_CONDITION:
            return "SET_CONDITION";


        case ProtocolCommandType::SET_MODE:
            return "SET_MODE";


        case ProtocolCommandType::SET_STREAM:
            return "SET_STREAM";


        case ProtocolCommandType::STRESS:
            return "STRESS";


        case ProtocolCommandType::FAULT_I2C:
            return "FAULT_I2C";


        case ProtocolCommandType::FAULT_TIMEOUT:
            return "FAULT_TIMEOUT";


        case ProtocolCommandType::FAULT_DISCONNECT:
            return "FAULT_DISCONNECT";


        case ProtocolCommandType::FAULT_BUFFER_OVERFLOW:
            return "FAULT_BUFFER_OVERFLOW";


        case ProtocolCommandType::FAULT_FIFO_OVERRUN:
            return "FAULT_FIFO_OVERRUN";


        case ProtocolCommandType::RECOVER:
            return "RECOVER";


        case ProtocolCommandType::CLEAR_FAULT:
            return "CLEAR_FAULT";


        case ProtocolCommandType::INVALID:
            return "INVALID";
    }


    return "UNKNOWN";
}






const char* Protocol::modeName(
    AcquisitionMode mode
)
{
    switch (mode)
    {
        case AcquisitionMode::DATA_READY:
            return "DATA_READY";


        case AcquisitionMode::FIFO:
            return "FIFO";
    }


    return "UNKNOWN";
}






char* Protocol::trim(
    char* text
)
{
    if (text == nullptr)
    {
        return nullptr;
    }


    while (
        *text != '\0'
        &&
        isspace(
            static_cast<unsigned char>(
                *text
            )
        )
    )
    {
        text++;
    }


    if (*text == '\0')
    {
        return text;
    }


    char* end =
        text
        + strlen(text)
        - 1;


    while (
        end >= text
        &&
        isspace(
            static_cast<unsigned char>(
                *end
            )
        )
    )
    {
        *end =
            '\0';


        end--;
    }


    return text;
}






void Protocol::toUpper(
    char* text
)
{
    if (text == nullptr)
    {
        return;
    }


    while (*text != '\0')
    {
        *text =
            static_cast<char>(
                toupper(
                    static_cast<unsigned char>(
                        *text
                    )
                )
            );


        text++;
    }
}






bool Protocol::parseUnsigned(
    const char* text,
    uint32_t& value
)
{
    if (
        text == nullptr
        ||
        text[0] == '\0'
    )
    {
        return false;
    }


    const char* p =
        text;


    while (*p != '\0')
    {
        if (
            !isdigit(
                static_cast<unsigned char>(
                    *p
                )
            )
        )
        {
            return false;
        }


        p++;
    }


    char* endPtr =
        nullptr;


    unsigned long parsed =
        strtoul(
            text,
            &endPtr,
            10
        );


    if (
        endPtr == text
        ||
        *endPtr != '\0'
    )
    {
        return false;
    }


    value =
        static_cast<uint32_t>(
            parsed
        );


    return true;
}






bool Protocol::copyText(
    char* destination,
    size_t destinationSize,
    const char* source
)
{
    if (
        destination == nullptr
        ||
        source == nullptr
        ||
        destinationSize == 0
    )
    {
        return false;
    }


    size_t length =
        strlen(
            source
        );


    if (
        length >=
        destinationSize
    )
    {
        return false;
    }


    memcpy(
        destination,
        source,
        length + 1
    );


    return true;
}