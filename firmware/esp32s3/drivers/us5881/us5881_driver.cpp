#include "us5881_driver.h"

bool US5881Driver::begin(
    int pin
)
{
    if (pin < 0)
    {
        return false;
    }

    portENTER_CRITICAL(
        &mux_
    );

    pin_ = pin;
    initialized_ = true;
    edge_stats_ = {};

    portEXIT_CRITICAL(
        &mux_
    );

    pinMode(
        pin,
        INPUT
    );

    return true;
}

void US5881Driver::end()
{
    portENTER_CRITICAL(
        &mux_
    );

    initialized_ = false;
    pin_ = -1;
    edge_stats_ = {};

    portEXIT_CRITICAL(
        &mux_
    );
}

bool US5881Driver::readDetected(
    bool& detected
) const
{
    uint8_t level = 0;

    if (!readLevel(level))
    {
        return false;
    }

    return decodeLevel(
        level,
        detected
    );
}

bool US5881Driver::readLevel(
    uint8_t& level
) const
{
    int pin = -1;
    bool initialized = false;

    portENTER_CRITICAL(
        &mux_
    );

    pin = pin_;
    initialized = initialized_;

    portEXIT_CRITICAL(
        &mux_
    );

    if (
        !initialized ||
        pin < 0
    )
    {
        return false;
    }

    const int raw_level =
        digitalRead(
            pin
        );

    if (
        raw_level != LOW &&
        raw_level != HIGH
    )
    {
        return false;
    }

    level =
        static_cast<uint8_t>(
            raw_level
        );

    return true;
}

bool US5881Driver::decodeLevel(
    uint8_t level,
    bool& detected
)
{
    if (
        level != LOW &&
        level != HIGH
    )
    {
        return false;
    }

    detected =
        level == LOW;

    return true;
}

bool US5881Driver::recordFallingEdge(
    uint64_t timestamp_us
)
{
    bool accepted = false;

    portENTER_CRITICAL(
        &mux_
    );

    accepted =
        recordFallingEdgeLocked(
            timestamp_us
        );

    portEXIT_CRITICAL(
        &mux_
    );

    return accepted;
}

bool IRAM_ATTR US5881Driver::recordFallingEdgeFromISR(
    uint64_t timestamp_us
)
{
    bool accepted = false;

    portENTER_CRITICAL_ISR(
        &mux_
    );

    accepted =
        recordFallingEdgeLocked(
            timestamp_us
        );

    portEXIT_CRITICAL_ISR(
        &mux_
    );

    return accepted;
}

bool IRAM_ATTR US5881Driver::recordFallingEdgeLocked(
    uint64_t timestamp_us
)
{
    if (!initialized_)
    {
        return false;
    }

    if (timestamp_us == 0)
    {
        edge_stats_.invalid_timestamp_count++;

        return false;
    }

    if (
        edge_stats_.edge_count > 0 &&
        timestamp_us <=
            edge_stats_.last_edge_timestamp_us
    )
    {
        edge_stats_.invalid_timestamp_count++;

        return false;
    }

    if (edge_stats_.edge_count > 0)
    {
        edge_stats_.last_period_us =
            timestamp_us -
            edge_stats_.last_edge_timestamp_us;

        edge_stats_.period_valid = true;
    }
    else
    {
        edge_stats_.last_period_us = 0;
        edge_stats_.period_valid = false;
    }

    edge_stats_.last_edge_timestamp_us =
        timestamp_us;

    edge_stats_.edge_count++;

    return true;
}

void US5881Driver::resetEdges()
{
    portENTER_CRITICAL(
        &mux_
    );

    edge_stats_ = {};

    portEXIT_CRITICAL(
        &mux_
    );
}

US5881EdgeStats US5881Driver::edgeStats() const
{
    US5881EdgeStats stats = {};

    portENTER_CRITICAL(
        &mux_
    );

    stats = edge_stats_;

    portEXIT_CRITICAL(
        &mux_
    );

    return stats;
}

bool US5881Driver::isInitialized() const
{
    bool initialized = false;

    portENTER_CRITICAL(
        &mux_
    );

    initialized = initialized_;

    portEXIT_CRITICAL(
        &mux_
    );

    return initialized;
}

int US5881Driver::pin() const
{
    int pin = -1;

    portENTER_CRITICAL(
        &mux_
    );

    pin = pin_;

    portEXIT_CRITICAL(
        &mux_
    );

    return pin;
}