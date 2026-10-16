#include "state_machine.h"


// =====================================================
// BEGIN
// =====================================================

void StateMachine::begin()
{
    currentState =
        DaqState::INIT;
}


// =====================================================
// GET STATE
// =====================================================

DaqState StateMachine::getState() const
{
    return currentState;
}


// =====================================================
// TRANSITION
// =====================================================

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


// =====================================================
// TRANSITION RULES
// =====================================================

bool StateMachine::isTransitionAllowed(
    DaqState from,
    DaqState to
) const
{
    switch (from)
    {
        // ---------------------------------------------
        // INIT
        // ---------------------------------------------

        case DaqState::INIT:

            return (
                to == DaqState::SELF_CHECK ||
                to == DaqState::ERROR
            );


        // ---------------------------------------------
        // SELF CHECK
        // ---------------------------------------------

        case DaqState::SELF_CHECK:

            return (
                to == DaqState::READY ||
                to == DaqState::ERROR
            );


        // ---------------------------------------------
        // READY
        // ---------------------------------------------

        case DaqState::READY:

            return (
                to == DaqState::STABILIZE ||
                to == DaqState::ERROR
            );


        // ---------------------------------------------
        // STABILIZE
        // ---------------------------------------------

        case DaqState::STABILIZE:

            return (
                to == DaqState::RECORD ||
                to == DaqState::STOPPING ||
                to == DaqState::ERROR
            );


        // ---------------------------------------------
        // RECORD
        // ---------------------------------------------

        case DaqState::RECORD:

            return (
                to == DaqState::STOPPING ||
                to == DaqState::ERROR
            );


        // ---------------------------------------------
        // STOPPING
        // ---------------------------------------------

        case DaqState::STOPPING:

            return (
                to == DaqState::QC ||
                to == DaqState::ERROR
            );


        // ---------------------------------------------
        // QC
        // ---------------------------------------------

        case DaqState::QC:

            return (
                to == DaqState::READY ||
                to == DaqState::ERROR
            );


        // ---------------------------------------------
        // ERROR
        //
        // Recovery thành công:
        // ERROR -> SELF_CHECK
        // ---------------------------------------------

        case DaqState::ERROR:

            return (
                to == DaqState::SELF_CHECK
            );
    }


    return false;
}


// =====================================================
// STATE NAME - CURRENT
// =====================================================

const char* StateMachine::stateName() const
{
    return stateName(
        currentState
    );
}


// =====================================================
// STATE NAME
// =====================================================

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