#include <Arduino.h>
#include <unity.h>

#include "../../acquisition/buffer/sample_ring_buffer.h"

static SampleRingBuffer ringBuffer;

void setUp()
{
    ringBuffer.clear();
    ringBuffer.resetStats();
}

void tearDown()
{
}

void test_empty_buffer_fails_pop_and_peek()
{
    SamplePacket packet = {};

    TEST_ASSERT_TRUE(ringBuffer.empty());
    TEST_ASSERT_EQUAL_UINT16(0, ringBuffer.size());
    TEST_ASSERT_FALSE(ringBuffer.pop(packet));
    TEST_ASSERT_FALSE(ringBuffer.peek(packet));
}

void test_push_pop_preserves_fifo_order()
{
    SamplePacket first = {};
    first.sample_index = 10;
    first.timestamp_us = 1000;

    SamplePacket second = {};
    second.sample_index = 11;
    second.timestamp_us = 2000;

    TEST_ASSERT_TRUE(ringBuffer.push(first));
    TEST_ASSERT_TRUE(ringBuffer.push(second));
    TEST_ASSERT_EQUAL_UINT16(2, ringBuffer.size());

    SamplePacket output = {};

    TEST_ASSERT_TRUE(ringBuffer.pop(output));
    TEST_ASSERT_EQUAL_UINT32(10, output.sample_index);
    TEST_ASSERT_TRUE(output.timestamp_us == 1000ULL);

    TEST_ASSERT_TRUE(ringBuffer.pop(output));
    TEST_ASSERT_EQUAL_UINT32(11, output.sample_index);
    TEST_ASSERT_TRUE(output.timestamp_us == 2000ULL);

    TEST_ASSERT_TRUE(ringBuffer.empty());
}

void test_full_buffer_rejects_new_sample_and_counts_drop()
{
    for (
        uint16_t i = 0;
        i < Config::RING_BUFFER_CAPACITY;
        i++
    )
    {
        SamplePacket packet = {};
        packet.sample_index = i;

        TEST_ASSERT_TRUE(ringBuffer.push(packet));
    }

    TEST_ASSERT_TRUE(ringBuffer.full());
    TEST_ASSERT_EQUAL_UINT16(
        Config::RING_BUFFER_CAPACITY,
        ringBuffer.size()
    );

    SamplePacket overflowPacket = {};
    overflowPacket.sample_index =
        Config::RING_BUFFER_CAPACITY;

    TEST_ASSERT_FALSE(
        ringBuffer.push(overflowPacket)
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        ringBuffer.overflowCount()
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        ringBuffer.droppedSamples()
    );

    TEST_ASSERT_EQUAL_UINT16(
        Config::RING_BUFFER_CAPACITY,
        ringBuffer.size()
    );
}

void test_wraparound_preserves_order()
{
    for (
        uint16_t i = 0;
        i < Config::RING_BUFFER_CAPACITY;
        i++
    )
    {
        SamplePacket packet = {};
        packet.sample_index = i;

        TEST_ASSERT_TRUE(ringBuffer.push(packet));
    }

    SamplePacket output = {};

    for (
        uint16_t i = 0;
        i < 32;
        i++
    )
    {
        TEST_ASSERT_TRUE(ringBuffer.pop(output));
        TEST_ASSERT_EQUAL_UINT32(
            i,
            output.sample_index
        );
    }

    for (
        uint16_t i = 0;
        i < 32;
        i++
    )
    {
        SamplePacket packet = {};
        packet.sample_index =
            Config::RING_BUFFER_CAPACITY + i;

        TEST_ASSERT_TRUE(ringBuffer.push(packet));
    }

    uint32_t expected = 32;

    while (!ringBuffer.empty())
    {
        TEST_ASSERT_TRUE(ringBuffer.pop(output));
        TEST_ASSERT_EQUAL_UINT32(
            expected,
            output.sample_index
        );
        expected++;
    }

    TEST_ASSERT_EQUAL_UINT32(
        Config::RING_BUFFER_CAPACITY + 32,
        expected
    );
}

void setup()
{
    delay(3000);

    UNITY_BEGIN();

    RUN_TEST(test_empty_buffer_fails_pop_and_peek);
    RUN_TEST(test_push_pop_preserves_fifo_order);
    RUN_TEST(test_full_buffer_rejects_new_sample_and_counts_drop);
    RUN_TEST(test_wraparound_preserves_order);

    UNITY_END();
}

void loop()
{
}