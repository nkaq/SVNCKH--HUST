#include "adxl345_driver.h"






static constexpr uint8_t REG_DEVID =
    0x00;

static constexpr uint8_t REG_BW_RATE =
    0x2C;

static constexpr uint8_t REG_POWER_CTL =
    0x2D;

static constexpr uint8_t REG_INT_ENABLE =
    0x2E;

static constexpr uint8_t REG_INT_MAP =
    0x2F;

static constexpr uint8_t REG_INT_SOURCE =
    0x30;

static constexpr uint8_t REG_DATA_FORMAT =
    0x31;

static constexpr uint8_t REG_DATAX0 =
    0x32;

static constexpr uint8_t REG_FIFO_CTL =
    0x38;

static constexpr uint8_t REG_FIFO_STATUS =
    0x39;






static constexpr uint8_t ADXL345_DEVID =
    0xE5;



static constexpr uint8_t ADDRESS_LOW =
    0x53;



static constexpr uint8_t ADDRESS_HIGH =
    0x1D;






static constexpr uint8_t INT_DATA_READY =
    0x80;



static constexpr uint8_t INT_WATERMARK =
    0x02;



static constexpr uint8_t INT_OVERRUN =
    0x01;













static constexpr uint8_t FIFO_MODE_BYPASS =
    0x00;


static constexpr uint8_t FIFO_MODE_STREAM =
    0x80;






static constexpr uint8_t POWER_STANDBY =
    0x00;


static constexpr uint8_t POWER_MEASURE =
    0x08;






bool ADXL345Driver::begin()
{
    initialized = false;










    if (!Config::ADXL_HARDWARE_PRESENT)
    {
        deviceAddress = 0;

        initialized = true;

        return true;
    }






    if (
        Config::ADXL_SDA_PIN < 0 ||
        Config::ADXL_SCL_PIN < 0 ||
        Config::ADXL_INT_PIN < 0
    )
    {
        return false;
    }


    Wire.begin(
        Config::ADXL_SDA_PIN,
        Config::ADXL_SCL_PIN
    );


    Wire.setClock(
        Config::I2C_CLOCK_HZ
    );


    Wire.setTimeOut(
        Config::I2C_TIMEOUT_MS
    );






    if (!detectDevice())
    {
        return false;
    }






    if (!configureCommon())
    {
        return false;
    }


    initialized = true;


    return true;
}






bool ADXL345Driver::detectDevice()
{
    const uint8_t possibleAddresses[] =
    {
        ADDRESS_LOW,
        ADDRESS_HIGH
    };


    for (
        uint8_t address :
        possibleAddresses
    )
    {
        deviceAddress =
            address;


        uint8_t id = 0;


        if (
            readRegister(
                REG_DEVID,
                id
            )
            &&
            id == ADXL345_DEVID
        )
        {
            return true;
        }
    }


    deviceAddress = 0;


    return false;
}






bool ADXL345Driver::configureCommon()
{




    if (
        !writeRegister(
            REG_POWER_CTL,
            POWER_STANDBY
        )
    )
    {
        return false;
    }






    if (
        !writeRegister(
            REG_BW_RATE,
            Config::ADXL_ODR_CODE
        )
    )
    {
        return false;
    }










    uint8_t rangeCode = 0;


    switch (Config::ADXL_RANGE_G)
    {
        case 2:
            rangeCode = 0x00;
            break;

        case 4:
            rangeCode = 0x01;
            break;

        case 8:
            rangeCode = 0x02;
            break;

        case 16:
            rangeCode = 0x03;
            break;

        default:
            return false;
    }


    uint8_t dataFormat =
        0x08 | rangeCode;


    if (
        !writeRegister(
            REG_DATA_FORMAT,
            dataFormat
        )
    )
    {
        return false;
    }






    if (
        !writeRegister(
            REG_INT_ENABLE,
            0x00
        )
    )
    {
        return false;
    }








    if (
        !writeRegister(
            REG_INT_MAP,
            0x00
        )
    )
    {
        return false;
    }






    if (
        !writeRegister(
            REG_FIFO_CTL,
            FIFO_MODE_BYPASS
        )
    )
    {
        return false;
    }






    if (
        !writeRegister(
            REG_POWER_CTL,
            POWER_MEASURE
        )
    )
    {
        return false;
    }


    return true;
}






