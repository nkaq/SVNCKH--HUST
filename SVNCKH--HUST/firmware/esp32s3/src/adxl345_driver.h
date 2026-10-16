#pragma once

#include <Arduino.h>
#include <Wire.h>

#include "config.h"
#include "daq_types.h"


class ADXL345Driver
{
public:





    bool begin();

    bool selfCheck();







    bool configureDataReady();



    bool configureFifo(
        uint8_t watermark
    );



    bool disableInterrupts();







    bool readRaw(
        int16_t& x,
        int16_t& y,
        int16_t& z
    );



    bool readFifo(
        RawSample* samples,
        uint8_t maxSamples,
        uint8_t& samplesRead
    );



    bool getFifoEntries(
        uint8_t& entries
    );






    bool readInterruptSource(
        uint8_t& source
    );


    bool fifoOverrunDetected(
        bool& overrun
    );







    bool recover(
        AcquisitionMode mode
    );






    uint8_t getAddress() const;

    bool isInitialized() const;


private:

    bool initialized = false;

    uint8_t deviceAddress = 0;






    bool detectDevice();






    bool configureCommon();






    bool writeRegister(
        uint8_t reg,
        uint8_t value
    );


    bool readRegister(
        uint8_t reg,
        uint8_t& value
    );


    bool readRegisters(
        uint8_t startReg,
        uint8_t* data,
        uint8_t length
    );
};