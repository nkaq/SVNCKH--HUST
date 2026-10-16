#pragma once

#include <Arduino.h>

#include "daq_types.h"


// =====================================================
// EE2 -> IT2 DATA CONTRACT v1.0
//
// Quy tắc:
// - Metadata và footer bắt đầu bằng '#'
// - Sample data là CSV thuần
// - Không đổi thứ tự sample columns nếu chưa bump version
// - Raw ADXL345 luôn được giữ lại
// =====================================================

namespace DataContract
{

// =====================================================
// VERSION
// =====================================================

constexpr char CONTRACT_VERSION[] =
    "EE2_IT2_DC_v1.0";


// =====================================================
// FIXED SAMPLE SCHEMA
// =====================================================

constexpr char SAMPLE_FORMAT[] =
    "CSV";

constexpr char SAMPLE_COLUMNS[] =
    "sample_index,timestamp_us,x_raw,y_raw,z_raw,status_flags";


// =====================================================
// DATA SOURCE
// =====================================================

constexpr char SOURCE_MOCK[] =
    "MOCK";

constexpr char SOURCE_HARDWARE[] =
    "HARDWARE";


// =====================================================
// TIMESTAMP CLOCK
// =====================================================

constexpr char TIMESTAMP_CLOCK[] =
    "ESP32_MONOTONIC_US";


// =====================================================
// TIMESTAMP SEMANTICS
// =====================================================

constexpr char TS_DATA_READY[] =
    "IRQ_TIMESTAMP_PROXY";

constexpr char TS_FIFO[] =
    "RECONSTRUCTED_FROM_FIFO_IRQ_AND_ODR";


// =====================================================
// DEFAULT SENSOR ID
// =====================================================

constexpr char SENSOR_ID_ADXL345[] =
    "ADXL345";


// =====================================================
// RECORD METADATA
// =====================================================

struct RecordMetadata
{
    const char* protocolVersion =
        "UNSET";

    const char* firmwareVersion =
        "UNSET";


    // MOCK hoặc HARDWARE
    const char* dataSource =
        "UNSET";


    const char* recordId =
        "UNSET";

    const char* conditionId =
        "UNSET";


    const char* sensorId =
        SENSOR_ID_ADXL345;


    uint16_t odrHz =
        0;

    uint8_t rangeG =
        0;


    AcquisitionMode acquisitionMode =
        AcquisitionMode::DATA_READY;


    uint16_t fifoWatermark =
        0;


    const char* timestampClock =
        TIMESTAMP_CLOCK;

    const char* timestampSemantics =
        "UNSET";


    const char* calibrationVersion =
        "UNSET";

    const char* sensorHealthVersion =
        "UNSET";


    uint64_t startTimeUs =
        0;


    uint32_t requestedDurationS =
        0;


    // Stress test fields.
    bool stressActive =
        false;

    uint16_t stressRunIndex =
        0;

    uint16_t stressRunTotal =
        0;
};


// =====================================================
// RECORD FOOTER
// =====================================================

struct RecordFooter
{
    uint64_t stopTimeUs =
        0;


    uint32_t expectedSamples =
        0;

    uint32_t receivedSamples =
        0;

    uint32_t consumedSamples =
        0;


    uint32_t eventIrqCount =
        0;

    uint32_t eventProcessedCount =
        0;

    uint32_t eventQueueOverflow =
        0;


    uint32_t fifoBatchesProcessed =
        0;

    uint32_t fifoSamplesRead =
        0;

    uint32_t finalFifoPartialSamples =
        0;


    uint32_t ringPeakOccupancy =
        0;


    uint32_t sequenceErrors =
        0;

    uint32_t timestampOrderErrors =
        0;


    ErrorCounters errors;


    const char* stopReason =
        "UNSET";

    const char* qcStatus =
        "UNSET";
};


// =====================================================
// PUBLIC FUNCTIONS
// =====================================================

const char* modeName(
    AcquisitionMode mode
);


void printRecordBegin(
    const RecordMetadata& metadata
);


void printSampleHeader();


void printSample(
    const SamplePacket& sample
);


void printRecordFooter(
    const RecordFooter& footer
);

} // namespace DataContract