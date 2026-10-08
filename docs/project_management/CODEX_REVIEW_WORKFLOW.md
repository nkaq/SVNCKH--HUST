# Codex Review Workflow

Codex is the **first-pass technical reviewer**, not the final research/safety authority.

## Standard flow

```text
GitHub Issue
→ feature branch
→ member implementation
→ local test
→ push
→ Pull Request to dev
→ Codex review
→ member fixes findings
→ Codex re-review if needed
→ human domain gate
→ merge to dev
→ integration test
→ dev → main
```

## What Codex should review

### EE2 firmware
Ask Codex to focus on:
- acquisition timing;
- FIFO/buffer overflow;
- timestamp correctness;
- I2C/SPI recovery;
- ISR safety;
- sample loss;
- RAM/stack pressure.

### IT2 ML/Data
Ask Codex to focus on:
- recording-level leakage;
- train/test contamination;
- windowing;
- metadata consistency;
- metric correctness;
- reproducibility;
- resource benchmark.

### MS2 sensor-health code/results
Ask Codex to focus on:
- calculation consistency;
- unit handling;
- calibration logic;
- statistical summaries;
- clear distinction between noise, bias, sensitivity and drift.

### ET1 electronics documentation
Ask Codex to focus on:
- schematic/pinout consistency;
- documented voltage/interface assumptions;
- missing protection/conditioning notes.
Human bench verification remains mandatory.

### ME1 mechanical/docs
Ask Codex to focus on:
- version consistency;
- requirements traceability;
- BOM/drawing references;
- experiment-interface consistency.
Human CAD/safety review remains mandatory.

## Human final gate is mandatory for

- mechanical safety;
- electrical power/current compatibility;
- Ground Truth;
- experiment protocol changes;
- dataset inclusion/exclusion;
- architecture changes;
- scientific claims;
- release to main.

## Suggested Codex review prompt

```text
Review this pull request against the repository AGENTS.md instructions.
Prioritize correctness and project-specific risks over style.

Report:
1. Blockers
2. Major findings
3. Minor findings
4. Questions
5. Tests/evidence checked
6. Human review required
7. Merge recommendation

Do not invent experiment results or assume physical safety from documentation alone.
```

## Official Codex context

The repository contains `AGENTS.md` files because Codex can use them as persistent project instructions.
For current setup/use of Codex Code Review, follow OpenAI's official Code Review documentation rather than relying on repository text for UI-specific steps.
