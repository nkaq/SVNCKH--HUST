
#include <Arduino.h>
#include <unity.h>
#include <math.h>

#include "../../drivers/max31865/max31865_driver.h"

// =====================================================
// Fake MAX31865 register bus
// =====================================================

class FakeMax31865Bus final : public IMax31865Bus
{
public:
    uint8_t rtd_msb = 0;
    uint8_t rtd_lsb = 0;
    uint8_t fault_status = 0;

    bool fail_rtd_read = false;
    bool fail_fault_read = false;
    bool fail_write = false;

    uint32_t rtd_reads = 0;
    uint32_t fault_reads = 0;
    uint32_t write_count = 0;

    uint8_t last_write_address = 0;
    uint8_t last_write_value = 0;

    bool readRegisters(
        uint8_t start_address,
        uint8_t* destination,
        size_t length
    ) override
    {
        if (destination == nullptr)
        {
            return false;
        }

        if (start_address == 0x01 && length == 2)
        {
            ++rtd_reads;

            if (fail_rtd_read)
            {
                return false;
            }

            destination[0] = rtd_msb;
            destination[1] = rtd_lsb;
            return true;
        }

        if (start_address == 0x07 && length == 1)
        {
            ++fault_reads;

            if (fail_fault_read)
            {
                return false;
            }

            destination[0] = fault_status;
            return true;
        }

        return false;
    }

    bool writeRegister(
        uint8_t address,
        uint8_t value
    ) override
    {
        ++write_count;
        last_write_address = address;
        last_write_value = value;

        return !fail_write;
    }
};

static FakeMax31865Bus bus;
static Max31865Driver driver;

void setUp()
{
    driver.end();
    bus = FakeMax31865Bus();
}

void tearDown()
{
    driver.end();
}

// =====================================================
// A. Codec tests: 4
// =====================================================

void test_codec_decodes_rtd_adc_code()
{
    // RTD register 0x8000 -> ADC 16384.
    // R = 16384 * 430 / 32768 = 215 ohms.
    const Max31865RtdReading reading =
        Max31865Codec::decode(
            0x80, 0x00, 430.0f
        );

    TEST_ASSERT_EQUAL_UINT16(
        16384, reading.adc_code
    );

    TEST_ASSERT_FALSE(reading.fault_flag);
    TEST_ASSERT_TRUE(reading.valid);

    TEST_ASSERT_FLOAT_WITHIN(
        0.001f,
        215.0f,
        reading.resistance_ohms
    );
}

void test_codec_rejects_fault_bit()
{
    const Max31865RtdReading reading =
        Max31865Codec::decode(
            0x80, 0x01, 430.0f
        );

    TEST_ASSERT_EQUAL_UINT16(
        16384, reading.adc_code
    );

    TEST_ASSERT_TRUE(reading.fault_flag);
    TEST_ASSERT_FALSE(reading.valid);

    TEST_ASSERT_TRUE(
        isnan(reading.resistance_ohms)
    );
}

void test_codec_rejects_invalid_rref()
{
    const Max31865RtdReading zero =
        Max31865Codec::decode(
            0x80, 0x00, 0.0f
        );

    const Max31865RtdReading negative =
        Max31865Codec::decode(
            0x80, 0x00, -430.0f
        );

    const Max31865RtdReading nan_ref =
        Max31865Codec::decode(
            0x80, 0x00, NAN
        );

    TEST_ASSERT_FALSE(zero.valid);
    TEST_ASSERT_FALSE(negative.valid);
    TEST_ASSERT_FALSE(nan_ref.valid);

    TEST_ASSERT_TRUE(
        isnan(zero.resistance_ohms)
    );

    TEST_ASSERT_TRUE(
        isnan(negative.resistance_ohms)
    );

    TEST_ASSERT_TRUE(
        isnan(nan_ref.resistance_ohms)
    );
}

