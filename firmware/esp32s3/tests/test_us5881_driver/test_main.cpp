#include <Arduino.h>
#include <unity.h>
#include <stdint.h>

#include "../../drivers/us5881/us5881_driver.h"

static US5881Driver driver;

static hw_timer_t* testTimer = nullptr;

static volatile uint32_t isrAttempts = 0;
static volatile uint32_t isrAccepted = 0;
static volatile uint32_t isrLimit = 0;
static volatile bool isrEnabled = false;
static volatile uint32_t isrGeneration = 0;

void ARDUINO_ISR_ATTR onTestTimer()
{
    if (!isrEnabled)
    {
        return;
    }

    if (isrAttempts >= isrLimit)
    {
        return;
    }

    const uint32_t sequence =
        isrAttempts + 1;

    const uint64_t timestamp_us =
        static_cast<uint64_t>(
            sequence
        ) * 1000ULL;

    isrAttempts++;

    if (
        driver.recordFallingEdgeFromISR(timestamp_us, isrGeneration)
    )
    {
        isrAccepted++;
    }
}

bool startTestTimer(
    uint32_t eventLimit,
    uint64_t periodUs = 1000ULL,
    uint32_t token = UINT32_MAX
)
{
    isrAttempts = 0;
    isrAccepted = 0;
    isrLimit = eventLimit;
    isrEnabled = true;
    isrGeneration = (token == UINT32_MAX) ? driver.irqGeneration() : token;

    testTimer = timerBegin(
            0,
            80,
            true
        );

    if (testTimer == nullptr)
    {
        isrEnabled = false;
        return false;
    }

    timerAttachInterrupt(
        testTimer,
        &onTestTimer,
        true
    );

    timerAlarmWrite(
        testTimer,
        periodUs,
        true
    );

    timerAlarmEnable(
        testTimer
    );

    return true;
}

void stopTestTimer()
{
    isrEnabled = false;

    if (testTimer == nullptr)
    {
        return;
    }

    timerAlarmDisable(
        testTimer
    );

    timerDetachInterrupt(
        testTimer
    );

    timerEnd(
        testTimer
    );

    testTimer = nullptr;
}

bool waitForIsrAttempts(
    uint32_t target,
    uint32_t timeoutMs
)
{
    const uint32_t start =
        millis();

    while (
        isrAttempts < target
    )
    {
        if (
            millis() - start >=
            timeoutMs
        )
        {
            return false;
        }

        delay(1);
    }

    return true;
}

void setUp()
{
    stopTestTimer();

    driver.end();

    isrAttempts = 0;
    isrAccepted = 0;
    isrLimit = 0;
    isrEnabled = false;
}

void tearDown()
{
    stopTestTimer();
    driver.end();
}

void test_default_state_is_not_initialized()
{
    TEST_ASSERT_FALSE(
        driver.isInitialized()
    );

    TEST_ASSERT_EQUAL_INT(
        -1,
        driver.pin()
    );
}

void test_begin_rejects_negative_pin()
{
    TEST_ASSERT_FALSE(
        driver.begin(
            -1
        )
    );

    TEST_ASSERT_FALSE(
        driver.isInitialized()
    );
}

void test_read_level_fails_before_begin()
{
    uint8_t level = HIGH;

    TEST_ASSERT_FALSE(
        driver.readLevel(
            level
        )
    );
}

void test_read_detected_fails_before_begin()
{
    bool detected = false;

    TEST_ASSERT_FALSE(
        driver.readDetected(
            detected
        )
    );
}

void test_low_level_means_detected()
{
    bool detected = false;

    TEST_ASSERT_TRUE(
        US5881Driver::decodeLevel(
            LOW,
            detected
        )
    );

    TEST_ASSERT_TRUE(
        detected
    );
}

void test_high_level_means_not_detected()
{
    bool detected = true;

    TEST_ASSERT_TRUE(
        US5881Driver::decodeLevel(
            HIGH,
            detected
        )
    );

    TEST_ASSERT_FALSE(
        detected
    );
}

void test_decode_rejects_invalid_level()
{
    bool detected = true;

    TEST_ASSERT_FALSE(
        US5881Driver::decodeLevel(
            2,
            detected
        )
    );
}

