#include <Arduino.h>
#include <unity.h>

#include "../../drivers/adxl345/adxl345_driver.h"

void test_begin_fails_without_hardware()
{
    ADXL345Driver driver;

    TEST_ASSERT_FALSE(driver.begin());
    TEST_ASSERT_FALSE(driver.isInitialized());
}

void test_self_check_fails_without_hardware()
{
    ADXL345Driver driver;

    TEST_ASSERT_FALSE(driver.begin());
    TEST_ASSERT_FALSE(driver.selfCheck());
}

void test_read_raw_does_not_emit_valid_sample_without_hardware()
{
    ADXL345Driver driver;

    int16_t x = 123;
    int16_t y = 456;
    int16_t z = 789;

    TEST_ASSERT_FALSE(driver.begin());
    TEST_ASSERT_FALSE(driver.readRaw(x, y, z));
}

void test_read_fifo_does_not_emit_samples_without_hardware()
{
    ADXL345Driver driver;

    RawSample samples[16];
    uint8_t samplesRead = 255;

    TEST_ASSERT_FALSE(driver.begin());
    TEST_ASSERT_FALSE(
        driver.readFifo(
            samples,
            16,
            samplesRead
        )
    );
    TEST_ASSERT_EQUAL_UINT8(0, samplesRead);
}

void test_recover_fails_without_hardware()
{
    ADXL345Driver driver;

    TEST_ASSERT_FALSE(
        driver.recover(
            AcquisitionMode::FIFO
        )
    );
    TEST_ASSERT_FALSE(driver.isInitialized());
}

void setup()
{
    delay(3000);

    UNITY_BEGIN();

    RUN_TEST(test_begin_fails_without_hardware);
    RUN_TEST(test_self_check_fails_without_hardware);
    RUN_TEST(test_read_raw_does_not_emit_valid_sample_without_hardware);
    RUN_TEST(test_read_fifo_does_not_emit_samples_without_hardware);
    RUN_TEST(test_recover_fails_without_hardware);

    UNITY_END();
}

void loop()
{
}
