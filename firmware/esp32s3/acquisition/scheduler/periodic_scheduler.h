#pragma once

#include <Arduino.h>

class PeriodicScheduler
{
public:
    bool configure(
        uint64_t period_us
    )
    {
        running_ = false;
        next_due_us_ = 0;
        missed_intervals_ = 0;

        if (period_us == 0)
        {
            configured_ = false;
            period_us_ = 0;
            return false;
        }

        period_us_ = period_us;
        configured_ = true;

        return true;
    }

    bool start(
        uint64_t start_time_us
    )
    {
        if (!configured_)
        {
            return false;
        }

        next_due_us_ =
            start_time_us + period_us_;

        missed_intervals_ = 0;
        running_ = true;

        return true;
    }

    bool poll(
        uint64_t now_us,
        uint64_t& scheduled_time_us
    )
    {
        if (
            !configured_ ||
            !running_ ||
            now_us < next_due_us_
        )
        {
            return false;
        }

        const uint64_t elapsed_intervals =
            (now_us - next_due_us_) /
            period_us_;

        missed_intervals_ +=
            elapsed_intervals;

        scheduled_time_us =
            next_due_us_ +
            elapsed_intervals * period_us_;

        next_due_us_ =
            scheduled_time_us + period_us_;

        return true;
    }

    void stop()
    {
        running_ = false;
    }

    void reset()
    {
        period_us_ = 0;
        next_due_us_ = 0;
        missed_intervals_ = 0;
        configured_ = false;
        running_ = false;
    }

    uint64_t periodUs() const
    {
        return period_us_;
    }

    uint64_t nextDueUs() const
    {
        return next_due_us_;
    }

    uint64_t missedIntervals() const
    {
        return missed_intervals_;
    }

    bool configured() const
    {
        return configured_;
    }

    bool running() const
    {
        return running_;
    }

private:
    uint64_t period_us_ = 0;
    uint64_t next_due_us_ = 0;
    uint64_t missed_intervals_ = 0;

    bool configured_ = false;
    bool running_ = false;
};