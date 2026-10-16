#!/usr/bin/env python3

"""
EE2 Automated Mock Regression

Tests:
1. FIFO 1 s
2. DATA_READY 1 s
3. FIFO 5 s
4. FIFO STRESS 5 s x 2 runs

Requirements:
- capture_record.py
- validate_dc_v1.py
- pyserial

Firmware must currently be in MOCK mode.
"""

from __future__ import annotations

import argparse
import subprocess
import sys
import time

from pathlib import Path

import serial


# ============================================================
# PATHS
# ============================================================

TEST_DIR = Path(__file__).resolve().parent

CAPTURE_SCRIPT = (
    TEST_DIR
    / "capture_record.py"
)

VALIDATOR_SCRIPT = (
    TEST_DIR
    / "validate_dc_v1.py"
)


# ============================================================
# RESULT TYPE
# ============================================================

class RegressionResult:

    def __init__(
        self,
        name: str,
        passed: bool,
        detail: str = "",
    ):
        self.name = name
        self.passed = passed
        self.detail = detail


# ============================================================
# SERIAL HELPERS
# ============================================================

def send_command(
    ser: serial.Serial,
    command: str,
) -> None:

    payload = (
        command.strip()
        + "\n"
    ).encode(
        "utf-8"
    )

    ser.write(
        payload
    )

    ser.flush()

    print(
        f">> {command}"
    )


def read_available(
    ser: serial.Serial,
    duration_s: float,
) -> list[str]:

    lines: list[str] = []

    deadline = (
        time.monotonic()
        + duration_s
    )

    while (
        time.monotonic()
        < deadline
    ):

        if ser.in_waiting:

            raw = ser.readline()

            line = raw.decode(
                "utf-8",
                errors="replace",
            ).rstrip(
                "\r\n"
            )

            if line:

                print(
                    line
                )

                lines.append(
                    line
                )

        else:

            time.sleep(
                0.01
            )

    return lines


# ============================================================
# NORMAL CAPTURE TEST
# ============================================================

def run_capture_case(
    port: str,
    baud: int,
    mode: str,
    seconds: int,
    name: str,
    output_path: Path,
) -> RegressionResult:

    print()
    print(
        "========================================"
    )
    print(
        f"TEST: {name}"
    )
    print(
        "========================================"
    )


    command = [
        sys.executable,

        str(
            CAPTURE_SCRIPT
        ),

        "--port",
        port,

        "--baud",
        str(
            baud
        ),

        "--mode",
        mode,

        "--seconds",
        str(
            seconds
        ),

        "--stream",
        "1",

        "--record-id",
        name,

        "--condition",
        "MOCK",

        "--output",
        str(
            output_path
        ),

        "--expect-qc",
        "PASS",
    ]


    completed = subprocess.run(
        command,
        check=False,
    )


    if completed.returncode == 0:

        return RegressionResult(
            name=name,
            passed=True,
            detail="capture+validator PASS",
        )


    return RegressionResult(
        name=name,
        passed=False,
        detail=(
            "capture_record.py "
            f"exit={completed.returncode}"
        ),
    )


# ============================================================
# STRESS TEST
# ============================================================

def run_stress_case(
    port: str,
    baud: int,
    output_path: Path,
) -> RegressionResult:

    name = "FIFO_STRESS_5S_X2"

    print()
    print(
        "========================================"
    )
    print(
        f"TEST: {name}"
    )
    print(
        "========================================"
    )


    try:

        ser = serial.Serial(
            port=port,
            baudrate=baud,
            timeout=0.10,
            write_timeout=1.0,
        )

    except serial.SerialException as exc:

        return RegressionResult(
            name=name,
            passed=False,
            detail=(
                "cannot open serial: "
                f"{exc}"
            ),
        )


    captured_lines: list[str] = []

    stress_pass = False

    stress_fail = False

    record_count = 0


    try:

        # ----------------------------------------------------
        # Opening serial may reset ESP32.
        # ----------------------------------------------------

        time.sleep(
            2.0
        )


        captured_lines.extend(
            read_available(
                ser,
                1.0,
            )
        )


        # ----------------------------------------------------
        # CONFIG
        # ----------------------------------------------------

        commands = [
            "FIFO",
            "STREAM0",
            "SET_RECORD_ID DAY3_STRESS",
            "SET_CONDITION MOCK",
        ]


        for command in commands:

            send_command(
                ser,
                command
            )

            captured_lines.extend(
                read_available(
                    ser,
                    0.35,
                )
            )


        # ----------------------------------------------------
        # START STRESS
        # ----------------------------------------------------

        send_command(
            ser,
            "STRESS5X2",
        )


        timeout_s = 25.0

        deadline = (
            time.monotonic()
            + timeout_s
        )


        while (
            time.monotonic()
            < deadline
        ):

            if not ser.in_waiting:

                time.sleep(
                    0.005
                )

                continue


            raw = ser.readline()

            line = raw.decode(
                "utf-8",
                errors="replace",
            ).rstrip(
                "\r\n"
            )


            if not line:
                continue


            print(
                line
            )


            captured_lines.append(
                line
            )


            if line == "#BEGIN_RECORD":

                record_count += 1


            if (
                line
                == "STRESS_STATUS=PASS"
            ):

                stress_pass = True

                break


            if (
                line
                == "STRESS_STATUS=FAIL"
            ):

                stress_fail = True

                break


    finally:

        ser.close()


    # ========================================================
    # SAVE COMPLETE STRESS LOG
    # ========================================================

    output_path.parent.mkdir(
        parents=True,
        exist_ok=True,
    )


    output_path.write_text(
        "\n".join(
            captured_lines
        )
        + "\n",
        encoding="utf-8",
    )


    print()
    print(
        f"stress_log={output_path}"
    )


    # ========================================================
    # BASIC STRESS RESULT
    # ========================================================

    if stress_fail:

        return RegressionResult(
            name=name,
            passed=False,
            detail="firmware STRESS FAIL",
        )


    if not stress_pass:

        return RegressionResult(
            name=name,
            passed=False,
            detail=(
                "STRESS_STATUS=PASS "
                "not received"
            ),
        )


    if record_count != 2:

        return RegressionResult(
            name=name,
            passed=False,
            detail=(
                "expected 2 records, "
                f"got {record_count}"
            ),
        )


    # ========================================================
    # VALIDATE BOTH RECORDS
    # ========================================================

    validator_command = [
        sys.executable,

        str(
            VALIDATOR_SCRIPT
        ),

        str(
            output_path
        ),

        "--expect-qc",
        "PASS",
    ]


    completed = subprocess.run(
        validator_command,
        check=False,
    )


    if completed.returncode != 0:

        return RegressionResult(
            name=name,
            passed=False,
            detail=(
                "stress records "
                "failed validator"
            ),
        )


    return RegressionResult(
        name=name,
        passed=True,
        detail=(
            "2/2 runs + "
            "validator PASS"
        ),
    )


