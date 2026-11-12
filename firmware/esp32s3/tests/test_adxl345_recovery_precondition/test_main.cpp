#include <Arduino.h>
#include <unity.h>

#include "../../drivers/adxl345/adxl345_driver.h"

void test_recover_before_begin_fails_closed()
{
    ADXL345Driver driver;

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

    RUN_TEST(test_recover_before_begin_fails_closed);

    UNITY_END();
}

void loop()
{
}