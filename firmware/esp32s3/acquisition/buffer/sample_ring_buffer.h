#pragma once

#include <Arduino.h>

#include "../common/config.h"
#include "../common/daq_types.h"

class SampleRingBuffer
{
public:
    bool push(
        const SamplePacket& packet
    )
    {
        if (count_ >= Config::RING_BUFFER_CAPACITY)
        {
            overflow_count_++;
            dropped_samples_++;
            return false;
        }

        buffer_[head_] = packet;

        head_ =
            static_cast<uint16_t>(
                (head_ + 1) %
                Config::RING_BUFFER_CAPACITY
            );

        count_++;

        return true;
    }

    bool pop(
        SamplePacket& packet
    )
    {
        if (count_ == 0)
        {
            return false;
        }

        packet = buffer_[tail_];

        tail_ =
            static_cast<uint16_t>(
                (tail_ + 1) %
                Config::RING_BUFFER_CAPACITY
            );

        count_--;

        return true;
    }

    bool peek(
        SamplePacket& packet
    ) const
    {
        if (count_ == 0)
        {
            return false;
        }

        packet = buffer_[tail_];

        return true;
    }

    void clear()
    {
        head_ = 0;
        tail_ = 0;
        count_ = 0;
    }

    void resetStats()
    {
        overflow_count_ = 0;
        dropped_samples_ = 0;
    }

    uint16_t size() const
    {
        return count_;
    }

    constexpr uint16_t capacity() const
    {
        return Config::RING_BUFFER_CAPACITY;
    }

    bool empty() const
    {
        return count_ == 0;
    }

    bool full() const
    {
        return count_ >= Config::RING_BUFFER_CAPACITY;
    }

    uint32_t overflowCount() const
    {
        return overflow_count_;
    }

    uint32_t droppedSamples() const
    {
        return dropped_samples_;
    }

private:
    SamplePacket buffer_[
        Config::RING_BUFFER_CAPACITY
    ] = {};

    uint16_t head_ = 0;
    uint16_t tail_ = 0;
    uint16_t count_ = 0;

    uint32_t overflow_count_ = 0;
    uint32_t dropped_samples_ = 0;
};