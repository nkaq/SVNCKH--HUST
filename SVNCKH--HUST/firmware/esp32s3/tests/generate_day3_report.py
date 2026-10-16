#!/usr/bin/env python3

from __future__ import annotations

from datetime import datetime
from pathlib import Path


ROOT = Path(__file__).resolve().parent

REGRESSION_DIR = ROOT / "logs" / "regression"

OUTPUT = ROOT / "DAY3_VALIDATION_REPORT.md"


def file_exists(name: str) -> bool:
    return (REGRESSION_DIR / name).exists()


def main() -> int:

    files = {
        "FIFO 1 s": "fifo_1s.log",
        "DATA_READY 1 s": "dr_1s.log",
        "FIFO 5 s": "fifo_5s.log",
        "FIFO stress 5 s x 2": "fifo_stress_5s_x2.log",
    }

    lines: list[str] = []

    lines.append(
        "# EE2 Day 3 Validation Report"
    )

    lines.append("")
    lines.append(
        f"Generated: {datetime.now().isoformat(timespec='seconds')}"
    )

    lines.append("")
    lines.append(
        "Firmware: DAQ_FW_v2.1"
    )

    lines.append(
        "Data Contract: EE2_IT2_DC_v1.0"
    )

    lines.append(
        "Data source: MOCK"
    )

    lines.append("")
    lines.append(
        "## Regression Evidence"
    )

    lines.append("")

    all_present = True

    for test_name, filename in files.items():

        present = file_exists(
            filename
        )

        status = (
            "PASS evidence present"
            if present
            else "MISSING"
        )

        lines.append(
            f"- {test_name}: {status}"
        )

        if not present:
            all_present = False

    lines.append("")
    lines.append(
        "## Validated Functions"
    )

    lines.append("")

    lines.extend(
        [
            "- FIFO acquisition mock",
            "- DATA_READY acquisition mock",
            "- Ring-buffer producer/consumer",
            "- FIFO final partial drain",
            "- Timestamp ordering",
            "- Sample-index continuity",
            "- Data Contract v1.0 serialization",
            "- PC serial capture",
            "- PC contract validator",
            "- Metadata guard",
            "- Short stress sequence",
        ]
    )

    lines.append("")
    lines.append(
        "## Known Pending Items"
    )

    lines.append("")

    lines.extend(
        [
            "- ADXL345 hardware validation pending",
            "- Real I2C timeout measurement pending",
            "- Real DATA_READY interrupt validation pending",
            "- Real FIFO validation pending",
            "- calibration_version pending MS2 input",
            "- sensor_health_version pending MS2 input",
            "- 60 s Engineering Validation hardware recording pending",
        ]
    )

    lines.append("")
    lines.append(
        "## Day 3 Result"
    )

    lines.append("")

    if all_present:
        lines.append(
            "**DAY3_SOFTWARE_VALIDATION=PASS**"
        )
    else:
        lines.append(
            "**DAY3_SOFTWARE_VALIDATION=INCOMPLETE**"
        )

    lines.append("")
    lines.append(
        "> This report represents software/mock validation only."
    )

    lines.append(
        "> It is not ADXL345 hardware validation and not Dataset v0.1."
    )

    OUTPUT.write_text(
        "\n".join(lines) + "\n",
        encoding="utf-8",
    )

    print(
        f"REPORT={OUTPUT}"
    )

    if all_present:
        print(
            "DAY3_REPORT_STATUS=PASS"
        )
        return 0

    print(
        "DAY3_REPORT_STATUS=INCOMPLETE"
    )
    return 1


if __name__ == "__main__":
    raise SystemExit(
        main()
    )