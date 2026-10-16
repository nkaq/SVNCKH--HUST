#pragma once

#include <Arduino.h>

#include "adxl345_driver.h"
#include "ring_buffer.h"
#include "error_manager.h"


class AcquisitionEngine
{
public:


    void begin(
        ADXL345Driver* sensorDriver,
        RingBuffer* ringBuffer,
        ErrorManager* errorManager
    );



    void reset();











    bool acquireOneSample(
        uint64_t irqTimestampUs
    );













    bool pushRawSample(
        uint64_t timestampUs,
        int16_t x,
        int16_t y,
        int16_t z,
        uint16_t statusFlags = STATUS_OK
    );



    uint32_t getSampleIndex() const;





    uint8_t getConsecutiveReadFailures() const;



    void clearReadFailures();


private:

    ADXL345Driver* sensor = nullptr;

    RingBuffer* buffer = nullptr;

    ErrorManager* errors = nullptr;






    uint32_t sampleIndex = 0;








    uint8_t consecutiveReadFailures = 0;
};