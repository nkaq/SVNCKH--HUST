#include <Arduino.h>
#include <unity.h>

#include "../../acquisition/timebase/acquisition_timebase.h"

static AcquisitionTimebase timebase;

void setUp()
{
    timebase.reset();
}

void tearDown()
{
}

void test_first_stamp_starts_at_zero()
{
    uint32_t sampleIndex = 999;

    TEST_ASSERT_TRUE(
        timebase.stamp(
            1000ULL,
            sampleIndex
        )
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        sampleIndex
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        timebase.nextSampleIndex()
    );

    TEST_ASSERT_TRUE(
        timebase.lastTimestampUs() == 1000ULL
    );

    TEST_ASSERT_TRUE(
        timebase.started()
    );
}

void test_valid_timestamps_increment_index()
{
    uint32_t sampleIndex = 0;

    TEST_ASSERT_TRUE(
        timebase.stamp(
            1000ULL,
            sampleIndex
        )
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        sampleIndex
    );

    TEST_ASSERT_TRUE(
        timebase.stamp(
            2000ULL,
            sampleIndex
        )
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        sampleIndex
    );

    TEST_ASSERT_TRUE(
        timebase.stamp(
            3000ULL,
            sampleIndex
        )
    );

    TEST_ASSERT_EQUAL_UINT32(
        2,
        sampleIndex
    );

    TEST_ASSERT_EQUAL_UINT32(
        3,
        timebase.nextSampleIndex()
    );
}

void test_duplicate_timestamp_is_rejected_without_consuming_index()
{
    uint32_t sampleIndex = 0;

    TEST_ASSERT_TRUE(
        timebase.stamp(
            1000ULL,
            sampleIndex
        )
    );

    TEST_ASSERT_FALSE(
        timebase.stamp(
            1000ULL,
            sampleIndex
        )
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        timebase.nextSampleIndex()
    );

    TEST_ASSERT_TRUE(
        timebase.lastTimestampUs() == 1000ULL
    );
}

void test_backward_timestamp_is_rejected_without_consuming_index()
{
    uint32_t sampleIndex = 0;

    TEST_ASSERT_TRUE(
        timebase.stamp(
            2000ULL,
            sampleIndex
        )
    );

    TEST_ASSERT_FALSE(
        timebase.stamp(
            1500ULL,
            sampleIndex
        )
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        timebase.nextSampleIndex()
    );

    TEST_ASSERT_TRUE(
        timebase.lastTimestampUs() == 2000ULL
    );
}

void test_stamp_now_is_monotonic()
{
    uint32_t firstIndex = 0;
    uint64_t firstTimestamp = 0;

    uint32_t secondIndex = 0;
    uint64_t secondTimestamp = 0;

    TEST_ASSERT_TRUE(
        timebase.stampNow(
            firstIndex,
            firstTimestamp
        )
    );

    delayMicroseconds(10);

    TEST_ASSERT_TRUE(
        timebase.stampNow(
            secondIndex,
            secondTimestamp
        )
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        firstIndex
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        secondIndex
    );

    TEST_ASSERT_TRUE(
        secondTimestamp > firstTimestamp
    );
}

void test_reset_clears_state()
{
    uint32_t sampleIndex = 0;

    TEST_ASSERT_TRUE(
        timebase.stamp(
            1000ULL,
            sampleIndex
        )
    );

    timebase.reset();

    TEST_ASSERT_EQUAL_UINT32(
        0,
        timebase.nextSampleIndex()
    );

    TEST_ASSERT_TRUE(
        timebase.lastTimestampUs() == 0ULL
    );

    TEST_ASSERT_FALSE(
        timebase.started()
    );

    TEST_ASSERT_TRUE(
        timebase.stamp(
            500ULL,
            sampleIndex
        )
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        sampleIndex
    );
}

void setup()
{
    delay(3000);

    UNITY_BEGIN();

    RUN_TEST(test_first_stamp_starts_at_zero);
    RUN_TEST(test_valid_timestamps_increment_index);
    RUN_TEST(test_duplicate_timestamp_is_rejected_without_consuming_index);
    RUN_TEST(test_backward_timestamp_is_rejected_without_consuming_index);
    RUN_TEST(test_stamp_now_is_monotonic);
    RUN_TEST(test_reset_clears_state);

    UNITY_END();
}

void loop()
{
}