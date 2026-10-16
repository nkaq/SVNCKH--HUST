#pragma once

#include <Arduino.h>

namespace Config {

constexpr char FIRMWARE_VERSION[] = "DAQ_FW_v2.1";
constexpr char PROTOCOL_VERSION[] = "P0.1";

constexpr uint32_t SERIAL_BAUD = 115200;


// =====================================================
// BUFFER
// =====================================================

constexpr uint16_t RING_BUFFER_CAPACITY = 256;

// Queue chứa timestamp từ DATA_READY / FIFO watermark.
constexpr uint16_t EVENT_QUEUE_CAPACITY = 64;


// =====================================================
// ADXL345 / I2C
// =====================================================

// Hiện tại chưa có ADXL345 thật.
//
// false:
// chạy MOCK → hoàn thành software Day 1.
//
// true:
// chạy sensor thật.
// Khi đó phải điền SDA/SCL/INT pin.
constexpr bool ADXL_HARDWARE_PRESENT = false;


// Chưa có pin map hardware thật.
constexpr int ADXL_SDA_PIN = -1;
constexpr int ADXL_SCL_PIN = -1;
constexpr int ADXL_INT_PIN = -1;


// I2C
constexpr uint32_t I2C_CLOCK_HZ = 400000;

constexpr uint32_t I2C_TIMEOUT_MS = 20;


// =====================================================
// ADXL345 ACQUISITION CONFIG
// =====================================================

// Bring-up / Engineering Mock configuration.
//
// CHƯA phải ODR cuối của experiment motor.
constexpr uint16_t ADXL_ODR_HZ = 100;

// ADXL345 register code:
// 0x0A = 100 Hz
constexpr uint8_t ADXL_ODR_CODE = 0x0A;


// Range bring-up
constexpr uint8_t ADXL_RANGE_G = 2;


// FIFO watermark
constexpr uint16_t FIFO_WATERMARK = 16;


// ADXL345 FIFO tối đa 32 samples
constexpr uint16_t ADXL_FIFO_MAX_SAMPLES = 32;


// =====================================================
// STATE MACHINE
// =====================================================

// Stabilization tạm cho mock/software test.
//
// Khi hardware + fixture thật có thể phải chỉnh.
constexpr uint32_t STABILIZE_TIME_MS = 100;


// Nếu START không truyền thời gian:
constexpr uint32_t DEFAULT_RECORD_SECONDS = 5;


// =====================================================
// STEP 11 - ERROR / RECOVERY
// =====================================================

// Bao nhiêu read failure liên tiếp thì thử recover sensor.
constexpr uint8_t
    RECOVERY_TRIGGER_CONSECUTIVE_READ_FAILURES = 3;


// Recovery fail liên tiếp quá mức này
// → chuyển ERROR.
constexpr uint8_t
    MAX_RECOVERY_FAILURES_BEFORE_ERROR = 2;


// =====================================================
// STEP 12 - STRESS TEST
// =====================================================

// Mặc định mỗi run 60 giây
constexpr uint32_t DEFAULT_STRESS_SECONDS = 60;


// Mặc định chạy 3 run
constexpr uint16_t DEFAULT_STRESS_RUNS = 3;


// Khoảng nghỉ giữa hai stress run
constexpr uint32_t STRESS_GAP_MS = 500;

} // namespace Config