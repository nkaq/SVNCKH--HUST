
#pragma once

#include <stddef.h>
#include <stdint.h>

// Platform-independent SPI operations.
// Implemented by Arduino hardware or a test fake.
class IMax31865SpiIo
{
public:
    virtual ~IMax31865SpiIo() = default;

    virtual void beginTransaction() = 0;

    // selected=true means CS LOW.
    // selected=false means CS HIGH.
    virtual void setChipSelect(bool selected) = 0;

    virtual void csDelay() = 0;

    virtual uint8_t transfer(uint8_t value) = 0;

    virtual void endTransaction() = 0;
};

// Defines the actual register-level SPI sequence.
//
// Does not detect physical slave absence.
// A successful return means the request is
// syntactically valid and transfers were issued.
class Max31865SpiProtocol
{
public:
    static constexpr uint8_t kLastRegister = 0x07;
    static constexpr uint8_t kWriteFlag = 0x80;

    static bool read(
        IMax31865SpiIo& io,
        uint8_t address,
        uint8_t* destination,
        size_t length
    )
    {
        if (destination == nullptr || length == 0)
        {
            return false;
        }

        if (address > kLastRegister)
        {
            return false;
        }

        // Avoid address overflow and out-of-map reads.
        const size_t available =
            static_cast<size_t>(
                kLastRegister - address
            ) + 1U;

        if (length > available)
        {
            return false;
        }

        io.beginTransaction();
        io.setChipSelect(true);
        io.csDelay();

        // Read address: bit 7 = 0.
        io.transfer(address);

        for (size_t i = 0; i < length; ++i)
        {
            destination[i] = io.transfer(0x00);
        }

        io.csDelay();
        io.setChipSelect(false);
        io.csDelay();
        io.endTransaction();

        return true;
    }

    static bool write(
        IMax31865SpiIo& io,
        uint8_t address,
        uint8_t value
    )
    {
        if (address > kLastRegister)
        {
            return false;
        }

        io.beginTransaction();
        io.setChipSelect(true);
        io.csDelay();

        io.transfer(
            static_cast<uint8_t>(
                address | kWriteFlag
            )
        );

        io.transfer(value);

        io.csDelay();
        io.setChipSelect(false);
        io.csDelay();
        io.endTransaction();

        return true;
    }
};
