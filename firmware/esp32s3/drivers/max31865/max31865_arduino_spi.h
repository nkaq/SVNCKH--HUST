
#pragma once

#include <stddef.h>
#include <stdint.h>

#include "max31865_driver.h"

class SPIClass;

// MAX31865 physical SPI register transport.
//
// The caller owns SPIClass and calls spi.begin(...).
// This class owns only the MAX31865 chip-select GPIO.
//
// Not thread-safe: the caller must serialize access
// to this adapter and other users of the SPI bus.
class Max31865ArduinoSpiBus final
    : public IMax31865Bus
{
public:
    static constexpr uint32_t kDefaultClockHz =
        1000000UL;

    static constexpr uint32_t kMaxClockHz =
        5000000UL;

    // Attach to an already initialized SPI bus.
    //
    // Returns false for:
    // - an invalid ESP32-S3 output GPIO
    // - a zero or excessive SPI clock
    // - an adapter that is already initialized
    //
    // This method does not call spi.begin().
    bool begin(
        SPIClass& spi,
        int cs_pin,
        uint32_t clock_hz = kDefaultClockHz
    );

    // Release logical ownership.
    // Leaves CS configured as OUTPUT HIGH.
    //
    // Does not call spi.end() and does not
    // power down the MAX31865 sensor.
    void end();

    bool isInitialized() const;

    // Read consecutive logical register addresses.
    // One CS-low interval covers address and data.
    //
    // Valid address range: 0x00 through 0x07.
    // All requested registers must stay in range.
    bool readRegisters(
        uint8_t start_address,
        uint8_t* destination,
        size_t length
    ) override;

    // Write one logical register.
    // Physical address bit 7 is set internally.
    //
    // true means bytes were transmitted using
    // the Arduino SPI API. It does not prove
    // that the sensor accepted the register value.
    bool writeRegister(
        uint8_t address,
        uint8_t value
    ) override;

private:
    static constexpr uint8_t kLastRegister = 0x07;
    static constexpr uint8_t kWriteFlag = 0x80;

    SPIClass* spi_ = nullptr;

    int cs_pin_ = -1;

    uint32_t clock_hz_ = kDefaultClockHz;

    bool initialized_ = false;
};
