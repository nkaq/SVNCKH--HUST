#pragma once

#include <Arduino.h>


// =====================================================
// DAQ STATE MACHINE
// =====================================================

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


// =====================================================
// ACQUISITION MODE
// =====================================================

enum class AcquisitionMode : uint8_t
{
    DATA_READY,
    FIFO
};


// =====================================================
// ERROR CODE
// =====================================================

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


// =====================================================
// SAMPLE STATUS FLAGS
// =====================================================

enum SampleStatus : uint16_t
{
    STATUS_OK = 0,

    STATUS_CAL_APPLIED =
        1 << 0,

    STATUS_RECOVERY_SEEN =
        1 << 1,

    STATUS_BUFFER_WARNING =
        1 << 2,

    STATUS_SENSOR_WARNING =
        1 << 3
};


// =====================================================
// RAW SAMPLE
//
// Dùng khi đọc FIFO.
// =====================================================

struct RawSample
{
    int16_t x = 0;

    int16_t y = 0;

    int16_t z = 0;
};


// =====================================================
// SAMPLE PACKET P0.1
// =====================================================

struct SamplePacket
{
    // Sequence number phía EE2
    uint32_t sample_index;


    // Monotonic timestamp, microsecond
    uint64_t timestamp_us;


    // Raw accelerometer data
    int16_t x_raw;

    int16_t y_raw;

    int16_t z_raw;


    // Sample quality/status bitmask
    uint16_t status_flags;
};


// =====================================================
// ERROR / RECOVERY COUNTERS
// =====================================================

struct ErrorCounters
{
    // Tổng error event
    uint32_t total_errors = 0;


    // I2C transaction error
    uint32_t i2c_errors = 0;


    // Timeout
    uint32_t timeouts = 0;


    // Sensor disconnect
    uint32_t disconnects = 0;


    // Ring buffer overflow
    uint32_t buffer_overflows = 0;


    // ADXL FIFO overrun
    uint32_t fifo_overruns = 0;


    // Sample bị drop
    uint32_t dropped_samples = 0;


    // Command/state không hợp lệ
    uint32_t invalid_state = 0;


    // Config/command value không hợp lệ
    uint32_t invalid_config = 0;


    // =============================================
    // STEP 11 RECOVERY
    // =============================================

    uint32_t recovery_attempts = 0;

    uint32_t recovery_successes = 0;

    uint32_t recovery_failures = 0;
};