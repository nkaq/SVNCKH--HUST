#include <Arduino.h>
#include "esp_timer.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/portmacro.h"

#include "config.h"
#include "daq_types.h"
#include "state_machine.h"
#include "error_manager.h"
#include "ring_buffer.h"
#include "adxl345_driver.h"
#include "acquisition.h"
#include "protocol.h"
#include "fault_injector.h"
#include "data_contract.h"














constexpr uint32_t MAX_RECORD_SECONDS = 3600;
constexpr uint16_t MAX_STRESS_RUNS = 100;
constexpr uint64_t SAMPLE_PERIOD_US =
    1000000ULL / Config::ADXL_ODR_HZ;





StateMachine stateMachine;
ErrorManager errorManager;
RingBuffer ringBuffer;
ADXL345Driver adxl;
AcquisitionEngine acquisition;
Protocol protocol;
FaultInjector faultInjector;





AcquisitionMode selectedMode = AcquisitionMode::DATA_READY;
AcquisitionMode activeMode = AcquisitionMode::DATA_READY;

bool streamEnabled = false;

char recordId[48] = "UNSET";
char conditionId[48] = "UNSET";





volatile uint64_t eventTimestampQueue[Config::EVENT_QUEUE_CAPACITY];
volatile uint16_t eventWriteIndex = 0;
volatile uint16_t eventReadIndex = 0;
volatile uint16_t eventQueueCount = 0;

portMUX_TYPE eventMux = portMUX_INITIALIZER_UNLOCKED;

volatile uint32_t eventIrqCount = 0;
volatile uint32_t eventQueueOverflowCount = 0;
volatile uint32_t eventProcessedCount = 0;





volatile bool producerEnabled = false;
volatile bool processingEnabled = false;
volatile bool acquisitionBusy = false;
volatile bool criticalFaultRequested = false;

bool runActive = false;
bool stopRequested = false;
bool finalFifoDrainDone = false;

uint64_t recordStartTimeUs = 0;
uint64_t recordStopTimeUs = 0;
uint32_t requestedDurationSeconds = Config::DEFAULT_RECORD_SECONDS;

char stopReason[32] = "NONE";





volatile uint32_t consumedCount = 0;
volatile uint32_t sequenceErrors = 0;
volatile uint32_t timestampOrderErrors = 0;
volatile uint32_t intraBatchTimestampErrors = 0;

volatile uint32_t fifoBatchesProcessed = 0;
volatile uint32_t fifoSamplesRead = 0;
volatile uint32_t finalFifoPartialSamples = 0;

uint32_t expectedOutputIndex = 0;
uint64_t previousOutputTimestampUs = 0;

uint8_t consecutiveAcquisitionFailures = 0;
uint8_t consecutiveRecoveryFailures = 0;
bool recoverySeen = false;





bool stressActive = false;
bool stressAbortRequested = false;

uint32_t stressDurationSeconds = Config::DEFAULT_STRESS_SECONDS;
uint16_t stressTotalRuns = Config::DEFAULT_STRESS_RUNS;
uint16_t stressRunsCompleted = 0;
uint16_t stressPassCount = 0;
uint16_t stressFailCount = 0;

uint32_t nextStressStartMs = 0;





const char* modeName(AcquisitionMode mode)
{
    return mode == AcquisitionMode::FIFO
        ? "FIFO"
        : "DATA_READY";
}

const char* timestampSemantics(AcquisitionMode mode)
{
    return mode == AcquisitionMode::FIFO
        ? "RECONSTRUCTED_FROM_FIFO_IRQ_AND_ODR"
        : "DATA_READY_IRQ_TIME";
}

void copyTextSafe(
    char* destination,
    size_t destinationSize,
    const char* source
)
{
    if (
        destination == nullptr ||
        source == nullptr ||
        destinationSize == 0
    ) {
        return;
    }

    strncpy(
        destination,
        source,
        destinationSize - 1
    );

    destination[destinationSize - 1] = '\0';
}

void printState()
{


    Serial.printf(
        "#STATE=%s\n",
        stateMachine.stateName()
    );
}

uint32_t expectedSamplesFromElapsed()
{
    if (recordStopTimeUs <= recordStartTimeUs) {
        return 0;
    }

    const uint64_t durationUs =
        recordStopTimeUs - recordStartTimeUs;

    return static_cast<uint32_t>(
        (durationUs * Config::ADXL_ODR_HZ) /
        1000000ULL
    );
}





bool IRAM_ATTR pushEventTimestampFromISR(uint64_t timestampUs)
{
    bool success = false;

    portENTER_CRITICAL_ISR(&eventMux);

    if (eventQueueCount < Config::EVENT_QUEUE_CAPACITY) {
        eventTimestampQueue[eventWriteIndex] = timestampUs;

        eventWriteIndex++;
        if (eventWriteIndex >= Config::EVENT_QUEUE_CAPACITY) {
            eventWriteIndex = 0;
        }

        eventQueueCount++;
        success = true;
    } else {
        eventQueueOverflowCount++;
    }

    portEXIT_CRITICAL_ISR(&eventMux);

    return success;
}

bool pushEventTimestampFromTask(uint64_t timestampUs)
{
    bool success = false;

    portENTER_CRITICAL(&eventMux);

    if (eventQueueCount < Config::EVENT_QUEUE_CAPACITY) {
        eventTimestampQueue[eventWriteIndex] = timestampUs;

        eventWriteIndex++;
        if (eventWriteIndex >= Config::EVENT_QUEUE_CAPACITY) {
            eventWriteIndex = 0;
        }

        eventQueueCount++;
        success = true;
    } else {
        eventQueueOverflowCount++;
    }

    portEXIT_CRITICAL(&eventMux);

    return success;
}

bool popEventTimestamp(uint64_t& timestampUs)
{
    bool success = false;

    portENTER_CRITICAL(&eventMux);

    if (eventQueueCount > 0) {
        timestampUs = eventTimestampQueue[eventReadIndex];

        eventReadIndex++;
        if (eventReadIndex >= Config::EVENT_QUEUE_CAPACITY) {
            eventReadIndex = 0;
        }

        eventQueueCount--;
        success = true;
    }

    portEXIT_CRITICAL(&eventMux);

    return success;
}

