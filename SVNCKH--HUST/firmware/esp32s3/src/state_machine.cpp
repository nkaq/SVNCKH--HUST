#include "state_machine.h"






void StateMachine::begin()
{
    currentState =
        DaqState::INIT;
}






DaqState StateMachine::getState() const
{
    return currentState;
}






bool StateMachine::transitionTo(
    DaqState newState
)
{
    if (
        !isTransitionAllowed(
            currentState,
            newState
        )
    )
    {
        return false;
    }


    currentState =
        newState;


    return true;
}






bool StateMachine::isTransitionAllowed(
    DaqState from,
    DaqState to
) const
{
    switch (from)
    {




        case DaqState::INIT:

            return (
                to == DaqState::SELF_CHECK ||
                to == DaqState::ERROR
            );






        case DaqState::SELF_CHECK:

            return (
                to == DaqState::READY ||
                to == DaqState::ERROR
            );






        case DaqState::READY:

            return (
                to == DaqState::STABILIZE ||
                to == DaqState::ERROR
            );






        case DaqState::STABILIZE:

            return (
                to == DaqState::RECORD ||
                to == DaqState::STOPPING ||
                to == DaqState::ERROR
            );






        case DaqState::RECORD:

            return (
                to == DaqState::STOPPING ||
                to == DaqState::ERROR
            );






        case DaqState::STOPPING:

            return (
                to == DaqState::QC ||
                to == DaqState::ERROR
            );






        case DaqState::QC:

            return (
                to == DaqState::READY ||
                to == DaqState::ERROR
            );









        case DaqState::ERROR:

            return (
                to == DaqState::SELF_CHECK
            );
    }


    return false;
}






const char* StateMachine::stateName() const
{
    return stateName(
        currentState
    );
}






const char* StateMachine::stateName(
    DaqState state
)
{
    switch (state)
    {
        case DaqState::INIT:
            return "INIT";

        case DaqState::SELF_CHECK:
            return "SELF_CHECK";

        case DaqState::READY:
            return "READY";

        case DaqState::STABILIZE:
            return "STABILIZE";

        case DaqState::RECORD:
            return "RECORD";

        case DaqState::STOPPING:
            return "STOPPING";

        case DaqState::QC:
            return "QC";

        case DaqState::ERROR:
            return "ERROR";
    }


    return "UNKNOWN";
}