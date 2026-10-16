#pragma once

#include <Arduino.h>

#include "daq_types.h"


class StateMachine
{
public:

    // Khởi tạo state machine về INIT
    void begin();


    // Lấy state hiện tại
    DaqState getState() const;


    // Chuyển state.
    //
    // return true:
    // transition hợp lệ.
    //
    // return false:
    // transition không hợp lệ.
    bool transitionTo(
        DaqState newState
    );


    // Tên state để log/status
    const char* stateName() const;


    // Tên của một state bất kỳ
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