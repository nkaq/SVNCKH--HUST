#include <Arduino.h>
#include <unity.h>

#include "../../acquisition/manager/acquisition_manager.h"

class FakeAdxlDriver
{
public:
    bool read_ok = true;
    uint32_t read_calls = 0;

    RawSample sample = {};

    bool readRaw(
        int16_t& x,
        int16_t& y,
        int16_t& z
    )
    {
        read_calls++;

        if (!read_ok)
        {
            return false;
        }

        x = sample.x;
        y = sample.y;
        z = sample.z;

        return true;
    }

    void reset()
    {
        read_ok = true;
        read_calls = 0;
        sample = {};
    }
};

static FakeAdxlDriver driver;
static AcquisitionManager<FakeAdxlDriver> manager(
    driver
);

void setUp()
{
    driver.reset();
    manager.reset();
}

void tearDown()
{
}

void test_scheduled_event_reads_adxl_and_creates_sample()
{
    driver.sample.x = 100;
    driver.sample.y = -200;
    driver.sample.z = 300;

    TEST_ASSERT_TRUE(
        manager.configureAdxlPeriodic(
            1000ULL
        )
    );

    TEST_ASSERT_TRUE(
        manager.start(
            0ULL
        )
    );

    TEST_ASSERT_EQUAL_UINT8(
        1,
        manager.pollScheduler(
            1000ULL
        )
    );

    TEST_ASSERT_EQUAL_UINT16(
        1,
        manager.pendingEventCount()
    );

    TEST_ASSERT_EQUAL_UINT16(
        1,
        manager.processAvailable(
            1
        )
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        driver.read_calls
    );

    TEST_ASSERT_EQUAL_UINT16(
        1,
        manager.sampleCount()
    );

    SamplePacket packet = {};

    TEST_ASSERT_TRUE(
        manager.popSample(
            packet
        )
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        packet.sample_index
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

    TEST_ASSERT_TRUE(
        packet.timestamp_us > 0ULL
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        manager.stats().events_seen
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        manager.stats().events_handled
    );
}

void test_data_ready_event_preserves_event_timestamp()
{
    driver.sample.x = 11;
    driver.sample.y = 22;
    driver.sample.z = 33;

    TEST_ASSERT_TRUE(
        manager.enqueueAdxlDataReady(
            12345ULL
        )
    );

    TEST_ASSERT_EQUAL_UINT16(
        1,
        manager.processAvailable(
            1
        )
    );

    SamplePacket packet = {};

    TEST_ASSERT_TRUE(
        manager.popSample(
            packet
        )
    );

    TEST_ASSERT_TRUE(
        packet.timestamp_us ==
        12345ULL
    );

    TEST_ASSERT_EQUAL_INT16(
        11,
        packet.x_raw
    );

    TEST_ASSERT_EQUAL_INT16(
        22,
        packet.y_raw
    );

    TEST_ASSERT_EQUAL_INT16(
        33,
        packet.z_raw
    );
}

void test_duplicate_data_ready_timestamp_is_rejected()
{
    TEST_ASSERT_TRUE(
        manager.enqueueAdxlDataReady(
            5000ULL
        )
    );

    TEST_ASSERT_TRUE(
        manager.enqueueAdxlDataReady(
            5000ULL
        )
    );

    TEST_ASSERT_EQUAL_UINT16(
        2,
        manager.processAvailable(
            2
        )
    );

    TEST_ASSERT_EQUAL_UINT32(
        2,
        manager.stats().events_seen
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        manager.stats().events_handled
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        manager.stats().
            pipeline_submit_failures
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        manager.pipelineErrors().
            timestamp_errors
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        manager.pipelineErrors().
            dropped_samples
    );

    TEST_ASSERT_EQUAL_UINT16(
        1,
        manager.sampleCount()
    );
}

void test_adxl_read_failure_is_visible()
{
    driver.read_ok = false;

    TEST_ASSERT_TRUE(
        manager.enqueueAdxlDataReady(
            1000ULL
        )
    );

    TEST_ASSERT_EQUAL_UINT16(
        1,
        manager.processAvailable(
            1
        )
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        driver.read_calls
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        manager.stats().
            adxl_read_failures
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        manager.stats().
            events_handled
    );

    TEST_ASSERT_TRUE(
        manager.samplesEmpty()
    );
}

void test_unsupported_source_is_rejected()
{
    AcquisitionEvent event = {};

    event.source =
        AcquisitionEventSource::MAX31865;

    event.type =
        AcquisitionEventType::SAMPLE_DUE;

    event.scheduled_time_us =
        1000ULL;

    TEST_ASSERT_TRUE(
        manager.enqueue(
            event
        )
    );

    TEST_ASSERT_EQUAL_UINT16(
        1,
        manager.processAvailable(
            1
        )
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        manager.stats().
            unsupported_source_events
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        driver.read_calls
    );

    TEST_ASSERT_TRUE(
        manager.samplesEmpty()
    );
}

void test_fifo_watermark_is_explicitly_unsupported()
{
    AcquisitionEvent event = {};

    event.source =
        AcquisitionEventSource::ADXL345;

    event.type =
        AcquisitionEventType::FIFO_WATERMARK;

    event.scheduled_time_us =
        1000ULL;

    TEST_ASSERT_TRUE(
        manager.enqueue(
            event
        )
    );

    TEST_ASSERT_EQUAL_UINT16(
        1,
        manager.processAvailable(
            1
        )
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        manager.stats().
            unsupported_type_events
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        driver.read_calls
    );

    TEST_ASSERT_TRUE(
        manager.samplesEmpty()
    );
}

void test_process_available_respects_limit()
{
    TEST_ASSERT_TRUE(
        manager.enqueueAdxlDataReady(
            1000ULL
        )
    );

    TEST_ASSERT_TRUE(
        manager.enqueueAdxlDataReady(
            2000ULL
        )
    );

    TEST_ASSERT_TRUE(
        manager.enqueueAdxlDataReady(
            3000ULL
        )
    );

    TEST_ASSERT_EQUAL_UINT16(
        2,
        manager.processAvailable(
            2
        )
    );

    TEST_ASSERT_EQUAL_UINT16(
        1,
        manager.pendingEventCount()
    );

    TEST_ASSERT_EQUAL_UINT16(
        2,
        manager.sampleCount()
    );

    TEST_ASSERT_EQUAL_UINT32(
        2,
        driver.read_calls
    );
}

void test_event_queue_overflow_is_separate_from_sample_buffer_overflow()
{
    AcquisitionEvent event = {};

    event.source =
        AcquisitionEventSource::MAX31865;

    event.type =
        AcquisitionEventType::SAMPLE_DUE;

    for (
        uint16_t i = 0;
        i < Config::EVENT_QUEUE_CAPACITY;
        i++
    )
    {
        event.scheduled_time_us =
            static_cast<uint64_t>(
                i + 1
            );

        TEST_ASSERT_TRUE(
            manager.enqueue(
                event
            )
        );
    }

    TEST_ASSERT_FALSE(
        manager.enqueue(
            event
        )
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        manager.eventQueueOverflowCount()
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        manager.pipelineErrors().
            buffer_overflows
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        manager.pipelineErrors().
            dropped_samples
    );
}

void test_scheduler_missed_intervals_are_visible()
{
    TEST_ASSERT_TRUE(
        manager.configureAdxlPeriodic(
            1000ULL
        )
    );

    TEST_ASSERT_TRUE(
        manager.start(
            0ULL
        )
    );

    TEST_ASSERT_EQUAL_UINT8(
        1,
        manager.pollScheduler(
            3500ULL
        )
    );

    TEST_ASSERT_TRUE(
        manager.adxlMissedIntervals() ==
        2ULL
    );

    TEST_ASSERT_EQUAL_UINT16(
        1,
        manager.pendingEventCount()
    );
}

void test_reset_clears_manager_state()
{
    TEST_ASSERT_TRUE(
        manager.enqueueAdxlDataReady(
            1000ULL
        )
    );

    TEST_ASSERT_EQUAL_UINT16(
        1,
        manager.processAvailable(
            1
        )
    );

    AcquisitionEvent event = {};

    event.source =
        AcquisitionEventSource::MAX31865;

    for (
        uint16_t i = 0;
        i < Config::EVENT_QUEUE_CAPACITY;
        i++
    )
    {
        TEST_ASSERT_TRUE(
            manager.enqueue(
                event
            )
        );
    }

    TEST_ASSERT_FALSE(
        manager.enqueue(
            event
        )
    );

    manager.reset();

    TEST_ASSERT_EQUAL_UINT16(
        0,
        manager.pendingEventCount()
    );

    TEST_ASSERT_TRUE(
        manager.samplesEmpty()
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        manager.eventQueueOverflowCount()
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        manager.stats().events_seen
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        manager.stats().events_handled
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        manager.pipelineErrors().
            total_errors
    );
}

void setup()
{
    delay(3000);

    UNITY_BEGIN();

    RUN_TEST(
        test_scheduled_event_reads_adxl_and_creates_sample
    );

    RUN_TEST(
        test_data_ready_event_preserves_event_timestamp
    );

    RUN_TEST(
        test_duplicate_data_ready_timestamp_is_rejected
    );

    RUN_TEST(
        test_adxl_read_failure_is_visible
    );

    RUN_TEST(
        test_unsupported_source_is_rejected
    );

    RUN_TEST(
        test_fifo_watermark_is_explicitly_unsupported
    );

    RUN_TEST(
        test_process_available_respects_limit
    );

    RUN_TEST(
        test_event_queue_overflow_is_separate_from_sample_buffer_overflow
    );

    RUN_TEST(
        test_scheduler_missed_intervals_are_visible
    );

    RUN_TEST(
        test_reset_clears_manager_state
    );

    UNITY_END();
}

void loop()
{
}