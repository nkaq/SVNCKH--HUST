#pragma once

#include <Arduino.h>
#include <esp_timer.h>

#include "../common/daq_types.h"
#include "../events/acquisition_event_queue.h"
#include "../pipeline/adxl_sample_pipeline.h"
#include "../scheduler/multi_rate_scheduler.h"

enum class AdxlTriggerMode : uint8_t
{
    NONE = 0,
    PERIODIC,
    DATA_READY
};

struct AcquisitionManagerStats
{
    uint32_t events_seen = 0;
    uint32_t events_handled = 0;
    uint32_t dropped_events = 0;

    uint32_t unsupported_source_events = 0;
    uint32_t unsupported_type_events = 0;
    uint32_t invalid_mode_events = 0;
    uint32_t rejected_while_stopped = 0;

    uint32_t adxl_read_failures = 0;
    uint32_t pipeline_submit_failures = 0;

    uint32_t invalid_data_ready_timestamps = 0;
    uint32_t data_ready_clock_errors = 0;
    uint32_t stale_data_ready_events = 0;
    uint32_t data_ready_config_errors = 0;

    uint32_t scheduled_clock_errors = 0;
    uint32_t scheduled_events_observed = 0;

    uint64_t scheduled_lateness_total_us = 0;
    uint64_t scheduled_lateness_max_us = 0;

    uint32_t data_ready_age_observations = 0;
    uint64_t data_ready_age_total_us = 0;
    uint64_t data_ready_age_max_us = 0;
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

        portENTER_CRITICAL(
            &mux_
        );

        running_ = false;

        adxl_mode_ =
            AdxlTriggerMode::NONE;

        adxl_data_ready_max_age_us_ = 0;
        latest_adxl_data_ready_timestamp_us_ = 0;

        stats_ = {};

        event_queue_.clear();
        event_queue_.resetStats();

        adxl_pipeline_.reset();

