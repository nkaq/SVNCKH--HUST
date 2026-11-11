#include <Arduino.h>
#include <unity.h>

#include "../../drivers/adxl345/adxl345_driver.h"

void test_begin_fails_with_invalid_pins()
{
    ADXL345Driver driver;

    TEST_ASSERT_FALSE(driver.begin());
    TEST_ASSERT_FALSE(driver.isInitialized());
    TEST_ASSERT_EQUAL_UINT8(0, driver.getAddress());
}

void test_recover_after_invalid_pin_begin_fails_closed()
{
    ADXL345Driver driver;

    TEST_ASSERT_FALSE(driver.begin());

    TEST_ASSERT_FALSE(
        driver.recover(
            AcquisitionMode::FIFO
        )
    );

    TEST_ASSERT_FALSE(driver.isInitialized());
    TEST_ASSERT_EQUAL_UINT8(0, driver.getAddress());
}

void setup()
{
    delay(3000);

    UNITY_BEGIN();

    RUN_TEST(test_begin_fails_with_invalid_pins);
    RUN_TEST(test_recover_after_invalid_pin_begin_fails_closed);

    UNITY_END();
}

void loop()
{
}