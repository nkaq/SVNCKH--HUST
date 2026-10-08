# Firmware review instructions

Applies to `firmware/`.

Prioritize correctness of acquisition and timing over stylistic refactoring.

Required review focus:
- non-blocking acquisition;
- ISR/shared-state safety;
- timestamp/sample-index monotonicity;
- buffer/FIFO bounds;
- error recovery;
- dropped-sample accounting;
- memory usage;
- sensor interface assumptions;
- OLED/logging must not disturb sampling timing.

Any change to sampling rate, sensor range, ODR, pin mapping, packet schema, or timestamp semantics must be documented in the PR.
