# Week 3 — ACTIVE (ME1 Onshape amendment pending official effective date)

Prototype Integration & Mechanical Testbench Build.

## ME1 — fixture / DFM / architecture v0.3

**ME1 tool decision (approved for drafting migration):** use **Onshape**, not SolidWorks, for **new** native mechanical CAD work. This section records the ME1-approved **proposed change of deliverable format**; it is **NOT YET AN ACTIVE INSTRUCTION** until PR #20 is reviewed/merged into `dev` and the ME1 Project Update states its effective date/condition and communication to the affected team. The original work objectives **fixture + DFM + Architecture v0.3** remain; no physical fixture or CAD release is approved by this text.

**Before effective date:** any Week 3 ME1 task requiring a new native CAD deliverable is `BLOCKED — ME1 CAD SOURCE/DELIVERABLE TRANSITION` pending a documented ME1 decision/Project Update. Do not silently submit SolidWorks files against the new proposal, or treat a proposed Onshape export as accepted work. Other members' Week 3 tasks are unchanged.

**After effective date, planned ME1 handoff for new CAD work (not a completed task):**
- Native source: **Onshape ME1_MAIN_CAD**; separately managed **CAD TEST** for explorations; use **immutable MAIN Document ID + Version ID + Version URL + Element ID** for candidate release, not a mutable Workspace URL.
- GitHub deliverables when actually produced: `hardware/mechanical/onshape/cad_registry.md`, genuine STEP/Drawing PDF/DXF exports as applicable, `hardware/bom/` mechanical BOM, `hardware/mechanical/reports/` validation report and `hardware/mechanical/fabrication/<RELEASE_ID>/manifest.md` only when release conditions are met.
- Record part/engineering revision, fixture/setup version, units/coordinates, ADXL345 mounting, DFM/assembly/clearance, applicable verification and real evidence. CAD/FEA/safety/fixture acceptance remain `NOT_RUN/PENDING` without measurement and qualified human review.
- **Task ID, measurable acceptance criteria, delivery deadline and Onshape actual IDs:** PENDING — ME1 must confirm from the authoritative weekly task record/Project Update; do not invent them. No change to GT/dataset or MS2/EE2/ET1/IT2 assignments.
- Traceability: [ME1 Onshape Requirements V2](../../project_management/NCKH_ME1_ONSHAPE_CAD_GITHUB_WORKFLOW_REQUIREMENTS_V2.md), [ME1 Project Update proposal](../../project_management/NCKH_ME1_ONSHAPE_PROJECT_UPDATE_PROPOSAL_V1.md), [Team Submission Workflow V3](../../project_management/NCKH_TEAM_SUBMISSION_WORKFLOW_V3.md).

**Historical Week 3 task wording (unchanged, preserved for audit; not the new CAD format once migration is ACTIVE):**

> Fixture + SolidWorks + DFM + Architecture v0.3.

## MS2
ADXL345 Calibration & Sensor Health.

## EE2
Acquisition Firmware v2.

## ET1
Sensor Interface Board & Robust Hardware Integration.

## IT2
On-device DSP & Live Engineering Monitor.
