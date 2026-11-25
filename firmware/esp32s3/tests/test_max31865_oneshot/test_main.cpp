
#include <Arduino.h>
#include <unity.h>
#include <math.h>

#include "../../drivers/max31865/max31865_driver.h"

// =====================================================
// Fake MAX31865 Bus
// =====================================================

class FakeBus : public IMax31865Bus
{
public:
    uint8_t config = 0;
    uint8_t rtd[2] = {0x80, 0x00};
    uint8_t fault = 0;

    bool fail_write = false;
    bool fail_config_read = false;
    bool fail_rtd_read = false;
    bool fail_fault_read = false;
    bool fail_trigger = false;

    uint32_t writes = 0;
    uint32_t triggers = 0;
    uint32_t rtd_reads = 0;
    uint32_t fault_reads = 0;

    uint8_t last_write = 0;

    bool readRegisters(
        uint8_t addr,
        uint8_t* dst,
        size_t len
    ) override
    {
        if (dst == nullptr)
        {
            return false;
        }

        if (addr == 0x00 && len == 1)
        {
            if (fail_config_read)
            {
                return false;
            }

            dst[0] = config;
            return true;
        }

        if (addr == 0x01 && len == 2)
        {
            ++rtd_reads;

            if (fail_rtd_read)
            {
                return false;
            }

            dst[0] = rtd[0];
            dst[1] = rtd[1];

            return true;
        }

        if (addr == 0x07 && len == 1)
        {
            ++fault_reads;

            if (fail_fault_read)
            {
                return false;
            }

            dst[0] = fault;
            return true;
        }

        return false;
    }

    bool writeRegister(
        uint8_t addr,
        uint8_t value
    ) override
    {
        ++writes;
        last_write = value;

        if (fail_write || addr != 0x00)
        {
            return false;
        }

        if ((value & 0x20) != 0)
        {
            if (fail_trigger)
            {
                return false;
            }

            ++triggers;
        }

        // Keep persistent configuration bits.
        config = static_cast<uint8_t>(
            value & 0xD1
        );

        // Fault clear bit.
        if ((value & 0x02) != 0)
        {
            fault = 0;
        }

        return true;
    }
};

static FakeBus bus;
static Max31865Driver driver;

void setUp()
{
    driver.end();
    bus = FakeBus();
}

void tearDown()
{
    driver.end();
}

// =====================================================
// Helper: configure a one-shot sensor
// =====================================================

static void configured(
    Max31865Filter filter = Max31865Filter::Hz60
)
{
    TEST_ASSERT_TRUE(
        driver.begin(bus, 430.0f)
    );

    Max31865Config config;
    config.filter = filter;

    TEST_ASSERT_TRUE(
        driver.configure(config)
    );
}

// =====================================================
// TEST 01 — Reject unconfigured shot
// =====================================================

void test_rejects_unconfigured_shot()
{
    TEST_ASSERT_TRUE(
        driver.begin(bus, 430.0f)
    );

    TEST_ASSERT_FALSE(
        driver.startOneShot(0, 12)
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        bus.triggers
    );
}

// =====================================================
// TEST 02 — Reject zero VBIAS settle time
// =====================================================

void test_rejects_zero_settle_time()
{
    configured();

    TEST_ASSERT_FALSE(
        driver.startOneShot(0, 0)
    );
}

// =====================================================
// TEST 03 — Reject incompatible modes
// =====================================================

void test_rejects_auto_mode_and_disabled_bias()
{
    TEST_ASSERT_TRUE(
        driver.begin(bus, 430.0f)
    );

    Max31865Config cfg;

    cfg.conversion =
        Max31865ConversionMode::Automatic;

    TEST_ASSERT_TRUE(
        driver.configure(cfg)
    );

    TEST_ASSERT_FALSE(
        driver.startOneShot(0, 12)
    );

    cfg.conversion =
        Max31865ConversionMode::NormallyOff;

    cfg.bias_enabled = false;

    TEST_ASSERT_TRUE(
        driver.configure(cfg)
    );

    TEST_ASSERT_FALSE(
        driver.startOneShot(0, 12)
    );
}

// =====================================================
// TEST 04 — 60 Hz conversion timing
// =====================================================

