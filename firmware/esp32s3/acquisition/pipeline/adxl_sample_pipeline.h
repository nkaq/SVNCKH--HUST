#pragma once

#include <Arduino.h>

#include "../buffer/sample_ring_buffer.h"
#include "../common/daq_types.h"
#include "../diagnostics/error_tracker.h"
#include "../timebase/acquisition_timebase.h"

class AdxlSamplePipeline
{
public:
    void reset()
    {
        buffer_.clear();
        buffer_.resetStats();
        timebase_.reset();
        errors_.reset();
    }

    bool submit(
        const RawSample& raw,
        uint64_t timestamp_us,
        uint16_t status_flags = STATUS_OK
    )
    {
        uint32_t sample_index = 0;

        if (
            !timebase_.stamp(
                timestamp_us,
                sample_index
            )
        )
        {
            errors_.record(
                ErrorCode::TIMESTAMP_ERROR
            );

            errors_.recordDroppedSamples();

            return false;
        }

        SamplePacket packet = {};

        packet.sample_index = sample_index;
        packet.timestamp_us = timestamp_us;
        packet.x_raw = raw.x;
        packet.y_raw = raw.y;
        packet.z_raw = raw.z;
        packet.status_flags = status_flags;

        if (!buffer_.push(packet))
        {
            errors_.record(
                ErrorCode::BUFFER_OVERFLOW
            );

            errors_.recordDroppedSamples();

            return false;
        }

        return true;
    }

    bool submitNow(
        const RawSample& raw,
        uint16_t status_flags = STATUS_OK
    )
    {
        uint32_t sample_index = 0;
        uint64_t timestamp_us = 0;

        if (
            !timebase_.stampNow(
                sample_index,
                timestamp_us
            )
        )
        {
            errors_.record(
                ErrorCode::TIMESTAMP_ERROR
            );

            errors_.recordDroppedSamples();

            return false;
        }

        SamplePacket packet = {};

        packet.sample_index = sample_index;
        packet.timestamp_us = timestamp_us;
        packet.x_raw = raw.x;
        packet.y_raw = raw.y;
        packet.z_raw = raw.z;
        packet.status_flags = status_flags;

        if (!buffer_.push(packet))
        {
            errors_.record(
                ErrorCode::BUFFER_OVERFLOW
            );

            errors_.recordDroppedSamples();

            return false;
        }

        return true;
    }

    bool pop(
        SamplePacket& packet
    )
    {
        return buffer_.pop(packet);
    }

    bool peek(
        SamplePacket& packet
    ) const
    {
        return buffer_.peek(packet);
    }

    uint16_t size() const
    {
        return buffer_.size();
    }

    bool empty() const
    {
        return buffer_.empty();
    }

    bool full() const
    {
        return buffer_.full();
    }

    const ErrorCounters& errors() const
    {
        return errors_.counters();
    }

    ErrorCode lastError() const
    {
        return errors_.lastError();
    }

    uint32_t nextSampleIndex() const
    {
        return timebase_.nextSampleIndex();
    }

    uint64_t lastTimestampUs() const
    {
        return timebase_.lastTimestampUs();
    }

private:
    SampleRingBuffer buffer_;
    AcquisitionTimebase timebase_;
    ErrorTracker errors_;
};