#pragma once

#include <Arduino.h>

#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"

#include "config.h"
#include "daq_types.h"


class RingBuffer
{
public:

    // Xóa toàn bộ packet đang có.
    void reset();


    // Producer thêm packet mới.
    //
    // true  = push thành công
    // false = buffer full → DROP NEWEST
    bool push(
        const SamplePacket& packet
    );


    // Consumer lấy packet cũ nhất.
    //
    // true  = pop thành công
    // false = buffer empty
    bool pop(
        SamplePacket& packet
    );


    bool isEmpty() const;

    bool isFull() const;


    // Số packet hiện tại trong buffer
    uint16_t size() const;


    // Capacity cố định
    uint16_t capacity() const;


    // Occupancy lớn nhất từng đạt
    uint16_t peakOccupancy() const;


private:

    // =================================================
    // STATIC STORAGE
    //
    // Không malloc/free trong RECORD.
    // =================================================

    SamplePacket
        buffer[Config::RING_BUFFER_CAPACITY];


    uint16_t writeIndex = 0;

    uint16_t readIndex = 0;

    uint16_t count = 0;

    uint16_t peakCount = 0;


    // =================================================
    // THREAD SAFETY
    //
    // Acquisition Task = producer
    // Output Task      = consumer
    // =================================================

    mutable portMUX_TYPE mux =
        portMUX_INITIALIZER_UNLOCKED;
};