#include <Arduino.h>
#include <esp_timer.h>
#include <unity.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include "../../acquisition/manager/acquisition_manager.h"

class FakeAdxlDriver
{
public:
    bool read_ok = true;
    uint32_t read_calls = 0;
    RawSample sample = {};
    SemaphoreHandle_t read_entered = nullptr;
    SemaphoreHandle_t read_release = nullptr;

    bool readRaw(int16_t& x, int16_t& y, int16_t& z)
    {
        ++read_calls;
        if (read_entered != nullptr && read_release != nullptr)
        {
            xSemaphoreGive(read_entered);
            // Bounded wait: a test failure must not deadlock the board.
            if (xSemaphoreTake(read_release, pdMS_TO_TICKS(3000)) != pdTRUE)
                return false;
        }
        if (!read_ok) return false;
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
        read_entered = nullptr;
        read_release = nullptr;
    }
};

static FakeAdxlDriver driver;

static AcquisitionManager<FakeAdxlDriver> manager(
    driver
);

static hw_timer_t* stopTimer = nullptr;

static volatile bool stopIsrFired = false;
static volatile bool stopIsrAccepted = false;
static volatile uint64_t stopIsrTimestamp = 0;
static SemaphoreHandle_t readEntered = nullptr;
static SemaphoreHandle_t readRelease = nullptr;
static SemaphoreHandle_t consumerDone = nullptr;

void ARDUINO_ISR_ATTR onStopTimer()
{
    if (stopIsrFired)
    {
        return;
    }

    stopIsrAccepted =
        manager.enqueueAdxlDataReadyFromISR(
            stopIsrTimestamp
        );

    stopIsrFired = true;
}

bool startStopTestTimer()
{
    stopIsrFired = false;
    stopIsrAccepted = false;

    stopTimer =
        timerBegin(
            0,
            80,
            true
        );

    if (stopTimer == nullptr)
    {
        return false;
    }

    timerAttachInterrupt(
        stopTimer,
        &onStopTimer,
        true
    );

    timerAlarmWrite(
        stopTimer,
        1000ULL,
        false
    );

    timerAlarmEnable(
        stopTimer
    );

    return true;
}

void stopStopTestTimer()
{
    if (stopTimer == nullptr)
    {
        return;
    }

    timerAlarmDisable(
        stopTimer
    );

    timerDetachInterrupt(
        stopTimer
    );

    timerEnd(
        stopTimer
    );

    stopTimer = nullptr;
}

bool waitForStopIsr(
    uint32_t timeout_ms
)
{
    const uint32_t start =
        millis();

    while (!stopIsrFired)
    {
        if (
            millis() - start >=
            timeout_ms
        )
        {
            return false;
        }

        delay(1);
    }

    return true;
}

uint64_t nowUs()
{
    return static_cast<uint64_t>(
        esp_timer_get_time()
    );
}

void startDataReadyMode(
    uint64_t max_age_us = 1000000ULL
)
{
    TEST_ASSERT_TRUE(
        manager.configureAdxlDataReady(
            max_age_us
        )
    );

    TEST_ASSERT_TRUE(
        manager.startAcquisition(
            nowUs()
        )
    );
}

void consumerTask(void*)
{
    manager.processAvailable(1);
    xSemaphoreGive(consumerDone);
    vTaskDelete(nullptr);
}

bool startBlockedConsumer()
{
    readEntered = xSemaphoreCreateBinary();
    readRelease = xSemaphoreCreateBinary();
    consumerDone = xSemaphoreCreateBinary();
    if (!readEntered || !readRelease || !consumerDone) return false;
    driver.read_entered = readEntered;
    driver.read_release = readRelease;
    if (xTaskCreatePinnedToCore(consumerTask, "ee2-race", 4096,
                                nullptr, 2, nullptr, 1) != pdPASS)
        return false;
    return xSemaphoreTake(readEntered, pdMS_TO_TICKS(2000)) == pdTRUE;
}