void test_codec_maximum_adc_code()
{
    // 0xFFFE -> 32767, fault flag cleared.
    const Max31865RtdReading reading =
        Max31865Codec::decode(
            0xFF, 0xFE, 430.0f
        );

    TEST_ASSERT_EQUAL_UINT16(
        32767, reading.adc_code
    );

    TEST_ASSERT_FALSE(reading.fault_flag);
    TEST_ASSERT_TRUE(reading.valid);

    TEST_ASSERT_FLOAT_WITHIN(
        0.001f,
        430.0f * 32767.0f / 32768.0f,
        reading.resistance_ohms
    );
}

// =====================================================
// B. Driver lifecycle tests: 4
// =====================================================

void test_driver_initially_not_initialized()
{
    TEST_ASSERT_FALSE(
        driver.isInitialized()
    );

    const Max31865Sample sample =
        driver.readSample();

    TEST_ASSERT_FALSE(sample.valid);

    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(
            Max31865ReadStatus::NotInitialized
        ),
        static_cast<int>(sample.status)
    );

    TEST_ASSERT_EQUAL_UINT32(
        0, bus.rtd_reads
    );
}

void test_driver_rejects_invalid_rref()
{
    TEST_ASSERT_FALSE(
        driver.begin(bus, 0.0f)
    );

    TEST_ASSERT_FALSE(
        driver.begin(bus, -1.0f)
    );

    TEST_ASSERT_FALSE(
        driver.begin(bus, NAN)
    );

    TEST_ASSERT_FALSE(
        driver.isInitialized()
    );
}

void test_driver_begin_and_end()
{
    TEST_ASSERT_TRUE(
        driver.begin(bus, 430.0f)
    );

    TEST_ASSERT_TRUE(
        driver.isInitialized()
    );

    driver.end();

    TEST_ASSERT_FALSE(
        driver.isInitialized()
    );

    const Max31865Sample sample =
        driver.readSample();

    TEST_ASSERT_FALSE(sample.valid);

    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(
            Max31865ReadStatus::NotInitialized
        ),
        static_cast<int>(sample.status)
    );
}

void test_driver_rejects_second_begin()
{
    TEST_ASSERT_TRUE(
        driver.begin(bus, 430.0f)
    );

    TEST_ASSERT_FALSE(
        driver.begin(bus, 100.0f)
    );

    TEST_ASSERT_TRUE(
        driver.isInitialized()
    );
}

// =====================================================
// C. Register read / fault tests: 6
// =====================================================

void test_driver_reads_valid_rtd_sample()
{
    bus.rtd_msb = 0x80;
    bus.rtd_lsb = 0x00;

    TEST_ASSERT_TRUE(
        driver.begin(bus, 430.0f)
    );

    const Max31865Sample sample =
        driver.readSample();

    TEST_ASSERT_TRUE(sample.valid);

    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(
            Max31865ReadStatus::Valid
        ),
        static_cast<int>(sample.status)
    );

    TEST_ASSERT_EQUAL_UINT16(
        16384, sample.adc_code
    );

    TEST_ASSERT_FLOAT_WITHIN(
        0.001f,
        215.0f,
        sample.resistance_ohms
    );

    TEST_ASSERT_EQUAL_UINT32(
        1, bus.rtd_reads
    );

    TEST_ASSERT_EQUAL_UINT32(
        0, bus.fault_reads
    );
}

void test_driver_reports_rtd_spi_failure()
{
    bus.fail_rtd_read = true;

    TEST_ASSERT_TRUE(
        driver.begin(bus, 430.0f)
    );

    const Max31865Sample sample =
        driver.readSample();

    TEST_ASSERT_FALSE(sample.valid);

    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(
            Max31865ReadStatus::TransportError
        ),
        static_cast<int>(sample.status)
    );

    TEST_ASSERT_EQUAL_UINT32(
        1, bus.rtd_reads
    );

    TEST_ASSERT_EQUAL_UINT32(
        0, bus.fault_reads
    );
}

