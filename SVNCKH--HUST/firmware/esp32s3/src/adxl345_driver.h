#pragma once

#include <Arduino.h>
#include <Wire.h>

#include "config.h"
#include "daq_types.h"


class ADXL345Driver
{
public:

    // =================================================
    // INITIALIZATION / HEALTH
    // =================================================

    bool begin();

    bool selfCheck();


    // =================================================
    // ACQUISITION CONFIGURATION
    // =================================================

    // 1 DATA_READY interrupt → 1 sample
    bool configureDataReady();


    // FIFO STREAM mode + WATERMARK interrupt
    bool configureFifo(
        uint8_t watermark
    );


    // Disable sensor interrupts
    bool disableInterrupts();


    // =================================================
    // DATA READ
    // =================================================

    // Đọc một XYZ sample
    bool readRaw(
        int16_t& x,
        int16_t& y,
        int16_t& z
    );


    // Đọc nhiều sample từ FIFO
    bool readFifo(
        RawSample* samples,
        uint8_t maxSamples,
        uint8_t& samplesRead
    );


    // Số sample hiện có trong ADXL FIFO
    bool getFifoEntries(
        uint8_t& entries
    );


    // =================================================
    // INTERRUPT STATUS
    // =================================================

    bool readInterruptSource(
        uint8_t& source
    );


    bool fifoOverrunDetected(
        bool& overrun
    );


    // =================================================
    // STEP 11 - RECOVERY
    // =================================================

    // Reinitialize sensor và phục hồi acquisition mode.
    bool recover(
        AcquisitionMode mode
    );


    // =================================================
    // INFO
    // =================================================

    uint8_t getAddress() const;

    bool isInitialized() const;


private:

    bool initialized = false;

    uint8_t deviceAddress = 0;


    // =================================================
    // DEVICE DETECTION
    // =================================================

    bool detectDevice();


    // =================================================
    // COMMON CONFIGURATION
    // =================================================

    bool configureCommon();


    // =================================================
    // LOW-LEVEL REGISTER ACCESS
    // =================================================

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