#include "ring_buffer.h"






void RingBuffer::reset()
{
    portENTER_CRITICAL(&mux);


    writeIndex = 0;

    readIndex = 0;

    count = 0;

    peakCount = 0;


    portEXIT_CRITICAL(&mux);
}






bool RingBuffer::push(
    const SamplePacket& packet
)
{
    bool success = false;


    portENTER_CRITICAL(&mux);










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






bool RingBuffer::pop(
    SamplePacket& packet
)
{
    bool success = false;


    portENTER_CRITICAL(&mux);


    if (count > 0)
    {


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






bool RingBuffer::isEmpty() const
{
    bool result;


    portENTER_CRITICAL(&mux);


    result =
        (count == 0);


    portEXIT_CRITICAL(&mux);


    return result;
}






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






uint16_t RingBuffer::size() const
{
    uint16_t result;


    portENTER_CRITICAL(&mux);


    result = count;


    portEXIT_CRITICAL(&mux);


    return result;
}






uint16_t RingBuffer::capacity() const
{
    return Config::RING_BUFFER_CAPACITY;
}






uint16_t RingBuffer::peakOccupancy() const
{
    uint16_t result;


    portENTER_CRITICAL(&mux);


    result =
        peakCount;


    portEXIT_CRITICAL(&mux);


    return result;
}