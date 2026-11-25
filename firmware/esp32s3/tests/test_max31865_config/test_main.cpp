
#include <Arduino.h>
#include <unity.h>

#include "../../drivers/max31865/max31865_driver.h"

// =====================================================
// Fake MAX31865 configuration bus
// =====================================================

class FakeConfigBus : public IMax31865Bus
{
public:
    uint8_t config_register = 0;

    bool fail_write = false;
    bool fail_read = false;
    bool corrupt_readback = false;

    uint32_t write_count = 0;
    uint32_t read_count = 0;

    uint8_t last_write_address = 0;
    uint8_t last_write_value = 0;

    bool readRegisters(
        uint8_t address,
        uint8_t* destination,
        size_t length
    ) override
    {
        if (
            address != 0x00 ||
            destination == nullptr ||
            length != 1
        )
        {
            return false;
        }

        ++read_count;

        if (fail_read)
        {
            return false;
        }

        destination[0] = corrupt_readback
            ? static_cast<uint8_t>(
                config_register ^ 0x10
              )
            : config_register;

        return true;
    }

    bool writeRegister(
        uint8_t address,
        uint8_t value
    ) override
    {
        ++write_count;

        last_write_address = address;
        last_write_value = value;

        if (fail_write || address != 0x00)
        {
            return false;
        }

        // Persistent configuration bits:
        // D7, D6, D4 and D0.
        //
        // One-shot D5 and fault-clear D1
        // are self-clearing.
        config_register =
            static_cast<uint8_t>(
                value & 0xD1
            );

        return true;
    }
};

static FakeConfigBus bus;
static Max31865Driver driver;

void setUp()
{
    driver.end();
    bus = FakeConfigBus();
}

void tearDown()
{
    driver.end();
}

// =====================================================
// TEST 01 — Reject configuration before begin
// =====================================================

void test_configure_rejects_before_begin()
{
    Max31865Config config;

    TEST_ASSERT_FALSE(
        driver.configure(config)
    );

    TEST_ASSERT_FALSE(
        driver.isConfigured()
    );

    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(
            Max31865ConfigStatus::NotInitialized
        ),
        static_cast<int>(
            driver.configStatus()
        )
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        bus.write_count
    );
}

// =====================================================
// TEST 02 — 3-wire, 50 Hz configuration
// =====================================================

void test_configure_three_wire_50hz()
{
    TEST_ASSERT_TRUE(
        driver.begin(bus, 430.0f)
    );

    Max31865Config config;

    config.wires =
        Max31865WireMode::ThreeWire;

    config.filter =
        Max31865Filter::Hz50;

    config.conversion =
        Max31865ConversionMode::NormallyOff;

    config.bias_enabled = true;

    TEST_ASSERT_TRUE(
        driver.configure(config)
    );

    TEST_ASSERT_TRUE(
        driver.isConfigured()
    );

    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(
            Max31865ConfigStatus::Configured
        ),
        static_cast<int>(
            driver.configStatus()
        )
    );

    // VBIAS 0x80
    // 3-wire 0x10
    // 50 Hz  0x01
    // Total  0x91
    TEST_ASSERT_EQUAL_HEX8(
        0x91,
        driver.expectedConfigRegister()
    );

    TEST_ASSERT_EQUAL_HEX8(
        0x00,
        bus.last_write_address
    );

    TEST_ASSERT_EQUAL_HEX8(
        0x91,
        bus.last_write_value
    );

    TEST_ASSERT_EQUAL_HEX8(
        0x91,
        bus.config_register
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        bus.write_count
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        bus.read_count
    );
}

// =====================================================
// TEST 03 — SPI write failure
// =====================================================

void test_configure_reports_write_failure()
{
    TEST_ASSERT_TRUE(
        driver.begin(bus, 430.0f)
    );

    bus.fail_write = true;

    Max31865Config config;

    TEST_ASSERT_FALSE(
        driver.configure(config)
    );

    TEST_ASSERT_FALSE(
        driver.isConfigured()
    );

    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(
            Max31865ConfigStatus::WriteFailed
        ),
        static_cast<int>(
            driver.configStatus()
        )
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        bus.write_count
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        bus.read_count
    );
}

// =====================================================
// TEST 04 — Readback transport failure
// =====================================================

void test_configure_reports_readback_failure()
{
    TEST_ASSERT_TRUE(
        driver.begin(bus, 430.0f)
    );

    bus.fail_read = true;

    Max31865Config config;

    TEST_ASSERT_FALSE(
        driver.configure(config)
    );

    TEST_ASSERT_FALSE(
        driver.isConfigured()
    );

    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(
            Max31865ConfigStatus::ReadbackFailed
        ),
        static_cast<int>(
            driver.configStatus()
        )
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        bus.write_count
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        bus.read_count
    );
}

// =====================================================
// TEST 05 — Readback mismatch
// =====================================================

void test_configure_rejects_readback_mismatch()
{
    TEST_ASSERT_TRUE(
        driver.begin(bus, 430.0f)
    );

    bus.corrupt_readback = true;

    Max31865Config config;

    TEST_ASSERT_FALSE(
        driver.configure(config)
    );

    TEST_ASSERT_FALSE(
        driver.isConfigured()
    );

    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(
            Max31865ConfigStatus::Mismatch
        ),
        static_cast<int>(
            driver.configStatus()
        )
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        bus.write_count
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        bus.read_count
    );
}

// =====================================================
// Unity runner — 5 configuration tests
// =====================================================

void setup()
{
    delay(3000);

    UNITY_BEGIN();

    RUN_TEST(test_configure_rejects_before_begin);
    RUN_TEST(test_configure_three_wire_50hz);
    RUN_TEST(test_configure_reports_write_failure);
    RUN_TEST(test_configure_reports_readback_failure);
    RUN_TEST(test_configure_rejects_readback_mismatch);

    UNITY_END();
}

void loop()
{
}
