#include "acquisition.h"






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






void AcquisitionEngine::reset()
{
    sampleIndex = 0;

    consecutiveReadFailures = 0;
}






bool AcquisitionEngine::acquireOneSample(
    uint64_t irqTimestampUs
)
{




    if (
        sensor == nullptr ||
        buffer == nullptr ||
        errors == nullptr
    )
    {
        return false;
    }






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




    consecutiveReadFailures = 0;










    return pushRawSample(
        irqTimestampUs,
        x,
        y,
        z,
        STATUS_OK
    );
}









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


















    sampleIndex++;






    if (!buffer->push(packet))
    {
        errors->report(
            ErrorCode::BUFFER_OVERFLOW
        );


        return false;
    }


    return true;
}






uint32_t AcquisitionEngine::getSampleIndex() const
{
    return sampleIndex;
}






uint8_t
AcquisitionEngine::getConsecutiveReadFailures() const
{
    return consecutiveReadFailures;
}






void AcquisitionEngine::clearReadFailures()
{
    consecutiveReadFailures = 0;
}