void test_driver_reports_sensor_fault()
{
    bus.rtd_msb = 0x80;
    bus.rtd_lsb = 0x01;
    bus.fault_status = 0x80;

    TEST_ASSERT_TRUE(
        driver.begin(bus, 430.0f)
    );

    const Max31865Sample sample =
        driver.readSample();

    TEST_ASSERT_FALSE(sample.valid);
    TEST_ASSERT_TRUE(sample.fault_flag);

    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(
            Max31865ReadStatus::SensorFault
        ),
        static_cast<int>(sample.status)
    );

    TEST_ASSERT_EQUAL_HEX8(
        0x80, sample.fault_status
    );

    TEST_ASSERT_EQUAL_UINT32(
        1, bus.fault_reads
    );
}

void test_driver_reports_fault_register_spi_failure()
{
    bus.rtd_msb = 0x80;
    bus.rtd_lsb = 0x01;
    bus.fail_fault_read = true;

    TEST_ASSERT_TRUE(
        driver.begin(bus, 430.0f)
    );

    const Max31865Sample sample =
        driver.readSample();

    TEST_ASSERT_FALSE(sample.valid);
    TEST_ASSERT_TRUE(sample.fault_flag);

    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(
            Max31865ReadStatus::TransportError
        ),
        static_cast<int>(sample.status)
    );

    TEST_ASSERT_EQUAL_UINT32(
        1, bus.rtd_reads
    );

    TEST_ASSERT_EQUAL_UINT32(
        1, bus.fault_reads
    );
}

void test_driver_does_not_ignore_fault_with_zero_status()
{
    bus.rtd_msb = 0x80;
    bus.rtd_lsb = 0x01;
    bus.fault_status = 0x00;

    TEST_ASSERT_TRUE(
        driver.begin(bus, 430.0f)
    );

    const Max31865Sample sample =
        driver.readSample();

    TEST_ASSERT_FALSE(sample.valid);
    TEST_ASSERT_TRUE(sample.fault_flag);

    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(
            Max31865ReadStatus::SensorFault
        ),
        static_cast<int>(sample.status)
    );
}

void test_driver_can_recover_after_transport_failure()
{
    bus.fail_rtd_read = true;

    TEST_ASSERT_TRUE(
        driver.begin(bus, 430.0f)
    );

    const Max31865Sample failed =
        driver.readSample();

    TEST_ASSERT_FALSE(failed.valid);

    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(
            Max31865ReadStatus::TransportError
        ),
        static_cast<int>(failed.status)
    );

    bus.fail_rtd_read = false;
    bus.rtd_msb = 0x80;
    bus.rtd_lsb = 0x00;

    const Max31865Sample recovered =
        driver.readSample();

    TEST_ASSERT_TRUE(recovered.valid);

    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(
            Max31865ReadStatus::Valid
        ),
        static_cast<int>(recovered.status)
    );

    TEST_ASSERT_EQUAL_UINT32(
        2, bus.rtd_reads
    );
}

// =====================================================
// Unity runner: exactly 14 tests
// =====================================================

void setup()
{
    delay(3000);

    UNITY_BEGIN();

    // Codec: 4
    RUN_TEST(test_codec_decodes_rtd_adc_code);
    RUN_TEST(test_codec_rejects_fault_bit);
    RUN_TEST(test_codec_rejects_invalid_rref);
    RUN_TEST(test_codec_maximum_adc_code);

    // Lifecycle: 4
    RUN_TEST(test_driver_initially_not_initialized);
    RUN_TEST(test_driver_rejects_invalid_rref);
    RUN_TEST(test_driver_begin_and_end);
    RUN_TEST(test_driver_rejects_second_begin);

    // RTD and transport: 6
    RUN_TEST(test_driver_reads_valid_rtd_sample);
    RUN_TEST(test_driver_reports_rtd_spi_failure);
    RUN_TEST(test_driver_reports_sensor_fault);
    RUN_TEST(test_driver_reports_fault_register_spi_failure);
    RUN_TEST(test_driver_does_not_ignore_fault_with_zero_status);
    RUN_TEST(test_driver_can_recover_after_transport_failure);

    UNITY_END();
}

void loop()
{
}
