#pragma once

#include <Arduino.h>
#include <esp_timer.h>

class AcquisitionTimebase
{
public:
    void reset()
    {
        next_sample_index_ = 0;
        last_timestamp_us_ = 0;
        started_ = false;
    }

    bool stamp(
        uint64_t timestamp_us,
        uint32_t& sample_index
    )
    {
        if (
            started_ &&
            timestamp_us <= last_timestamp_us_
        )
        {
            return false;
        }

        sample_index = next_sample_index_;
        next_sample_index_++;

        last_timestamp_us_ = timestamp_us;
        started_ = true;

        return true;
    }

    bool stampNow(
        uint32_t& sample_index,
        uint64_t& timestamp_us
    )
    {
        timestamp_us =
            static_cast<uint64_t>(
                esp_timer_get_time()
            );

        if (
            started_ &&
            timestamp_us <= last_timestamp_us_
        )
        {
            timestamp_us =
                last_timestamp_us_ + 1;
        }

        return stamp(
            timestamp_us,
            sample_index
        );
    }

    uint32_t nextSampleIndex() const
    {
        return next_sample_index_;
    }

    uint64_t lastTimestampUs() const
    {
        return last_timestamp_us_;
    }

    bool started() const
    {
        return started_;
    }

private:
    uint32_t next_sample_index_ = 0;
    uint64_t last_timestamp_us_ = 0;
    bool started_ = false;
};