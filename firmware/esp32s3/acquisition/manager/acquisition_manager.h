#pragma once

#include <Arduino.h>

#include "../common/daq_types.h"
#include "../events/acquisition_event_queue.h"
#include "../pipeline/adxl_sample_pipeline.h"
#include "../scheduler/multi_rate_scheduler.h"

struct AcquisitionManagerStats
{
    uint32_t events_seen = 0;
    uint32_t events_handled = 0;
    uint32_t unsupported_source_events = 0;
    uint32_t unsupported_type_events = 0;
    uint32_t adxl_read_failures = 0;
    uint32_t pipeline_submit_failures = 0;
};

template <typename AdxlDriverT>
class AcquisitionManager
{
public:
    explicit AcquisitionManager(
        AdxlDriverT& adxl_driver
    )
        : adxl_driver_(
              adxl_driver
          )
    {
    }

    void reset()
    {
        scheduler_.reset();

        event_queue_.clear();
        event_queue_.resetStats();

        adxl_pipeline_.reset();

        stats_ = {};
    }

    bool configureAdxlPeriodic(
        uint64_t period_us
    )
    {
        return scheduler_.configure(
            AcquisitionEventSource::ADXL345,
            period_us
        );
    }

    bool disableAdxlPeriodic()
    {
        return scheduler_.disable(
            AcquisitionEventSource::ADXL345
        );
    }

    bool start(
        uint64_t start_time_us
    )
    {
        return scheduler_.start(
            start_time_us
        );
    }

    void stop()
    {
        scheduler_.stop();
    }

    uint8_t pollScheduler(
        uint64_t now_us
    )
    {
        return scheduler_.poll(
            now_us,
            event_queue_
        );
    }

    bool enqueue(
        const AcquisitionEvent& event
    )
    {
        return event_queue_.push(
            event
        );
    }

    bool enqueueFromISR(
        const AcquisitionEvent& event
    )
    {
        return event_queue_.pushFromISR(
            event
        );
    }

    bool enqueueAdxlDataReady(
        uint64_t timestamp_us
    )
    {
        AcquisitionEvent event = {};

        event.source =
            AcquisitionEventSource::ADXL345;

        event.type =
            AcquisitionEventType::DATA_READY;

        event.scheduled_time_us =
            timestamp_us;

        return enqueue(
            event
        );
    }

    bool enqueueAdxlDataReadyFromISR(
        uint64_t timestamp_us
    )
    {
        AcquisitionEvent event = {};

        event.source =
            AcquisitionEventSource::ADXL345;

        event.type =
            AcquisitionEventType::DATA_READY;

        event.scheduled_time_us =
            timestamp_us;

        return enqueueFromISR(
            event
        );
    }

    uint16_t processAvailable(
        uint16_t max_events
    )
    {
        uint16_t processed = 0;

        AcquisitionEvent event = {};

        while (
            processed < max_events &&
            event_queue_.pop(event)
        )
        {
            handleEvent(
                event
            );

            processed++;
        }

        return processed;
    }

    bool popSample(
        SamplePacket& packet
    )
    {
        return adxl_pipeline_.pop(
            packet
        );
    }

    bool peekSample(
        SamplePacket& packet
    ) const
    {
        return adxl_pipeline_.peek(
            packet
        );
    }

    uint16_t sampleCount() const
    {
        return adxl_pipeline_.size();
    }

    bool samplesEmpty() const
    {
        return adxl_pipeline_.empty();
    }

    uint16_t pendingEventCount()
    {
        return event_queue_.size();
    }

    uint32_t eventQueueOverflowCount()
    {
        return event_queue_.overflowCount();
    }

    uint64_t adxlMissedIntervals() const
    {
        return scheduler_.missedIntervals(
            AcquisitionEventSource::ADXL345
        );
    }

    const AcquisitionManagerStats& stats() const
    {
        return stats_;
    }

    const ErrorCounters& pipelineErrors() const
    {
        return adxl_pipeline_.errors();
    }

    ErrorCode pipelineLastError() const
    {
        return adxl_pipeline_.lastError();
    }

private:
    bool handleEvent(
        const AcquisitionEvent& event
    )
    {
        stats_.events_seen++;

        if (
            event.source !=
            AcquisitionEventSource::ADXL345
        )
        {
            stats_.unsupported_source_events++;

            return false;
        }

        bool handled = false;

        switch (event.type)
        {
            case AcquisitionEventType::SAMPLE_DUE:
                handled =
                    handleAdxlScheduledSample();
                break;

            case AcquisitionEventType::DATA_READY:
                handled =
                    handleAdxlDataReady(
                        event.scheduled_time_us
                    );
                break;

            case AcquisitionEventType::FIFO_WATERMARK:
                stats_.unsupported_type_events++;
                return false;
        }

        if (handled)
        {
            stats_.events_handled++;
        }

        return handled;
    }

    bool handleAdxlScheduledSample()
    {
        RawSample raw = {};

        if (
            !adxl_driver_.readRaw(
                raw.x,
                raw.y,
                raw.z
            )
        )
        {
            stats_.adxl_read_failures++;

            return false;
        }

        if (
            !adxl_pipeline_.submitNow(
                raw
            )
        )
        {
            stats_.pipeline_submit_failures++;

            return false;
        }

        return true;
    }

    bool handleAdxlDataReady(
        uint64_t timestamp_us
    )
    {
        RawSample raw = {};

        if (
            !adxl_driver_.readRaw(
                raw.x,
                raw.y,
                raw.z
            )
        )
        {
            stats_.adxl_read_failures++;

            return false;
        }

        bool submitted = false;

        if (timestamp_us == 0)
        {
            submitted =
                adxl_pipeline_.submitNow(
                    raw
                );
        }
        else
        {
            submitted =
                adxl_pipeline_.submit(
                    raw,
                    timestamp_us
                );
        }

        if (!submitted)
        {
            stats_.pipeline_submit_failures++;

            return false;
        }

        return true;
    }

    AdxlDriverT& adxl_driver_;

    MultiRateScheduler scheduler_;
    AcquisitionEventQueue event_queue_;
    AdxlSamplePipeline adxl_pipeline_;

    AcquisitionManagerStats stats_ = {};
};