void releaseAndJoinConsumer()
{
    if (readRelease) xSemaphoreGive(readRelease);
    if (consumerDone) xSemaphoreTake(consumerDone, pdMS_TO_TICKS(3500));
    driver.read_entered = nullptr;
    driver.read_release = nullptr;
    if (readEntered) { vSemaphoreDelete(readEntered); readEntered = nullptr; }
    if (readRelease) { vSemaphoreDelete(readRelease); readRelease = nullptr; }
    if (consumerDone) { vSemaphoreDelete(consumerDone); consumerDone = nullptr; }
}

void setUp()
{
    stopStopTestTimer();

    driver.reset();
    manager.reset();

    stopIsrFired = false;
    stopIsrAccepted = false;
    stopIsrTimestamp = 0;
}

void tearDown()
{
    stopStopTestTimer();
    if (readEntered || readRelease || consumerDone) releaseAndJoinConsumer();
}

void test_periodic_event_reads_adxl_and_creates_sample()
{
    driver.sample.x = 100;
    driver.sample.y = -200;
    driver.sample.z = 300;

    TEST_ASSERT_TRUE(
        manager.configureAdxlPeriodic(
            1000ULL
        )
    );

    const uint64_t start_us =
        nowUs();

    TEST_ASSERT_TRUE(
        manager.startAcquisition(
            start_us
        )
    );

    delay(2);

    TEST_ASSERT_EQUAL_UINT8(
        1,
        manager.pollScheduler(
            nowUs()
        )
    );

    delay(2);

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
        packet.timestamp_us >
        start_us
    );

    const AcquisitionManagerStats stats =
        manager.stats();

    TEST_ASSERT_EQUAL_UINT32(
        1,
        stats.events_seen
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        stats.events_handled
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        stats.scheduled_events_observed
    );

    TEST_ASSERT_TRUE(
        stats.scheduled_lateness_max_us >
        0ULL
    );
}

