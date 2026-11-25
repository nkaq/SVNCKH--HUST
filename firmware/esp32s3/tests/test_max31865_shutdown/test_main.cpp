
#include <Arduino.h>
#include <unity.h>

#include "../../drivers/max31865/max31865_driver.h"

// =====================================================
// Fake SPI bus for shutdown tests
// =====================================================

class FakeShutdownBus : public IMax31865Bus
{
public:
    uint8_t config_register = 0;

    bool fail_write = false;
    bool fail_read = false;
    bool force_bias_on = false;

    uint32_t writes = 0;
    uint32_t reads = 0;

    uint8_t last_address = 0xFF;
    uint8_t last_value = 0xFF;

    bool writeRegister(
        uint8_t address,
        uint8_t value
    ) override
    {
        ++writes;
        last_address = address;
        last_value = value;

        if (fail_write || address != 0x00)
        {
            return false;
        }

        // Keep persistent MAX31865 configuration bits.
        config_register =
            static_cast<uint8_t>(value & 0xD1);

        return true;
    }

    bool readRegisters(
        uint8_t address,
        uint8_t* destination,
        size_t length
    ) override
    {
        ++reads;

        if (
            fail_read ||
            address != 0x00 ||
            destination == nullptr ||
            length != 1
        )
        {
            return false;
        }

        destination[0] = force_bias_on
            ? static_cast<uint8_t>(
                config_register | 0x80
              )
            : config_register;

        return true;
    }
};

static FakeShutdownBus bus;
static Max31865Driver driver;

void setUp()
{
    driver.end();
    bus = FakeShutdownBus();
}

void tearDown()
{
    driver.end();
}

static void configureSensor()
{
    TEST_ASSERT_TRUE(
        driver.begin(bus, 430.0f)
    );

    Max31865Config config;
    config.bias_enabled = true;

    TEST_ASSERT_TRUE(
        driver.configure(config)
    );

    TEST_ASSERT_TRUE(
        driver.isConfigured()
    );
}

// =====================================================
// TEST 1: Successful hardware shutdown
// =====================================================

void test_shutdown_success()
{
    configureSensor();

    const uint32_t writes_before = bus.writes;
    const uint32_t reads_before = bus.reads;

    TEST_ASSERT_TRUE(
        driver.shutdownHardware()
    );

    TEST_ASSERT_EQUAL_HEX8(
        0x00,
        bus.last_address
    );

    TEST_ASSERT_EQUAL_HEX8(
        0x00,
        bus.last_value
    );

    TEST_ASSERT_EQUAL_HEX8(
        0x00,
        bus.config_register & 0xC0
    );

    TEST_ASSERT_EQUAL_UINT32(
        writes_before + 1,
        bus.writes
    );

    TEST_ASSERT_EQUAL_UINT32(
        reads_before + 1,
        bus.reads
    );

    TEST_ASSERT_FALSE(
        driver.isConfigured()
    );

    TEST_ASSERT_FALSE(
        driver.startOneShot(0, 12)
    );
}

// =====================================================
// TEST 2: Reject shutdown before begin
// =====================================================

void test_shutdown_before_begin()
{
    TEST_ASSERT_FALSE(
        driver.shutdownHardware()
    );

    TEST_ASSERT_EQUAL_UINT32(0, bus.writes);
    TEST_ASSERT_EQUAL_UINT32(0, bus.reads);
}

// =====================================================
// TEST 3: SPI write failure
// =====================================================

void test_shutdown_write_failure()
{
    configureSensor();

    bus.fail_write = true;

    TEST_ASSERT_FALSE(
        driver.shutdownHardware()
    );

    TEST_ASSERT_FALSE(
        driver.isConfigured()
    );

    TEST_ASSERT_FALSE(
        driver.startOneShot(0, 12)
    );
}

// =====================================================
// TEST 4: Readback failure
// =====================================================

void test_shutdown_readback_failure()
{
    configureSensor();

    bus.fail_read = true;

    TEST_ASSERT_FALSE(
        driver.shutdownHardware()
    );

    TEST_ASSERT_FALSE(
        driver.isConfigured()
    );

    TEST_ASSERT_FALSE(
        driver.startOneShot(0, 12)
    );
}

// =====================================================
// TEST 5: VBIAS remains ON after write
// =====================================================

void test_shutdown_rejects_bias_still_on()
{
    configureSensor();

    bus.force_bias_on = true;

    TEST_ASSERT_FALSE(
        driver.shutdownHardware()
    );

    TEST_ASSERT_FALSE(
        driver.isConfigured()
    );

    TEST_ASSERT_FALSE(
        driver.startOneShot(0, 12)
    );
}

// =====================================================
// TEST 6: Shutdown cancels pending conversion
// =====================================================

void test_shutdown_cancels_oneshot()
{
    configureSensor();

    TEST_ASSERT_TRUE(
        driver.startOneShot(100, 12)
    );

    TEST_ASSERT_TRUE(
        driver.oneShotActive()
    );

    TEST_ASSERT_TRUE(
        driver.shutdownHardware()
    );

    TEST_ASSERT_FALSE(
        driver.oneShotActive()
    );

    TEST_ASSERT_FALSE(
        driver.isConfigured()
    );

    const Max31865Sample result =
        driver.pollOneShot(1000);

    TEST_ASSERT_FALSE(result.valid);

    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(
            Max31865ReadStatus::InvalidState
        ),
        static_cast<int>(result.status)
    );
}

// =====================================================
// Unity runner: 6 tests
// =====================================================

void setup()
{
    delay(3000);

    UNITY_BEGIN();

    RUN_TEST(test_shutdown_success);
    RUN_TEST(test_shutdown_before_begin);
    RUN_TEST(test_shutdown_write_failure);
    RUN_TEST(test_shutdown_readback_failure);
    RUN_TEST(test_shutdown_rejects_bias_still_on);
    RUN_TEST(test_shutdown_cancels_oneshot);

    UNITY_END();
}

void loop()
{
}
