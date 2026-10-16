#pragma once

#include <Arduino.h>

#include "adxl345_driver.h"
#include "ring_buffer.h"
#include "error_manager.h"


class AcquisitionEngine
{
public:

    // Kết nối acquisition với driver/buffer/error manager.
    void begin(
        ADXL345Driver* sensorDriver,
        RingBuffer* ringBuffer,
        ErrorManager* errorManager
    );


    // Reset trước mỗi recording.
    void reset();


    // =================================================
    // DATA_READY MODE
    //
    // 1 DATA_READY IRQ
    // → 1 sample.
    //
    // Timestamp phải được capture từ IRQ trước khi gọi.
    // =================================================

    bool acquireOneSample(
        uint64_t irqTimestampUs
    );


    // =================================================
    // FIFO MODE
    //
    // Driver đã đọc raw XYZ từ FIFO.
    //
    // AcquisitionEngine chỉ:
    // raw + reconstructed timestamp
    // → SamplePacket
    // → RingBuffer
    // =================================================

    bool pushRawSample(
        uint64_t timestampUs,
        int16_t x,
        int16_t y,
        int16_t z,
        uint16_t statusFlags = STATUS_OK
    );


    // Sequence number tiếp theo.
    uint32_t getSampleIndex() const;


    // Số lần sensor read liên tiếp thất bại.
    //
    // Dùng cho Step 11 recovery.
    uint8_t getConsecutiveReadFailures() const;


    // Clear failure streak sau recovery.
    void clearReadFailures();


private:

    ADXL345Driver* sensor = nullptr;

    RingBuffer* buffer = nullptr;

    ErrorManager* errors = nullptr;


    // =================================================
    // SAMPLE SEQUENCE
    // =================================================

    uint32_t sampleIndex = 0;


    // =================================================
    // STEP 11
    //
    // Theo dõi read failure liên tiếp.
    // =================================================

    uint8_t consecutiveReadFailures = 0;
};