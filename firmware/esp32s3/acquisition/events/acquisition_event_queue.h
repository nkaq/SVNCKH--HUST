#pragma once

#include <Arduino.h>

#include "../common/config.h"

enum class AcquisitionEventSource : uint8_t
{
    ADXL345 = 0,
    MAX31865,
    ACS724,
    US5881,
    DHT22,
    OLED
};

enum class AcquisitionEventType : uint8_t
{
    SAMPLE_DUE = 0,
    DATA_READY,
    FIFO_WATERMARK
};

struct AcquisitionEvent
{
    AcquisitionEventSource source =
        AcquisitionEventSource::ADXL345;

    AcquisitionEventType type =
        AcquisitionEventType::SAMPLE_DUE;

    uint64_t scheduled_time_us = 0;
};

class AcquisitionEventQueue
{
public:
    bool push(
        const AcquisitionEvent& event
    )
    {
        portENTER_CRITICAL(
            &mux_
        );

        const bool result =
            pushUnsafe(
                event
            );

        portEXIT_CRITICAL(
            &mux_
        );

        return result;
    }

    bool pushFromISR(
        const AcquisitionEvent& event
    )
    {
        portENTER_CRITICAL_ISR(
            &mux_
        );

        const bool result =
            pushUnsafe(
                event
            );

        portEXIT_CRITICAL_ISR(
            &mux_
        );

        return result;
    }

    bool pop(
        AcquisitionEvent& event
    )
    {
        portENTER_CRITICAL(
            &mux_
        );

        if (count_ == 0)
        {
            portEXIT_CRITICAL(
                &mux_
            );

            return false;
        }

        event =
            buffer_[tail_];

        tail_ =
            static_cast<uint16_t>(
                (tail_ + 1) %
                Config::EVENT_QUEUE_CAPACITY
            );

        count_--;

        portEXIT_CRITICAL(
            &mux_
        );

        return true;
    }

    void clear()
    {
        portENTER_CRITICAL(
            &mux_
        );

        head_ = 0;
        tail_ = 0;
        count_ = 0;

        portEXIT_CRITICAL(
            &mux_
        );
    }

    void resetStats()
    {
        portENTER_CRITICAL(
            &mux_
        );

        overflow_count_ = 0;

        portEXIT_CRITICAL(
            &mux_
        );
    }

    uint16_t size()
    {
        portENTER_CRITICAL(
            &mux_
        );

        const uint16_t value =
            count_;

        portEXIT_CRITICAL(
            &mux_
        );

        return value;
    }

    bool empty()
    {
        return size() == 0;
    }

    bool full()
    {
        return size() >=
               Config::EVENT_QUEUE_CAPACITY;
    }

    uint32_t overflowCount()
    {
        portENTER_CRITICAL(
            &mux_
        );

        const uint32_t value =
            overflow_count_;

        portEXIT_CRITICAL(
            &mux_
        );

        return value;
    }

private:
    bool pushUnsafe(
        const AcquisitionEvent& event
    )
    {
        if (
            count_ >=
            Config::EVENT_QUEUE_CAPACITY
        )
        {
            overflow_count_++;
            return false;
        }

        buffer_[head_] =
            event;

        head_ =
            static_cast<uint16_t>(
                (head_ + 1) %
                Config::EVENT_QUEUE_CAPACITY
            );

        count_++;

        return true;
    }

    AcquisitionEvent buffer_[
        Config::EVENT_QUEUE_CAPACITY
    ] = {};

    uint16_t head_ = 0;
    uint16_t tail_ = 0;
    uint16_t count_ = 0;

    uint32_t overflow_count_ = 0;

    portMUX_TYPE mux_ =
        portMUX_INITIALIZER_UNLOCKED;
};