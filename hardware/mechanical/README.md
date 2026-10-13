# Mechanical Testbench — Onshape-first (ME1)

**Owner:** ME1 (`@nkaq`). **Current tool for all new mechanical CAD:** **Onshape** (Part Studio, Assembly, Drawing, controlled Versions). **No new SolidWorks design**. Existing `solidworks/` files/directories are **LEGACY/READ-ONLY** for historical traceability; preserve their paths, including older exports. This document does **not** certify an actual fixture design or safety.

## Status and conflict with current weekly assignment

**ME1 đã chọn Onshape cho thiết kế mới**, nhưng chuẩn bàn giao Onshape cho toàn nhóm vẫn **PROPOSED — NOT ACTIVE** cho đến khi ME1 cấp Project Update ID, phê duyệt nội dung thay đổi các nguồn task đang ACTIVE, công bố mốc hiệu lực và xử lý ACK/compatibility. Đặc biệt `docs/plans/week03/README.md` vẫn có task “Fixture + SolidWorks” và `NCKH_TEAM_SUBMISSION_WORKFLOW_V3.md` còn yêu cầu file `.SLD*`; **không tự sửa hồ sơ tuần lịch sử trong PR này** và **không bắt thành viên tự làm SolidWorks hoặc tự đổi deliverable sang Onshape khi nhiệm vụ đang ACTIVE mâu thuẫn**. Đánh dấu task liên quan `BLOCKED — SOURCE CONFLICT`, yêu cầu ME1 quyết định và cấp Project Update/task source hợp lệ. Bộ tài liệu mới sẵn sàng để áp dụng khi gate hoàn tất.

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
| Legacy | `solidworks/` | **Do not delete/move/rename**, no new SolidWorks files for active ME1 work |

`fabrication/`, `step/` and `dxf/` pre-exist. Directory rows without tracked files are **not** a request to create empty placeholder artifacts.

## Geometry, safety, simulation and provenance

- Before a release ME1 must lock actual axis/datum, drawing units, physical dimensions/tolerances, alignment, motor/coupling/bearing clearance, ADXL345 orientation and repeatable mounting. Default `PENDING` — do not invent coordinates/loads/RPM/factors of safety.
- Choose Linear Static, Modal, fatigue, forced vibration, thermal/CFD and physical tests by **component risk and actual solver capability**. Modal natural frequencies do not show forced vibration amplitudes under motor operation.
- Tests are `PASS/FAIL/NOT_RUN/BLOCKED/N/A` (N/A requires rationale). `PASS` requires approved acceptance criteria, real evidence and qualified human review. Onshape import/mesh/CI alone is not acceptance.
- Changes to fixture, mount, physical fault mechanism or setup after freeze require owner decision, fixture/setup version and downstream notification. Physical Ground Truth comes from controlled physical conditions; Engineering Validation Data cannot be reclassified as Dataset v0.1 merely by QC PASS or fixture acceptance.
- Keep registry ↔ version ↔ manifest ↔ drawings/STEP/BOM ↔ report ↔ Git PR links traceable. Do not commit raw/private data, temporary CAD outputs or unknown-size binaries without checking LFS/quota.

## Team workflow

ME1 proposes a feature branch + PR into `dev`; use [requirements V2](../../docs/project_management/NCKH_ME1_ONSHAPE_CAD_GITHUB_WORKFLOW_REQUIREMENTS_V2.md), root/scoped `AGENTS.md` and the team's Project Update process. This repository document is a *workflow proposal until formally announced*, not a claim that CAD TEST/MAIN Documents or fabrication release already exist.
