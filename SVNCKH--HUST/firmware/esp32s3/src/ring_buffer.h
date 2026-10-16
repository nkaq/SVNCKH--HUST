#pragma once

#include <Arduino.h>

#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"

#include "config.h"
#include "daq_types.h"


class RingBuffer
{
public:


    void reset();






    bool push(
        const SamplePacket& packet
    );






    bool pop(
        SamplePacket& packet
    );


    bool isEmpty() const;

    bool isFull() const;



    uint16_t size() const;



    uint16_t capacity() const;



    uint16_t peakOccupancy() const;


private:







    SamplePacket
        buffer[Config::RING_BUFFER_CAPACITY];


    uint16_t writeIndex = 0;

    uint16_t readIndex = 0;

    uint16_t count = 0;

    uint16_t peakCount = 0;









    mutable portMUX_TYPE mux =
        portMUX_INITIALIZER_UNLOCKED;
};