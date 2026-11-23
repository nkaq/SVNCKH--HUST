
#pragma once

#include <Arduino.h>
#include <freertos/FreeRTOS.h>

struct US5881EdgeStats
{
    uint64_t edge_count = 0;
    uint64_t last_edge_timestamp_us = 0;
    uint64_t last_period_us = 0;
    uint32_t invalid_timestamp_count = 0;
    bool period_valid = false;
};

class US5881Driver
{
public:
    // GPIO configuration only.
    // Success does not prove a physical US5881 is present.
    //
    // Caller must serialize begin/end and quiesce
    // GPIO interrupts before reconfiguration.
    bool begin(int pin);

    void end();

    bool readDetected(bool& detected) const;

    bool readLevel(uint8_t& level) const;

    static bool decodeLevel(
        uint8_t level,
        bool& detected
    );

    // Task-side edge recorder.
    // Caller is responsible for validating edge origin.
    bool recordFallingEdge(
        uint64_t timestamp_us
    );

    // Token must be captured at GPIO IRQ attachment.
    // Old IRQ callbacks must not use a fresh token.
    bool IRAM_ATTR recordFallingEdgeFromISR(
        uint64_t timestamp_us,
        uint32_t irq_generation
    );

    // Obtain a token after begin() succeeds.
    // Returns 0 if driver is not initialized.
    uint32_t irqGeneration() const;

    void resetEdges();

    US5881EdgeStats edgeStats() const;

    bool isInitialized() const;

    int pin() const;

private:
    bool IRAM_ATTR recordFallingEdgeLocked(
        uint64_t timestamp_us
    );

    void advanceGenerationLocked();

    int pin_ = -1;
    bool initialized_ = false;

    // Incremented at lifecycle transitions.
    // Access must be protected by mux_.
    uint32_t irq_generation_ = 0;

    mutable portMUX_TYPE mux_ =
        portMUX_INITIALIZER_UNLOCKED;

    US5881EdgeStats edge_stats_ = {};
};