uint16_t getEventQueueSize()
{
    portENTER_CRITICAL(&eventMux);
    const uint16_t size = eventQueueCount;
    portEXIT_CRITICAL(&eventMux);

    return size;
}

void resetEventQueue()
{
    portENTER_CRITICAL(&eventMux);

    eventWriteIndex = 0;
    eventReadIndex = 0;
    eventQueueCount = 0;
    eventIrqCount = 0;
    eventQueueOverflowCount = 0;

    portEXIT_CRITICAL(&eventMux);

    eventProcessedCount = 0;
}






void IRAM_ATTR adxlInterruptISR()
{
    if (!producerEnabled) {
        return;
    }

    const uint64_t timestampUs =
        static_cast<uint64_t>(esp_timer_get_time());

    eventIrqCount++;
    pushEventTimestampFromISR(timestampUs);
}





bool ringBufferSelfTest()
{
    ringBuffer.reset();

    for (uint32_t i = 0; i < ringBuffer.capacity(); i++) {
        SamplePacket packet{};
        packet.sample_index = i;
        packet.timestamp_us = i * SAMPLE_PERIOD_US;
        packet.status_flags = STATUS_OK;

        if (!ringBuffer.push(packet)) {
            ringBuffer.reset();
            return false;
        }
    }

    if (!ringBuffer.isFull()) {
        ringBuffer.reset();
        return false;
    }

    SamplePacket extra{};
    extra.sample_index = 999999;

    if (ringBuffer.push(extra)) {
        ringBuffer.reset();
        return false;
    }

    for (uint32_t expected = 0;
         expected < ringBuffer.capacity();
         expected++) {

        SamplePacket packet{};

        if (!ringBuffer.pop(packet)) {
            ringBuffer.reset();
            return false;
        }

        if (packet.sample_index != expected) {
            ringBuffer.reset();
            return false;
        }
    }

    const bool passed = ringBuffer.isEmpty();
    ringBuffer.reset();

    return passed;
}

bool recoverySoftwareSelfTest()
{
    if (Config::ADXL_HARDWARE_PRESENT) {
        Serial.println("Recovery self-test=SKIP_HARDWARE_MODE");
        return true;
    }

    errorManager.markRecoveryAttempt();

    if (!adxl.recover(AcquisitionMode::DATA_READY)) {
        errorManager.markRecoveryFailure();
        return false;
    }

    errorManager.markRecoverySuccess();
    errorManager.reset();

    return true;
}





bool configureSensorForRun(AcquisitionMode mode)
{
    if (mode == AcquisitionMode::FIFO) {
        return adxl.configureFifo(
            Config::FIFO_WATERMARK
        );
    }

    return adxl.configureDataReady();
}





void armFault(InjectedFault fault)
{
    faultInjector.arm(fault);

    Serial.println();
    Serial.println("===== FAULT INJECTION =====");
    Serial.print("FAULT_ARMED=");
    Serial.println(faultInjector.faultName(fault));

    if (fault == InjectedFault::SENSOR_DISCONNECT) {
        Serial.println("disconnect_active=1");
    }

    Serial.println("===========================");
}

void clearInjectedFault()
{
    faultInjector.reset();
    consecutiveAcquisitionFailures = 0;
    consecutiveRecoveryFailures = 0;
    acquisition.clearReadFailures();

    Serial.println();
    Serial.println("===== FAULT CLEAR =====");
    Serial.println("fault_pending=NONE");
    Serial.println("disconnect_active=0");
    Serial.println("FAULT_CLEAR=OK");
    Serial.println("=======================");
}

bool attemptSensorRecovery()
{
    errorManager.markRecoveryAttempt();

    Serial.println("RECOVERY_ATTEMPT");



    if (faultInjector.isDisconnectActive()) {
        errorManager.markRecoveryFailure();
        consecutiveRecoveryFailures++;

        Serial.println("RECOVERY_RESULT=FAIL");
        Serial.println("reason=INJECTED_SENSOR_DISCONNECT");

        if (
            consecutiveRecoveryFailures >=
            Config::MAX_RECOVERY_FAILURES_BEFORE_ERROR
        ) {
            errorManager.report(
                ErrorCode::SENSOR_DISCONNECT
            );

            criticalFaultRequested = true;
        }

        return false;
    }

    if (adxl.recover(activeMode)) {
        errorManager.markRecoverySuccess();

        consecutiveAcquisitionFailures = 0;
        consecutiveRecoveryFailures = 0;
        acquisition.clearReadFailures();
        recoverySeen = true;

        Serial.println("RECOVERY_RESULT=PASS");
        return true;
    }

    errorManager.markRecoveryFailure();
    consecutiveRecoveryFailures++;

    Serial.println("RECOVERY_RESULT=FAIL");
    Serial.println("reason=DRIVER_RECOVERY_FAILED");

    if (
        consecutiveRecoveryFailures >=
        Config::MAX_RECOVERY_FAILURES_BEFORE_ERROR
    ) {
        errorManager.report(
            ErrorCode::SENSOR_DISCONNECT
        );

        criticalFaultRequested = true;
    }

    return false;
}

void runManualRecoveryTest()
{
    Serial.println();
    Serial.println("===== MANUAL RECOVERY TEST =====");



    const bool wasError =
        stateMachine.getState() == DaqState::ERROR;

    if (wasError) {
        if (!stateMachine.transitionTo(DaqState::SELF_CHECK)) {
            Serial.println("RECOVERY_RESULT=FAIL");
            Serial.println("reason=STATE_TRANSITION_FAILED");
            Serial.println("================================");
            return;
        }

        printState();
    }

    errorManager.markRecoveryAttempt();
    Serial.println("RECOVERY_ATTEMPT");

    bool recovered = false;

    if (!faultInjector.isDisconnectActive()) {
        recovered = adxl.recover(selectedMode);

        if (recovered) {
            recovered = adxl.selfCheck();
        }
    }

    if (recovered) {
        errorManager.markRecoverySuccess();
        acquisition.clearReadFailures();
        consecutiveAcquisitionFailures = 0;
        consecutiveRecoveryFailures = 0;
        criticalFaultRequested = false;
        recoverySeen = true;

        Serial.println("RECOVERY_RESULT=PASS");

        if (wasError) {
            stateMachine.transitionTo(DaqState::READY);
            printState();
        }
    } else {
        errorManager.markRecoveryFailure();

        Serial.println("RECOVERY_RESULT=FAIL");

        if (faultInjector.isDisconnectActive()) {
            Serial.println("reason=INJECTED_SENSOR_DISCONNECT");
        } else {
            Serial.println("reason=DRIVER_RECOVERY_FAILED");
        }

        if (wasError) {
            stateMachine.transitionTo(DaqState::ERROR);
            printState();
        }
    }

    Serial.println("================================");
}


