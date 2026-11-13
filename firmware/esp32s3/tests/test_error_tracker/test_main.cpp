#include <Arduino.h>
#include <unity.h>

#include "../../acquisition/diagnostics/error_tracker.h"

static ErrorTracker errorTracker;

void setUp()
{
    errorTracker.reset();
}

void tearDown()
{
}

void test_reset_clears_all_state()
{
    errorTracker.record(
        ErrorCode::I2C_ERROR
    );

    errorTracker.record(
        ErrorCode::TIMESTAMP_ERROR
    );

    errorTracker.recordDroppedSamples(
        3
    );

    errorTracker.recordRecoveryAttempt();
    errorTracker.recordRecoverySuccess();

    errorTracker.reset();

    const ErrorCounters& counters =
        errorTracker.counters();

    TEST_ASSERT_EQUAL_UINT32(
        0,
        counters.total_errors
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        counters.i2c_errors
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        counters.timestamp_errors
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        counters.dropped_samples
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        counters.recovery_attempts
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        counters.recovery_successes
    );

    TEST_ASSERT_EQUAL(
        static_cast<int>(
            ErrorCode::NONE
        ),
        static_cast<int>(
            errorTracker.lastError()
        )
    );
}

void test_error_types_increment_correct_counters()
{
    errorTracker.record(
        ErrorCode::I2C_ERROR
    );

    errorTracker.record(
        ErrorCode::TIMEOUT
    );

    errorTracker.record(
        ErrorCode::SENSOR_DISCONNECT
    );

    errorTracker.record(
        ErrorCode::BUFFER_OVERFLOW
    );

    errorTracker.record(
        ErrorCode::FIFO_OVERRUN
    );

    errorTracker.record(
        ErrorCode::INVALID_STATE
    );

    errorTracker.record(
        ErrorCode::INVALID_CONFIG
    );

    errorTracker.record(
        ErrorCode::TIMESTAMP_ERROR
    );

    const ErrorCounters& counters =
        errorTracker.counters();

    TEST_ASSERT_EQUAL_UINT32(
        8,
        counters.total_errors
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        counters.i2c_errors
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        counters.timeouts
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        counters.disconnects
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        counters.buffer_overflows
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        counters.fifo_overruns
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        counters.invalid_state
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        counters.invalid_config
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        counters.timestamp_errors
    );
}

void test_none_does_not_increment_error_count()
{
    errorTracker.record(
        ErrorCode::NONE
    );

    const ErrorCounters& counters =
        errorTracker.counters();

    TEST_ASSERT_EQUAL_UINT32(
        0,
        counters.total_errors
    );

    TEST_ASSERT_EQUAL(
        static_cast<int>(
            ErrorCode::NONE
        ),
        static_cast<int>(
            errorTracker.lastError()
        )
    );
}

void test_dropped_samples_are_counted()
{
    errorTracker.recordDroppedSamples();
    errorTracker.recordDroppedSamples(
        4
    );

    TEST_ASSERT_EQUAL_UINT32(
        5,
        errorTracker.counters().dropped_samples
    );
}

void test_recovery_counters_are_independent()
{
    errorTracker.recordRecoveryAttempt();
    errorTracker.recordRecoveryAttempt();

    errorTracker.recordRecoverySuccess();

    errorTracker.recordRecoveryFailure();

    const ErrorCounters& counters =
        errorTracker.counters();

    TEST_ASSERT_EQUAL_UINT32(
        2,
        counters.recovery_attempts
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        counters.recovery_successes
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        counters.recovery_failures
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        counters.total_errors
    );
}

void setup()
{
    delay(3000);

    UNITY_BEGIN();

    RUN_TEST(test_reset_clears_all_state);
    RUN_TEST(test_error_types_increment_correct_counters);
    RUN_TEST(test_none_does_not_increment_error_count);
    RUN_TEST(test_dropped_samples_are_counted);
    RUN_TEST(test_recovery_counters_are_independent);

    UNITY_END();
}

void loop()
{
}