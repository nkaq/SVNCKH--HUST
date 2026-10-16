#!/usr/bin/env python3

"""
EE2 Data Contract v1.0 Validator

Validates serial/log output produced by:
    ESP32-S3 Acquisition Firmware v2.x

Contract:
    EE2_IT2_DC_v1.0

No external Python packages required.
"""

from __future__ import annotations

import argparse
import csv
import sys

from dataclasses import dataclass, field
from pathlib import Path
from typing import Dict, List, Optional


# ============================================================
# CONTRACT CONSTANTS
# ============================================================

CONTRACT_VERSION = "EE2_IT2_DC_v1.0"

EXPECTED_COLUMNS = [
    "sample_index",
    "timestamp_us",
    "x_raw",
    "y_raw",
    "z_raw",
    "status_flags",
]


# ============================================================
# DATA TYPES
# ============================================================

@dataclass
class Sample:
    sample_index: int
    timestamp_us: int
    x_raw: int
    y_raw: int
    z_raw: int
    status_flags: int


@dataclass
class Record:
    metadata: Dict[str, str] = field(default_factory=dict)

    footer: Dict[str, str] = field(default_factory=dict)

    samples: List[Sample] = field(default_factory=list)

    events: List[str] = field(default_factory=list)

    malformed_lines: List[str] = field(default_factory=list)


@dataclass
class ValidationResult:
    passed: bool = True

    errors: List[str] = field(default_factory=list)

    warnings: List[str] = field(default_factory=list)

    info: List[str] = field(default_factory=list)

    def fail(self, message: str) -> None:
        self.passed = False
        self.errors.append(message)

    def warn(self, message: str) -> None:
        self.warnings.append(message)

    def add_info(self, message: str) -> None:
        self.info.append(message)


# ============================================================
# HELPERS
# ============================================================

def parse_int(
    mapping: Dict[str, str],
    key: str,
) -> Optional[int]:

    value = mapping.get(key)

    if value is None:
        return None

    try:
        return int(value)

    except ValueError:
        return None


def parse_sample_line(
    line: str,
) -> Optional[Sample]:

    try:
        row = next(
            csv.reader(
                [line]
            )
        )

    except Exception:
        return None

    if len(row) != 6:
        return None

    try:
        return Sample(
            sample_index=int(row[0]),
            timestamp_us=int(row[1]),
            x_raw=int(row[2]),
            y_raw=int(row[3]),
            z_raw=int(row[4]),
            status_flags=int(row[5]),
        )

    except ValueError:
        return None


# ============================================================
# LOG PARSER
# ============================================================

def parse_log(
    path: Path,
) -> List[Record]:

    records: List[Record] = []

    current: Optional[Record] = None

    in_footer = False

    with path.open(
        "r",
        encoding="utf-8",
        errors="replace",
    ) as handle:

        for raw_line in handle:

            line = raw_line.strip()

            if not line:
                continue


            # ------------------------------------------------
            # BEGIN
            # ------------------------------------------------

            if line == "#BEGIN_RECORD":

                current = Record()

                records.append(
                    current
                )

                in_footer = False

                continue


            # ------------------------------------------------
            # Ignore everything outside record
            # ------------------------------------------------

            if current is None:
                continue


            # ------------------------------------------------
            # END
            # ------------------------------------------------

            if line == "#END_RECORD":

                current = None

                in_footer = False

                continue


            # ------------------------------------------------
            # FOOTER MARKER
            # ------------------------------------------------

            if line == "#FOOTER":

                in_footer = True

                continue


            # ------------------------------------------------
            # COMMENT / METADATA / EVENT
            # ------------------------------------------------

            if line.startswith("#"):

                content = line[1:]


                # State transitions are event lines.
                if content.startswith("STATE="):

                    current.events.append(
                        content
                    )

                    continue


                if "=" not in content:

                    current.events.append(
                        content
                    )

                    continue


                key, value = content.split(
                    "=",
                    1,
                )


                if in_footer:

                    current.footer[key] = value

                else:

                    current.metadata[key] = value


                continue


            # ------------------------------------------------
            # SAMPLE CSV
            # ------------------------------------------------

            sample = parse_sample_line(
                line
            )


            if sample is not None:

                current.samples.append(
                    sample
                )

            else:

                current.malformed_lines.append(
                    line
                )


    return records


# ============================================================
# VALIDATION
# ============================================================

