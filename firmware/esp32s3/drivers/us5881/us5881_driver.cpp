
#include "us5881_driver.h"
#include <soc/soc_caps.h>
#include <driver/gpio.h>

void US5881Driver::advanceGenerationLocked()
{
    ++irq_generation_;

    // Zero is reserved for an invalid IRQ token.
    if (irq_generation_ == 0)
    {
        ++irq_generation_;
    }
}

bool US5881Driver::begin(int pin)
{
    // Validate against ESP-IDF GPIO capabilities.
    // This does not validate production PCB wiring.
    if (
        pin < 0 ||
        pin >= SOC_GPIO_PIN_COUNT
    )
    {
        return false;
    }

    // Reject GPIO holes (e.g. GPIO22-25 on ESP32-S3).
    // Silicon validity is not PCB pinmap approval.
    if (
        !GPIO_IS_VALID_GPIO(
            static_cast<gpio_num_t>(pin)
        )
    )
    {
        return false;
    }

    portENTER_CRITICAL(&mux_);

    // Rebinding requires end() first.
    if (initialized_)
    {
        portEXIT_CRITICAL(&mux_);
        return false;
    }

    portEXIT_CRITICAL(&mux_);

    // No blocking GPIO configuration inside
    // the shared-state spinlock.
    pinMode(pin, INPUT);

    portENTER_CRITICAL(&mux_);

    pin_ = pin;
    edge_stats_ = {};

    advanceGenerationLocked();

    // Published only after GPIO configuration
    // has been attempted.
    initialized_ = true;

    portEXIT_CRITICAL(&mux_);

    return true;
}

void US5881Driver::end()
{
    // Caller must detach/quiesce the GPIO ISR
    // before ending or rebinding the driver.
    portENTER_CRITICAL(&mux_);

    initialized_ = false;
    pin_ = -1;
    edge_stats_ = {};

    // Invalidate tokens from the previous binding.
    advanceGenerationLocked();

    portEXIT_CRITICAL(&mux_);
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

    return decodeLevel(level, detected);
}

bool US5881Driver::readLevel(
    uint8_t& level
) const
{
    int pin = -1;
    bool initialized = false;

    portENTER_CRITICAL(&mux_);

    pin = pin_;
    initialized = initialized_;

    portEXIT_CRITICAL(&mux_);

    if (!initialized || pin < 0)
    {
        return false;
    }

    const int raw_level = digitalRead(pin);

    if (
        raw_level != LOW &&
        raw_level != HIGH
    )
    {
        return false;
    }

    level = static_cast<uint8_t>(raw_level);

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

    // US5881 output is active-low.
    detected = (level == LOW);

    return true;
}

bool US5881Driver::recordFallingEdge(
    uint64_t timestamp_us
)
{
    portENTER_CRITICAL(&mux_);

    const bool accepted =
        recordFallingEdgeLocked(timestamp_us);

    portEXIT_CRITICAL(&mux_);

    return accepted;
}

bool IRAM_ATTR
US5881Driver::recordFallingEdgeFromISR(
    uint64_t timestamp_us,
    uint32_t irq_generation
)
{
    portENTER_CRITICAL_ISR(&mux_);

    // Reject callbacks belonging to old bindings.
    if (
        !initialized_ ||
        irq_generation == 0 ||
        irq_generation != irq_generation_
    )
    {
        portEXIT_CRITICAL_ISR(&mux_);
        return false;
    }

    const bool accepted =
        recordFallingEdgeLocked(timestamp_us);

    portEXIT_CRITICAL_ISR(&mux_);

    return accepted;
}

bool IRAM_ATTR
US5881Driver::recordFallingEdgeLocked(
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

    ++edge_stats_.edge_count;

    return true;
}

uint32_t US5881Driver::irqGeneration() const
{
    portENTER_CRITICAL(&mux_);

    const uint32_t generation =
        initialized_ ? irq_generation_ : 0;

    portEXIT_CRITICAL(&mux_);

    return generation;
}

void US5881Driver::resetEdges()
{
    portENTER_CRITICAL(&mux_);

    edge_stats_ = {};

    portEXIT_CRITICAL(&mux_);
}

US5881EdgeStats US5881Driver::edgeStats() const
{
    portENTER_CRITICAL(&mux_);

    const US5881EdgeStats stats = edge_stats_;

    portEXIT_CRITICAL(&mux_);

    return stats;
}

bool US5881Driver::isInitialized() const
{
    portENTER_CRITICAL(&mux_);

    const bool initialized = initialized_;

    portEXIT_CRITICAL(&mux_);

    return initialized;
}

int US5881Driver::pin() const
{
    portENTER_CRITICAL(&mux_);

    const int current_pin = pin_;

    portEXIT_CRITICAL(&mux_);

    return current_pin;
}
