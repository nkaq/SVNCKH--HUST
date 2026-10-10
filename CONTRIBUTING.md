# Contributing to SVNCKH--HUST

## Core workflow

```text
Read weekly task in `docs/plans/weekXX/README.md` on `dev`
→ optional GitHub Issue for tracking
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

## Official task source

ME1 assigns tasks in `dev/docs/plans/weekXX/README.md` with task ID, owner, deliverables, acceptance criteria, deadline and evidence. **GitHub Issues are optional** tracking records, not mandatory for a weekly assignment. When there is no Issue, the PR must state the weekly README path and task ID; do not fabricate an Issue number.

## Before starting

```bash
git switch dev
git pull --ff-only origin dev
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

Use `.github/PULL_REQUEST_TEMPLATE.md` completely. Enter the task source, task ID, evidence, and `Issue: None` when no Issue was created. Stage only exact task files, review `git diff --cached`, and never indiscriminately stage an entire subsystem.

Request an **independent human reviewer**, especially when the PR author is the sole CODEOWNER of files they changed. Reviewers cannot approve their own PRs. Required code-owner approval needs an eligible code owner other than the author; merely inviting an unrelated reviewer does not fulfill that rule.

Wait for Codex review to finish and investigate P1/P2 before merge, or document why Codex was unavailable and get human review. Codex is not a required GitHub status check unless specifically configured.

## Codex

Codex follows `AGENTS.md`.

Treat generated findings as evidence to investigate, not infallible truth. Human gate is mandatory for safety, Ground Truth, dataset decisions, architecture and scientific claims.

## Stable release

Only after integration:

```text
dev → main
```