def validate_record(
    record: Record,
    record_number: int,
    expected_qc: Optional[str],
) -> ValidationResult:

    result = ValidationResult()


    # ========================================================
    # CONTRACT VERSION
    # ========================================================

    contract_version = record.metadata.get(
        "data_contract_version"
    )


    if contract_version is None:

        result.fail(
            "Missing data_contract_version"
        )

    elif contract_version != CONTRACT_VERSION:

        result.fail(
            "Unsupported contract version: "
            f"{contract_version}"
        )


    # ========================================================
    # REQUIRED METADATA
    # ========================================================

    required_metadata = [
        "protocol_version",
        "firmware_version",
        "data_source",
        "record_id",
        "condition_id",
        "sensor_id",
        "odr_hz",
        "range_g",
        "acquisition_mode",
        "timestamp_clock",
        "timestamp_semantics",
        "start_time_us",
        "requested_duration_s",
        "sample_format",
        "COLUMNS",
    ]


    for key in required_metadata:

        if key not in record.metadata:

            result.fail(
                f"Missing metadata: {key}"
            )


    # ========================================================
    # DATA SOURCE
    # ========================================================

    data_source = record.metadata.get(
        "data_source"
    )


    if data_source not in {
        "MOCK",
        "HARDWARE",
    }:

        result.fail(
            "Invalid data_source: "
            f"{data_source}"
        )


    # ========================================================
    # SAMPLE FORMAT
    # ========================================================

    sample_format = record.metadata.get(
        "sample_format"
    )


    if sample_format != "CSV":

        result.fail(
            "sample_format must be CSV"
        )


    columns = record.metadata.get(
        "COLUMNS"
    )


    if columns is not None:

        parsed_columns = [
            item.strip()
            for item in columns.split(",")
        ]


        if parsed_columns != EXPECTED_COLUMNS:

            result.fail(
                "Sample column schema mismatch"
            )


    # ========================================================
    # MALFORMED LINES
    # ========================================================

    if record.malformed_lines:

        for line in record.malformed_lines:

            result.fail(
                "Malformed non-comment line: "
                f"{line}"
            )


    # ========================================================
    # SAMPLE INDEX
    # ========================================================

    samples = record.samples


    if samples:

        if samples[0].sample_index != 0:

            result.fail(
                "First sample_index is not 0"
            )


        previous_index = (
            samples[0].sample_index
        )


        for sample in samples[1:]:

            expected_index = (
                previous_index + 1
            )


            if (
                sample.sample_index
                != expected_index
            ):

                result.fail(
                    "sample_index discontinuity: "
                    f"expected {expected_index}, "
                    f"got {sample.sample_index}"
                )

                break


            previous_index = (
                sample.sample_index
            )


    # ========================================================
    # TIMESTAMP ORDER
    # ========================================================

    if len(samples) >= 2:

        previous_timestamp = (
            samples[0].timestamp_us
        )


        for sample in samples[1:]:

            if (
                sample.timestamp_us
                <= previous_timestamp
            ):

                result.fail(
                    "timestamp_us is not "
                    "strictly increasing"
                )

                break


            previous_timestamp = (
                sample.timestamp_us
            )


    # ========================================================
    # FOOTER REQUIRED FIELDS
    # ========================================================

    required_footer = [
        "stop_time_us",
        "expected_samples",
        "received_samples",
        "consumed_samples",
        "event_queue_overflow",
        "sequence_errors",
        "timestamp_order_errors",
        "buffer_overflows",
        "fifo_overruns",
        "dropped_samples",
        "total_errors",
        "stop_reason",
        "qc_status",
    ]


    for key in required_footer:

        if key not in record.footer:

            result.fail(
                f"Missing footer field: {key}"
            )


    # ========================================================
    # SAMPLE COUNT
    # ========================================================

    received_samples = parse_int(
        record.footer,
        "received_samples",
    )


    consumed_samples = parse_int(
        record.footer,
        "consumed_samples",
    )


    expected_samples = parse_int(
        record.footer,
        "expected_samples",
    )


    if (
        received_samples is not None
        and consumed_samples is not None
        and received_samples
        != consumed_samples
    ):

        result.fail(
            "received_samples != "
            "consumed_samples"
        )


    # --------------------------------------------------------
    # If samples are actually streamed,
    # parsed rows must equal received_samples.
    #
    # STREAM0 intentionally has zero CSV rows.
    # --------------------------------------------------------

    if (
        samples
        and received_samples is not None
    ):

        if (
            len(samples)
            != received_samples
        ):

            result.fail(
                "CSV sample row count "
                "does not match received_samples: "
                f"{len(samples)} != "
                f"{received_samples}"
            )


    if not samples:

        result.warn(
            "No sample CSV rows found. "
            "Assuming STREAM0/footer-only log."
        )


    # ========================================================
    # FIRMWARE REPORTED SEQUENCE ERRORS
    # ========================================================

    sequence_errors = parse_int(
        record.footer,
        "sequence_errors",
    )


    timestamp_errors = parse_int(
        record.footer,
        "timestamp_order_errors",
    )


    if (
        sequence_errors is not None
        and sequence_errors != 0
    ):

        result.fail(
            "Firmware reported "
            f"sequence_errors={sequence_errors}"
        )


    if (
        timestamp_errors is not None
        and timestamp_errors != 0
    ):

        result.fail(
            "Firmware reported "
            "timestamp_order_errors="
            f"{timestamp_errors}"
        )


    # ========================================================
    # FIFO CONSISTENCY
    # ========================================================

    acquisition_mode = (
        record.metadata.get(
            "acquisition_mode"
        )
    )


    if acquisition_mode == "FIFO":

        fifo_samples_read = parse_int(
            record.footer,
            "fifo_samples_read",
        )


        if (
            fifo_samples_read is not None
            and received_samples is not None
            and fifo_samples_read
            != received_samples
        ):

            result.fail(
                "FIFO accounting mismatch: "
                "fifo_samples_read="
                f"{fifo_samples_read}, "
                "received_samples="
                f"{received_samples}"
            )


    # ========================================================
    # QC EXPECTATION
    # ========================================================

    qc_status = record.footer.get(
        "qc_status"
    )


    if qc_status not in {
        "PASS",
        "FAIL",
    }:

        result.fail(
            f"Invalid qc_status: {qc_status}"
        )


    if expected_qc is not None:

        if qc_status != expected_qc:

            result.fail(
                "Unexpected QC status: "
                f"expected {expected_qc}, "
                f"got {qc_status}"
            )


    # ========================================================
    # SUMMARY INFO
    # ========================================================

    result.add_info(
        f"record={record_number}"
    )

    result.add_info(
        "source="
        f"{data_source}"
    )

    result.add_info(
        "mode="
        f"{acquisition_mode}"
    )

    result.add_info(
        "expected_samples="
        f"{expected_samples}"
    )

    result.add_info(
        "received_samples="
        f"{received_samples}"
    )

    result.add_info(
        "csv_rows="
        f"{len(samples)}"
    )

    result.add_info(
        "qc="
        f"{qc_status}"
    )


    return result