bool applyInjectedFaultBeforeAcquisition(uint32_t expectedDroppedSamples)
{

    if (faultInjector.isDisconnectActive()) {
        errorManager.report(ErrorCode::I2C_ERROR);
        errorManager.addDroppedSamples(expectedDroppedSamples);

        Serial.println("FAULT_INJECTED=SENSOR_DISCONNECT");
        return true;
    }

    if (
        faultInjector.consume(
            InjectedFault::I2C_ERROR
        )
    ) {
        errorManager.report(ErrorCode::I2C_ERROR);
        errorManager.addDroppedSamples(expectedDroppedSamples);

        Serial.println("FAULT_INJECTED=I2C_ERROR");
        return true;
    }

    if (
        faultInjector.consume(
            InjectedFault::TIMEOUT
        )
    ) {


        errorManager.report(ErrorCode::TIMEOUT);
        errorManager.addDroppedSamples(expectedDroppedSamples);

        Serial.println("FAULT_INJECTED=TIMEOUT");
        return true;
    }

    if (
        faultInjector.consume(
            InjectedFault::BUFFER_OVERFLOW
        )
    ) {

        errorManager.report(
            ErrorCode::BUFFER_OVERFLOW
        );

        Serial.println("FAULT_INJECTED=BUFFER_OVERFLOW");
    }

    if (
        faultInjector.consume(
            InjectedFault::FIFO_OVERRUN
        )
    ) {
        errorManager.report(
            ErrorCode::FIFO_OVERRUN
        );

        Serial.println("FAULT_INJECTED=FIFO_OVERRUN");
    }

    return false;
}

void handleFailedAcquisition(
    uint64_t operationDurationUs,
    uint32_t droppedSamplesAlreadyCounted
)
{
    consecutiveAcquisitionFailures++;



    if (
        operationDurationUs >=
        (static_cast<uint64_t>(Config::I2C_TIMEOUT_MS) * 1000ULL)
    ) {
        errorManager.report(ErrorCode::TIMEOUT);
    }


    if (droppedSamplesAlreadyCounted == 0) {
        errorManager.addDroppedSamples(1);
    }

    if (
        consecutiveAcquisitionFailures >=
        Config::RECOVERY_TRIGGER_CONSECUTIVE_READ_FAILURES
    ) {
        attemptSensorRecovery();
    }
}





void mockEventTask(void* parameter)
{
    bool wasEnabled = false;
    TickType_t lastWake = xTaskGetTickCount();

    while (true) {
        if (
            Config::ADXL_HARDWARE_PRESENT ||
            !producerEnabled
        ) {
            wasEnabled = false;
            vTaskDelay(pdMS_TO_TICKS(1));
            continue;
        }

        if (!wasEnabled) {
            lastWake = xTaskGetTickCount();
            wasEnabled = true;
        }

        const AcquisitionMode modeSnapshot = activeMode;

        uint32_t periodMs =
            1000UL / Config::ADXL_ODR_HZ;

        if (modeSnapshot == AcquisitionMode::FIFO) {
            periodMs =
                (Config::FIFO_WATERMARK * 1000UL) /
                Config::ADXL_ODR_HZ;
        }

        if (periodMs == 0) {
            periodMs = 1;
        }

        vTaskDelayUntil(
            &lastWake,
            pdMS_TO_TICKS(periodMs)
        );

        if (!producerEnabled) {
            continue;
        }

        const uint64_t timestampUs =
            static_cast<uint64_t>(esp_timer_get_time());

        eventIrqCount++;
        pushEventTimestampFromTask(timestampUs);
    }
}





bool processMockFifoEvent(uint64_t irqTimestampUs)
{
    const uint16_t status =
        recoverySeen
            ? STATUS_RECOVERY_SEEN
            : STATUS_OK;

    for (uint16_t i = 0;
         i < Config::FIFO_WATERMARK;
         i++) {

        const uint64_t samplesBack =
            Config::FIFO_WATERMARK - 1 - i;

        const uint64_t timestampUs =
            irqTimestampUs -
            (samplesBack * SAMPLE_PERIOD_US);

        acquisition.pushRawSample(
            timestampUs,
            0,
            0,
            0,
            status
        );
    }

    fifoBatchesProcessed++;
    fifoSamplesRead += Config::FIFO_WATERMARK;
    recoverySeen = false;

    return true;
}

bool processHardwareFifoEvent(uint64_t irqTimestampUs)
{
    RawSample samples[Config::ADXL_FIFO_MAX_SAMPLES];
    uint8_t samplesRead = 0;

    if (
        !adxl.readFifo(
            samples,
            Config::ADXL_FIFO_MAX_SAMPLES,
            samplesRead
        )
    ) {
        errorManager.report(ErrorCode::I2C_ERROR);
        return false;
    }

    if (samplesRead == 0) {
        errorManager.report(ErrorCode::TIMEOUT);
        return false;
    }

    const uint16_t status =
        recoverySeen
            ? STATUS_RECOVERY_SEEN
            : STATUS_OK;

    for (uint8_t i = 0; i < samplesRead; i++) {
        const uint64_t samplesBack =
            samplesRead - 1 - i;

        const uint64_t timestampUs =
            irqTimestampUs -
            (samplesBack * SAMPLE_PERIOD_US);

        acquisition.pushRawSample(
            timestampUs,
            samples[i].x,
            samples[i].y,
            samples[i].z,
            status
        );
    }

    fifoBatchesProcessed++;
    fifoSamplesRead += samplesRead;
    recoverySeen = false;

    return true;
}