void test_waits_for_bias_and_conversion_60hz()
{
    configured();

    TEST_ASSERT_TRUE(
        driver.startOneShot(100, 12)
    );

    TEST_ASSERT_TRUE(
        driver.oneShotActive()
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        bus.triggers
    );

    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(
            Max31865ReadStatus::NotReady
        ),
        static_cast<int>(
            driver.pollOneShot(111).status
        )
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        bus.triggers
    );

    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(
            Max31865ReadStatus::NotReady
        ),
        static_cast<int>(
            driver.pollOneShot(112).status
        )
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        bus.triggers
    );

    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(
            Max31865ReadStatus::NotReady
        ),
        static_cast<int>(
            driver.pollOneShot(166).status
        )
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        bus.rtd_reads
    );

    const Max31865Sample sample =
        driver.pollOneShot(167);

    TEST_ASSERT_TRUE(sample.valid);

    TEST_ASSERT_FLOAT_WITHIN(
        0.001f,
        215.0f,
        sample.resistance_ohms
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        bus.rtd_reads
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        bus.fault_reads
    );

    TEST_ASSERT_FALSE(
        driver.oneShotActive()
    );
}

// =====================================================
// TEST 05 — 50 Hz conversion timing
// =====================================================


void test_50hz_waits_65ms()
{
    configured(Max31865Filter::Hz50);

    TEST_ASSERT_TRUE(
        driver.startOneShot(0, 12)
    );

    // VBIAS settling completes at t = 12 ms.
    // Conversion is triggered here.
    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(
            Max31865ReadStatus::NotReady
        ),
        static_cast<int>(
            driver.pollOneShot(12).status
        )
    );

    // 65 ms elapsed: conversion still pending.
    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(
            Max31865ReadStatus::NotReady
        ),
        static_cast<int>(
            driver.pollOneShot(77).status
        )
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        bus.rtd_reads
    );

    // 66 ms elapsed: read is allowed.
    const Max31865Sample sample =
        driver.pollOneShot(78);

    TEST_ASSERT_TRUE(sample.valid);

    TEST_ASSERT_EQUAL_UINT32(
        1,
        bus.rtd_reads
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        bus.fault_reads
    );
}


// =====================================================
// TEST 06 — Consume sample only once
// =====================================================

void test_sample_consumed_once_and_restart()
{
    configured();

    TEST_ASSERT_TRUE(
        driver.startOneShot(0, 12)
    );

    TEST_ASSERT_FALSE(
        driver.startOneShot(2, 12)
    );

    driver.pollOneShot(12);

    TEST_ASSERT_TRUE(
        driver.pollOneShot(67).valid
    );

    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(
            Max31865ReadStatus::InvalidState
        ),
        static_cast<int>(
            driver.pollOneShot(1000).status
        )
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        bus.rtd_reads
    );

    TEST_ASSERT_TRUE(
        driver.startOneShot(1001, 12)
    );

    driver.pollOneShot(1013);

    TEST_ASSERT_TRUE(
        driver.pollOneShot(1068).valid
    );

    TEST_ASSERT_EQUAL_UINT32(
        2,
        bus.rtd_reads
    );
}

// =====================================================
// TEST 07 — Fault Status without RTD fault bit
// =====================================================

void test_fault_status_without_rtd_fault_bit_is_rejected()
{
    configured();

    TEST_ASSERT_TRUE(
        driver.startOneShot(0, 12)
    );

    driver.pollOneShot(12);

    // Inject a new fault after clearing old faults.
    bus.fault = 0x80;

    const Max31865Sample sample =
        driver.pollOneShot(67);

    TEST_ASSERT_FALSE(sample.valid);

    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(
            Max31865ReadStatus::SensorFault
        ),
        static_cast<int>(sample.status)
    );

    TEST_ASSERT_EQUAL_HEX8(
        0x80,
        sample.fault_status
    );

    TEST_ASSERT_TRUE(
        isnan(sample.resistance_ohms)
    );
}

// =====================================================
// TEST 08 — One-shot trigger write failure
// =====================================================

void test_trigger_write_failure_consumes_request()
{
    configured();

    TEST_ASSERT_TRUE(
        driver.startOneShot(0, 12)
    );

    bus.fail_trigger = true;

    const Max31865Sample sample =
        driver.pollOneShot(12);

    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(
            Max31865ReadStatus::TransportError
        ),
        static_cast<int>(sample.status)
    );

    TEST_ASSERT_FALSE(
        driver.oneShotActive()
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        bus.rtd_reads
    );
}

// =====================================================
// TEST 09 — Fault Status read failure
// =====================================================

