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
    bool begin(
        int pin
    );

    void end();

    bool readDetected(
        bool& detected
    ) const;

    bool readLevel(
        uint8_t& level
    ) const;

    static bool decodeLevel(
        uint8_t level,
        bool& detected
    );

    bool recordFallingEdge(
        uint64_t timestamp_us
    );

    bool IRAM_ATTR recordFallingEdgeFromISR(
        uint64_t timestamp_us
    );

    void resetEdges();

    US5881EdgeStats edgeStats() const;

    bool isInitialized() const;

    int pin() const;

private:
    bool IRAM_ATTR recordFallingEdgeLocked(
        uint64_t timestamp_us
    );

    int pin_ = -1;
    bool initialized_ = false;

    mutable portMUX_TYPE mux_ =
        portMUX_INITIALIZER_UNLOCKED;

    US5881EdgeStats edge_stats_ = {};
};