#pragma once

#include <Arduino.h>

namespace Config
{
constexpr char FIRMWARE_VERSION[] = "DAQ_FW_v2.1";
constexpr char PROTOCOL_VERSION[] = "P0.1";

constexpr uint32_t SERIAL_BAUD = 115200;

constexpr uint16_t RING_BUFFER_CAPACITY = 256;
constexpr uint16_t EVENT_QUEUE_CAPACITY = 64;

constexpr bool ADXL_HARDWARE_PRESENT = false;

constexpr int ADXL_SDA_PIN = -1;
constexpr int ADXL_SCL_PIN = -1;
constexpr int ADXL_INT_PIN = -1;

constexpr uint32_t I2C_CLOCK_HZ = 400000;
constexpr uint32_t I2C_TIMEOUT_MS = 20;

constexpr uint16_t ADXL_ODR_HZ = 100;
constexpr uint8_t ADXL_ODR_CODE = 0x0A;
constexpr uint8_t ADXL_RANGE_G = 2;

constexpr uint16_t FIFO_WATERMARK = 16;
constexpr uint16_t ADXL_FIFO_MAX_SAMPLES = 32;

constexpr uint32_t STABILIZE_TIME_MS = 100;
constexpr uint32_t DEFAULT_RECORD_SECONDS = 5;

constexpr uint8_t RECOVERY_TRIGGER_CONSECUTIVE_READ_FAILURES = 3;
constexpr uint8_t MAX_RECOVERY_FAILURES_BEFORE_ERROR = 2;

constexpr uint32_t DEFAULT_STRESS_SECONDS = 60;
constexpr uint16_t DEFAULT_STRESS_RUNS = 3;
constexpr uint32_t STRESS_GAP_MS = 500;
}