void test_edge_rejected_before_begin()
{
    TEST_ASSERT_FALSE(
        driver.recordFallingEdge(
            1000ULL
        )
    );

    const US5881EdgeStats stats =
        driver.edgeStats();

    TEST_ASSERT_TRUE(
        stats.edge_count == 0ULL
    );
}

void test_first_edge_has_no_valid_period()
{
    TEST_ASSERT_TRUE(
        driver.begin(
            1
        )
    );

    TEST_ASSERT_TRUE(
        driver.recordFallingEdge(
            1000ULL
        )
    );

    const US5881EdgeStats stats =
        driver.edgeStats();

    TEST_ASSERT_TRUE(
        stats.edge_count == 1ULL
    );

    TEST_ASSERT_TRUE(
        stats.last_edge_timestamp_us ==
        1000ULL
    );

    TEST_ASSERT_TRUE(
        stats.last_period_us ==
        0ULL
    );

    TEST_ASSERT_FALSE(
        stats.period_valid
    );
}

void test_second_edge_creates_period()
{
    TEST_ASSERT_TRUE(
        driver.begin(
            1
        )
    );

    TEST_ASSERT_TRUE(
        driver.recordFallingEdge(
            1000ULL
        )
    );

    TEST_ASSERT_TRUE(
        driver.recordFallingEdge(
            2500ULL
        )
    );

    const US5881EdgeStats stats =
        driver.edgeStats();

    TEST_ASSERT_TRUE(
        stats.edge_count == 2ULL
    );

    TEST_ASSERT_TRUE(
        stats.last_edge_timestamp_us ==
        2500ULL
    );

    TEST_ASSERT_TRUE(
        stats.last_period_us ==
        1500ULL
    );

    TEST_ASSERT_TRUE(
        stats.period_valid
    );
}

void test_zero_timestamp_is_rejected()
{
    TEST_ASSERT_TRUE(
        driver.begin(
            1
        )
    );

    TEST_ASSERT_FALSE(
        driver.recordFallingEdge(
            0ULL
        )
    );

    const US5881EdgeStats stats =
        driver.edgeStats();

    TEST_ASSERT_TRUE(
        stats.edge_count == 0ULL
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        stats.invalid_timestamp_count
    );
}

void test_duplicate_timestamp_is_rejected()
{
    TEST_ASSERT_TRUE(
        driver.begin(
            1
        )
    );

    TEST_ASSERT_TRUE(
        driver.recordFallingEdge(
            1000ULL
        )
    );

    TEST_ASSERT_FALSE(
        driver.recordFallingEdge(
            1000ULL
        )
    );

    const US5881EdgeStats stats =
        driver.edgeStats();

    TEST_ASSERT_TRUE(
        stats.edge_count == 1ULL
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        stats.invalid_timestamp_count
    );
}

void test_backward_timestamp_is_rejected()
{
    TEST_ASSERT_TRUE(
        driver.begin(
            1
        )
    );

    TEST_ASSERT_TRUE(
        driver.recordFallingEdge(
            2000ULL
        )
    );

    TEST_ASSERT_FALSE(
        driver.recordFallingEdge(
            1500ULL
        )
    );

    const US5881EdgeStats stats =
        driver.edgeStats();

    TEST_ASSERT_TRUE(
        stats.edge_count == 1ULL
    );

    TEST_ASSERT_TRUE(
        stats.last_edge_timestamp_us ==
        2000ULL
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        stats.invalid_timestamp_count
    );
}

void test_reset_edges_clears_edge_state()
{
    TEST_ASSERT_TRUE(
        driver.begin(
            1
        )
    );

    TEST_ASSERT_TRUE(
        driver.recordFallingEdge(
            1000ULL
        )
    );

    TEST_ASSERT_TRUE(
        driver.recordFallingEdge(
            2000ULL
        )
    );

    driver.resetEdges();

    const US5881EdgeStats stats =
        driver.edgeStats();

    TEST_ASSERT_TRUE(
        stats.edge_count == 0ULL
    );

    TEST_ASSERT_TRUE(
        stats.last_edge_timestamp_us ==
        0ULL
    );

    TEST_ASSERT_TRUE(
        stats.last_period_us ==
        0ULL
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        stats.invalid_timestamp_count
    );

    TEST_ASSERT_FALSE(
        stats.period_valid
    );
}

