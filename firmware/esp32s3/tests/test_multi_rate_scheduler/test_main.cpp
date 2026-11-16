#include <Arduino.h>
#include <unity.h>

#include "../../acquisition/events/acquisition_event_queue.h"
#include "../../acquisition/scheduler/multi_rate_scheduler.h"

static MultiRateScheduler scheduler;
static AcquisitionEventQueue eventQueue;

void setUp()
{
    scheduler.reset();
    eventQueue.clear();
    eventQueue.resetStats();
}

void tearDown()
{
}

void test_start_fails_when_no_source_is_enabled()
{
    TEST_ASSERT_FALSE(
        scheduler.start(
            0ULL
        )
    );

    TEST_ASSERT_TRUE(
        eventQueue.empty()
    );
}

void test_configure_enables_source()
{
    TEST_ASSERT_TRUE(
        scheduler.configure(
            AcquisitionEventSource::ADXL345,
            1000ULL
        )
    );

    TEST_ASSERT_TRUE(
        scheduler.enabled(
            AcquisitionEventSource::ADXL345
        )
    );

    TEST_ASSERT_FALSE(
        scheduler.enabled(
            AcquisitionEventSource::DHT22
        )
    );
}

void test_zero_period_disables_source()
{
    TEST_ASSERT_TRUE(
        scheduler.configure(
            AcquisitionEventSource::ADXL345,
            1000ULL
        )
    );

    TEST_ASSERT_FALSE(
        scheduler.configure(
            AcquisitionEventSource::ADXL345,
            0ULL
        )
    );

    TEST_ASSERT_FALSE(
        scheduler.enabled(
            AcquisitionEventSource::ADXL345
        )
    );
}

void test_single_source_generates_due_event()
{
    TEST_ASSERT_TRUE(
        scheduler.configure(
            AcquisitionEventSource::ADXL345,
            1000ULL
        )
    );

    TEST_ASSERT_TRUE(
        scheduler.start(
            0ULL
        )
    );

    TEST_ASSERT_EQUAL_UINT8(
        0,
        scheduler.poll(
            999ULL,
            eventQueue
        )
    );

    TEST_ASSERT_EQUAL_UINT8(
        1,
        scheduler.poll(
            1000ULL,
            eventQueue
        )
    );

    AcquisitionEvent event = {};

    TEST_ASSERT_TRUE(
        eventQueue.pop(event)
    );

    TEST_ASSERT_EQUAL(
        static_cast<int>(
            AcquisitionEventSource::ADXL345
        ),
        static_cast<int>(
            event.source
        )
    );

    TEST_ASSERT_EQUAL(
        static_cast<int>(
            AcquisitionEventType::SAMPLE_DUE
        ),
        static_cast<int>(
            event.type
        )
    );

    TEST_ASSERT_TRUE(
        event.scheduled_time_us ==
        1000ULL
    );
}

void test_multiple_sources_run_at_independent_periods()
{
    TEST_ASSERT_TRUE(
        scheduler.configure(
            AcquisitionEventSource::ADXL345,
            1000ULL
        )
    );

    TEST_ASSERT_TRUE(
        scheduler.configure(
            AcquisitionEventSource::DHT22,
            3000ULL
        )
    );

    TEST_ASSERT_TRUE(
        scheduler.start(
            0ULL
        )
    );

    TEST_ASSERT_EQUAL_UINT8(
        1,
        scheduler.poll(
            1000ULL,
            eventQueue
        )
    );

    TEST_ASSERT_EQUAL_UINT8(
        1,
        scheduler.poll(
            2000ULL,
            eventQueue
        )
    );

    TEST_ASSERT_EQUAL_UINT8(
        2,
        scheduler.poll(
            3000ULL,
            eventQueue
        )
    );

    TEST_ASSERT_EQUAL_UINT16(
        4,
        eventQueue.size()
    );

    AcquisitionEvent event = {};

    TEST_ASSERT_TRUE(
        eventQueue.pop(event)
    );

    TEST_ASSERT_EQUAL(
        static_cast<int>(
            AcquisitionEventSource::ADXL345
        ),
        static_cast<int>(
            event.source
        )
    );

    TEST_ASSERT_TRUE(
        event.scheduled_time_us ==
        1000ULL
    );

    TEST_ASSERT_TRUE(
        eventQueue.pop(event)
    );

    TEST_ASSERT_EQUAL(
        static_cast<int>(
            AcquisitionEventSource::ADXL345
        ),
        static_cast<int>(
            event.source
        )
    );

    TEST_ASSERT_TRUE(
        event.scheduled_time_us ==
        2000ULL
    );

    TEST_ASSERT_TRUE(
        eventQueue.pop(event)
    );

    TEST_ASSERT_TRUE(
        event.scheduled_time_us ==
        3000ULL
    );

    TEST_ASSERT_TRUE(
        eventQueue.pop(event)
    );

    TEST_ASSERT_TRUE(
        event.scheduled_time_us ==
        3000ULL
    );
}