void acquisitionTask(void* parameter)
{
    while (true) {
        if (!processingEnabled) {
            vTaskDelay(pdMS_TO_TICKS(1));
            continue;
        }

        uint64_t irqTimestampUs = 0;

        if (!popEventTimestamp(irqTimestampUs)) {
            vTaskDelay(pdMS_TO_TICKS(1));
            continue;
        }

        acquisitionBusy = true;

        const uint64_t operationStartUs =
            static_cast<uint64_t>(esp_timer_get_time());

        bool success = false;
        bool droppedAlreadyCounted = false;

        const uint32_t expectedDrops =
            activeMode == AcquisitionMode::FIFO
                ? Config::FIFO_WATERMARK
                : 1;

        if (
            applyInjectedFaultBeforeAcquisition(
                expectedDrops
            )
        ) {
            droppedAlreadyCounted = true;
            success = false;
        } else {
            if (activeMode == AcquisitionMode::DATA_READY) {
                success = acquisition.acquireOneSample(
                    irqTimestampUs
                );
            } else {
                success = Config::ADXL_HARDWARE_PRESENT
                    ? processHardwareFifoEvent(irqTimestampUs)
                    : processMockFifoEvent(irqTimestampUs);
            }
        }

        const uint64_t operationEndUs =
            static_cast<uint64_t>(esp_timer_get_time());

        const uint64_t operationDurationUs =
            operationEndUs - operationStartUs;

        if (success) {
            consecutiveAcquisitionFailures = 0;
        } else {
            handleFailedAcquisition(
                operationDurationUs,
                droppedAlreadyCounted
                    ? expectedDrops
                    : 0
            );
        }

        eventProcessedCount++;
        acquisitionBusy = false;
    }
}





void outputTask(void* parameter)
{
    while (true) {
        SamplePacket packet{};

        if (!ringBuffer.pop(packet)) {
            vTaskDelay(pdMS_TO_TICKS(1));
            continue;
        }

        if (packet.sample_index != expectedOutputIndex) {
            sequenceErrors++;
            expectedOutputIndex = packet.sample_index + 1;
        } else {
            expectedOutputIndex++;
        }

        if (
            previousOutputTimestampUs != 0 &&
            packet.timestamp_us <= previousOutputTimestampUs
        ) {
            timestampOrderErrors++;
        }

        if (
            activeMode == AcquisitionMode::FIFO &&
            previousOutputTimestampUs != 0 &&
            (packet.sample_index % Config::FIFO_WATERMARK) != 0
        ) {
            const uint64_t deltaUs =
                packet.timestamp_us -
                previousOutputTimestampUs;

            if (deltaUs != SAMPLE_PERIOD_US) {
                intraBatchTimestampErrors++;
            }
        }

        previousOutputTimestampUs = packet.timestamp_us;
        consumedCount++;

        if (streamEnabled && runActive) {
            DataContract::printSample(
                packet
            );
        }
    }
}





void resetRunData()
{
    producerEnabled = false;
    processingEnabled = false;

    ringBuffer.reset();
    resetEventQueue();
    acquisition.reset();
    errorManager.reset();

    consumedCount = 0;
    sequenceErrors = 0;
    timestampOrderErrors = 0;
    intraBatchTimestampErrors = 0;

    fifoBatchesProcessed = 0;
    fifoSamplesRead = 0;
    finalFifoPartialSamples = 0;

    expectedOutputIndex = 0;
    previousOutputTimestampUs = 0;

    consecutiveAcquisitionFailures = 0;
    consecutiveRecoveryFailures = 0;
    recoverySeen = false;

    criticalFaultRequested = false;
    acquisitionBusy = false;

    runActive = false;
    stopRequested = false;
    finalFifoDrainDone = false;

    recordStartTimeUs = 0;
    recordStopTimeUs = 0;

    copyTextSafe(
        stopReason,
        sizeof(stopReason),
        "NONE"
    );

    acquisition.clearReadFailures();
}





void printMetadataHeader()
{
    DataContract::RecordMetadata metadata;

    metadata.protocolVersion =
        Config::PROTOCOL_VERSION;

    metadata.firmwareVersion =
        Config::FIRMWARE_VERSION;

    metadata.dataSource =
        Config::ADXL_HARDWARE_PRESENT
            ? DataContract::SOURCE_HARDWARE
            : DataContract::SOURCE_MOCK;

    metadata.recordId =
        recordId;

    metadata.conditionId =
        conditionId;

    metadata.sensorId =
        DataContract::SENSOR_ID_ADXL345;

    metadata.odrHz =
        Config::ADXL_ODR_HZ;

    metadata.rangeG =
        Config::ADXL_RANGE_G;

    metadata.acquisitionMode =
        activeMode;

    metadata.fifoWatermark =
        activeMode == AcquisitionMode::FIFO
            ? Config::FIFO_WATERMARK
            : 0;

    metadata.timestampClock =
        DataContract::TIMESTAMP_CLOCK;

    metadata.timestampSemantics =
        activeMode == AcquisitionMode::FIFO
            ? DataContract::TS_FIFO
            : DataContract::TS_DATA_READY;

    metadata.calibrationVersion =
        "UNSET";

    metadata.sensorHealthVersion =
        "UNSET";

    metadata.startTimeUs =
        recordStartTimeUs;

    metadata.requestedDurationS =
        requestedDurationSeconds;

    metadata.stressActive =
        stressActive;

    metadata.stressRunIndex =
        stressActive
            ? static_cast<uint16_t>(
                stressRunsCompleted + 1
            )
            : 0;

    metadata.stressRunTotal =
        stressActive
            ? stressTotalRuns
            : 0;

    DataContract::printRecordBegin(
        metadata
    );
}

void printFooter(bool qcPass)
{
    DataContract::RecordFooter footer;

    footer.stopTimeUs =
        recordStopTimeUs;

    footer.expectedSamples =
        expectedSamplesFromElapsed();

    footer.receivedSamples =
        acquisition.getSampleIndex();

    footer.consumedSamples =
        consumedCount;

    footer.eventIrqCount =
        eventIrqCount;

    footer.eventProcessedCount =
        eventProcessedCount;

    footer.eventQueueOverflow =
        eventQueueOverflowCount;

    footer.fifoBatchesProcessed =
        fifoBatchesProcessed;

    footer.fifoSamplesRead =
        fifoSamplesRead;

    footer.finalFifoPartialSamples =
        finalFifoPartialSamples;

    footer.ringPeakOccupancy =
        ringBuffer.peakOccupancy();

    footer.sequenceErrors =
        sequenceErrors;

    footer.timestampOrderErrors =
        timestampOrderErrors;

    footer.errors =
        errorManager.getCounters();

    footer.stopReason =
        stopReason;

    footer.qcStatus =
        qcPass
            ? "PASS"
            : "FAIL";

    DataContract::printRecordFooter(
        footer
    );
}





