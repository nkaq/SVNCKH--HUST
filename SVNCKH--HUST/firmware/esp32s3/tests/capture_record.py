#!/usr/bin/env python3

"""
EE2 Serial Capture Logger

Purpose:
- Open ESP32-S3 serial port.
- Configure acquisition mode.
- Start one recording.
- Capture exactly:
      #BEGIN_RECORD
          ...
      #END_RECORD
- Save to .log file.
- Run EE2 Data Contract validator automatically.

Requires:
    pip install pyserial
"""

from __future__ import annotations

import argparse
import subprocess
import sys
import time

from datetime import datetime
from pathlib import Path

import serial


# ============================================================
# CONSTANTS
# ============================================================

DEFAULT_BAUD = 115200

BEGIN_MARKER = "#BEGIN_RECORD"
END_MARKER = "#END_RECORD"


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


def drain_serial(
    ser: serial.Serial,
    duration_s: float = 0.5,
) -> None:

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

        else:
            time.sleep(
                0.01
            )


# ============================================================
# CAPTURE
# ============================================================

def capture_record(
    ser: serial.Serial,
    output_path: Path,
    timeout_s: float,
) -> bool:

    started = False

    captured_lines: list[str] = []

    deadline = (
        time.monotonic()
        + timeout_s
    )


    print()
    print(
        "===== RECORD CAPTURE ====="
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


        # ----------------------------------------------------
        # Wait for beginning of official record
        # ----------------------------------------------------

        if not started:

            if line == BEGIN_MARKER:

                started = True

                captured_lines.append(
                    line
                )

            continue


        # ----------------------------------------------------
        # Inside record
        # ----------------------------------------------------

        captured_lines.append(
            line
        )


        # ----------------------------------------------------
        # End
        # ----------------------------------------------------

        if line == END_MARKER:

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
                "CAPTURE_STATUS=PASS"
            )

            print(
                f"SAVED={output_path}"
            )

            return True


    print()
    print(
        "CAPTURE_STATUS=FAIL"
    )


    if not started:

        print(
            "ERROR=no_BEGIN_RECORD"
        )

    else:

        print(
            "ERROR=no_END_RECORD"
        )


    return False


# ============================================================
# VALIDATOR
# ============================================================

def run_validator(
    log_path: Path,
    expected_qc: str,
) -> int:

    validator_path = (
        Path(__file__).resolve().parent
        / "validate_dc_v1.py"
    )


    if not validator_path.exists():

        print(
            "ERROR: validator not found:"
        )

        print(
            validator_path
        )

        return 2


    print()
    print(
        "===== RUN VALIDATOR ====="
    )


    command = [
        sys.executable,
        str(
            validator_path
        ),
        str(
            log_path
        ),
        "--expect-qc",
        expected_qc,
    ]


    completed = subprocess.run(
        command,
        check=False,
    )


    return completed.returncode


# ============================================================
# MAIN
# ============================================================

def main() -> int:

    parser = argparse.ArgumentParser(
        description=(
            "Capture one EE2 ESP32-S3 "
            "recording over Serial"
        )
    )


    parser.add_argument(
        "--port",
        required=True,
        help=(
            "Serial port, e.g. COM4"
        ),
    )


    parser.add_argument(
        "--baud",
        type=int,
        default=DEFAULT_BAUD,
    )


    parser.add_argument(
        "--mode",
        choices=[
            "FIFO",
            "DR",
        ],
        default="FIFO",
    )


    parser.add_argument(
        "--seconds",
        type=int,
        default=1,
    )


    parser.add_argument(
        "--stream",
        type=int,
        choices=[
            0,
            1,
        ],
        default=1,
    )


    parser.add_argument(
        "--record-id",
        default="DAY3_CAPTURE",
    )


    parser.add_argument(
        "--condition",
        default="MOCK",
    )


    parser.add_argument(
        "--output",
        type=Path,
        default=None,
    )


    parser.add_argument(
        "--expect-qc",
        choices=[
            "PASS",
            "FAIL",
        ],
        default="PASS",
    )


    parser.add_argument(
        "--no-validate",
        action="store_true",
    )


    args = parser.parse_args()


    # ========================================================
    # ARGUMENT CHECK
    # ========================================================

    if args.seconds <= 0:

        print(
            "ERROR: seconds must be > 0"
        )

        return 2


    # ========================================================
    # OUTPUT PATH
    # ========================================================

    if args.output is None:

        timestamp = datetime.now().strftime(
            "%Y%m%d_%H%M%S"
        )


        filename = (
            f"{args.mode.lower()}_"
            f"{args.seconds}s_"
            f"{timestamp}.log"
        )


        output_path = (
            Path(__file__).resolve().parent
            / "logs"
            / filename
        )

    else:

        output_path = (
            args.output
        )


    # ========================================================
    # OPEN SERIAL
    # ========================================================

    print(
        "===== EE2 SERIAL LOGGER ====="
    )

    print(
        f"port={args.port}"
    )

    print(
        f"baud={args.baud}"
    )

    print(
        f"mode={args.mode}"
    )

    print(
        f"seconds={args.seconds}"
    )

    print(
        f"stream={args.stream}"
    )


    try:

        ser = serial.Serial(
            port=args.port,
            baudrate=args.baud,
            timeout=0.10,
            write_timeout=1.0,
        )

    except serial.SerialException as exc:

        print(
            f"ERROR: cannot open serial port: {exc}"
        )

        return 2


    try:

        # ----------------------------------------------------
        # Some ESP32 USB/UART paths reset when port opens.
        # Give firmware time to boot.
        # ----------------------------------------------------

        time.sleep(
            2.0
        )


        drain_serial(
            ser,
            duration_s=1.0,
        )


        # ----------------------------------------------------
        # CONFIGURE
        # ----------------------------------------------------

        send_command(
            ser,
            args.mode,
        )

        time.sleep(
            0.2
        )

        drain_serial(
            ser,
            duration_s=0.3,
        )


        send_command(
            ser,
            f"STREAM{args.stream}",
        )

        time.sleep(
            0.2
        )

        drain_serial(
            ser,
            duration_s=0.3,
        )


        # ----------------------------------------------------
        # Use standard PC commands here.
        # Serial script has no keyboard-space problem.
        # ----------------------------------------------------

        send_command(
            ser,
            (
                "SET_RECORD_ID "
                + args.record_id
            ),
        )

        time.sleep(
            0.2
        )

        drain_serial(
            ser,
            duration_s=0.3,
        )


        send_command(
            ser,
            (
                "SET_CONDITION "
                + args.condition
            ),
        )

        time.sleep(
            0.2
        )

        drain_serial(
            ser,
            duration_s=0.3,
        )


        # ----------------------------------------------------
        # START RECORDING
        # ----------------------------------------------------

        send_command(
            ser,
            f"START{args.seconds}",
        )


        timeout_s = (
            float(
                args.seconds
            )
            + 15.0
        )


        capture_ok = capture_record(
            ser=ser,
            output_path=output_path,
            timeout_s=timeout_s,
        )


        if not capture_ok:

            return 1


    finally:

        ser.close()


    # ========================================================
    # VALIDATE
    # ========================================================

    if args.no_validate:

        return 0


    validator_result = run_validator(
        log_path=output_path,
        expected_qc=args.expect_qc,
    )


    print()


    if validator_result == 0:

        print(
            "CAPTURE_AND_VALIDATION=PASS"
        )

    else:

        print(
            "CAPTURE_AND_VALIDATION=FAIL"
        )


    return validator_result


if __name__ == "__main__":

    sys.exit(
        main()
    )