bool ADXL345Driver::selfCheck()
{







    if (!Config::ADXL_HARDWARE_PRESENT)
    {
        return initialized;
    }


    if (!initialized)
    {
        return false;
    }


    uint8_t id = 0;


    if (
        !readRegister(
            REG_DEVID,
            id
        )
    )
    {
        return false;
    }


    return (
        id == ADXL345_DEVID
    );
}






bool ADXL345Driver::configureDataReady()
{
    if (!initialized)
    {
        return false;
    }



    if (!Config::ADXL_HARDWARE_PRESENT)
    {
        return true;
    }






    if (!disableInterrupts())
    {
        return false;
    }






    if (
        !writeRegister(
            REG_FIFO_CTL,
            FIFO_MODE_BYPASS
        )
    )
    {
        return false;
    }






    if (
        !writeRegister(
            REG_INT_MAP,
            0x00
        )
    )
    {
        return false;
    }



    uint8_t dummy = 0;

    readInterruptSource(
        dummy
    );






    if (
        !writeRegister(
            REG_INT_ENABLE,
            INT_DATA_READY
        )
    )
    {
        return false;
    }


    return true;
}






bool ADXL345Driver::configureFifo(
    uint8_t watermark
)
{
    if (!initialized)
    {
        return false;
    }



    if (
        watermark == 0 ||
        watermark > 31
    )
    {
        return false;
    }



    if (!Config::ADXL_HARDWARE_PRESENT)
    {
        return true;
    }






    if (!disableInterrupts())
    {
        return false;
    }









    uint8_t fifoControl =
        FIFO_MODE_STREAM |
        (watermark & 0x1F);


    if (
        !writeRegister(
            REG_FIFO_CTL,
            fifoControl
        )
    )
    {
        return false;
    }






    if (
        !writeRegister(
            REG_INT_MAP,
            0x00
        )
    )
    {
        return false;
    }


    uint8_t dummy = 0;

    readInterruptSource(
        dummy
    );








    if (
        !writeRegister(
            REG_INT_ENABLE,
            INT_WATERMARK |
            INT_OVERRUN
        )
    )
    {
        return false;
    }


    return true;
}






bool ADXL345Driver::disableInterrupts()
{
    if (!initialized)
    {
        return false;
    }


    if (!Config::ADXL_HARDWARE_PRESENT)
    {
        return true;
    }


    return writeRegister(
        REG_INT_ENABLE,
        0x00
    );
}






bool ADXL345Driver::readRaw(
    int16_t& x,
    int16_t& y,
    int16_t& z
)
{
    if (!initialized)
    {
        return false;
    }








    if (!Config::ADXL_HARDWARE_PRESENT)
    {
        x = 0;

        y = 0;

        z = 0;

        return true;
    }






    uint8_t raw[6];


    if (
        !readRegisters(
            REG_DATAX0,
            raw,
            6
        )
    )
    {
        return false;
    }


    x =
        static_cast<int16_t>(
            (
                static_cast<uint16_t>(raw[1])
                << 8
            )
            |
            raw[0]
        );


    y =
        static_cast<int16_t>(
            (
                static_cast<uint16_t>(raw[3])
                << 8
            )
            |
            raw[2]
        );


    z =
        static_cast<int16_t>(
            (
                static_cast<uint16_t>(raw[5])
                << 8
            )
            |
            raw[4]
        );


    return true;
}






bool ADXL345Driver::getFifoEntries(
    uint8_t& entries
)
{
    entries = 0;


    if (!initialized)
    {
        return false;
    }



    if (!Config::ADXL_HARDWARE_PRESENT)
    {
        return true;
    }


    uint8_t status = 0;


    if (
        !readRegister(
            REG_FIFO_STATUS,
            status
        )
    )
    {
        return false;
    }



    entries =
        status & 0x3F;



    if (
        entries
        > Config::ADXL_FIFO_MAX_SAMPLES
    )
    {
        entries =
            Config::ADXL_FIFO_MAX_SAMPLES;
    }


    return true;
}






