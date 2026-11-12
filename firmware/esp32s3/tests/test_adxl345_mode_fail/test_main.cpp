#include <Arduino.h>
#include <unity.h>

#include "../../drivers/adxl345/adxl345_driver.h"

void test_mode_reconfigure_failure_clears_initialized_state()
{
    ADXL345Driver driver;

    TEST_ASSERT_TRUE(driver.begin());
    TEST_ASSERT_TRUE(driver.isInitialized());

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

    RUN_TEST(test_mode_reconfigure_failure_clears_initialized_state);

    UNITY_END();
}

void loop()
{
}