bool startRecording(
    uint32_t durationSeconds,
    bool fromStress = false
)
{
    if (stateMachine.getState() != DaqState::READY) {
        errorManager.report(ErrorCode::INVALID_STATE);
        Serial.println("ERR: START_INVALID_STATE");
        return false;
    }

    if (
        durationSeconds == 0 ||
        durationSeconds > MAX_RECORD_SECONDS
    ) {
        errorManager.report(ErrorCode::INVALID_CONFIG);
        Serial.println("ERR: INVALID_DURATION");
        return false;
    }



    (void)fromStress;





    resetRunData();

    requestedDurationSeconds = durationSeconds;
    activeMode = selectedMode;





    if (!configureSensorForRun(activeMode)) {
        errorManager.report(ErrorCode::INVALID_CONFIG);
        criticalFaultRequested = true;
        Serial.println("ERR: SENSOR_CONFIG_FAILED");
        return false;
    }





    if (!stateMachine.transitionTo(DaqState::STABILIZE)) {
        errorManager.report(ErrorCode::INVALID_STATE);
        return false;
    }

    printState();

    delay(Config::STABILIZE_TIME_MS);


    resetEventQueue();
    ringBuffer.reset();
    acquisition.reset();





    if (!stateMachine.transitionTo(DaqState::RECORD)) {
        errorManager.report(ErrorCode::INVALID_STATE);
        return false;
    }





    recordStartTimeUs =
        static_cast<uint64_t>(esp_timer_get_time());

    runActive = true;
    processingEnabled = false;
    producerEnabled = true;


    printState();
    printMetadataHeader();



    processingEnabled = true;

    return true;
}

void requestStop(const char* reason)
{
    if (stateMachine.getState() != DaqState::RECORD) {
        errorManager.report(ErrorCode::INVALID_STATE);
        Serial.println("ERR: STOP_INVALID_STATE");
        return;
    }


    producerEnabled = false;

    if (Config::ADXL_HARDWARE_PRESENT) {
        adxl.disableInterrupts();
    }

    recordStopTimeUs =
        static_cast<uint64_t>(esp_timer_get_time());

    copyTextSafe(
        stopReason,
        sizeof(stopReason),
        reason
    );

    stopRequested = true;

    stateMachine.transitionTo(DaqState::STOPPING);
    printState();
}





void performFinalFifoDrain()
{
    if (finalFifoDrainDone) {
        return;
    }

    finalFifoDrainDone = true;

    if (activeMode != AcquisitionMode::FIFO) {
        return;
    }





    if (!Config::ADXL_HARDWARE_PRESENT) {
        const uint32_t expected =
            expectedSamplesFromElapsed();















        const uint32_t partial =
            expected % Config::FIFO_WATERMARK;

        if (partial == 0) {
            return;
        }

        const uint16_t status =
            recoverySeen
                ? STATUS_RECOVERY_SEEN
                : STATUS_OK;

        for (uint32_t i = 0; i < partial; i++) {
            const uint64_t samplesBack =
                partial - 1 - i;

            const uint64_t timestampUs =
                recordStopTimeUs -
                (samplesBack * SAMPLE_PERIOD_US);

            acquisition.pushRawSample(
                timestampUs,
                0,
                0,
                0,
                status
            );
        }

        fifoSamplesRead += partial;
        finalFifoPartialSamples = partial;
        recoverySeen = false;
        return;
    }






    RawSample samples[Config::ADXL_FIFO_MAX_SAMPLES];
    uint8_t samplesRead = 0;

    if (
        !adxl.readFifo(
            samples,
            Config::ADXL_FIFO_MAX_SAMPLES,
            samplesRead
        )
    ) {
        errorManager.report(ErrorCode::I2C_ERROR);
        return;
    }

    if (samplesRead == 0) {
        return;
    }

    const uint16_t status =
        recoverySeen
            ? STATUS_RECOVERY_SEEN
            : STATUS_OK;

    for (uint8_t i = 0; i < samplesRead; i++) {
        const uint64_t samplesBack =
            samplesRead - 1 - i;

        const uint64_t timestampUs =
            recordStopTimeUs -
            (samplesBack * SAMPLE_PERIOD_US);

        acquisition.pushRawSample(
            timestampUs,
            samples[i].x,
            samples[i].y,
            samples[i].z,
            status
        );
    }

    fifoSamplesRead += samplesRead;
    finalFifoPartialSamples = samplesRead;
    recoverySeen = false;
}





bool evaluateRunQc()
{
    const ErrorCounters counters =
        errorManager.getCounters();

    const uint32_t expected =
        expectedSamplesFromElapsed();

    const uint32_t produced =
        acquisition.getSampleIndex();

    uint32_t tolerance = 0;

    if (Config::ADXL_HARDWARE_PRESENT) {
        tolerance =
            activeMode == AcquisitionMode::FIFO
                ? Config::FIFO_WATERMARK
                : 2;
    } else {
        tolerance =
            activeMode == AcquisitionMode::FIFO
                ? 0
                : 1;
    }

    const uint32_t sampleDifference =
        produced > expected
            ? produced - expected
            : expected - produced;

    bool pass = true;

    pass = pass && (produced > 0);
    pass = pass && (sampleDifference <= tolerance);
    pass = pass && (consumedCount == produced);
    pass = pass && (eventQueueOverflowCount == 0);
    pass = pass && (sequenceErrors == 0);
    pass = pass && (timestampOrderErrors == 0);
    pass = pass && (counters.buffer_overflows == 0);
    pass = pass && (counters.fifo_overruns == 0);
    pass = pass && (counters.dropped_samples == 0);
    pass = pass && (counters.total_errors == 0);

    if (activeMode == AcquisitionMode::FIFO) {
        pass = pass && (intraBatchTimestampErrors == 0);
    }

    return pass;
}