void test_valid_data_ready_preserves_timestamp()
{
    startDataReadyMode();

    driver.sample.x = 11;
    driver.sample.y = 22;
    driver.sample.z = 33;

    const uint64_t timestamp_us =
        nowUs();

    TEST_ASSERT_TRUE(
        manager.enqueueAdxlDataReady(
            timestamp_us
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
        timestamp_us
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

    const AcquisitionManagerStats stats =
        manager.stats();

    TEST_ASSERT_EQUAL_UINT32(
        1,
        stats.events_handled
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        stats.data_ready_age_observations
    );
}

void test_zero_data_ready_timestamp_is_rejected()
{
    startDataReadyMode();

    TEST_ASSERT_FALSE(
        manager.enqueueAdxlDataReady(
            0ULL
        )
    );

    TEST_ASSERT_EQUAL_UINT16(
        0,
        manager.pendingEventCount()
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        driver.read_calls
    );

    const AcquisitionManagerStats stats =
        manager.stats();

    TEST_ASSERT_EQUAL_UINT32(
        1,
        stats.invalid_data_ready_timestamps
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        stats.dropped_events
    );
}

void test_backlogged_data_ready_only_newest_event_can_create_sample()
{
    startDataReadyMode();

    driver.sample.x = 777;

    const uint64_t first_timestamp =
        nowUs();

    TEST_ASSERT_TRUE(
        manager.enqueueAdxlDataReady(
            first_timestamp
        )
    );

    delayMicroseconds(
        20
    );

    const uint64_t second_timestamp =
        nowUs();

    TEST_ASSERT_TRUE(
        second_timestamp >
        first_timestamp
    );

    TEST_ASSERT_TRUE(
        manager.enqueueAdxlDataReady(
            second_timestamp
        )
    );

    TEST_ASSERT_EQUAL_UINT16(
        2,
        manager.pendingEventCount()
    );

    TEST_ASSERT_EQUAL_UINT16(
        2,
        manager.processAvailable(
            2
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

    TEST_ASSERT_TRUE(
        packet.timestamp_us ==
        second_timestamp
    );

    const AcquisitionManagerStats stats =
        manager.stats();

    TEST_ASSERT_EQUAL_UINT32(
        1,
        stats.stale_data_ready_events
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        stats.dropped_events
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        stats.events_handled
    );
}

void test_stale_data_ready_is_dropped_before_sensor_read()
{
    startDataReadyMode(
        1000ULL
    );

    const uint64_t timestamp_us =
        nowUs();

    TEST_ASSERT_TRUE(
        manager.enqueueAdxlDataReady(
            timestamp_us
        )
    );

    delay(
        3
    );

    TEST_ASSERT_EQUAL_UINT16(
        1,
        manager.processAvailable(
            1
        )
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        driver.read_calls
    );

    TEST_ASSERT_TRUE(
        manager.samplesEmpty()
    );

    const AcquisitionManagerStats stats =
        manager.stats();

    TEST_ASSERT_EQUAL_UINT32(
        1,
        stats.stale_data_ready_events
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        stats.dropped_events
    );
}

void test_future_data_ready_timestamp_is_dropped()
{
    startDataReadyMode();

    const uint64_t future_timestamp =
        nowUs() +
        1000000ULL;

    TEST_ASSERT_TRUE(
        manager.enqueueAdxlDataReady(
            future_timestamp
        )
    );

    TEST_ASSERT_EQUAL_UINT16(
        1,
        manager.processAvailable(
            1
        )
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        driver.read_calls
    );

    TEST_ASSERT_TRUE(
        manager.samplesEmpty()
    );

    const AcquisitionManagerStats stats =
        manager.stats();

    TEST_ASSERT_EQUAL_UINT32(
        1,
        stats.data_ready_clock_errors
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        stats.dropped_events
    );
}

void test_adxl_read_failure_is_visible()
{
    startDataReadyMode();

    driver.read_ok = false;

    TEST_ASSERT_TRUE(
        manager.enqueueAdxlDataReady(
            nowUs()
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

    TEST_ASSERT_TRUE(
        manager.samplesEmpty()
    );

    const AcquisitionManagerStats stats =
        manager.stats();

    TEST_ASSERT_EQUAL_UINT32(
        1,
        stats.adxl_read_failures
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        stats.events_handled
    );
}

void test_stop_acquisition_flushes_queued_event()
{
    startDataReadyMode();

    TEST_ASSERT_TRUE(
        manager.enqueueAdxlDataReady(
            nowUs()
        )
    );

    TEST_ASSERT_EQUAL_UINT16(
        1,
        manager.pendingEventCount()
    );

    manager.stopAcquisition();

    TEST_ASSERT_FALSE(
        manager.running()
    );

    TEST_ASSERT_EQUAL_UINT16(
        0,
        manager.pendingEventCount()
    );

    TEST_ASSERT_EQUAL_UINT16(
        0,
        manager.processAvailable(
            1
        )
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        driver.read_calls
    );

    TEST_ASSERT_TRUE(
        manager.samplesEmpty()
    );
}

void test_stop_acquisition_rejects_isr_event()
{
    startDataReadyMode();

    manager.stopAcquisition();

    stopIsrTimestamp =
        nowUs();

    TEST_ASSERT_TRUE(
        startStopTestTimer()
    );

    TEST_ASSERT_TRUE(
        waitForStopIsr(
            1000
        )
    );

    stopStopTestTimer();

    TEST_ASSERT_FALSE(
        stopIsrAccepted
    );

    TEST_ASSERT_EQUAL_UINT16(
        0,
        manager.pendingEventCount()
    );

    TEST_ASSERT_EQUAL_UINT16(
        0,
        manager.processAvailable(
            1
        )
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        driver.read_calls
    );

    TEST_ASSERT_TRUE(
        manager.samplesEmpty()
    );

    const AcquisitionManagerStats stats =
        manager.stats();

    TEST_ASSERT_EQUAL_UINT32(
        1,
        stats.rejected_while_stopped
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        stats.dropped_events
    );
}

void test_data_ready_is_rejected_in_periodic_mode()
{
    TEST_ASSERT_TRUE(
        manager.configureAdxlPeriodic(
            1000ULL
        )
    );

    TEST_ASSERT_TRUE(
        manager.startAcquisition(
            nowUs()
        )
    );

    TEST_ASSERT_FALSE(
        manager.enqueueAdxlDataReady(
            nowUs()
        )
    );

    TEST_ASSERT_EQUAL_UINT16(
        0,
        manager.pendingEventCount()
    );

    const AcquisitionManagerStats stats =
        manager.stats();

    TEST_ASSERT_EQUAL_UINT32(
        1,
        stats.invalid_mode_events
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        stats.dropped_events
    );
}

void test_unsupported_source_is_rejected_during_processing()
{
    startDataReadyMode();

    AcquisitionEvent event = {};

    event.source =
        AcquisitionEventSource::MAX31865;

    event.type =
        AcquisitionEventType::SAMPLE_DUE;

    event.scheduled_time_us =
        nowUs();

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
        0,
        driver.read_calls
    );

    TEST_ASSERT_TRUE(
        manager.samplesEmpty()
    );

    const AcquisitionManagerStats stats =
        manager.stats();

    TEST_ASSERT_EQUAL_UINT32(
        1,
        stats.unsupported_source_events
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        stats.dropped_events
    );
}

void test_fifo_watermark_is_explicitly_unsupported()
{
    startDataReadyMode();

    AcquisitionEvent event = {};

    event.source =
        AcquisitionEventSource::ADXL345;

    event.type =
        AcquisitionEventType::FIFO_WATERMARK;

    event.scheduled_time_us =
        nowUs();

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
        0,
        driver.read_calls
    );

    TEST_ASSERT_TRUE(
        manager.samplesEmpty()
    );

    const AcquisitionManagerStats stats =
        manager.stats();

    TEST_ASSERT_EQUAL_UINT32(
        1,
        stats.unsupported_type_events
    );

    TEST_ASSERT_EQUAL_UINT32(
        1,
        stats.dropped_events
    );
}

void test_process_available_respects_limit()
{
    startDataReadyMode();

    AcquisitionEvent event = {};

    event.source =
        AcquisitionEventSource::MAX31865;

    event.type =
        AcquisitionEventType::SAMPLE_DUE;

    for (
        uint8_t i = 0;
        i < 3;
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

    const AcquisitionManagerStats stats =
        manager.stats();

    TEST_ASSERT_EQUAL_UINT32(
        2,
        stats.events_seen
    );

    TEST_ASSERT_EQUAL_UINT32(
        2,
        stats.unsupported_source_events
    );
}

void test_event_queue_overflow_is_separate_from_sample_buffer_overflow()
{
    startDataReadyMode();

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

    const AcquisitionManagerStats stats =
        manager.stats();

    TEST_ASSERT_EQUAL_UINT32(
        1,
        stats.dropped_events
    );
}

void test_scheduler_missed_intervals_and_lateness_are_visible()
{
    TEST_ASSERT_TRUE(
        manager.configureAdxlPeriodic(
            1000ULL
        )
    );

    TEST_ASSERT_TRUE(
        manager.startAcquisition(
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
        manager.processAvailable(
            1
        )
    );

    const AcquisitionManagerStats stats =
        manager.stats();

    TEST_ASSERT_EQUAL_UINT32(
        1,
        stats.scheduled_events_observed
    );

    TEST_ASSERT_TRUE(
        stats.scheduled_lateness_max_us >
        0ULL
    );

    TEST_ASSERT_TRUE(
        stats.scheduled_lateness_total_us >=
        stats.scheduled_lateness_max_us
    );
}

void test_reset_clears_manager_state()
{
    startDataReadyMode();

    TEST_ASSERT_TRUE(
        manager.enqueueAdxlDataReady(
            nowUs()
        )
    );

    TEST_ASSERT_EQUAL_UINT16(
        1,
        manager.processAvailable(
            1
        )
    );

    manager.stopAcquisition();

    TEST_ASSERT_FALSE(
        manager.enqueueAdxlDataReady(
            nowUs()
        )
    );

    manager.reset();

    TEST_ASSERT_FALSE(
        manager.running()
    );

    TEST_ASSERT_EQUAL(
        static_cast<int>(
            AdxlTriggerMode::NONE
        ),
        static_cast<int>(
            manager.adxlMode()
        )
    );

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

    const AcquisitionManagerStats stats =
        manager.stats();

    TEST_ASSERT_EQUAL_UINT32(
        0,
        stats.events_seen
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        stats.events_handled
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        stats.dropped_events
    );

    TEST_ASSERT_EQUAL_UINT32(
        0,
        manager.pipelineErrors().
            total_errors
    );
}

// New: deterministic stop/restart while a PERIODIC read is in flight.
void test_periodic_inflight_read_cannot_cross_session()
{
    driver.sample.x = 321;
    driver.sample.y = 0;
    driver.sample.z = 0;
    TEST_ASSERT_TRUE(manager.configureAdxlPeriodic(1000ULL));
    TEST_ASSERT_TRUE(manager.startAcquisition(nowUs()));
    AcquisitionEvent event = {};
    event.source = AcquisitionEventSource::ADXL345;
    event.type = AcquisitionEventType::SAMPLE_DUE;
    event.scheduled_time_us = nowUs() - 100ULL;
    TEST_ASSERT_TRUE(manager.enqueue(event));
    TEST_ASSERT_TRUE_MESSAGE(startBlockedConsumer(), "Consumer did not enter readRaw");
    manager.stopAcquisition();
    TEST_ASSERT_TRUE(manager.startAcquisition(nowUs()));
    releaseAndJoinConsumer();
    TEST_ASSERT_EQUAL_UINT16(0, manager.sampleCount());
    TEST_ASSERT_EQUAL_UINT32(1, manager.stats().dropped_events);
    TEST_ASSERT_EQUAL_UINT32(0, manager.stats().events_handled);
    TEST_ASSERT_EQUAL_UINT32(1, driver.read_calls);
    event.scheduled_time_us = nowUs() - 100ULL;
    TEST_ASSERT_TRUE(manager.enqueue(event));
    TEST_ASSERT_EQUAL_UINT16(1, manager.processAvailable(1));
    TEST_ASSERT_EQUAL_UINT16(1, manager.sampleCount());
    SamplePacket packet = {};
    TEST_ASSERT_TRUE(manager.popSample(packet));
    TEST_ASSERT_EQUAL_UINT32(0, packet.sample_index);
    TEST_ASSERT_EQUAL_INT16(321, packet.x_raw);
}

// New: identical stop/restart test for DATA_READY timestamp provenance.
void test_data_ready_inflight_read_cannot_cross_session()
{
    driver.sample.x = 654;
    driver.sample.y = 0;
    driver.sample.z = 0;
    startDataReadyMode(10000000ULL);
    TEST_ASSERT_TRUE(manager.enqueueAdxlDataReady(nowUs()));
    TEST_ASSERT_TRUE_MESSAGE(startBlockedConsumer(), "Consumer did not enter readRaw");
    manager.stopAcquisition();
    TEST_ASSERT_TRUE(manager.startAcquisition(nowUs()));
    releaseAndJoinConsumer();
    TEST_ASSERT_EQUAL_UINT16(0, manager.sampleCount());
    TEST_ASSERT_EQUAL_UINT32(1, manager.stats().dropped_events);
    TEST_ASSERT_EQUAL_UINT32(0, manager.stats().events_handled);
    TEST_ASSERT_EQUAL_UINT32(1, driver.read_calls);
    const uint64_t new_timestamp_us = nowUs();
    TEST_ASSERT_TRUE(manager.enqueueAdxlDataReady(new_timestamp_us));
    TEST_ASSERT_EQUAL_UINT16(1, manager.processAvailable(1));
    SamplePacket packet = {};
    TEST_ASSERT_TRUE(manager.popSample(packet));
    TEST_ASSERT_EQUAL_UINT32(0, packet.sample_index);
    TEST_ASSERT_EQUAL_INT16(654, packet.x_raw);
    TEST_ASSERT_TRUE(packet.timestamp_us == new_timestamp_us);
}

// New: start/stop gate the stream; buffered samples persist until drained.
void test_stop_restart_preserves_buffer_and_sample_index()
{
    startDataReadyMode();
    driver.sample.x = 10;
    const uint64_t first = nowUs();
    TEST_ASSERT_TRUE(manager.enqueueAdxlDataReady(first));
    TEST_ASSERT_EQUAL_UINT16(1, manager.processAvailable(1));
    TEST_ASSERT_EQUAL_UINT16(1, manager.sampleCount());
    manager.stopAcquisition();
    TEST_ASSERT_EQUAL_UINT16(1, manager.sampleCount());
    TEST_ASSERT_TRUE(manager.startAcquisition(nowUs()));
    driver.sample.x = 20;
    delayMicroseconds(20);
    const uint64_t second = nowUs();
    TEST_ASSERT_TRUE(manager.enqueueAdxlDataReady(second));
    TEST_ASSERT_EQUAL_UINT16(1, manager.processAvailable(1));
    TEST_ASSERT_EQUAL_UINT16(2, manager.sampleCount());
    SamplePacket packet = {};
    TEST_ASSERT_TRUE(manager.popSample(packet));
    TEST_ASSERT_EQUAL_UINT32(0, packet.sample_index);
    TEST_ASSERT_EQUAL_INT16(10, packet.x_raw);
    TEST_ASSERT_TRUE(packet.timestamp_us == first);
    TEST_ASSERT_TRUE(manager.popSample(packet));
    TEST_ASSERT_EQUAL_UINT32(1, packet.sample_index);
    TEST_ASSERT_EQUAL_INT16(20, packet.x_raw);
    TEST_ASSERT_TRUE(packet.timestamp_us == second);
    TEST_ASSERT_TRUE(manager.samplesEmpty());
}

void setup()
{
    delay(
        3000
    );

    UNITY_BEGIN();

    RUN_TEST(
        test_periodic_event_reads_adxl_and_creates_sample
    );

    RUN_TEST(
        test_valid_data_ready_preserves_timestamp
    );

    RUN_TEST(
        test_zero_data_ready_timestamp_is_rejected
    );

    RUN_TEST(
        test_backlogged_data_ready_only_newest_event_can_create_sample
    );

    RUN_TEST(
        test_stale_data_ready_is_dropped_before_sensor_read
    );

    RUN_TEST(
        test_future_data_ready_timestamp_is_dropped
    );

    RUN_TEST(
        test_adxl_read_failure_is_visible
    );

    RUN_TEST(
        test_stop_acquisition_flushes_queued_event
    );

    RUN_TEST(
        test_stop_acquisition_rejects_isr_event
    );

    RUN_TEST(
        test_data_ready_is_rejected_in_periodic_mode
    );

    RUN_TEST(
        test_unsupported_source_is_rejected_during_processing
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
        test_scheduler_missed_intervals_and_lateness_are_visible
    );

    RUN_TEST(
        test_reset_clears_manager_state
    );

    RUN_TEST(test_periodic_inflight_read_cannot_cross_session);
    RUN_TEST(test_data_ready_inflight_read_cannot_cross_session);
    RUN_TEST(test_stop_restart_preserves_buffer_and_sample_index);
    UNITY_END();
}

void loop()
{
}
