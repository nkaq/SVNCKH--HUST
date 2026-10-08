# Codex Review Prompt Library

## Firmware — EE2

```text
Review this PR as an embedded acquisition reviewer.
Focus on timestamp correctness, FIFO/ring-buffer bounds, dropped samples,
ISR/shared-state safety, I2C/SPI error recovery, blocking calls, memory usage,
and whether OLED/logging can disturb sampling.
Classify findings as BLOCKER/MAJOR/MINOR/QUESTION.
```

## ML/Data — IT2

```text
Review this PR for data leakage and scientific reproducibility.
Check recording-aware split, window overlap, preprocessing fit scope,
metadata/label provenance, metric correctness, random seeds/configuration,
and resource-cost claims for ESP32-S3.
```

## Sensor reliability — MS2

```text
Review this PR for calibration/sensor-health correctness.
Check units, baseline definitions, bias/noise/sensitivity terminology,
temperature association, statistics, and whether drift claims exceed evidence.
```

## Electronics — ET1

```text
Review this PR for interface-document consistency.
Check voltage levels, power/ground assumptions, pull-ups, ADC range,
connector pinout, MAX31865/PT100, ACS724 and US5881 documentation.
Do not declare hardware safe; list items requiring bench verification.
```

## Mechanical / Integration — ME1

```text
Review this PR for project traceability and integration.
Check fixture versioning, BOM/drawing consistency, sensor mounting references,
alignment/fault-creation requirements, interface dependencies and acceptance criteria.
Flag all mechanical-safety items for human review.
```
