
#include "max31865_arduino_spi.h"
#include "max31865_spi_protocol.h"

#include <Arduino.h>
#include <SPI.h>

#include "driver/gpio.h"
#include "soc/soc_caps.h"

namespace
{
constexpr unsigned int kCsDelayUs = 1;

// Arduino hardware implementation of the same
// interface exercised by protocol unit tests.
class ArduinoMax31865SpiIo final
    : public IMax31865SpiIo
{
public:
    ArduinoMax31865SpiIo(
        SPIClass& spi,
        uint8_t cs_pin,
        uint32_t clock_hz
    )
        : spi_(spi),
          cs_pin_(cs_pin),
          clock_hz_(clock_hz)
    {
    }

    void beginTransaction() override
    {
        const SPISettings settings(
            clock_hz_,
            MSBFIRST,
            SPI_MODE1
        );

        spi_.beginTransaction(settings);
    }

    void setChipSelect(bool selected) override
    {
        digitalWrite(
            cs_pin_,
            selected ? LOW : HIGH
        );
    }

    void csDelay() override
    {
        delayMicroseconds(kCsDelayUs);
    }

    uint8_t transfer(uint8_t value) override
    {
        return spi_.transfer(value);
    }

    void endTransaction() override
    {
        spi_.endTransaction();
    }

private:
    SPIClass& spi_;
    uint8_t cs_pin_;
    uint32_t clock_hz_;
};
}

bool Max31865ArduinoSpiBus::begin(
    SPIClass& spi,
    int cs_pin,
    uint32_t clock_hz
)
{
    if (initialized_)
    {
        return false;
    }

    if (
        cs_pin < 0 ||
        cs_pin >= SOC_GPIO_PIN_COUNT
    )
    {
        return false;
    }

    const gpio_num_t gpio =
        static_cast<gpio_num_t>(cs_pin);

    if (!GPIO_IS_VALID_OUTPUT_GPIO(gpio))
    {
        return false;
    }

    if (
        clock_hz == 0 ||
        clock_hz > kMaxClockHz
    )
    {
        return false;
    }

    // Preload inactive CS level before
    // configuring the pin as output.
    digitalWrite(
        static_cast<uint8_t>(cs_pin),
        HIGH
    );

    pinMode(
        static_cast<uint8_t>(cs_pin),
        OUTPUT
    );

    digitalWrite(
        static_cast<uint8_t>(cs_pin),
        HIGH
    );

    spi_ = &spi;
    cs_pin_ = cs_pin;
    clock_hz_ = clock_hz;
    initialized_ = true;

    return true;
}

void Max31865ArduinoSpiBus::end()
{
    if (
        initialized_ &&
        cs_pin_ >= 0
    )
    {
        digitalWrite(
            static_cast<uint8_t>(cs_pin_),
            HIGH
        );
    }

    // Caller owns SPIClass.
    // Never call spi_->end().
    spi_ = nullptr;
    cs_pin_ = -1;
    clock_hz_ = kDefaultClockHz;
    initialized_ = false;
}

bool Max31865ArduinoSpiBus::isInitialized() const
{
    return initialized_;
}

bool Max31865ArduinoSpiBus::readRegisters(
    uint8_t start_address,
    uint8_t* destination,
    size_t length
)
{
    if (!initialized_ || spi_ == nullptr)
    {
        return false;
    }

    // Protocol validates address, pointer,
    // length and register boundaries before I/O.
    ArduinoMax31865SpiIo io(
        *spi_,
        static_cast<uint8_t>(cs_pin_),
        clock_hz_
    );

    return Max31865SpiProtocol::read(
        io,
        start_address,
        destination,
        length
    );
}

bool Max31865ArduinoSpiBus::writeRegister(
    uint8_t address,
    uint8_t value
)
{
    if (!initialized_ || spi_ == nullptr)
    {
        return false;
    }

    ArduinoMax31865SpiIo io(
        *spi_,
        static_cast<uint8_t>(cs_pin_),
        clock_hz_
    );

    return Max31865SpiProtocol::write(
        io,
        address,
        value
    );
}
