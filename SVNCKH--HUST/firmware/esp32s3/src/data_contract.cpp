#include "data_contract.h"


namespace DataContract
{





const char* modeName(
    AcquisitionMode mode
)
{
    switch (mode)
    {
        case AcquisitionMode::DATA_READY:
            return "DATA_READY";


        case AcquisitionMode::FIFO:
            return "FIFO";
    }


    return "UNKNOWN";
}






void printRecordBegin(
    const RecordMetadata& metadata
)
{
    Serial.println();

    Serial.println(
        "#BEGIN_RECORD"
    );






    Serial.print(
        "#data_contract_version="
    );

    Serial.println(
        CONTRACT_VERSION
    );


    Serial.print(
        "#protocol_version="
    );

    Serial.println(
        metadata.protocolVersion
    );


    Serial.print(
        "#firmware_version="
    );

    Serial.println(
        metadata.firmwareVersion
    );









    Serial.print(
        "#data_source="
    );

    Serial.println(
        metadata.dataSource
    );






    Serial.print(
        "#record_id="
    );

    Serial.println(
        metadata.recordId
    );


    Serial.print(
        "#condition_id="
    );

    Serial.println(
        metadata.conditionId
    );






    Serial.print(
        "#sensor_id="
    );

    Serial.println(
        metadata.sensorId
    );


    Serial.print(
        "#odr_hz="
    );

    Serial.println(
        metadata.odrHz
    );


    Serial.print(
        "#range_g="
    );

    Serial.println(
        metadata.rangeG
    );


    Serial.print(
        "#acquisition_mode="
    );

    Serial.println(
        modeName(
            metadata.acquisitionMode
        )
    );


    Serial.print(
        "#fifo_watermark="
    );

    Serial.println(
        metadata.fifoWatermark
    );






    Serial.print(
        "#timestamp_clock="
    );

    Serial.println(
        metadata.timestampClock
    );


    Serial.print(
        "#timestamp_semantics="
    );

    Serial.println(
        metadata.timestampSemantics
    );






    Serial.print(
        "#calibration_version="
    );

    Serial.println(
        metadata.calibrationVersion
    );


    Serial.print(
        "#sensor_health_version="
    );

    Serial.println(
        metadata.sensorHealthVersion
    );






    Serial.printf(
        "#start_time_us=%llu\n",
        static_cast<unsigned long long>(
            metadata.startTimeUs
        )
    );


    Serial.print(
        "#requested_duration_s="
    );

    Serial.println(
        metadata.requestedDurationS
    );






    if (metadata.stressActive)
    {
        Serial.print(
            "#stress_run="
        );


        Serial.print(
            metadata.stressRunIndex
        );


        Serial.print(
            "/"
        );


        Serial.println(
            metadata.stressRunTotal
        );
    }






    Serial.print(
        "#sample_format="
    );

    Serial.println(
        SAMPLE_FORMAT
    );


    printSampleHeader();
}






void printSampleHeader()
{
    Serial.print(
        "#COLUMNS="
    );

    Serial.println(
        SAMPLE_COLUMNS
    );
}






void printSample(
    const SamplePacket& sample
)
{
    Serial.printf(
        "%lu,%llu,%d,%d,%d,%u\n",

        static_cast<unsigned long>(
            sample.sample_index
        ),

        static_cast<unsigned long long>(
            sample.timestamp_us
        ),

        static_cast<int>(
            sample.x_raw
        ),

        static_cast<int>(
            sample.y_raw
        ),

        static_cast<int>(
            sample.z_raw
        ),

        static_cast<unsigned int>(
            sample.status_flags
        )
    );
}






void printRecordFooter(
    const RecordFooter& footer
)
{
    Serial.println();

    Serial.println(
        "#FOOTER"
    );






    Serial.printf(
        "#stop_time_us=%llu\n",
        static_cast<unsigned long long>(
            footer.stopTimeUs
        )
    );






    Serial.print(
        "#expected_samples="
    );

    Serial.println(
        footer.expectedSamples
    );


    Serial.print(
        "#received_samples="
    );

    Serial.println(
        footer.receivedSamples
    );


    Serial.print(
        "#consumed_samples="
    );

    Serial.println(
        footer.consumedSamples
    );






    Serial.print(
        "#event_irq_count="
    );

    Serial.println(
        footer.eventIrqCount
    );


    Serial.print(
        "#event_processed_count="
    );

    Serial.println(
        footer.eventProcessedCount
    );


    Serial.print(
        "#event_queue_overflow="
    );

    Serial.println(
        footer.eventQueueOverflow
    );






    Serial.print(
        "#fifo_batches_processed="
    );

    Serial.println(
        footer.fifoBatchesProcessed
    );


    Serial.print(
        "#fifo_samples_read="
    );

    Serial.println(
        footer.fifoSamplesRead
    );


    Serial.print(
        "#final_fifo_partial_samples="
    );

    Serial.println(
        footer.finalFifoPartialSamples
    );
    Serial.print(
        "#ring_peak_occupancy="
    );
    Serial.println(
        footer.ringPeakOccupancy
    );
    Serial.print(
        "#sequence_errors="
    );
    Serial.println(
        footer.sequenceErrors
    );
    Serial.print(
        "#timestamp_order_errors="
    );
    Serial.println(
        footer.timestampOrderErrors
    );
    Serial.print(
        "#i2c_errors="
    );
    Serial.println(
        footer.errors.i2c_errors
    );
    Serial.print(
        "#timeouts="
    );
    Serial.println(
        footer.errors.timeouts
    );
    Serial.print(
        "#disconnects="
    );
    Serial.println(
        footer.errors.disconnects
    );
    Serial.print(
        "#buffer_overflows="
    );
    Serial.println(
        footer.errors.buffer_overflows
    );
    Serial.print(
        "#fifo_overruns="
    );
    Serial.println(
        footer.errors.fifo_overruns
    );
    Serial.print(
        "#dropped_samples="
    );
    Serial.println(
        footer.errors.dropped_samples
    );
    Serial.print(
        "#recovery_attempts="
    );
    Serial.println(
        footer.errors.recovery_attempts
    );
    Serial.print(
        "#recovery_successes="
    );
    Serial.println(
        footer.errors.recovery_successes
    );
    Serial.print(
        "#recovery_failures="
    );
    Serial.println(
        footer.errors.recovery_failures
    );
    Serial.print(
        "#total_errors="
    );
    Serial.println(
        footer.errors.total_errors
    );
    Serial.print(
        "#stop_reason="
    );
    Serial.println(
        footer.stopReason
    );
    Serial.print(
        "#qc_status="
    );
    Serial.println(
        footer.qcStatus
    );
    Serial.println(
        "#END_RECORD"
    );
}
}
