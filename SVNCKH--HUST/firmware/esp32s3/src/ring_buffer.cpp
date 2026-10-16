#include "ring_buffer.h"


// =====================================================
// RESET
// =====================================================

void RingBuffer::reset()
{
    portENTER_CRITICAL(&mux);


    writeIndex = 0;

    readIndex = 0;

    count = 0;

    peakCount = 0;


    portEXIT_CRITICAL(&mux);
}


// =====================================================
// PUSH
// =====================================================

bool RingBuffer::push(
    const SamplePacket& packet
)
{
    bool success = false;


    portENTER_CRITICAL(&mux);


    // -------------------------------------------------
    // POLICY:
    //
    // Buffer full → DROP NEWEST
    //
    // Không overwrite packet cũ.
    // -------------------------------------------------

    if (
        count
        < Config::RING_BUFFER_CAPACITY
    )
    {
        buffer[writeIndex] =
            packet;


        writeIndex =
            (writeIndex + 1)
            % Config::RING_BUFFER_CAPACITY;


        count++;


        if (count > peakCount)
        {
            peakCount = count;
        }


        success = true;
    }


    portEXIT_CRITICAL(&mux);


    return success;
}


// =====================================================
// POP
// =====================================================

bool RingBuffer::pop(
    SamplePacket& packet
)
{
    bool success = false;


    portENTER_CRITICAL(&mux);


    if (count > 0)
    {
        // FIFO:
        // packet cũ nhất được lấy trước.
        packet =
            buffer[readIndex];


        readIndex =
            (readIndex + 1)
            % Config::RING_BUFFER_CAPACITY;


        count--;


        success = true;
    }


    portEXIT_CRITICAL(&mux);


    return success;
}


// =====================================================
// IS EMPTY
// =====================================================

bool RingBuffer::isEmpty() const
{
    bool result;


    portENTER_CRITICAL(&mux);


    result =
        (count == 0);


    portEXIT_CRITICAL(&mux);


    return result;
}


// =====================================================
// IS FULL
// =====================================================

bool RingBuffer::isFull() const
{
    bool result;


    portENTER_CRITICAL(&mux);


    result =
        (
            count
            >= Config::RING_BUFFER_CAPACITY
        );


    portEXIT_CRITICAL(&mux);


    return result;
}


// =====================================================
// SIZE
// =====================================================

uint16_t RingBuffer::size() const
{
    uint16_t result;


    portENTER_CRITICAL(&mux);


    result = count;


    portEXIT_CRITICAL(&mux);


    return result;
}


// =====================================================
// CAPACITY
// =====================================================

uint16_t RingBuffer::capacity() const
{
    return Config::RING_BUFFER_CAPACITY;
}


// =====================================================
// PEAK OCCUPANCY
// =====================================================

uint16_t RingBuffer::peakOccupancy() const
{
    uint16_t result;


    portENTER_CRITICAL(&mux);


    result =
        peakCount;


    portEXIT_CRITICAL(&mux);


    return result;
}