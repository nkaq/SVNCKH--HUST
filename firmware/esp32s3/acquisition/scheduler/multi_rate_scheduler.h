#pragma once

#include <Arduino.h>

#include "../events/acquisition_event_queue.h"
#include "periodic_scheduler.h"

class MultiRateScheduler
{
public:
    static constexpr uint8_t SOURCE_COUNT =
        static_cast<uint8_t>(
            AcquisitionEventSource::OLED
        ) + 1;

    bool configure(
        AcquisitionEventSource source,
        uint64_t period_us
    )
    {
        const uint8_t index =
            sourceIndex(source);

        if (index >= SOURCE_COUNT)
        {
            return false;
        }

        if (
            !schedulers_[index].configure(
                period_us
            )
        )
        {
            enabled_[index] = false;
            return false;
        }

        enabled_[index] = true;

        return true;
    }

    bool disable(
        AcquisitionEventSource source
    )
    {
        const uint8_t index =
            sourceIndex(source);

        if (index >= SOURCE_COUNT)
        {
            return false;
        }

        schedulers_[index].stop();
        enabled_[index] = false;

        return true;
    }

    bool start(
        uint64_t start_time_us
    )
    {
        bool startedAny = false;

        for (
            uint8_t i = 0;
            i < SOURCE_COUNT;
            i++
        )
        {
            if (!enabled_[i])
            {
                continue;
            }

            if (
                schedulers_[i].start(
                    start_time_us
                )
            )
            {
                startedAny = true;
            }
        }

        return startedAny;
    }

    uint8_t poll(
        uint64_t now_us,
        AcquisitionEventQueue& queue
    )
    {
        AcquisitionEvent pending[
            SOURCE_COUNT
        ] = {};

        uint8_t pendingCount = 0;

        for (
            uint8_t i = 0;
            i < SOURCE_COUNT;
            i++
        )
        {
            if (!enabled_[i])
            {
                continue;
            }

            uint64_t scheduledTime = 0;

            if (
                !schedulers_[i].poll(
                    now_us,
                    scheduledTime
                )
            )
            {
                continue;
            }

            pending[pendingCount].source =
                static_cast<
                    AcquisitionEventSource
                >(i);

            pending[pendingCount].type =
                AcquisitionEventType::SAMPLE_DUE;

            pending[pendingCount].
                scheduled_time_us =
                    scheduledTime;

            pendingCount++;
        }

        sortByScheduledTime(
            pending,
            pendingCount
        );

        uint8_t queuedCount = 0;

        for (
            uint8_t i = 0;
            i < pendingCount;
            i++
        )
        {
            if (
                queue.push(
                    pending[i]
                )
            )
            {
                queuedCount++;
            }
        }

        return queuedCount;
    }

    void stop()
    {
        for (
            uint8_t i = 0;
            i < SOURCE_COUNT;
            i++
        )
        {
            schedulers_[i].stop();
        }
    }

    void reset()
    {
        for (
            uint8_t i = 0;
            i < SOURCE_COUNT;
            i++
        )
        {
            schedulers_[i].reset();
            enabled_[i] = false;
        }
    }

    bool enabled(
        AcquisitionEventSource source
    ) const
    {
        const uint8_t index =
            sourceIndex(source);

        if (index >= SOURCE_COUNT)
        {
            return false;
        }

        return enabled_[index];
    }

    uint64_t missedIntervals(
        AcquisitionEventSource source
    ) const
    {
        const uint8_t index =
            sourceIndex(source);

        if (index >= SOURCE_COUNT)
        {
            return 0;
        }

        return schedulers_[index].
            missedIntervals();
    }

    uint64_t nextDueUs(
        AcquisitionEventSource source
    ) const
    {
        const uint8_t index =
            sourceIndex(source);

        if (index >= SOURCE_COUNT)
        {
            return 0;
        }

        return schedulers_[index].
            nextDueUs();
    }

private:
    static uint8_t sourceIndex(
        AcquisitionEventSource source
    )
    {
        return static_cast<uint8_t>(
            source
        );
    }

    static void sortByScheduledTime(
        AcquisitionEvent* events,
        uint8_t count
    )
    {
        for (
            uint8_t i = 1;
            i < count;
            i++
        )
        {
            const AcquisitionEvent key =
                events[i];

            uint8_t j = i;

            while (
                j > 0 &&
                events[j - 1].
                    scheduled_time_us >
                key.scheduled_time_us
            )
            {
                events[j] =
                    events[j - 1];

                j--;
            }

            events[j] = key;
        }
    }

    PeriodicScheduler schedulers_[
        SOURCE_COUNT
    ] = {};

    bool enabled_[
        SOURCE_COUNT
    ] = {};
};