void test_end_clears_state()
{
    TEST_ASSERT_TRUE(
        driver.begin(
            1
        )
    );

    TEST_ASSERT_TRUE(
        driver.recordFallingEdge(
            1000ULL
        )
    );

    driver.end();

    TEST_ASSERT_FALSE(
        driver.isInitialized()
    );

    TEST_ASSERT_EQUAL_INT(
        -1,
        driver.pin()
    );

    const US5881EdgeStats stats =
        driver.edgeStats();

    TEST_ASSERT_TRUE(
        stats.edge_count == 0ULL
    );
}

void test_hardware_timer_isr_records_edges()
{
    const uint32_t eventCount = 8;

    TEST_ASSERT_TRUE(
        driver.begin(
            1
        )
    );

    TEST_ASSERT_TRUE(
        startTestTimer(
            eventCount
        )
    );

    TEST_ASSERT_TRUE(
        waitForIsrAttempts(
            eventCount,
            1000
        )
    );

    stopTestTimer();

    TEST_ASSERT_EQUAL_UINT32(
        eventCount,
        isrAttempts
    );

    TEST_ASSERT_EQUAL_UINT32(
        eventCount,
        isrAccepted
    );

    const US5881EdgeStats stats =
        driver.edgeStats();

    TEST_ASSERT_TRUE(
        stats.edge_count ==
        static_cast<uint64_t>(
            eventCount
        )
    );

    TEST_ASSERT_TRUE(
        stats.last_edge_timestamp_us ==
        static_cast<uint64_t>(
            eventCount
        ) * 1000ULL
    );

    TEST_ASSERT_TRUE(
        stats.last_period_us ==
        1000ULL
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        stats.invalid_timestamp_count
    );

    TEST_ASSERT_TRUE(
        stats.period_valid
    );
}

// =====================================================
// NEW REGRESSION TESTS — REVIEW #1 / PR #27
// =====================================================

// MAJOR: Out-of-range GPIO must not mark ready.
void test_begin_rejects_out_of_range_gpio()
{
    TEST_ASSERT_FALSE(driver.begin(999));

    TEST_ASSERT_FALSE(driver.isInitialized());
    TEST_ASSERT_EQUAL_INT(-1, driver.pin());
    TEST_ASSERT_EQUAL_UINT32(
        0,
        driver.irqGeneration()
    );
}

// ESP32-S3 GPIO 22 is not a valid GPIO.
// This test is specific to the locked ESP32-S3 target.
void test_begin_rejects_unsupported_esp32s3_gpio()
{
    TEST_ASSERT_FALSE(driver.begin(22));

    TEST_ASSERT_FALSE(driver.isInitialized());
    TEST_ASSERT_EQUAL_INT(-1, driver.pin());
}

// Rebinding while initialized must be explicitly rejected.
void test_rebind_requires_end()
{
    TEST_ASSERT_TRUE(driver.begin(1));

    const uint32_t generation =
        driver.irqGeneration();

    TEST_ASSERT_TRUE(generation != 0);

    TEST_ASSERT_FALSE(driver.begin(2));

    TEST_ASSERT_TRUE(driver.isInitialized());
    TEST_ASSERT_EQUAL_INT(1, driver.pin());

    TEST_ASSERT_EQUAL_UINT32(
        generation,
        driver.irqGeneration()
    );
}

// Lifecycle: ending the driver invalidates old ISR tokens.
void test_old_irq_rejected_after_end()
{
    const uint32_t eventCount = 4;

    TEST_ASSERT_TRUE(driver.begin(1));

    const uint32_t oldGeneration =
        driver.irqGeneration();

    TEST_ASSERT_TRUE(oldGeneration != 0);

    // In production the application must quiesce and
    // detach GPIO IRQ before end().
    driver.end();

    TEST_ASSERT_TRUE(
        startTestTimer(
            eventCount,
            1000ULL,
            oldGeneration
        )
    );

    const bool completed =
        waitForIsrAttempts(eventCount, 1000);

    stopTestTimer();

    TEST_ASSERT_TRUE(completed);

    TEST_ASSERT_EQUAL_UINT32(
        eventCount,
        isrAttempts
    );

    TEST_ASSERT_EQUAL_UINT32(0, isrAccepted);

    const US5881EdgeStats stats = driver.edgeStats();

    TEST_ASSERT_TRUE(stats.edge_count == 0ULL);
}