void test_due_events_are_sorted_by_scheduled_time()
{
    TEST_ASSERT_TRUE(
        scheduler.configure(
            AcquisitionEventSource::ADXL345,
            1000ULL
        )
    );

    TEST_ASSERT_TRUE(
        scheduler.configure(
            AcquisitionEventSource::DHT22,
            2500ULL
        )
    );

    TEST_ASSERT_TRUE(
        scheduler.start(
            0ULL
        )
    );

    TEST_ASSERT_EQUAL_UINT8(
        2,
        scheduler.poll(
            3000ULL,
            eventQueue
        )
    );

    AcquisitionEvent first = {};
    AcquisitionEvent second = {};

    TEST_ASSERT_TRUE(
        eventQueue.pop(first)
    );

    TEST_ASSERT_TRUE(
        eventQueue.pop(second)
    );

    TEST_ASSERT_EQUAL(
        static_cast<int>(
            AcquisitionEventSource::DHT22
        ),
        static_cast<int>(
            first.source
        )
    );

    TEST_ASSERT_TRUE(
        first.scheduled_time_us ==
        2500ULL
    );

    TEST_ASSERT_EQUAL(
        static_cast<int>(
            AcquisitionEventSource::ADXL345
        ),
        static_cast<int>(
            second.source
        )
    );

    TEST_ASSERT_TRUE(
        second.scheduled_time_us ==
        3000ULL
    );
}

void test_late_poll_tracks_missed_intervals_per_source()
{
    TEST_ASSERT_TRUE(
        scheduler.configure(
            AcquisitionEventSource::ADXL345,
            1000ULL
        )
    );

    TEST_ASSERT_TRUE(
        scheduler.configure(
            AcquisitionEventSource::DHT22,
            2500ULL
        )
    );

    TEST_ASSERT_TRUE(
        scheduler.start(
            0ULL
        )
    );

    TEST_ASSERT_EQUAL_UINT8(
        2,
        scheduler.poll(
            5500ULL,
            eventQueue
        )
    );

    TEST_ASSERT_TRUE(
        scheduler.missedIntervals(
            AcquisitionEventSource::ADXL345
        ) == 4ULL
    );

    TEST_ASSERT_TRUE(
        scheduler.missedIntervals(
            AcquisitionEventSource::DHT22
        ) == 1ULL
    );

    TEST_ASSERT_TRUE(
        scheduler.nextDueUs(
            AcquisitionEventSource::ADXL345
        ) == 6000ULL
    );

    TEST_ASSERT_TRUE(
        scheduler.nextDueUs(
            AcquisitionEventSource::DHT22
        ) == 7500ULL
    );
}