void test_fault_register_read_failure_is_fail_closed()
{
    configured();

    TEST_ASSERT_TRUE(
        driver.startOneShot(0, 12)
    );

    driver.pollOneShot(12);

    bus.fail_fault_read = true;

    const Max31865Sample sample =
        driver.pollOneShot(67);

    TEST_ASSERT_FALSE(sample.valid);

    TEST_ASSERT_TRUE(
        isnan(sample.resistance_ohms)
    );

    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(
            Max31865ReadStatus::TransportError
        ),
        static_cast<int>(sample.status)
    );
}

// =====================================================
// TEST 10 — Reconfigure cancels active conversion
// =====================================================

void test_reconfigure_cancels_pending_conversion()
{
    configured();

    TEST_ASSERT_TRUE(
        driver.startOneShot(0, 12)
    );

    Max31865Config cfg;
    cfg.filter = Max31865Filter::Hz50;

    TEST_ASSERT_TRUE(
        driver.configure(cfg)
    );

    TEST_ASSERT_FALSE(
        driver.oneShotActive()
    );

    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(
            Max31865ReadStatus::InvalidState
        ),
        static_cast<int>(
            driver.pollOneShot(500).status
        )
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        bus.rtd_reads
    );
}

// =====================================================
// TEST 11 — End cancels active conversion
// =====================================================

void test_end_cancels_pending_conversion()
{
    configured();

    TEST_ASSERT_TRUE(
        driver.startOneShot(0, 12)
    );

    driver.end();

    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(
            Max31865ReadStatus::NotInitialized
        ),
        static_cast<int>(
            driver.pollOneShot(500).status
        )
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        bus.rtd_reads
    );
}

// =====================================================
// TEST 12 — millis() wraparound
// =====================================================

void test_millis_wraparound()
{
    configured();

    const uint32_t near_wrap = 0xFFFFFFFCu;

    TEST_ASSERT_TRUE(
        driver.startOneShot(near_wrap, 12)
    );

    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(
            Max31865ReadStatus::NotReady
        ),
        static_cast<int>(
            driver.pollOneShot(5).status
        )
    );

    driver.pollOneShot(8);

    TEST_ASSERT_EQUAL_UINT32(
        1,
        bus.triggers
    );

    TEST_ASSERT_TRUE(
        driver.pollOneShot(63).valid
    );
}

// =====================================================
// TEST 13 — Clear previous fault
// =====================================================

void test_clears_previous_fault_before_new_shot()
{
    configured();

    bus.fault = 0x80;

    TEST_ASSERT_TRUE(
        driver.startOneShot(0, 12)
    );

    TEST_ASSERT_EQUAL_HEX8(
        0x00,
        bus.fault
    );

    driver.pollOneShot(12);

    TEST_ASSERT_TRUE(
        driver.pollOneShot(67).valid
    );
}

// =====================================================
// TEST 14 — Clear fault write failure
// =====================================================

void test_clear_fault_write_failure_does_not_arm()
{
    configured();

    bus.fail_write = true;

    TEST_ASSERT_FALSE(
        driver.startOneShot(0, 12)
    );

    TEST_ASSERT_FALSE(
        driver.oneShotActive()
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        bus.rtd_reads
    );
}

// =====================================================
// Unity runner — 14 tests
// =====================================================

void setup()
{
    delay(3000);

    UNITY_BEGIN();

    RUN_TEST(test_rejects_unconfigured_shot);
    RUN_TEST(test_rejects_zero_settle_time);
    RUN_TEST(test_rejects_auto_mode_and_disabled_bias);

    RUN_TEST(test_waits_for_bias_and_conversion_60hz);
    RUN_TEST(test_50hz_waits_65ms);

    RUN_TEST(test_sample_consumed_once_and_restart);

    RUN_TEST(
        test_fault_status_without_rtd_fault_bit_is_rejected
    );

    RUN_TEST(
        test_trigger_write_failure_consumes_request
    );

    RUN_TEST(
        test_fault_register_read_failure_is_fail_closed
    );

    RUN_TEST(test_reconfigure_cancels_pending_conversion);
    RUN_TEST(test_end_cancels_pending_conversion);

    RUN_TEST(test_millis_wraparound);

    RUN_TEST(test_clears_previous_fault_before_new_shot);
    RUN_TEST(test_clear_fault_write_failure_does_not_arm);

    UNITY_END();
}

void loop()
{
}
