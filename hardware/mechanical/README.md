# Mechanical Testbench — ME1 Onshape migration proposal (not yet ACTIVE)

**Owner:** ME1 (`@nkaq`). **ME1's chosen target tool for new mechanical CAD:** **Onshape** (Part Studio, Assembly, Drawing, controlled Versions). **This PR proposes the replacement workflow; it is not yet an ACTIVE, team-wide task/deliverable instruction.** Week 3 and the Team Submission Workflow now contain a **proposed Onshape amendment** preserving the original SolidWorks wording for history, but this new handoff is **not ACTIVE** before PR #20 is merged and ME1 has announced an authorized Project Update and effective date/condition. Any affected new native-CAD task remains `BLOCKED — ME1 CAD SOURCE/DELIVERABLE TRANSITION` until then; ME1 does not intend to create new SolidWorks designs. Existing `solidworks/` files/directories remain **LEGACY/READ-ONLY** for historical traceability. This document does **not** certify any actual fixture design or mechanical safety.

## Status and conflict with current weekly assignment

**ME1 đã chọn Onshape cho thiết kế mới** và trong PR #20 đã chấp thuận điều chỉnh [Week 3 task source](../../docs/plans/week03/README.md) cùng [Team Submission Workflow V3](../../docs/project_management/NCKH_TEAM_SUBMISSION_WORKFLOW_V3.md): mục giao việc SolidWorks cũ chỉ còn là trích dẫn lịch sử; định dạng Onshape mới được đề xuất rõ ràng. **PROPOSED — NOT ACTIVE** cho đến khi PR #20 merge hợp lệ, ME1 cấp Update ID, quyết định/đăng thông báo về mốc hiệu lực, hướng dẫn người liên quan và xử lý ACK/compatibility. Trước thời điểm đó, giao việc native CAD liên quan vẫn `BLOCKED — ME1 CAD SOURCE/DELIVERABLE TRANSITION`, không bắt thành viên tự dùng SolidWorks hoặc tự nhận Onshape candidate là bài đã nghiệm thu. Không sửa hồi tố dữ liệu/fixture/GT đã khóa.

## Separate CAD environments

- **CAD TEST:** separate Onshape Document for exploratory parts, assemblies, text-to-cad Python/STEP trials, geometry checks and provisional simulations. State `DRAFT / UNDER_REVIEW / TESTED / REJECTED`, **never RELEASED**. TEST artifacts do not belong in a fabrication release.
- **ME1_MAIN_CAD:** separate authoritative Onshape Document for production-candidate Part Studios, Assembly and Drawings. Promote candidate only after ME1 reviews provenance, dimensions, fit, mounting, safety and dependencies; revalidate mates/drawings/BOM in MAIN.
- A **fixed MAIN Onshape Version ID + Version URL** is mandatory for a release; a mutable Workspace URL is **not** release evidence. Onshape Version ID, engineering Revision, Component ID, Release ID and Git commit are separate identifiers.

## Account / confidentiality gate

Before uploading research CAD: ME1 verifies Onshape subscription, Document privacy, ownership, reviewer permissions and Simulation availability. A plan which exposes restricted design using a public account is **BLOCKED — ACCOUNT/CONFIDENTIALITY**. Do not publish secrets or change permissions to accommodate AI review.

## Artifact locations (reuse existing folders)

| Artifact | Canonical repo path | Status / rule |
|---|---|---|
| Native Onshape source | Cloud MAIN Version + [CAD registry](onshape/cad_registry.md) | Actual URLs/IDs PENDING until verified |
| CAD release mapping | [release manifest template](onshape/release_manifest_template.md) | Actual release: `fabrication/<RELEASE_ID>/manifest.md` **only when real** |
| Python CAD source/parameters | `cad-source/scripts/`, `cad-source/params/` | Create only when actual source exists and reproduces an artifact |
| Neutral 3D for exchange/fabrication | `step/` | STEP exported from fixed MAIN Version; verify units and geometry |
| 2D fabrication exports | `dxf/`, `drawings/` | DXF if required; PDF Drawing with approved dimensions/datum/revision |
| Optional mesh/preview | `stl/`, `previews/` | STL/PNG only when needed; neither is native parametric CAD |
| Verification | `reports/TEST_REPORT_TEMPLATE.md` → `reports/<COMPONENT_ID>/<RELEASE_ID>/test-report.md` | Template exists; real report only after tests |
| BOM | `../bom/MECHANICAL_BOM_TEMPLATE.csv` | Real BOM must reference Part ID, Revision and CAD Version; coordinate with ET1 |
| Legacy | `solidworks/` | **Do not delete/move/rename**; historical artifacts only; new-task migration remains PROPOSED pending authoritative task-source update |

`fabrication/`, `step/` and `dxf/` pre-exist. Directory rows without tracked files are **not** a request to create empty placeholder artifacts.

## Geometry, safety, simulation and provenance

- Before a release ME1 must lock actual axis/datum, drawing units, physical dimensions/tolerances, alignment, motor/coupling/bearing clearance, ADXL345 orientation and repeatable mounting. Default `PENDING` — do not invent coordinates/loads/RPM/factors of safety.
- Choose Linear Static, Modal, fatigue, forced vibration, thermal/CFD and physical tests by **component risk and actual solver capability**. Modal natural frequencies do not show forced vibration amplitudes under motor operation.
- Tests are `PASS/FAIL/NOT_RUN/BLOCKED/N/A` (N/A requires rationale). `PASS` requires approved acceptance criteria, real evidence and qualified human review. Onshape import/mesh/CI alone is not acceptance.
- Changes to fixture, mount, physical fault mechanism or setup after freeze require owner decision, fixture/setup version and downstream notification. Physical Ground Truth comes from controlled physical conditions; Engineering Validation Data cannot be reclassified as Dataset v0.1 merely by QC PASS or fixture acceptance.
- Keep registry ↔ version ↔ manifest ↔ drawings/STEP/BOM ↔ report ↔ Git PR links traceable. `.gitattributes` on PR #20 disables Git newline normalization for STEP/STP/STL/DXF (`-text -eol`); this is **not an LFS policy**. A temporary synthetic byte test passed, but **real Onshape export/hash after checkout and team Windows verification remain NOT_RUN** before fabrication release. Do not commit raw/private data, temporary CAD outputs or unknown-size binaries without checking LFS/quota.

## Team workflow

ME1 proposes a feature branch + PR into `dev`; use [requirements V2](../../docs/project_management/NCKH_ME1_ONSHAPE_CAD_GITHUB_WORKFLOW_REQUIREMENTS_V2.md), root/scoped `AGENTS.md` and the team's Project Update process. This repository document is a *workflow proposal until formally announced*, not a claim that CAD TEST/MAIN Documents or fabrication release already exist.
