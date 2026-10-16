#pragma once

#include <Arduino.h>

#include "daq_types.h"


namespace DataContract
{


constexpr char CONTRACT_VERSION[] =
    "EE2_IT2_DC_v1.0";

constexpr char SAMPLE_FORMAT[] =
    "CSV";

constexpr char SAMPLE_COLUMNS[] =
    "sample_index,timestamp_us,x_raw,y_raw,z_raw,status_flags";

constexpr char SOURCE_MOCK[] =
    "MOCK";

constexpr char SOURCE_HARDWARE[] =
    "HARDWARE";

constexpr char TIMESTAMP_CLOCK[] =
    "ESP32_MONOTONIC_US";

constexpr char TS_DATA_READY[] =
    "IRQ_TIMESTAMP_PROXY";

constexpr char TS_FIFO[] =
    "RECONSTRUCTED_FROM_FIFO_IRQ_AND_ODR";

constexpr char SENSOR_ID_ADXL345[] =
    "ADXL345";

struct RecordMetadata
{
    const char* protocolVersion =
        "UNSET";

    const char* firmwareVersion =
        "UNSET";



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



    bool stressActive =
        false;

    uint16_t stressRunIndex =
        0;

    uint16_t stressRunTotal =
        0;
};

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

} 