        portEXIT_CRITICAL(
            &mux_
        );
    }

    bool configureAdxlPeriodic(
        uint64_t period_us
    )
    {
        if (period_us == 0)
        {
            return false;
        }

        portENTER_CRITICAL(
            &mux_
        );

        if (running_)
        {
            portEXIT_CRITICAL(
                &mux_
            );

            return false;
        }

        portEXIT_CRITICAL(
            &mux_
        );

        if (
            !scheduler_.configure(
                AcquisitionEventSource::ADXL345,
                period_us
            )
        )
        {
            return false;
        }

        portENTER_CRITICAL(
            &mux_
        );

        adxl_mode_ =
            AdxlTriggerMode::PERIODIC;

        adxl_data_ready_max_age_us_ = 0;
        latest_adxl_data_ready_timestamp_us_ = 0;

        portEXIT_CRITICAL(
            &mux_
        );

        return true;
    }

    bool configureAdxlDataReady(
        uint64_t max_event_age_us
    )
    {
        if (max_event_age_us == 0)
        {
            return false;
        }

        portENTER_CRITICAL(
            &mux_
        );

        if (running_)
        {
            portEXIT_CRITICAL(
                &mux_
            );

            return false;
        }

        portEXIT_CRITICAL(
            &mux_
        );

        scheduler_.disable(
            AcquisitionEventSource::ADXL345
        );

        portENTER_CRITICAL(
            &mux_
        );

        adxl_mode_ =
            AdxlTriggerMode::DATA_READY;

        adxl_data_ready_max_age_us_ =
            max_event_age_us;

        latest_adxl_data_ready_timestamp_us_ = 0;

        portEXIT_CRITICAL(
            &mux_
        );

        return true;
    }

    bool disableAdxl()
    {
        portENTER_CRITICAL(
            &mux_
        );

        if (running_)
        {
            portEXIT_CRITICAL(
                &mux_
            );

            return false;
        }

        portEXIT_CRITICAL(
            &mux_
        );

        scheduler_.disable(
            AcquisitionEventSource::ADXL345
        );

        portENTER_CRITICAL(
            &mux_
        );

        adxl_mode_ =
            AdxlTriggerMode::NONE;

        adxl_data_ready_max_age_us_ = 0;
        latest_adxl_data_ready_timestamp_us_ = 0;

        portEXIT_CRITICAL(
            &mux_
        );

        return true;
    }

    bool startAcquisition(
        uint64_t start_time_us
    )
    {
        AdxlTriggerMode mode =
            AdxlTriggerMode::NONE;

        portENTER_CRITICAL(
            &mux_
        );

        if (
            running_ ||
            adxl_mode_ ==
                AdxlTriggerMode::NONE
        )
        {
            portEXIT_CRITICAL(
                &mux_
            );

            return false;
        }

        event_queue_.clear();

        latest_adxl_data_ready_timestamp_us_ = 0;

        running_ = true;
        mode = adxl_mode_;

        portEXIT_CRITICAL(
            &mux_
        );

        if (
            mode ==
            AdxlTriggerMode::PERIODIC
        )
        {
            if (
                !scheduler_.start(
                    start_time_us
                )
            )
            {
                portENTER_CRITICAL(
                    &mux_
                );

                running_ = false;
                event_queue_.clear();

                portEXIT_CRITICAL(
                    &mux_
                );

                return false;
            }
        }

        return true;
    }

    void stopScheduler()
    {
        scheduler_.stop();
    }

    void stopAcquisition()
    {
        portENTER_CRITICAL(
            &mux_
        );

        running_ = false;
        latest_adxl_data_ready_timestamp_us_ = 0;

        event_queue_.clear();

        portEXIT_CRITICAL(
            &mux_
        );

        scheduler_.stop();
    }

    bool running() const
    {
        portENTER_CRITICAL(
            &mux_
        );

        const bool value =
            running_;

        portEXIT_CRITICAL(
            &mux_
        );

        return value;
    }

    AdxlTriggerMode adxlMode() const
    {
        portENTER_CRITICAL(
            &mux_
        );

        const AdxlTriggerMode value =
            adxl_mode_;

        portEXIT_CRITICAL(
            &mux_
        );

        return value;
    }

    uint8_t pollScheduler(
        uint64_t now_us
    )
    {
        portENTER_CRITICAL(
            &mux_
        );

        if (
            !running_ ||
            adxl_mode_ !=
                AdxlTriggerMode::PERIODIC
        )
        {
            portEXIT_CRITICAL(
                &mux_
            );

            return 0;
        }

        const uint8_t queued =
            scheduler_.poll(
                now_us,
                event_queue_
            );

        portEXIT_CRITICAL(
            &mux_
        );

        return queued;
    }

    bool enqueue(
        const AcquisitionEvent& event
    )
    {
        if (
            event.source ==
                AcquisitionEventSource::ADXL345 &&
            event.type ==
                AcquisitionEventType::DATA_READY
        )
        {
            return enqueueAdxlDataReady(
                event.scheduled_time_us
            );
        }

        portENTER_CRITICAL(
            &mux_
        );

        if (!running_)
        {
            stats_.rejected_while_stopped++;
            stats_.dropped_events++;

            portEXIT_CRITICAL(
                &mux_
            );

            return false;
        }

        const bool accepted =
            event_queue_.push(
                event
            );

        if (!accepted)
        {
            stats_.dropped_events++;
        }

        portEXIT_CRITICAL(
            &mux_
        );

        return accepted;
    }

    bool enqueueFromISR(
        const AcquisitionEvent& event
    )
    {
        if (
            event.source ==
                AcquisitionEventSource::ADXL345 &&
            event.type ==
                AcquisitionEventType::DATA_READY
        )
        {
            return enqueueAdxlDataReadyFromISR(
                event.scheduled_time_us
            );
        }

        portENTER_CRITICAL_ISR(
            &mux_
        );

        if (!running_)
        {
            stats_.rejected_while_stopped++;
            stats_.dropped_events++;

            portEXIT_CRITICAL_ISR(
                &mux_
            );

            return false;
        }

        const bool accepted =
            event_queue_.pushFromISR(
                event
            );

        if (!accepted)
        {
            stats_.dropped_events++;
        }

        portEXIT_CRITICAL_ISR(
            &mux_
        );

        return accepted;
    }

    bool enqueueAdxlDataReady(
        uint64_t timestamp_us
    )
    {
        portENTER_CRITICAL(
            &mux_
        );

        if (!running_)
        {
            stats_.rejected_while_stopped++;
            stats_.dropped_events++;

            portEXIT_CRITICAL(
                &mux_
            );

            return false;
        }

        if (
            adxl_mode_ !=
            AdxlTriggerMode::DATA_READY
        )
        {
            stats_.invalid_mode_events++;
            stats_.dropped_events++;

            portEXIT_CRITICAL(
                &mux_
            );

            return false;
        }

        if (
            timestamp_us == 0 ||
            timestamp_us <=
                latest_adxl_data_ready_timestamp_us_
        )
        {
            stats_.invalid_data_ready_timestamps++;
            stats_.dropped_events++;

            portEXIT_CRITICAL(
                &mux_
            );

            return false;
        }

        latest_adxl_data_ready_timestamp_us_ =
            timestamp_us;

        AcquisitionEvent event = {};

        event.source =
            AcquisitionEventSource::ADXL345;

        event.type =
            AcquisitionEventType::DATA_READY;

        event.scheduled_time_us =
            timestamp_us;

        const bool accepted =
            event_queue_.push(
                event
            );

        if (!accepted)
        {
            stats_.dropped_events++;
        }

        portEXIT_CRITICAL(
            &mux_
        );

        return accepted;
    }

    bool enqueueAdxlDataReadyFromISR(
        uint64_t timestamp_us
    )
    {
        portENTER_CRITICAL_ISR(
            &mux_
        );

        if (!running_)
        {
            stats_.rejected_while_stopped++;
            stats_.dropped_events++;

            portEXIT_CRITICAL_ISR(
                &mux_
            );

            return false;
        }

        if (
            adxl_mode_ !=
            AdxlTriggerMode::DATA_READY
        )
        {
            stats_.invalid_mode_events++;
            stats_.dropped_events++;

            portEXIT_CRITICAL_ISR(
                &mux_
            );

            return false;
        }

        if (
            timestamp_us == 0 ||
            timestamp_us <=
                latest_adxl_data_ready_timestamp_us_
        )
        {
            stats_.invalid_data_ready_timestamps++;
            stats_.dropped_events++;

            portEXIT_CRITICAL_ISR(
                &mux_
            );

            return false;
        }

        latest_adxl_data_ready_timestamp_us_ =
            timestamp_us;

        AcquisitionEvent event = {};

        event.source =
            AcquisitionEventSource::ADXL345;

        event.type =
            AcquisitionEventType::DATA_READY;

        event.scheduled_time_us =
            timestamp_us;

        const bool accepted =
            event_queue_.pushFromISR(
                event
            );

        if (!accepted)
        {
            stats_.dropped_events++;
        }

        portEXIT_CRITICAL_ISR(
            &mux_
        );

        return accepted;
    }

    uint16_t processAvailable(
        uint16_t max_events
    )
    {
        uint16_t processed = 0;

        while (
            processed <
            max_events
        )
        {
            AcquisitionEvent event = {};

            portENTER_CRITICAL(
                &mux_
            );

            if (!running_)
            {
                portEXIT_CRITICAL(
                    &mux_
                );

                break;
            }

            const bool available =
                event_queue_.pop(
                    event
                );

            portEXIT_CRITICAL(
                &mux_
            );

            if (!available)
            {
                break;
            }

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

    uint64_t adxlDataReadyMaxAgeUs() const
    {
        portENTER_CRITICAL(
            &mux_
        );

        const uint64_t value =
            adxl_data_ready_max_age_us_;

        portEXIT_CRITICAL(
            &mux_
        );

        return value;
    }

    AcquisitionManagerStats stats() const
    {
        portENTER_CRITICAL(
            &mux_
        );

        const AcquisitionManagerStats value =
            stats_;

        portEXIT_CRITICAL(
            &mux_
        );

        return value;
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
        portENTER_CRITICAL(
            &mux_
        );

        stats_.events_seen++;

        if (!running_)
        {
            stats_.rejected_while_stopped++;
            stats_.dropped_events++;

            portEXIT_CRITICAL(
                &mux_
            );

            return false;
        }

        portEXIT_CRITICAL(
            &mux_
        );

        if (
            event.source !=
            AcquisitionEventSource::ADXL345
        )
        {
            portENTER_CRITICAL(
                &mux_
            );

            stats_.unsupported_source_events++;
            stats_.dropped_events++;

            portEXIT_CRITICAL(
                &mux_
            );

            return false;
        }

        bool handled = false;

        switch (event.type)
        {
            case AcquisitionEventType::SAMPLE_DUE:
                handled =
                    handleAdxlScheduledSample(
                        event.scheduled_time_us
                    );
                break;

            case AcquisitionEventType::DATA_READY:
                handled =
                    handleAdxlDataReady(
                        event.scheduled_time_us
                    );
                break;

            case AcquisitionEventType::FIFO_WATERMARK:
                portENTER_CRITICAL(
                    &mux_
                );

                stats_.unsupported_type_events++;
                stats_.dropped_events++;

                portEXIT_CRITICAL(
                    &mux_
                );

                return false;
        }

        if (handled)
        {
            portENTER_CRITICAL(
                &mux_
            );

            stats_.events_handled++;

            portEXIT_CRITICAL(
                &mux_
            );
        }

        return handled;
    }

    bool handleAdxlScheduledSample(
        uint64_t scheduled_time_us
    )
    {
        portENTER_CRITICAL(
            &mux_
        );

        if (
            !running_ ||
            adxl_mode_ !=
                AdxlTriggerMode::PERIODIC
        )
        {
            stats_.invalid_mode_events++;
            stats_.dropped_events++;

            portEXIT_CRITICAL(
                &mux_
            );

            return false;
        }

        portEXIT_CRITICAL(
            &mux_
        );

        const uint64_t now_us =
            static_cast<uint64_t>(
                esp_timer_get_time()
            );

        if (
            scheduled_time_us == 0 ||
            scheduled_time_us >
                now_us
        )
        {
            portENTER_CRITICAL(
                &mux_
            );

            stats_.scheduled_clock_errors++;
            stats_.dropped_events++;

            portEXIT_CRITICAL(
                &mux_
            );

            return false;
        }

        const uint64_t lateness_us =
            now_us -
            scheduled_time_us;

        portENTER_CRITICAL(
            &mux_
        );

        stats_.scheduled_events_observed++;

        stats_.scheduled_lateness_total_us +=
            lateness_us;

        if (
            lateness_us >
            stats_.scheduled_lateness_max_us
        )
        {
            stats_.scheduled_lateness_max_us =
                lateness_us;
        }

        portEXIT_CRITICAL(
            &mux_
        );

        RawSample raw = {};

        if (
            !adxl_driver_.readRaw(
                raw.x,
                raw.y,
                raw.z
            )
        )
        {
            portENTER_CRITICAL(
                &mux_
            );

            stats_.adxl_read_failures++;

            portEXIT_CRITICAL(
                &mux_
            );

            return false;
        }

        portENTER_CRITICAL(
            &mux_
        );

        if (
            !running_ ||
            adxl_mode_ !=
                AdxlTriggerMode::PERIODIC
        )
        {
            stats_.rejected_while_stopped++;
            stats_.dropped_events++;

            portEXIT_CRITICAL(
                &mux_
            );

            return false;
        }

        const bool submitted =
            adxl_pipeline_.submitNow(
                raw
            );

        if (!submitted)
        {
            stats_.pipeline_submit_failures++;
        }

        portEXIT_CRITICAL(
            &mux_
        );

        return submitted;
    }

    bool handleAdxlDataReady(
        uint64_t timestamp_us
    )
    {
        uint64_t max_age_us = 0;
        uint64_t latest_timestamp_us = 0;

        portENTER_CRITICAL(
            &mux_
        );

        if (
            !running_ ||
            adxl_mode_ !=
                AdxlTriggerMode::DATA_READY
        )
        {
            stats_.invalid_mode_events++;
            stats_.dropped_events++;

            portEXIT_CRITICAL(
                &mux_
            );

            return false;
        }

        max_age_us =
            adxl_data_ready_max_age_us_;

        latest_timestamp_us =
            latest_adxl_data_ready_timestamp_us_;

        portEXIT_CRITICAL(
            &mux_
        );

        if (timestamp_us == 0)
        {
            portENTER_CRITICAL(
                &mux_
            );

            stats_.invalid_data_ready_timestamps++;
            stats_.dropped_events++;

            portEXIT_CRITICAL(
                &mux_
            );

            return false;
        }

        if (max_age_us == 0)
        {
            portENTER_CRITICAL(
                &mux_
            );

            stats_.data_ready_config_errors++;
            stats_.dropped_events++;

            portEXIT_CRITICAL(
                &mux_
            );

            return false;
        }

        const uint64_t now_before_read_us =
            static_cast<uint64_t>(
                esp_timer_get_time()
            );

        if (
            timestamp_us >
            now_before_read_us
        )
        {
            portENTER_CRITICAL(
                &mux_
            );

            stats_.data_ready_clock_errors++;
            stats_.dropped_events++;

            portEXIT_CRITICAL(
                &mux_
            );

            return false;
        }

        const uint64_t age_before_read_us =
            now_before_read_us -
            timestamp_us;

        portENTER_CRITICAL(
            &mux_
        );

        stats_.data_ready_age_observations++;

        stats_.data_ready_age_total_us +=
            age_before_read_us;

        if (
            age_before_read_us >
            stats_.data_ready_age_max_us
        )
        {
            stats_.data_ready_age_max_us =
                age_before_read_us;
        }

        portEXIT_CRITICAL(
            &mux_
        );

        if (
            timestamp_us !=
                latest_timestamp_us ||
            age_before_read_us >
                max_age_us
        )
        {
            portENTER_CRITICAL(
                &mux_
            );

            stats_.stale_data_ready_events++;
            stats_.dropped_events++;

            portEXIT_CRITICAL(
                &mux_
            );

            return false;
        }

        RawSample raw = {};

        if (
            !adxl_driver_.readRaw(
                raw.x,
                raw.y,
                raw.z
            )
        )
        {
            portENTER_CRITICAL(
                &mux_
            );

            stats_.adxl_read_failures++;

            portEXIT_CRITICAL(
                &mux_
            );

            return false;
        }

        const uint64_t now_after_read_us =
            static_cast<uint64_t>(
                esp_timer_get_time()
            );

        portENTER_CRITICAL(
            &mux_
        );

        if (
            !running_ ||
            adxl_mode_ !=
                AdxlTriggerMode::DATA_READY
        )
        {
            stats_.rejected_while_stopped++;
            stats_.dropped_events++;

            portEXIT_CRITICAL(
                &mux_
            );

            return false;
        }

        if (
            timestamp_us !=
            latest_adxl_data_ready_timestamp_us_
        )
        {
            stats_.stale_data_ready_events++;
            stats_.dropped_events++;

            portEXIT_CRITICAL(
                &mux_
            );

            return false;
        }

        if (
            now_after_read_us <
            timestamp_us
        )
        {
            stats_.data_ready_clock_errors++;
            stats_.dropped_events++;

            portEXIT_CRITICAL(
                &mux_
            );

            return false;
        }

        if (
            now_after_read_us -
                timestamp_us >
            adxl_data_ready_max_age_us_
        )
        {
            stats_.stale_data_ready_events++;
            stats_.dropped_events++;

            portEXIT_CRITICAL(
                &mux_
            );

            return false;
        }

        const bool submitted =
            adxl_pipeline_.submit(
                raw,
                timestamp_us
            );

        if (!submitted)
        {
            stats_.pipeline_submit_failures++;
        }

        portEXIT_CRITICAL(
            &mux_
        );

        return submitted;
    }

    AdxlDriverT& adxl_driver_;

    MultiRateScheduler scheduler_;
    AcquisitionEventQueue event_queue_;
    AdxlSamplePipeline adxl_pipeline_;

    bool running_ = false;

    AdxlTriggerMode adxl_mode_ =
        AdxlTriggerMode::NONE;

    uint64_t adxl_data_ready_max_age_us_ = 0;
    uint64_t latest_adxl_data_ready_timestamp_us_ = 0;

    AcquisitionManagerStats stats_ = {};

    mutable portMUX_TYPE mux_ =
        portMUX_INITIALIZER_UNLOCKED;
};