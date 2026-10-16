#!/usr/bin/env python3

from __future__ import annotations

import argparse
import sys

from pathlib import Path


BEGIN_MARKER = "#BEGIN_RECORD"
END_MARKER = "#END_RECORD"


def parse_records(path: Path) -> list[dict[str, str]]:
    records: list[dict[str, str]] = []

    current: dict[str, str] | None = None

    with path.open(
        "r",
        encoding="utf-8",
        errors="replace",
    ) as handle:

        for raw_line in handle:
            line = raw_line.strip()

            if line == BEGIN_MARKER:
                current = {}
                records.append(current)
                continue

            if line == END_MARKER:
                current = None
                continue

            if current is None:
                continue

            if not line.startswith("#"):
                continue

            content = line[1:]

            if "=" not in content:
                continue

            key, value = content.split(
                "=",
                1,
            )

            current[key] = value

    return records


def validate_record(
    metadata: dict[str, str],
    record_number: int,
    strict_identity: bool,
) -> bool:

    passed = True

    print()
    print(
        f"===== METADATA RECORD {record_number} ====="
    )

    required = [
        "data_contract_version",
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
    ]

    for key in required:
        if key not in metadata:
            print(
                f"ERROR: missing {key}"
            )
            passed = False

    source = metadata.get(
        "data_source",
        "UNKNOWN",
    )

    record_id = metadata.get(
        "record_id",
        "UNSET",
    )

    condition_id = metadata.get(
        "condition_id",
        "UNSET",
    )

    calibration = metadata.get(
        "calibration_version",
        "UNSET",
    )

    sensor_health = metadata.get(
        "sensor_health_version",
        "UNSET",
    )

    print(
        f"source={source}"
    )

    print(
        f"record_id={record_id}"
    )

    print(
        f"condition_id={condition_id}"
    )

    print(
        f"calibration_version={calibration}"
    )

    print(
        f"sensor_health_version={sensor_health}"
    )

    # -----------------------------------------------------
    # DATA SOURCE
    # -----------------------------------------------------

    if source not in {
        "MOCK",
        "HARDWARE",
    }:
        print(
            "ERROR: invalid data_source"
        )
        passed = False

    # -----------------------------------------------------
    # IDENTITY
    # -----------------------------------------------------

    if record_id == "UNSET":
        if strict_identity:
            print(
                "ERROR: record_id is UNSET"
            )
            passed = False
        else:
            print(
                "WARN: record_id is UNSET"
            )

    if condition_id == "UNSET":
        if strict_identity:
            print(
                "ERROR: condition_id is UNSET"
            )
            passed = False
        else:
            print(
                "WARN: condition_id is UNSET"
            )

    # -----------------------------------------------------
    # MS2 DEPENDENCY
    # -----------------------------------------------------

    if calibration == "UNSET":
        print(
            "WARN: calibration_version is UNSET"
        )

    if sensor_health == "UNSET":
        print(
            "WARN: sensor_health_version is UNSET"
        )

    # -----------------------------------------------------
    # RESULT
    # -----------------------------------------------------

    if passed:
        print(
            "METADATA_STATUS=PASS"
        )
    else:
        print(
            "METADATA_STATUS=FAIL"
        )

    return passed


def main() -> int:

    parser = argparse.ArgumentParser(
        description=(
            "EE2 recording metadata guard"
        )
    )

    parser.add_argument(
        "logfile",
        type=Path,
    )

    parser.add_argument(
        "--strict-identity",
        action="store_true",
        help=(
            "Fail when record_id or "
            "condition_id is UNSET"
        ),
    )

    args = parser.parse_args()

    if not args.logfile.exists():
        print(
            f"ERROR: file not found: {args.logfile}"
        )
        return 2

    records = parse_records(
        args.logfile
    )

    if not records:
        print(
            "ERROR: no record found"
        )
        return 2

    all_passed = True

    for index, metadata in enumerate(
        records,
        start=1,
    ):
        passed = validate_record(
            metadata=metadata,
            record_number=index,
            strict_identity=args.strict_identity,
        )

        if not passed:
            all_passed = False

    print()
    print(
        "===== METADATA SUMMARY ====="
    )

    print(
        f"records={len(records)}"
    )

    if all_passed:
        print(
            "METADATA_GUARD=PASS"
        )
        return 0

    print(
        "METADATA_GUARD=FAIL"
    )
    return 1


if __name__ == "__main__":
    sys.exit(
        main()
    )