# ME1 — Mechanical Verification Test Report (TEMPLATE)

> **CHỈ LÀ BIỂU MẪU.** Không phải kết quả đo, nghiệm thu fixture hoặc safety clearance. Lưu báo cáo thực theo `hardware/mechanical/reports/<COMPONENT_ID>/<RELEASE_ID>/test-report.md`; tạo khi có phép kiểm thật.

## 1. Identity & traceability

- Report ID / date and author: PENDING
- Component/assembly ID, revision, role: PENDING
- Fixture version / setup version: PENDING
- CAD TEST: Document ID, Version ID/URL and test evidence: PENDING / NOT_APPLICABLE
- MAIN CAD: **Document ID + immutable Version ID + Version URL:** PENDING
- Source Python/script commit/parameters ↔ STEP (if generated): NOT_APPLICABLE / PENDING
- Drawing ID/revision/path, STEP/DXF, BOM path: PENDING
- Release ID / release manifest relative path: PENDING
- Units, part coordinate system, axis/datum/alignment, ADXL345 XYZ mount frame: PENDING
- Material/process/physical dimensions, approved tolerance standard: PENDING

## 2. Scope, acceptance and assumptions

- Proposed intended load cases, motor RPM envelope, worst-case operation: TBD — ME1 DECISION
- Reference load/calculation basis, mass/constraints/support/contact, material properties: TBD
- Explicitly defined acceptance criteria, safety margins/allowable stresses/deflections and sources: TBD
- Applicable/NOT APPLICABLE methods with rationale: TBD
- Reviewer permissions and Onshape Simulation capability verified: PENDING

## 3. Test matrix (apply according to component risk)

| Check / question | Applicability and reason | Method / solver / setup | Acceptance criterion reference | Status | Evidence link / measured value | Reviewer |
|---|---|---|---|---|---|---|
| Dimensions, datum, tolerances | TBD | TBD | TBD | NOT_RUN | | |
| Assembly fit / clearance / alignment | TBD | TBD | TBD | NOT_RUN | | |
| Material, DFM and process | TBD | TBD | TBD | NOT_RUN | | |
| Linear Static: stress, displacement, FoS | TBD | TBD | TBD | NOT_RUN | | |
| Stiffness / fastener and support | TBD | TBD | TBD | NOT_RUN | | |
| Modal frequencies, shapes, mode participation | TBD | TBD | TBD | NOT_RUN | | |
| Motor rotating excitation / resonance risk | TBD | TBD | TBD | NOT_RUN | | |
| Shaft/coupling/bearing loads and fatigue | TBD | TBD | TBD | NOT_RUN | | |
| Thermal/cooling/CFD (only when relevant) | TBD | TBD | TBD | NOT_RUN | | |
| ADXL345 orientation / repeatable sensor mounting | TBD | TBD | TBD | NOT_RUN | | |
| Guarding, fault mechanism, mechanical safety | TBD | TBD | TBD | NOT_RUN | | |
| Physical inspection, run-up and fixture acceptance | TBD | TBD | TBD | NOT_RUN | | |

**Trạng thái hợp lệ:** `PASS`, `FAIL`, `NOT_RUN`, `BLOCKED`, `N/A` (kèm lý do). Chỉ `PASS` khi có tiêu chí được duyệt + evidence + human review. `N/A` phải có phân tích applicability, không phải cách lách test.

## 4. Simulation setup and limitations

- Software/solver/version/license actually available: NOT_VERIFIED
- Mesh/convergence/contact/boundary/material/load assumptions: NOT_RUN
- Linear Static: scope, image/result path, units, max stress/displacement, safety margin: NOT_RUN
- Modal: constraints, first relevant modes/shape images and frequency units: NOT_RUN
- Rotating motor excitation/orders/speed sweep; possible resonance interval and uncertainties: NOT_RUN
- Transient/harmonic response, fatigue, CFD: NOT_RUN unless supported by real tool/test; recommend external solver/evidence when necessary
- **Modal eigenfrequency alone ≠ forced vibration response; FEA ≠ physical safety validation.**
- Geometry source, version, mesh convergence and model limitations: PENDING

## 5. Physical inspection and evidence

- Fixture/testbench serial, operator, date, measurement instrument/calibration: PENDING
- RPM/load, safety enclosure, emergency stop and risk controls: PENDING
- Vibration / alignment / repeatability evidence: NOT_RUN
- Ground Truth source: **controlled physical condition only**; no FFT/ML inference as label.
- No Engineering Validation recording may be renamed as official Dataset v0.1 after passing QC or fixture freeze.

## 6. Findings, decisions and handoff

| Finding | Severity | Supporting evidence | Action/owner | Due/status |
|---|---|---|---|---|
| PENDING | NOT_ASSESSED | PENDING | PENDING | PENDING |

- Open risks and mitigation / `BLOCKED` escalation: PENDING
- Test author and qualified checker: PENDING
- Human CAD/fabrication/safety approval reference: PENDING
- Decision: **PENDING — NOT RELEASED**
- Registry backlink: [CAD Registry](../onshape/cad_registry.md) (adjust relative path when report is copied under a version directory)
- Release Manifest backlink: PENDING (file must exist before link is treated as valid)

Cross-reference [ME1 Onshape Requirements V2](../../../docs/project_management/NCKH_ME1_ONSHAPE_CAD_GITHUB_WORKFLOW_REQUIREMENTS_V2.md).
