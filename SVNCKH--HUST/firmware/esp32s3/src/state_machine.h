#pragma once

#include <Arduino.h>

#include "daq_types.h"


class StateMachine
{
public:


    void begin();



    DaqState getState() const;









    bool transitionTo(
        DaqState newState
    );



    const char* stateName() const;



    static const char* stateName(
        DaqState state
    );


private:

    DaqState currentState =
        DaqState::INIT;


    bool isTransitionAllowed(
        DaqState from,
        DaqState to
    ) const;
};