void completeStressRun(bool qcPass)
{
    if (!stressActive) {
        return;
    }

    stressRunsCompleted++;

    if (qcPass) {
        stressPassCount++;
    } else {
        stressFailCount++;
    }

    if (
        stressAbortRequested ||
        stressRunsCompleted >= stressTotalRuns
    ) {
        Serial.println();
        Serial.println("===== STEP 12 STRESS SUMMARY =====");

        Serial.print("runs_total=");
        Serial.println(stressTotalRuns);

        Serial.print("runs_completed=");
        Serial.println(stressRunsCompleted);

        Serial.print("runs_pass=");
        Serial.println(stressPassCount);

        Serial.print("runs_fail=");
        Serial.println(stressFailCount);

        Serial.print("stress_duration_s_each=");
        Serial.println(stressDurationSeconds);

        Serial.print("STRESS_STATUS=");
        if (stressAbortRequested) {
            Serial.println("ABORTED");
        } else {
            Serial.println(
                stressFailCount == 0
                    ? "PASS"
                    : "FAIL"
            );
        }

        Serial.println("==================================");

        stressActive = false;
        stressAbortRequested = false;
        return;
    }

    nextStressStartMs =
        millis() + Config::STRESS_GAP_MS;
}

void finalizeRun()
{
    if (!stateMachine.transitionTo(DaqState::QC)) {
        errorManager.report(ErrorCode::INVALID_STATE);
        return;
    }

    printState();

    const bool qcPass = evaluateRunQc();
    const ErrorCounters counters =
        errorManager.getCounters();

    printFooter(qcPass);

    Serial.println();
    Serial.println("===== RUN QC SUMMARY =====");

    Serial.print("mode=");
    Serial.println(modeName(activeMode));

    Serial.print("record_duration_us=");
    Serial.println(recordStopTimeUs - recordStartTimeUs);

    Serial.print("expected_samples=");
    Serial.println(expectedSamplesFromElapsed());

    Serial.print("sample_count=");
    Serial.println(acquisition.getSampleIndex());

    Serial.print("consumed_count=");
    Serial.println(consumedCount);

    Serial.print("event_queue_overflow=");
    Serial.println(eventQueueOverflowCount);

    Serial.print("sequence_errors=");
    Serial.println(sequenceErrors);

    Serial.print("timestamp_order_errors=");
    Serial.println(timestampOrderErrors);

    Serial.print("ring_peak_occupancy=");
    Serial.println(ringBuffer.peakOccupancy());

    Serial.print("buffer_overflows=");
    Serial.println(counters.buffer_overflows);

    Serial.print("fifo_overruns=");
    Serial.println(counters.fifo_overruns);

    Serial.print("dropped_samples=");
    Serial.println(counters.dropped_samples);

    Serial.print("total_errors=");
    Serial.println(counters.total_errors);

    Serial.print("QC_STATUS=");
    Serial.println(qcPass ? "PASS" : "FAIL");

    Serial.println("==========================");

    runActive = false;
    processingEnabled = false;
    producerEnabled = false;
    stopRequested = false;

    stateMachine.transitionTo(DaqState::READY);
    printState();

    completeStressRun(qcPass);
}

void serviceStopping()
{
    if (
        stateMachine.getState() != DaqState::STOPPING ||
        !stopRequested
    ) {
        return;
    }


    if (
        getEventQueueSize() > 0 ||
        acquisitionBusy
    ) {
        return;
    }


    if (!finalFifoDrainDone) {
        performFinalFifoDrain();
        return;
    }


    if (!ringBuffer.isEmpty()) {
        return;
    }

    if (
        consumedCount !=
        acquisition.getSampleIndex()
    ) {
        return;
    }

    finalizeRun();
}





void serviceCriticalError()
{
    if (!criticalFaultRequested) {
        return;
    }

    producerEnabled = false;
    processingEnabled = false;

    if (Config::ADXL_HARDWARE_PRESENT) {
        adxl.disableInterrupts();
    }

    if (
        runActive &&
        recordStopTimeUs == 0
    ) {
        recordStopTimeUs =
            static_cast<uint64_t>(esp_timer_get_time());
    }

    runActive = false;
    stopRequested = false;
    stressActive = false;

    if (
        stateMachine.getState() != DaqState::ERROR
    ) {
        stateMachine.transitionTo(DaqState::ERROR);
        printState();
    }

    Serial.println("CRITICAL_ERROR=SENSOR_RECOVERY_FAILED");

    criticalFaultRequested = false;
}





void printExtendedStatus()
{
    const ErrorCounters counters =
        errorManager.getCounters();

    Serial.println();
    Serial.println("===== STATUS =====");

    Serial.print("firmware=");
    Serial.println(Config::FIRMWARE_VERSION);

    Serial.print("protocol=");
    Serial.println(Config::PROTOCOL_VERSION);

    Serial.print("state=");
    Serial.println(stateMachine.stateName());

    Serial.print("source=");
    Serial.println(
        Config::ADXL_HARDWARE_PRESENT
            ? "HARDWARE"
            : "MOCK"
    );

    Serial.print("selected_mode=");
    Serial.println(modeName(selectedMode));

    Serial.print("active_mode=");
    Serial.println(modeName(activeMode));

    Serial.print("stream=");
    Serial.println(streamEnabled ? 1 : 0);

    Serial.print("record_id=");
    Serial.println(recordId);

    Serial.print("condition_id=");
    Serial.println(conditionId);

    Serial.print("fault_pending=");
    Serial.println(
        faultInjector.faultName(
            faultInjector.getPendingFault()
        )
    );

    Serial.print("disconnect_active=");
    Serial.println(
        faultInjector.isDisconnectActive()
            ? 1
            : 0
    );

    Serial.print("sample_count=");
    Serial.println(acquisition.getSampleIndex());

    Serial.print("consumed_count=");
    Serial.println(consumedCount);

    Serial.print("event_irq_count=");
    Serial.println(eventIrqCount);

    Serial.print("event_processed_count=");
    Serial.println(eventProcessedCount);

    Serial.print("event_queue_overflow=");
    Serial.println(eventQueueOverflowCount);

    Serial.print("ring_size=");
    Serial.println(ringBuffer.size());

    Serial.print("ring_peak_occupancy=");
    Serial.println(ringBuffer.peakOccupancy());

    Serial.print("last_error=");
    Serial.println(
        errorManager.errorName(
            errorManager.getLastError()
        )
    );

    Serial.print("total_errors=");
    Serial.println(counters.total_errors);

    Serial.print("i2c_errors=");
    Serial.println(counters.i2c_errors);

    Serial.print("timeouts=");
    Serial.println(counters.timeouts);

    Serial.print("disconnects=");
    Serial.println(counters.disconnects);

    Serial.print("buffer_overflows=");
    Serial.println(counters.buffer_overflows);

    Serial.print("fifo_overruns=");
    Serial.println(counters.fifo_overruns);

    Serial.print("dropped_samples=");
    Serial.println(counters.dropped_samples);

    Serial.print("recovery_attempts=");
    Serial.println(counters.recovery_attempts);

    Serial.print("recovery_successes=");
    Serial.println(counters.recovery_successes);

    Serial.print("recovery_failures=");
    Serial.println(counters.recovery_failures);

    Serial.println("==================");
}





