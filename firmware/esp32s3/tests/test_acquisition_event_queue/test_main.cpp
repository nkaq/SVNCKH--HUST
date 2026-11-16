#include <Arduino.h>
#include <unity.h>

#include "../../acquisition/events/acquisition_event_queue.h"

static AcquisitionEventQueue eventQueue;

void setUp()
{
    eventQueue.clear();
    eventQueue.resetStats();
}

void tearDown()
{
}

void test_empty_queue_rejects_pop()
{
    AcquisitionEvent event = {};

    TEST_ASSERT_TRUE(
        eventQueue.empty()
    );

    TEST_ASSERT_EQUAL_UINT16(
        0,
        eventQueue.size()
    );

    TEST_ASSERT_FALSE(
        eventQueue.pop(event)
    );
}

void test_push_pop_preserves_event_data()
{
    AcquisitionEvent input = {};

    input.source =
        AcquisitionEventSource::ADXL345;

    input.type =
        AcquisitionEventType::FIFO_WATERMARK;

    input.scheduled_time_us =
        123456ULL;

    TEST_ASSERT_TRUE(
        eventQueue.push(input)
    );

    TEST_ASSERT_EQUAL_UINT16(
        1,
        eventQueue.size()
    );

    AcquisitionEvent output = {};

    TEST_ASSERT_TRUE(
        eventQueue.pop(output)
    );

    TEST_ASSERT_EQUAL(
        static_cast<int>(
            AcquisitionEventSource::ADXL345
        ),
        static_cast<int>(
            output.source
        )
    );

    TEST_ASSERT_EQUAL(
        static_cast<int>(
            AcquisitionEventType::FIFO_WATERMARK
        ),
        static_cast<int>(
            output.type
        )
    );

    TEST_ASSERT_TRUE(
        output.scheduled_time_us ==
        123456ULL
    );

    TEST_ASSERT_TRUE(
        eventQueue.empty()
    );
}

void test_fifo_order_is_preserved()
{
    AcquisitionEvent first = {};
    first.source =
        AcquisitionEventSource::MAX31865;
    first.scheduled_time_us =
        1000ULL;

    AcquisitionEvent second = {};
    second.source =
        AcquisitionEventSource::ACS724;
    second.scheduled_time_us =
        2000ULL;

    AcquisitionEvent third = {};
    third.source =
        AcquisitionEventSource::DHT22;
    third.scheduled_time_us =
        3000ULL;

    TEST_ASSERT_TRUE(
        eventQueue.push(first)
    );

    TEST_ASSERT_TRUE(
        eventQueue.push(second)
    );

    TEST_ASSERT_TRUE(
        eventQueue.push(third)
    );

    AcquisitionEvent output = {};

    TEST_ASSERT_TRUE(
        eventQueue.pop(output)
    );

    TEST_ASSERT_EQUAL(
        static_cast<int>(
            AcquisitionEventSource::MAX31865
        ),
        static_cast<int>(
            output.source
        )
    );

    TEST_ASSERT_TRUE(
        output.scheduled_time_us ==
        1000ULL
    );

    TEST_ASSERT_TRUE(
        eventQueue.pop(output)
    );

    TEST_ASSERT_EQUAL(
        static_cast<int>(
            AcquisitionEventSource::ACS724
        ),
        static_cast<int>(
            output.source
        )
    );

    TEST_ASSERT_TRUE(
        output.scheduled_time_us ==
        2000ULL
    );

    TEST_ASSERT_TRUE(
        eventQueue.pop(output)
    );

    TEST_ASSERT_EQUAL(
        static_cast<int>(
            AcquisitionEventSource::DHT22
        ),
        static_cast<int>(
            output.source
        )
    );

    TEST_ASSERT_TRUE(
        output.scheduled_time_us ==
        3000ULL
    );
}

void test_full_queue_rejects_new_event_and_counts_overflow()
{
    for (
        uint16_t i = 0;
        i < Config::EVENT_QUEUE_CAPACITY;
        i++
    )
    {
        AcquisitionEvent event = {};

        event.scheduled_time_us =
            static_cast<uint64_t>(i);

        TEST_ASSERT_TRUE(
            eventQueue.push(event)
        );
    }

    TEST_ASSERT_TRUE(
        eventQueue.full()
    );

    TEST_ASSERT_EQUAL_UINT16(
        Config::EVENT_QUEUE_CAPACITY,
        eventQueue.size()
    );

    AcquisitionEvent overflowEvent = {};

    overflowEvent.scheduled_time_us =
        999999ULL;

    TEST_ASSERT_FALSE(
        eventQueue.push(
            overflowEvent
        )
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        eventQueue.overflowCount()
    );

    TEST_ASSERT_EQUAL_UINT16(
        Config::EVENT_QUEUE_CAPACITY,
        eventQueue.size()
    );
}