# ============================================================
# PRINT SUMMARY
# ============================================================

def print_summary(
    results: list[RegressionResult],
) -> bool:

    print()
    print()
    print(
        "========================================"
    )
    print(
        "EE2 MOCK REGRESSION SUMMARY"
    )
    print(
        "========================================"
    )


    pass_count = 0


    for result in results:

        status = (
            "PASS"
            if result.passed
            else "FAIL"
        )


        print(
            f"{result.name}: "
            f"{status}"
        )


        print(
            f"  {result.detail}"
        )


        if result.passed:

            pass_count += 1


    print()
    print(
        f"tests_total={len(results)}"
    )

    print(
        f"tests_pass={pass_count}"
    )

    print(
        "tests_fail="
        f"{len(results) - pass_count}"
    )


    all_passed = (
        pass_count
        == len(results)
    )


    if all_passed:

        print(
            "MOCK_REGRESSION=PASS"
        )

    else:

        print(
            "MOCK_REGRESSION=FAIL"
        )


    print(
        "========================================"
    )


    return all_passed


# ============================================================
# MAIN
# ============================================================

def main() -> int:

    parser = argparse.ArgumentParser(
        description=(
            "Run automated EE2 "
            "mock firmware regression"
        )
    )


    parser.add_argument(
        "--port",
        required=True,
        help="ESP32 serial port, e.g. COM4",
    )


    parser.add_argument(
        "--baud",
        type=int,
        default=115200,
    )


    parser.add_argument(
        "--output-dir",
        type=Path,
        default=(
            TEST_DIR
            / "logs"
            / "regression"
        ),
    )


    args = parser.parse_args()


    # ========================================================
    # CHECK DEPENDENCIES
    # ========================================================

    if not CAPTURE_SCRIPT.exists():

        print(
            "ERROR: missing capture_record.py"
        )

        return 2


    if not VALIDATOR_SCRIPT.exists():

        print(
            "ERROR: missing validate_dc_v1.py"
        )

        return 2


    args.output_dir.mkdir(
        parents=True,
        exist_ok=True,
    )


    # ========================================================
    # RUN TEST MATRIX
    # ========================================================

    results: list[RegressionResult] = []


    # --------------------------------------------------------
    # FIFO 1 SECOND
    # --------------------------------------------------------

    results.append(
        run_capture_case(
            port=args.port,
            baud=args.baud,
            mode="FIFO",
            seconds=1,
            name="FIFO_1S",
            output_path=(
                args.output_dir
                / "fifo_1s.log"
            ),
        )
    )


    # --------------------------------------------------------
    # DATA_READY 1 SECOND
    # --------------------------------------------------------

    results.append(
        run_capture_case(
            port=args.port,
            baud=args.baud,
            mode="DR",
            seconds=1,
            name="DR_1S",
            output_path=(
                args.output_dir
                / "dr_1s.log"
            ),
        )
    )


    # --------------------------------------------------------
    # FIFO 5 SECOND
    # --------------------------------------------------------

    results.append(
        run_capture_case(
            port=args.port,
            baud=args.baud,
            mode="FIFO",
            seconds=5,
            name="FIFO_5S",
            output_path=(
                args.output_dir
                / "fifo_5s.log"
            ),
        )
    )


    # --------------------------------------------------------
    # STRESS 5 SECOND x 2
    # --------------------------------------------------------

    results.append(
        run_stress_case(
            port=args.port,
            baud=args.baud,
            output_path=(
                args.output_dir
                / "fifo_stress_5s_x2.log"
            ),
        )
    )


    # ========================================================
    # SUMMARY
    # ========================================================

    all_passed = print_summary(
        results
    )


    if all_passed:

        return 0


    return 1


if __name__ == "__main__":

    sys.exit(
        main()
    )