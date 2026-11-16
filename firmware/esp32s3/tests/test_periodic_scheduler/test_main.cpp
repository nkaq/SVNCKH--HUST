#include <Arduino.h>
#include <unity.h>

#include "../../acquisition/scheduler/periodic_scheduler.h"

static PeriodicScheduler scheduler;

void setUp()
{
    scheduler.reset();
}

void tearDown()
{
}

void test_zero_period_is_rejected()
{
    TEST_ASSERT_FALSE(
        scheduler.configure(
            0ULL
        )
    );

    TEST_ASSERT_FALSE(
        scheduler.configured()
    );

    TEST_ASSERT_FALSE(
        scheduler.running()
    );
}

void test_start_requires_configuration()
{
    TEST_ASSERT_FALSE(
        scheduler.start(
            1000ULL
        )
    );

    TEST_ASSERT_FALSE(
        scheduler.running()
    );
}

void test_first_deadline_is_one_period_after_start()
{
    TEST_ASSERT_TRUE(
        scheduler.configure(
            1000ULL
        )
    );

    TEST_ASSERT_TRUE(
        scheduler.start(
            5000ULL
        )
    );

    TEST_ASSERT_TRUE(
        scheduler.nextDueUs() == 6000ULL
    );

    uint64_t scheduledTime = 0;

    TEST_ASSERT_FALSE(
        scheduler.poll(
            5999ULL,
            scheduledTime
        )
    );

    TEST_ASSERT_TRUE(
        scheduler.poll(
            6000ULL,
            scheduledTime
        )
    );

    TEST_ASSERT_TRUE(
        scheduledTime == 6000ULL
    );

    TEST_ASSERT_TRUE(
        scheduler.nextDueUs() == 7000ULL
    );
}

void test_regular_polling_preserves_periodic_deadlines()
{
    TEST_ASSERT_TRUE(
        scheduler.configure(
            1000ULL
        )
    );

    TEST_ASSERT_TRUE(
        scheduler.start(
            0ULL
        )
    );

    uint64_t scheduledTime = 0;

    TEST_ASSERT_TRUE(
        scheduler.poll(
            1000ULL,
            scheduledTime
        )
    );

    TEST_ASSERT_TRUE(
        scheduledTime == 1000ULL
    );

    TEST_ASSERT_TRUE(
        scheduler.poll(
            2000ULL,
            scheduledTime
        )
    );

    TEST_ASSERT_TRUE(
        scheduledTime == 2000ULL
    );

    TEST_ASSERT_TRUE(
        scheduler.poll(
            3000ULL,
            scheduledTime
        )
    );

    TEST_ASSERT_TRUE(
        scheduledTime == 3000ULL
    );

    TEST_ASSERT_TRUE(
        scheduler.missedIntervals() == 0ULL
    );
}

void test_late_poll_counts_skipped_intervals_without_burst()
{
    TEST_ASSERT_TRUE(
        scheduler.configure(
            1000ULL
        )
    );

    TEST_ASSERT_TRUE(
        scheduler.start(
            0ULL
        )
    );

    uint64_t scheduledTime = 0;

    TEST_ASSERT_TRUE(
        scheduler.poll(
            3500ULL,
            scheduledTime
        )
    );

    TEST_ASSERT_TRUE(
        scheduledTime == 3000ULL
    );

    TEST_ASSERT_TRUE(
        scheduler.missedIntervals() == 2ULL
    );

    TEST_ASSERT_TRUE(
        scheduler.nextDueUs() == 4000ULL
    );

    TEST_ASSERT_FALSE(
        scheduler.poll(
            3500ULL,
            scheduledTime
        )
    );
}

void test_exact_multiple_lateness_counts_skipped_intervals()
{
    TEST_ASSERT_TRUE(
        scheduler.configure(
            1000ULL
        )
    );

    TEST_ASSERT_TRUE(
        scheduler.start(
            0ULL
        )
    );

    uint64_t scheduledTime = 0;

    TEST_ASSERT_TRUE(
        scheduler.poll(
            4000ULL,
            scheduledTime
        )
    );

    TEST_ASSERT_TRUE(
        scheduledTime == 4000ULL
    );

    TEST_ASSERT_TRUE(
        scheduler.missedIntervals() == 3ULL
    );

    TEST_ASSERT_TRUE(
        scheduler.nextDueUs() == 5000ULL
    );
}

void test_stop_prevents_poll_until_restarted()
{
    TEST_ASSERT_TRUE(
        scheduler.configure(
            1000ULL
        )
    );

    TEST_ASSERT_TRUE(
        scheduler.start(
            0ULL
        )
    );

    scheduler.stop();

    TEST_ASSERT_FALSE(
        scheduler.running()
    );

    uint64_t scheduledTime = 0;

    TEST_ASSERT_FALSE(
        scheduler.poll(
            5000ULL,
            scheduledTime
        )
    );

    TEST_ASSERT_TRUE(
        scheduler.start(
            5000ULL
        )
    );

    TEST_ASSERT_TRUE(
        scheduler.nextDueUs() == 6000ULL
    );
}

void test_reconfigure_stops_scheduler_and_clears_schedule()
{
    TEST_ASSERT_TRUE(
        scheduler.configure(
            1000ULL
        )
    );

    TEST_ASSERT_TRUE(
        scheduler.start(
            0ULL
        )
    );

    uint64_t scheduledTime = 0;

    TEST_ASSERT_TRUE(
        scheduler.poll(
            3500ULL,
            scheduledTime
        )
    );

    TEST_ASSERT_TRUE(
        scheduler.missedIntervals() == 2ULL
    );

    TEST_ASSERT_TRUE(
        scheduler.configure(
            2000ULL
        )
    );

    TEST_ASSERT_TRUE(
        scheduler.configured()
    );

    TEST_ASSERT_FALSE(
        scheduler.running()
    );

    TEST_ASSERT_TRUE(
        scheduler.periodUs() == 2000ULL
    );

    TEST_ASSERT_TRUE(
        scheduler.nextDueUs() == 0ULL
    );

    TEST_ASSERT_TRUE(
        scheduler.missedIntervals() == 0ULL
    );
}

void setup()
{
    delay(3000);

    UNITY_BEGIN();

    RUN_TEST(test_zero_period_is_rejected);
    RUN_TEST(test_start_requires_configuration);
    RUN_TEST(test_first_deadline_is_one_period_after_start);
    RUN_TEST(test_regular_polling_preserves_periodic_deadlines);
    RUN_TEST(test_late_poll_counts_skipped_intervals_without_burst);
    RUN_TEST(test_exact_multiple_lateness_counts_skipped_intervals);
    RUN_TEST(test_stop_prevents_poll_until_restarted);
    RUN_TEST(test_reconfigure_stops_scheduler_and_clears_schedule);

    UNITY_END();
}

void loop()
{
}