void startStressTest(
    uint32_t durationSeconds,
    uint16_t runs
)
{
    if (
        stateMachine.getState() != DaqState::READY ||
        stressActive
    ) {
        errorManager.report(ErrorCode::INVALID_STATE);
        Serial.println("ERR: STRESS_INVALID_STATE");
        return;
    }

    if (
        durationSeconds == 0 ||
        durationSeconds > MAX_RECORD_SECONDS ||
        runs == 0 ||
        runs > MAX_STRESS_RUNS
    ) {
        errorManager.report(ErrorCode::INVALID_CONFIG);
        Serial.println("ERR: STRESS_INVALID_CONFIG");
        return;
    }

    stressDurationSeconds = durationSeconds;
    stressTotalRuns = runs;
    stressRunsCompleted = 0;
    stressPassCount = 0;
    stressFailCount = 0;
    stressAbortRequested = false;
    stressActive = true;
    nextStressStartMs = millis();

    Serial.println();
    Serial.println("STEP 12 STRESS TEST ARMED");

    Serial.print("duration_s_each=");
    Serial.println(stressDurationSeconds);

    Serial.print("runs=");
    Serial.println(stressTotalRuns);
}





void handleCommand(const ProtocolCommand& command)
{
    switch (command.type) {
        case ProtocolCommandType::HELP:
            protocol.printHelp();
            break;

        case ProtocolCommandType::STATUS:
            printExtendedStatus();
            break;

        case ProtocolCommandType::START: {
            if (stressActive) {
                Serial.println("ERR: STRESS_ACTIVE");
                break;
            }

            const uint32_t seconds =
                command.arg1 == 0
                    ? Config::DEFAULT_RECORD_SECONDS
                    : command.arg1;

            startRecording(seconds, false);
            break;
        }

        case ProtocolCommandType::STOP:
            if (stateMachine.getState() == DaqState::RECORD) {
                if (stressActive) {
                    stressAbortRequested = true;
                }

                requestStop("COMMAND");
            } else if (
                stressActive &&
                stateMachine.getState() == DaqState::READY
            ) {
                stressAbortRequested = true;
                stressActive = false;
                Serial.println("STRESS_STATUS=ABORTED");
            } else {
                errorManager.report(ErrorCode::INVALID_STATE);
                Serial.println("ERR: STOP_INVALID_STATE");
            }
            break;

        case ProtocolCommandType::SET_RECORD_ID:
            if (
                stateMachine.getState() != DaqState::READY ||
                stressActive
            ) {
                errorManager.report(ErrorCode::INVALID_STATE);
                Serial.println("ERR: SET_RECORD_ID_INVALID_STATE");
                break;
            }

            copyTextSafe(
                recordId,
                sizeof(recordId),
                command.text
            );

            Serial.print("OK: record_id=");
            Serial.println(recordId);
            break;

        case ProtocolCommandType::SET_CONDITION:
            if (
                stateMachine.getState() != DaqState::READY ||
                stressActive
            ) {
                errorManager.report(ErrorCode::INVALID_STATE);
                Serial.println("ERR: SET_CONDITION_INVALID_STATE");
                break;
            }

            copyTextSafe(
                conditionId,
                sizeof(conditionId),
                command.text
            );

            Serial.print("OK: condition=");
            Serial.println(conditionId);
            break;

        case ProtocolCommandType::SET_MODE:
            if (
                stateMachine.getState() != DaqState::READY ||
                stressActive
            ) {
                errorManager.report(ErrorCode::INVALID_STATE);
                Serial.println("ERR: SET_MODE_INVALID_STATE");
                break;
            }

            selectedMode = command.mode;

            Serial.print("OK: mode=");
            Serial.println(modeName(selectedMode));
            break;

        case ProtocolCommandType::SET_STREAM:
            if (
                stateMachine.getState() != DaqState::READY ||
                stressActive
            ) {
                errorManager.report(ErrorCode::INVALID_STATE);
                Serial.println("ERR: SET_STREAM_INVALID_STATE");
                break;
            }

            streamEnabled = command.boolValue;

            Serial.print("OK: stream=");
            Serial.println(streamEnabled ? 1 : 0);
            break;

        case ProtocolCommandType::STRESS: {
            const uint32_t seconds =
                command.arg1 == 0
                    ? Config::DEFAULT_STRESS_SECONDS
                    : command.arg1;

            const uint16_t runs =
                static_cast<uint16_t>(
                    command.arg2 == 0
                        ? Config::DEFAULT_STRESS_RUNS
                        : command.arg2
                );

            startStressTest(seconds, runs);
            break;
        }





        case ProtocolCommandType::FAULT_I2C:
            armFault(InjectedFault::I2C_ERROR);
            break;

        case ProtocolCommandType::FAULT_TIMEOUT:
            armFault(InjectedFault::TIMEOUT);
            break;

        case ProtocolCommandType::FAULT_DISCONNECT:
            armFault(InjectedFault::SENSOR_DISCONNECT);
            break;

        case ProtocolCommandType::FAULT_BUFFER_OVERFLOW:
            armFault(InjectedFault::BUFFER_OVERFLOW);
            break;

        case ProtocolCommandType::FAULT_FIFO_OVERRUN:
            armFault(InjectedFault::FIFO_OVERRUN);
            break;

        case ProtocolCommandType::RECOVER:
            if (
                stateMachine.getState() == DaqState::READY ||
                stateMachine.getState() == DaqState::ERROR
            ) {
                runManualRecoveryTest();
            } else {
                errorManager.report(ErrorCode::INVALID_STATE);
                Serial.println("ERR: RECOVER_INVALID_STATE");
            }
            break;

        case ProtocolCommandType::CLEAR_FAULT:
            clearInjectedFault();
            break;

        case ProtocolCommandType::INVALID:

            Serial.println("ERR: INVALID_COMMAND");
            break;

        case ProtocolCommandType::NONE:
        default:
            break;
    }
}