# ============================================================
# OUTPUT
# ============================================================

def print_result(
    result: ValidationResult,
    record_number: int,
) -> None:

    print()
    print(
        f"===== RECORD {record_number} ====="
    )


    for item in result.info:

        print(
            f"INFO: {item}"
        )


    for warning in result.warnings:

        print(
            f"WARN: {warning}"
        )


    for error in result.errors:

        print(
            f"ERROR: {error}"
        )


    if result.passed:

        print(
            "VALIDATION_STATUS=PASS"
        )

    else:

        print(
            "VALIDATION_STATUS=FAIL"
        )


# ============================================================
# MAIN
# ============================================================

def main() -> int:

    parser = argparse.ArgumentParser(
        description=(
            "Validate EE2_IT2_DC_v1.0 "
            "recording logs"
        )
    )


    parser.add_argument(
        "logfile",
        type=Path,
        help="Serial log file to validate",
    )


    parser.add_argument(
        "--expect-qc",
        choices=[
            "PASS",
            "FAIL",
        ],
        default=None,
        help=(
            "Require firmware QC status "
            "to match PASS or FAIL"
        ),
    )


    args = parser.parse_args()


    if not args.logfile.exists():

        print(
            "ERROR: file does not exist: "
            f"{args.logfile}"
        )

        return 2


    records = parse_log(
        args.logfile
    )


    if not records:

        print(
            "ERROR: no #BEGIN_RECORD "
            "block found"
        )

        return 2


    all_passed = True


    for index, record in enumerate(
        records,
        start=1,
    ):

        result = validate_record(
            record=record,
            record_number=index,
            expected_qc=args.expect_qc,
        )


        print_result(
            result,
            index,
        )


        if not result.passed:

            all_passed = False


    print()
    print(
        "===== FILE SUMMARY ====="
    )

    print(
        f"records={len(records)}"
    )


    if all_passed:

        print(
            "FILE_VALIDATION=PASS"
        )

        return 0


    print(
        "FILE_VALIDATION=FAIL"
    )

    return 1


if __name__ == "__main__":
    sys.exit(
        main()
    )