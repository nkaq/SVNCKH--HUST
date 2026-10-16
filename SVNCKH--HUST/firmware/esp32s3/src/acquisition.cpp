#include "acquisition.h"


// =====================================================
// BEGIN
// =====================================================

void AcquisitionEngine::begin(
    ADXL345Driver* sensorDriver,
    RingBuffer* ringBuffer,
    ErrorManager* errorManager
)
{
    sensor =
        sensorDriver;

    buffer =
        ringBuffer;

    errors =
        errorManager;


    sampleIndex = 0;

    consecutiveReadFailures = 0;
}


// =====================================================
// RESET
// =====================================================

void AcquisitionEngine::reset()
{
    sampleIndex = 0;

    consecutiveReadFailures = 0;
}


// =====================================================
// DATA_READY MODE
// =====================================================

bool AcquisitionEngine::acquireOneSample(
    uint64_t irqTimestampUs
)
{
    // -------------------------------------------------
    // MODULE VALIDATION
    // -------------------------------------------------

    if (
        sensor == nullptr ||
        buffer == nullptr ||
        errors == nullptr
    )
    {
        return false;
    }


    // -------------------------------------------------
    // READ RAW ADXL345
    // -------------------------------------------------

    int16_t x = 0;

    int16_t y = 0;

    int16_t z = 0;


    if (
        !sensor->readRaw(
            x,
            y,
            z
        )
    )
    {
        // I2C / sensor read failed.
        errors->report(
            ErrorCode::I2C_ERROR
        );


        if (
            consecutiveReadFailures
            < 255
        )
        {
            consecutiveReadFailures++;
        }


        return false;
    }


    // Sensor read thành công
    // → failure streak reset.
    consecutiveReadFailures = 0;


    // -------------------------------------------------
    // PACK SAMPLE
    //
    // Timestamp KHÔNG lấy ở đây.
    //
    // Nó đến từ DATA_READY IRQ.
    // -------------------------------------------------

    return pushRawSample(
        irqTimestampUs,
        x,
        y,
        z,
        STATUS_OK
    );
}


// =====================================================
// PUSH RAW SAMPLE
//
// Dùng chung cho:
// DATA_READY + FIFO.
// =====================================================

bool AcquisitionEngine::pushRawSample(
    uint64_t timestampUs,
    int16_t x,
    int16_t y,
    int16_t z,
    uint16_t statusFlags
)
{
    if (
        buffer == nullptr ||
        errors == nullptr
    )
    {
        return false;
    }


    // -------------------------------------------------
    // CREATE PACKET
    // -------------------------------------------------

    SamplePacket packet{};


    packet.sample_index =
        sampleIndex;


    packet.timestamp_us =
        timestampUs;


    packet.x_raw = x;

    packet.y_raw = y;

    packet.z_raw = z;


    packet.status_flags =
        statusFlags;


    // -------------------------------------------------
    // IMPORTANT
    //
    // sample_index tăng sau khi acquisition đã nhận
    // sample hợp lệ.
    //
    // Nếu RingBuffer full thì packet này bị drop,
    // nhưng sample_index vẫn đã được consume.
    //
    // Nhờ đó output có thể xuất hiện gap:
    //
    // 100,101,103
    //
    // → bằng chứng packet 102 bị drop.
    // -------------------------------------------------

    sampleIndex++;


    // -------------------------------------------------
    // PUSH RING BUFFER
    // -------------------------------------------------

    if (!buffer->push(packet))
    {
        errors->report(
            ErrorCode::BUFFER_OVERFLOW
        );


        return false;
    }


    return true;
}


// =====================================================
// GET SAMPLE INDEX
// =====================================================

uint32_t AcquisitionEngine::getSampleIndex() const
{
    return sampleIndex;
}


// =====================================================
// GET READ FAILURE STREAK
// =====================================================

uint8_t
AcquisitionEngine::getConsecutiveReadFailures() const
{
    return consecutiveReadFailures;
}


// =====================================================
// CLEAR FAILURE STREAK
// =====================================================

void AcquisitionEngine::clearReadFailures()
{
    consecutiveReadFailures = 0;
}