// MAJOR: callback captured on old GPIO must not
// record an edge after a new GPIO binding.
void test_stale_irq_rejected_after_rebind()
{
    const uint32_t eventCount = 4;

    TEST_ASSERT_TRUE(driver.begin(1));

    const uint32_t oldGeneration =
        driver.irqGeneration();

    TEST_ASSERT_TRUE(oldGeneration != 0);

    driver.end();

    TEST_ASSERT_TRUE(driver.begin(2));

    const uint32_t newGeneration =
        driver.irqGeneration();

    TEST_ASSERT_TRUE(newGeneration != 0);

    TEST_ASSERT_TRUE(
        newGeneration != oldGeneration
    );

    // Synthetic timer calls the ISR API using
    // a token captured before the rebind.
    TEST_ASSERT_TRUE(
        startTestTimer(
            eventCount,
            1000ULL,
            oldGeneration
        )
    );

    const bool oldCompleted =
        waitForIsrAttempts(eventCount, 1000);

    stopTestTimer();

    TEST_ASSERT_TRUE(oldCompleted);

    TEST_ASSERT_EQUAL_UINT32(
        eventCount,
        isrAttempts
    );

    TEST_ASSERT_EQUAL_UINT32(0, isrAccepted);

    TEST_ASSERT_TRUE(
        driver.edgeStats().edge_count == 0ULL
    );

    // A callback using the new token is accepted.
    TEST_ASSERT_TRUE(
        startTestTimer(
            eventCount,
            1000ULL,
            newGeneration
        )
    );

    const bool newCompleted =
        waitForIsrAttempts(eventCount, 1000);

    stopTestTimer();

    TEST_ASSERT_TRUE(newCompleted);

    TEST_ASSERT_EQUAL_UINT32(
        eventCount,
        isrAccepted
    );

    TEST_ASSERT_TRUE(
        driver.edgeStats().edge_count ==
            static_cast<uint64_t>(eventCount)
    );

    TEST_ASSERT_EQUAL_INT(2, driver.pin());
}

// A zero token can never authorize an ISR edge.
void test_zero_irq_token_is_rejected()
{
    const uint32_t eventCount = 4;

    TEST_ASSERT_TRUE(driver.begin(1));

    TEST_ASSERT_TRUE(
        startTestTimer(
            eventCount,
            1000ULL,
            0
        )
    );

    const bool completed =
        waitForIsrAttempts(eventCount, 1000);

    stopTestTimer();

    TEST_ASSERT_TRUE(completed);

    TEST_ASSERT_EQUAL_UINT32(0, isrAccepted);

    TEST_ASSERT_TRUE(
        driver.edgeStats().edge_count == 0ULL
    );
}

void setup()
{
    delay(3000);

    UNITY_BEGIN();

    RUN_TEST(test_default_state_is_not_initialized);
    RUN_TEST(test_begin_rejects_negative_pin);
    RUN_TEST(test_read_level_fails_before_begin);
    RUN_TEST(test_read_detected_fails_before_begin);
    RUN_TEST(test_low_level_means_detected);
    RUN_TEST(test_high_level_means_not_detected);
    RUN_TEST(test_decode_rejects_invalid_level);
    RUN_TEST(test_edge_rejected_before_begin);
    RUN_TEST(test_first_edge_has_no_valid_period);
    RUN_TEST(test_second_edge_creates_period);
    RUN_TEST(test_zero_timestamp_is_rejected);
    RUN_TEST(test_duplicate_timestamp_is_rejected);
    RUN_TEST(test_backward_timestamp_is_rejected);
    RUN_TEST(test_reset_edges_clears_edge_state);
    RUN_TEST(test_end_clears_state);
    RUN_TEST(test_hardware_timer_isr_records_edges);
    RUN_TEST(test_begin_rejects_out_of_range_gpio);
    RUN_TEST(test_begin_rejects_unsupported_esp32s3_gpio);
    RUN_TEST(test_rebind_requires_end);
    RUN_TEST(test_old_irq_rejected_after_end);
    RUN_TEST(test_stale_irq_rejected_after_rebind);
    RUN_TEST(test_zero_irq_token_is_rejected);

    UNITY_END();
}

void loop()
{
}