bool ADXL345Driver::readFifo(
    RawSample* samples,
    uint8_t maxSamples,
    uint8_t& samplesRead
)
{
    samplesRead = 0;


    if (
        !initialized ||
        samples == nullptr ||
        maxSamples == 0
    )
    {
        return false;
    }









    if (!Config::ADXL_HARDWARE_PRESENT)
    {
        uint8_t count =
            maxSamples;


        if (
            count
            > Config::ADXL_FIFO_MAX_SAMPLES
        )
        {
            count =
                Config::ADXL_FIFO_MAX_SAMPLES;
        }


        for (
            uint8_t i = 0;
            i < count;
            i++
        )
        {
            samples[i].x = 0;

            samples[i].y = 0;

            samples[i].z = 0;
        }


        samplesRead = count;


        return true;
    }






    uint8_t entries = 0;


    if (!getFifoEntries(entries))
    {
        return false;
    }


    uint8_t count =
        entries;


    if (count > maxSamples)
    {
        count =
            maxSamples;
    }


    for (
        uint8_t i = 0;
        i < count;
        i++
    )
    {
        if (
            !readRaw(
                samples[i].x,
                samples[i].y,
                samples[i].z
            )
        )
        {
            return false;
        }


        samplesRead++;
    }


    return true;
}






bool ADXL345Driver::readInterruptSource(
    uint8_t& source
)
{
    source = 0;


    if (!initialized)
    {
        return false;
    }


    if (!Config::ADXL_HARDWARE_PRESENT)
    {
        return true;
    }


    return readRegister(
        REG_INT_SOURCE,
        source
    );
}






bool ADXL345Driver::fifoOverrunDetected(
    bool& overrun
)
{
    overrun = false;


    uint8_t source = 0;


    if (
        !readInterruptSource(
            source
        )
    )
    {
        return false;
    }


    overrun =
        (
            source
            & INT_OVERRUN
        )
        != 0;


    return true;
}






bool ADXL345Driver::recover(
    AcquisitionMode mode
)
{




    if (!Config::ADXL_HARDWARE_PRESENT)
    {
        initialized = true;

        return true;
    }






    disableInterrupts();


    Wire.end();


    delay(2);


    initialized = false;

    deviceAddress = 0;



    if (!begin())
    {
        return false;
    }



    if (!selfCheck())
    {
        return false;
    }



    switch (mode)
    {
        case AcquisitionMode::DATA_READY:

            return configureDataReady();


        case AcquisitionMode::FIFO:

            return configureFifo(
                Config::FIFO_WATERMARK
            );
    }


    return false;
}






bool ADXL345Driver::writeRegister(
    uint8_t reg,
    uint8_t value
)
{
    if (deviceAddress == 0)
    {
        return false;
    }


    Wire.beginTransmission(
        deviceAddress
    );


    Wire.write(
        reg
    );


    Wire.write(
        value
    );


    uint8_t result =
        Wire.endTransmission(
            true
        );


    return (
        result == 0
    );
}






bool ADXL345Driver::readRegister(
    uint8_t reg,
    uint8_t& value
)
{
    return readRegisters(
        reg,
        &value,
        1
    );
}






bool ADXL345Driver::readRegisters(
    uint8_t startReg,
    uint8_t* data,
    uint8_t length
)
{
    if (
        deviceAddress == 0 ||
        data == nullptr ||
        length == 0
    )
    {
        return false;
    }






    Wire.beginTransmission(
        deviceAddress
    );


    Wire.write(
        startReg
    );


    uint8_t result =
        Wire.endTransmission(
            false
        );


    if (result != 0)
    {
        return false;
    }






    size_t received =
        Wire.requestFrom(
            static_cast<uint8_t>(
                deviceAddress
            ),
            static_cast<size_t>(
                length
            ),
            true
        );


    if (
        received != length
    )
    {

        while (Wire.available())
        {
            Wire.read();
        }


        return false;
    }


    for (
        uint8_t i = 0;
        i < length;
        i++
    )
    {
        if (!Wire.available())
        {
            return false;
        }


        data[i] =
            static_cast<uint8_t>(
                Wire.read()
            );
    }


    return true;
}






uint8_t ADXL345Driver::getAddress() const
{
    return deviceAddress;
}


bool ADXL345Driver::isInitialized() const
{
    return initialized;
}