void setup()
{
    Serial.begin(Config::SERIAL_BAUD);
    delay(1000);

    Serial.println();
    Serial.println("====================================");
    Serial.println("EE2 DAQ Firmware - DAY 2");
    Serial.println("====================================");

    stateMachine.begin();
    errorManager.reset();
    ringBuffer.reset();
    faultInjector.begin();

    protocol.begin(
        &stateMachine,
        &errorManager
    );

    acquisition.begin(
        &adxl,
        &ringBuffer,
        &errorManager
    );

    printState();

    if (!adxl.begin()) {
        errorManager.report(
            ErrorCode::SENSOR_DISCONNECT
        );

        stateMachine.transitionTo(
            DaqState::ERROR
        );

        printState();
        Serial.println("BOOT_FAIL: ADXL345_BEGIN");
        return;
    }

    stateMachine.transitionTo(
        DaqState::SELF_CHECK
    );

    printState();

    if (!adxl.selfCheck()) {
        errorManager.report(
            ErrorCode::SENSOR_DISCONNECT
        );

        stateMachine.transitionTo(
            DaqState::ERROR
        );

        printState();
        Serial.println("BOOT_FAIL: ADXL345_SELF_CHECK");
        return;
    }

    Serial.print("RingBuffer self-test=");
    if (!ringBufferSelfTest()) {
        Serial.println("FAIL");

        stateMachine.transitionTo(
            DaqState::ERROR
        );

        printState();
        return;
    }
    Serial.println("PASS");

    Serial.print("Recovery self-test=");
    if (!recoverySoftwareSelfTest()) {
        Serial.println("FAIL");

        stateMachine.transitionTo(
            DaqState::ERROR
        );

        printState();
        return;
    }

    if (!Config::ADXL_HARDWARE_PRESENT) {
        Serial.println("PASS");
    }


    if (Config::ADXL_HARDWARE_PRESENT) {
        pinMode(
            Config::ADXL_INT_PIN,
            INPUT
        );

        attachInterrupt(
            digitalPinToInterrupt(
                Config::ADXL_INT_PIN
            ),
            adxlInterruptISR,
            RISING
        );
    }

    BaseType_t mockTaskResult =
        xTaskCreatePinnedToCore(
            mockEventTask,
            "MockEvent",
            4096,
            nullptr,
            2,
            nullptr,
            0
        );

    BaseType_t acquisitionTaskResult =
        xTaskCreatePinnedToCore(
            acquisitionTask,
            "Acquisition",
            6144,
            nullptr,
            3,
            nullptr,
            1
        );

    BaseType_t outputTaskResult =
        xTaskCreatePinnedToCore(
            outputTask,
            "Output",
            4096,
            nullptr,
            1,
            nullptr,
            0
        );

    if (
        mockTaskResult != pdPASS ||
        acquisitionTaskResult != pdPASS ||
        outputTaskResult != pdPASS
    ) {
        Serial.println("BOOT_FAIL: TASK_CREATION");

        stateMachine.transitionTo(
            DaqState::ERROR
        );

        printState();
        return;
    }

    stateMachine.transitionTo(
        DaqState::READY
    );

    errorManager.reset();

    printState();

    Serial.print("ADXL hardware present=");
    Serial.println(
        Config::ADXL_HARDWARE_PRESENT
            ? "true"
            : "false"
    );

    Serial.print("sizeof(SamplePacket)=");
    Serial.println(sizeof(SamplePacket));

    Serial.print("RingBuffer capacity=");
    Serial.println(ringBuffer.capacity());

    Serial.print("Default mode=");
    Serial.println(modeName(selectedMode));

    Serial.println("Type HELP for commands.");
}





void loop()
{




    protocol.poll();

    if (protocol.commandAvailable()) {
        const ProtocolCommand command =
            protocol.getCommand();

        handleCommand(command);
    }





    serviceCriticalError();





    if (
        stateMachine.getState() == DaqState::RECORD &&
        runActive &&
        !stopRequested
    ) {
        const uint64_t nowUs =
            static_cast<uint64_t>(esp_timer_get_time());

        const uint64_t targetDurationUs =
            static_cast<uint64_t>(requestedDurationSeconds) *
            1000000ULL;

        const uint64_t elapsedUs =
            nowUs - recordStartTimeUs;

        bool durationComplete =
            elapsedUs >= targetDurationUs;




        if (
            durationComplete &&
            !Config::ADXL_HARDWARE_PRESENT &&
            activeMode == AcquisitionMode::DATA_READY
        ) {
            const uint32_t nominalSamples =
                requestedDurationSeconds *
                Config::ADXL_ODR_HZ;

            durationComplete =
                eventIrqCount >= nominalSamples;
        }



        if (
            durationComplete &&
            !Config::ADXL_HARDWARE_PRESENT &&
            activeMode == AcquisitionMode::FIFO
        ) {
            const uint32_t nominalSamples =
                requestedDurationSeconds *
                Config::ADXL_ODR_HZ;

            const uint32_t completeBatches =
                nominalSamples /
                Config::FIFO_WATERMARK;

            durationComplete =
                eventIrqCount >= completeBatches;
        }

        if (durationComplete) {
            requestStop("DURATION_COMPLETE");
        }
    }





    serviceStopping();





    if (
        stressActive &&
        !stressAbortRequested &&
        stateMachine.getState() == DaqState::READY &&
        stressRunsCompleted < stressTotalRuns &&
        static_cast<int32_t>(
            millis() - nextStressStartMs
        ) >= 0
    ) {
        Serial.println();
        Serial.print("===== STRESS RUN ");
        Serial.print(stressRunsCompleted + 1);
        Serial.print('/');
        Serial.print(stressTotalRuns);
        Serial.println(" =====");

        if (
            !startRecording(
                stressDurationSeconds,
                true
            )
        ) {
            stressRunsCompleted++;
            stressFailCount++;

            if (
                stressRunsCompleted >=
                stressTotalRuns
            ) {
                Serial.println("STRESS_STATUS=FAIL");
                stressActive = false;
            } else {
                nextStressStartMs =
                    millis() +
                    Config::STRESS_GAP_MS;
            }
        }
    }

    delay(1);
}
