#pragma once

#include <Arduino.h>

enum class DaqState : uint8_t
{
    INIT,
    SELF_CHECK,
    READY,
    STABILIZE,
    RECORD,
    STOPPING,
    QC,
    ERROR
};

enum class AcquisitionMode : uint8_t
{
    DATA_READY,
    FIFO
};

enum class ErrorCode : uint16_t
{
    NONE = 0,
    I2C_ERROR,
    TIMEOUT,
    SENSOR_DISCONNECT,
    BUFFER_OVERFLOW,
    FIFO_OVERRUN,
    INVALID_STATE,
    INVALID_CONFIG
};

enum SampleStatus : uint16_t
{
    STATUS_OK = 0,
    STATUS_CAL_APPLIED = 1 << 0,
    STATUS_RECOVERY_SEEN = 1 << 1,
    STATUS_BUFFER_WARNING = 1 << 2,
    STATUS_SENSOR_WARNING = 1 << 3
};

struct RawSample
{
    int16_t x = 0;
    int16_t y = 0;
    int16_t z = 0;
};

struct SamplePacket
{
    uint32_t sample_index;
    uint64_t timestamp_us;
    int16_t x_raw;
    int16_t y_raw;
    int16_t z_raw;
    uint16_t status_flags;
};

struct ErrorCounters
{
    uint32_t total_errors = 0;
    uint32_t i2c_errors = 0;
    uint32_t timeouts = 0;
    uint32_t disconnects = 0;
    uint32_t buffer_overflows = 0;
    uint32_t fifo_overruns = 0;
    uint32_t dropped_samples = 0;
    uint32_t invalid_state = 0;
    uint32_t invalid_config = 0;
    uint32_t recovery_attempts = 0;
    uint32_t recovery_successes = 0;
    uint32_t recovery_failures = 0;
};