void test_overflow_event_is_not_inserted()
{
    for (
        uint16_t i = 0;
        i < Config::EVENT_QUEUE_CAPACITY;
        i++
    )
    {
        AcquisitionEvent event = {};

        event.scheduled_time_us =
            static_cast<uint64_t>(i);

        TEST_ASSERT_TRUE(
            eventQueue.push(event)
        );
    }

    AcquisitionEvent overflowEvent = {};

    overflowEvent.scheduled_time_us =
        999999ULL;

    TEST_ASSERT_FALSE(
        eventQueue.push(
            overflowEvent
        )
    );

    AcquisitionEvent output = {};

    uint16_t expected = 0;

    while (
        eventQueue.pop(output)
    )
    {
        TEST_ASSERT_TRUE(
            output.scheduled_time_us ==
            static_cast<uint64_t>(
                expected
            )
        );

        TEST_ASSERT_TRUE(
            output.scheduled_time_us !=
            999999ULL
        );

        expected++;
    }

    TEST_ASSERT_EQUAL_UINT16(
        Config::EVENT_QUEUE_CAPACITY,
        expected
    );
}

void test_wraparound_preserves_fifo_order()
{
    for (
        uint16_t i = 0;
        i < Config::EVENT_QUEUE_CAPACITY;
        i++
    )
    {
        AcquisitionEvent event = {};

        event.scheduled_time_us =
            static_cast<uint64_t>(i);

        TEST_ASSERT_TRUE(
            eventQueue.push(event)
        );
    }

    AcquisitionEvent output = {};

    const uint16_t removeCount =
        Config::EVENT_QUEUE_CAPACITY / 2;

    for (
        uint16_t i = 0;
        i < removeCount;
        i++
    )
    {
        TEST_ASSERT_TRUE(
            eventQueue.pop(output)
        );

        TEST_ASSERT_TRUE(
            output.scheduled_time_us ==
            static_cast<uint64_t>(
                i
            )
        );
    }

    for (
        uint16_t i = 0;
        i < removeCount;
        i++
    )
    {
        AcquisitionEvent event = {};

        event.scheduled_time_us =
            static_cast<uint64_t>(
                Config::EVENT_QUEUE_CAPACITY +
                i
            );

        TEST_ASSERT_TRUE(
            eventQueue.push(event)
        );
    }

    uint64_t expected =
        removeCount;

    while (
        eventQueue.pop(output)
    )
    {
        TEST_ASSERT_TRUE(
            output.scheduled_time_us ==
            expected
        );

        expected++;
    }

    TEST_ASSERT_TRUE(
        expected ==
        static_cast<uint64_t>(
            Config::EVENT_QUEUE_CAPACITY +
            removeCount
        )
    );
}

void test_clear_keeps_statistics_until_reset()
{
    AcquisitionEvent event = {};

    for (
        uint16_t i = 0;
        i < Config::EVENT_QUEUE_CAPACITY;
        i++
    )
    {
        TEST_ASSERT_TRUE(
            eventQueue.push(event)
        );
    }

    TEST_ASSERT_FALSE(
        eventQueue.push(event)
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        eventQueue.overflowCount()
    );

    eventQueue.clear();

    TEST_ASSERT_TRUE(
        eventQueue.empty()
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        eventQueue.overflowCount()
    );

    eventQueue.resetStats();

    TEST_ASSERT_EQUAL_UINT32(
        0,
        eventQueue.overflowCount()
    );
}

void setup()
{
    delay(3000);

    UNITY_BEGIN();

    RUN_TEST(test_empty_queue_rejects_pop);
    RUN_TEST(test_push_pop_preserves_event_data);
    RUN_TEST(test_fifo_order_is_preserved);
    RUN_TEST(test_full_queue_rejects_new_event_and_counts_overflow);
    RUN_TEST(test_overflow_event_is_not_inserted);
    RUN_TEST(test_wraparound_preserves_fifo_order);
    RUN_TEST(test_clear_keeps_statistics_until_reset);

    UNITY_END();
}

void loop()
{
}