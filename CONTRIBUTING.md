# Contributing to SVNCKH--HUST

## Core workflow

```text
Issue
→ feature branch
→ work/test
→ commit
→ push
→ Pull Request to dev
→ Codex review
→ human gate
→ merge dev
→ integration test
→ dev → main
```

## Before starting

```bash
git switch dev
git pull origin dev
git switch -c feature/<member>-<task>
```

Examples:

```text
feature/me1-fixture-v01
feature/ms2-sensor-health
feature/ee2-adxl345-acquisition
feature/et1-interface-board
feature/it2-edge-dsp
```

## Before pushing

- run relevant tests;
- collect evidence;
- confirm files are in the correct subsystem;
- update docs if interface/schema changed;
- do not commit secrets or raw dataset;
- do not invent experiment results.

## Commit format

```text
type(scope): message
```

## Pull Request

Target `dev` by default.

Use `.github/PULL_REQUEST_TEMPLATE.md` completely.

## Codex

Codex follows `AGENTS.md`.

Treat generated findings as evidence to investigate, not infallible truth. Human gate is mandatory for safety, Ground Truth, dataset decisions, architecture and scientific claims.

## Stable release

Only after integration:

```text
dev → main
```
