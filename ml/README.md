# Data / DSP / ML / TinyML

Pipeline:

```text
Raw
→ QC
→ Alignment
→ Windowing
→ Features / Representation
→ Leakage-safe Split
→ Baseline
→ TinyML
→ Drift / Reliability
→ Adaptive Fusion
```

Không random-window split các window từ cùng một recording sang cả train và test.