void test_disable_stops_only_selected_source()
{
    TEST_ASSERT_TRUE(
        scheduler.configure(
            AcquisitionEventSource::ADXL345,
            1000ULL
        )
    );

    TEST_ASSERT_TRUE(
        scheduler.configure(
            AcquisitionEventSource::DHT22,
            1000ULL
        )
    );

    TEST_ASSERT_TRUE(
        scheduler.start(
            0ULL
        )
    );

    TEST_ASSERT_TRUE(
        scheduler.disable(
            AcquisitionEventSource::DHT22
        )
    );

    TEST_ASSERT_FALSE(
        scheduler.enabled(
            AcquisitionEventSource::DHT22
        )
    );

    TEST_ASSERT_TRUE(
        scheduler.enabled(
            AcquisitionEventSource::ADXL345
        )
    );

    TEST_ASSERT_EQUAL_UINT8(
        1,
        scheduler.poll(
            1000ULL,
            eventQueue
        )
    );

    AcquisitionEvent event = {};

    TEST_ASSERT_TRUE(
        eventQueue.pop(event)
    );

    TEST_ASSERT_EQUAL(
        static_cast<int>(
            AcquisitionEventSource::ADXL345
        ),
        static_cast<int>(
            event.source
        )
    );

    TEST_ASSERT_TRUE(
        eventQueue.empty()
    );
}

void test_queue_overflow_is_visible()
{
    AcquisitionEvent filler = {};

    for (
        uint16_t i = 0;
        i < Config::EVENT_QUEUE_CAPACITY;
        i++
    )
    {
        TEST_ASSERT_TRUE(
            eventQueue.push(
                filler
            )
        );
    }

    TEST_ASSERT_TRUE(
        scheduler.configure(
            AcquisitionEventSource::ADXL345,
            1000ULL
        )
    );

    TEST_ASSERT_TRUE(
        scheduler.start(
            0ULL
        )
    );

    TEST_ASSERT_EQUAL_UINT8(
        0,
        scheduler.poll(
            1000ULL,
            eventQueue
        )
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        eventQueue.overflowCount()
    );

    TEST_ASSERT_TRUE(
        scheduler.nextDueUs(
            AcquisitionEventSource::ADXL345
        ) == 2000ULL
    );
}

void test_reset_clears_all_scheduler_state()
{
    TEST_ASSERT_TRUE(
        scheduler.configure(
            AcquisitionEventSource::ADXL345,
            1000ULL
        )
    );

    TEST_ASSERT_TRUE(
        scheduler.configure(
            AcquisitionEventSource::DHT22,
            2000ULL
        )
    );

    TEST_ASSERT_TRUE(
        scheduler.start(
            0ULL
        )
    );

    scheduler.poll(
        3000ULL,
        eventQueue
    );

    scheduler.reset();

    TEST_ASSERT_FALSE(
        scheduler.enabled(
            AcquisitionEventSource::ADXL345
        )
    );

    TEST_ASSERT_FALSE(
        scheduler.enabled(
            AcquisitionEventSource::DHT22
        )
    );

    TEST_ASSERT_TRUE(
        scheduler.missedIntervals(
            AcquisitionEventSource::ADXL345
        ) == 0ULL
    );

    TEST_ASSERT_TRUE(
        scheduler.nextDueUs(
            AcquisitionEventSource::ADXL345
        ) == 0ULL
    );

    TEST_ASSERT_FALSE(
        scheduler.start(
            0ULL
        )
    );
}

void setup()
{
    delay(3000);

    UNITY_BEGIN();

    RUN_TEST(test_start_fails_when_no_source_is_enabled);
    RUN_TEST(test_configure_enables_source);
    RUN_TEST(test_zero_period_disables_source);
    RUN_TEST(test_single_source_generates_due_event);
    RUN_TEST(test_multiple_sources_run_at_independent_periods);
    RUN_TEST(test_due_events_are_sorted_by_scheduled_time);
    RUN_TEST(test_late_poll_tracks_missed_intervals_per_source);
    RUN_TEST(test_disable_stops_only_selected_source);
    RUN_TEST(test_queue_overflow_is_visible);
    RUN_TEST(test_reset_clears_all_scheduler_state);

    UNITY_END();
}

void loop()
{
}