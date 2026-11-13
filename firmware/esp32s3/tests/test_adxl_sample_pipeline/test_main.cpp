#include <Arduino.h>
#include <unity.h>

#include "../../acquisition/pipeline/adxl_sample_pipeline.h"

static AdxlSamplePipeline pipeline;

void setUp()
{
    pipeline.reset();
}

void tearDown()
{
}

void test_submit_creates_expected_packet()
{
    RawSample raw = {};

    raw.x = 100;
    raw.y = -200;
    raw.z = 300;

    TEST_ASSERT_TRUE(
        pipeline.submit(
            raw,
            1000ULL,
            STATUS_SENSOR_WARNING
        )
    );

    TEST_ASSERT_EQUAL_UINT16(
        1,
        pipeline.size()
    );

    SamplePacket packet = {};

    TEST_ASSERT_TRUE(
        pipeline.pop(packet)
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        packet.sample_index
    );

    TEST_ASSERT_TRUE(
        packet.timestamp_us == 1000ULL
    );

    TEST_ASSERT_EQUAL_INT16(
        100,
        packet.x_raw
    );

    TEST_ASSERT_EQUAL_INT16(
        -200,
        packet.y_raw
    );

    TEST_ASSERT_EQUAL_INT16(
        300,
        packet.z_raw
    );

    TEST_ASSERT_EQUAL_UINT16(
        STATUS_SENSOR_WARNING,
        packet.status_flags
    );
}

void test_valid_samples_receive_monotonic_indices()
{
    RawSample raw = {};

    TEST_ASSERT_TRUE(
        pipeline.submit(
            raw,
            1000ULL
        )
    );

    TEST_ASSERT_TRUE(
        pipeline.submit(
            raw,
            2000ULL
        )
    );

    TEST_ASSERT_TRUE(
        pipeline.submit(
            raw,
            3000ULL
        )
    );

    SamplePacket packet = {};

    TEST_ASSERT_TRUE(
        pipeline.pop(packet)
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        packet.sample_index
    );

    TEST_ASSERT_TRUE(
        pipeline.pop(packet)
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        packet.sample_index
    );

    TEST_ASSERT_TRUE(
        pipeline.pop(packet)
    );

    TEST_ASSERT_EQUAL_UINT32(
        2,
        packet.sample_index
    );
}

void test_invalid_timestamp_is_rejected_and_counted()
{
    RawSample raw = {};

    TEST_ASSERT_TRUE(
        pipeline.submit(
            raw,
            2000ULL
        )
    );

    TEST_ASSERT_FALSE(
        pipeline.submit(
            raw,
            2000ULL
        )
    );

    TEST_ASSERT_FALSE(
        pipeline.submit(
            raw,
            1500ULL
        )
    );

    const ErrorCounters& errors =
        pipeline.errors();

    TEST_ASSERT_EQUAL_UINT32(
        2,
        errors.total_errors
    );

    TEST_ASSERT_EQUAL_UINT32(
        2,
        errors.timestamp_errors
    );

    TEST_ASSERT_EQUAL_UINT32(
        2,
        errors.dropped_samples
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        pipeline.nextSampleIndex()
    );

    TEST_ASSERT_EQUAL_UINT16(
        1,
        pipeline.size()
    );
}

void test_buffer_overflow_is_rejected_and_counted()
{
    RawSample raw = {};

    uint64_t timestamp = 1000ULL;

    for (
        uint16_t i = 0;
        i < Config::RING_BUFFER_CAPACITY;
        i++
    )
    {
        TEST_ASSERT_TRUE(
            pipeline.submit(
                raw,
                timestamp
            )
        );

        timestamp += 1000ULL;
    }

    TEST_ASSERT_TRUE(
        pipeline.full()
    );

    TEST_ASSERT_FALSE(
        pipeline.submit(
            raw,
            timestamp
        )
    );

    const ErrorCounters& errors =
        pipeline.errors();

    TEST_ASSERT_EQUAL_UINT32(
        1,
        errors.total_errors
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        errors.buffer_overflows
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        errors.dropped_samples
    );

    TEST_ASSERT_EQUAL_UINT32(
        Config::RING_BUFFER_CAPACITY + 1,
        pipeline.nextSampleIndex()
    );

    TEST_ASSERT_EQUAL_UINT16(
        Config::RING_BUFFER_CAPACITY,
        pipeline.size()
    );
}

void test_overflowed_sample_is_not_silently_inserted()
{
    RawSample raw = {};

    uint64_t timestamp = 1000ULL;

    for (
        uint16_t i = 0;
        i < Config::RING_BUFFER_CAPACITY;
        i++
    )
    {
        raw.x =
            static_cast<int16_t>(i);

        TEST_ASSERT_TRUE(
            pipeline.submit(
                raw,
                timestamp
            )
        );

        timestamp += 1000ULL;
    }

    raw.x = 12345;

    TEST_ASSERT_FALSE(
        pipeline.submit(
            raw,
            timestamp
        )
    );

    SamplePacket packet = {};

    uint32_t expected_index = 0;

    while (
        pipeline.pop(packet)
    )
    {
        TEST_ASSERT_EQUAL_UINT32(
            expected_index,
            packet.sample_index
        );

        TEST_ASSERT_TRUE(
            packet.x_raw != 12345
        );

        expected_index++;
    }

    TEST_ASSERT_EQUAL_UINT32(
        Config::RING_BUFFER_CAPACITY,
        expected_index
    );
}

void test_reset_clears_pipeline_state()
{
    RawSample raw = {};

    TEST_ASSERT_TRUE(
        pipeline.submit(
            raw,
            1000ULL
        )
    );

    TEST_ASSERT_FALSE(
        pipeline.submit(
            raw,
            1000ULL
        )
    );

    pipeline.reset();

    TEST_ASSERT_TRUE(
        pipeline.empty()
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        pipeline.nextSampleIndex()
    );

    TEST_ASSERT_TRUE(
        pipeline.lastTimestampUs() == 0ULL
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        pipeline.errors().total_errors
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        pipeline.errors().dropped_samples
    );
}

void setup()
{
    delay(3000);

    UNITY_BEGIN();

    RUN_TEST(test_submit_creates_expected_packet);
    RUN_TEST(test_valid_samples_receive_monotonic_indices);
    RUN_TEST(test_invalid_timestamp_is_rejected_and_counted);
    RUN_TEST(test_buffer_overflow_is_rejected_and_counted);
    RUN_TEST(test_overflowed_sample_is_not_silently_inserted);
    RUN_TEST(test_reset_clears_pipeline_state);

    UNITY_END();
}

void loop()
{
}