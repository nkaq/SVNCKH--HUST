#include <Arduino.h>
#include <unity.h>

#include "../../acquisition/events/acquisition_event_queue.h"

static AcquisitionEventQueue eventQueue;

static hw_timer_t* testTimer = nullptr;

static volatile uint32_t isrAttempts = 0;
static volatile uint32_t isrAccepted = 0;
static volatile uint32_t isrLimit = 0;
static volatile bool isrEnabled = false;

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

    AcquisitionEvent event = {};

    event.source =
        AcquisitionEventSource::US5881;

    event.type =
        AcquisitionEventType::DATA_READY;

    event.scheduled_time_us =
        static_cast<uint64_t>(
            sequence
        );

    isrAttempts++;

    if (
        eventQueue.pushFromISR(
            event
        )
    )
    {
        isrAccepted++;
    }
}

bool startTestTimer(
    uint32_t eventLimit,
    uint64_t periodUs = 1000ULL
)
{
    isrAttempts = 0;
    isrAccepted = 0;
    isrLimit = eventLimit;
    isrEnabled = true;

    testTimer =
        timerBegin(
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

    eventQueue.clear();
    eventQueue.resetStats();

    isrAttempts = 0;
    isrAccepted = 0;
    isrLimit = 0;
    isrEnabled = false;
}

void tearDown()
{
    stopTestTimer();
}

void test_timer_isr_pushes_events_into_queue()
{
    const uint32_t eventCount = 8;

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

    TEST_ASSERT_EQUAL_UINT16(
        eventCount,
        eventQueue.size()
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        eventQueue.overflowCount()
    );

    AcquisitionEvent event = {};

    for (
        uint32_t i = 1;
        i <= eventCount;
        i++
    )
    {
        TEST_ASSERT_TRUE(
            eventQueue.pop(
                event
            )
        );

        TEST_ASSERT_EQUAL(
            static_cast<int>(
                AcquisitionEventSource::US5881
            ),
            static_cast<int>(
                event.source
            )
        );

        TEST_ASSERT_EQUAL(
            static_cast<int>(
                AcquisitionEventType::DATA_READY
            ),
            static_cast<int>(
                event.type
            )
        );

        TEST_ASSERT_TRUE(
            event.scheduled_time_us ==
            static_cast<uint64_t>(
                i
            )
        );
    }

    TEST_ASSERT_TRUE(
        eventQueue.empty()
    );
}

void test_timer_isr_overflow_is_bounded_and_counted()
{
    const uint32_t extraEvents = 5;

    const uint32_t eventCount =
        Config::EVENT_QUEUE_CAPACITY +
        extraEvents;

    TEST_ASSERT_TRUE(
        startTestTimer(
            eventCount
        )
    );

    TEST_ASSERT_TRUE(
        waitForIsrAttempts(
            eventCount,
            2000
        )
    );

    stopTestTimer();

    TEST_ASSERT_EQUAL_UINT32(
        eventCount,
        isrAttempts
    );

    TEST_ASSERT_EQUAL_UINT32(
        Config::EVENT_QUEUE_CAPACITY,
        isrAccepted
    );

    TEST_ASSERT_EQUAL_UINT16(
        Config::EVENT_QUEUE_CAPACITY,
        eventQueue.size()
    );

    TEST_ASSERT_EQUAL_UINT32(
        extraEvents,
        eventQueue.overflowCount()
    );

    AcquisitionEvent event = {};

    for (
        uint32_t i = 1;
        i <= Config::EVENT_QUEUE_CAPACITY;
        i++
    )
    {
        TEST_ASSERT_TRUE(
            eventQueue.pop(
                event
            )
        );

        TEST_ASSERT_TRUE(
            event.scheduled_time_us ==
            static_cast<uint64_t>(
                i
            )
        );
    }

    TEST_ASSERT_TRUE(
        eventQueue.empty()
    );
}

void test_task_pop_while_timer_isr_pushes_preserves_order()
{
    const uint32_t eventCount = 64;

    TEST_ASSERT_TRUE(
        startTestTimer(
            eventCount,
            5000ULL
        )
    );

    uint32_t poppedCount = 0;
    uint32_t expectedSequence = 1;

    const uint32_t start =
        millis();

    AcquisitionEvent event = {};

    while (
        poppedCount < eventCount
    )
    {
        if (
            eventQueue.pop(
                event
            )
        )
        {
            TEST_ASSERT_EQUAL(
                static_cast<int>(
                    AcquisitionEventSource::US5881
                ),
                static_cast<int>(
                    event.source
                )
            );

            TEST_ASSERT_EQUAL(
                static_cast<int>(
                    AcquisitionEventType::DATA_READY
                ),
                static_cast<int>(
                    event.type
                )
            );

            TEST_ASSERT_TRUE(
                event.scheduled_time_us ==
                static_cast<uint64_t>(
                    expectedSequence
                )
            );

            expectedSequence++;
            poppedCount++;

            continue;
        }

        if (
            millis() - start >=
            3000
        )
        {
            break;
        }

        delay(1);
    }

    stopTestTimer();

    TEST_ASSERT_EQUAL_UINT32(
        eventCount,
        isrAttempts
    );

    TEST_ASSERT_EQUAL_UINT32(
        eventCount,
        isrAccepted
    );

    TEST_ASSERT_EQUAL_UINT32(
        eventCount,
        poppedCount
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        eventQueue.overflowCount()
    );

    TEST_ASSERT_TRUE(
        eventQueue.empty()
    );
}

void setup()
{
    delay(3000);

    UNITY_BEGIN();

    RUN_TEST(
        test_timer_isr_pushes_events_into_queue
    );

    RUN_TEST(
        test_timer_isr_overflow_is_bounded_and_counted
    );

    RUN_TEST(
        test_task_pop_while_timer_isr_pushes_preserves_order
    );

    UNITY_END();
}

void loop()
{
}