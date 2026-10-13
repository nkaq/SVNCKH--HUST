# ME1 — Mechanical Release Manifest (TEMPLATE, NOT A RELEASE)

> **TEMPLATE ONLY — NOT_APPROVED / NOT_RELEASED.** Sao chép vào `hardware/mechanical/fabrication/<RELEASE_ID>/manifest.md` **chỉ khi có release thật**. Không gán giá trị hoặc PASS giả. Mọi `PENDING` cần quyết định của ME1.

## Identity and immutable source

- Release ID: PENDING
- Component/Assembly IDs and engineering revisions: PENDING
- Fixture version / setup version: PENDING / PENDING
- Native CAD source: **Onshape ME1_MAIN_CAD**, Document ID: PENDING
- Main Document URL / Owner / Privacy verified: PENDING
- **Immutable Onshape MAIN Version ID + Version URL:** PENDING
- MAIN Version name / created timestamp: PENDING
- Elements/Assemblies and revision references: PENDING
- Candidate CAD TEST Version + method transferred/verified in MAIN: PENDING / NOT_APPLICABLE
- Drawing datum, coordinate frame, units and ADXL345 mounting axis: PENDING

## Exported artifact inventory (populate with files that actually exist)

| Type | Component ID | Engineering Rev | Repo path | Exported from MAIN Version ID | Export units/settings | Exported by (account) | Exported at (ISO 8601 + timezone) | File size bytes | SHA256 (if actually calculated) | Verification/evidence |
|---|---|---|---|---|---|---|---|---|---|---|
| STEP (if required) | | | | | | | | | | |
| Drawing PDF (fabricated parts) | | | | | | | | | | |
| BOM CSV (assemblies) | | | | | | | | | | |
| DXF (if required) | | | | | | | | | | |
| STL/PNG (optional) | | | | | | | | | | |
| CAD Python script + parameters (if generated) | | | | | | | | | | |

Không tạo artifact chỉ để điền bảng. SHA256 là chứng cứ file *không đổi*, không chứng minh đúng hình học/đạt an toàn. Không dùng STEP/STL thay feature history của Onshape.

## Mechanical verification and release gate

- Release report path (`hardware/mechanical/reports/<COMPONENT_ID>/<RELEASE_ID>/test-report.md`): PENDING
- Drawing↔BOM↔Assembly quantity/revision check: NOT_RUN
- Geometric/fit/mounting/alignment verification: NOT_RUN
- Linear Static / Modal / dynamic/fatigue / CFD applicability and evidence: NOT_RUN / N/A WITH REASON
- Physical bench/fixture acceptance: NOT_RUN
- Guarding, rotating parts, coupling, electrical clearance: NOT_RUN
- Risk/acceptance criterion references: PENDING
- Unresolved limitations, `BLOCKED` items, mitigation: PENDING
- Human reviewer, CAD/safety decision + reference: PENDING
- PR URL, Git commit SHA after merge: PENDING
- Status: **PROPOSED** (no approval or release implied)
- Supersedes/rollback reference: NONE / PENDING

Release requires evidence of inspection, acceptance against **owner-approved criteria**, approval references, registry back-link, and no unhandled critical blocker. If Onshape permission/privacy blocks access, mark `BLOCKED` and **do not release**.

## Backlinks when materializing a real release (mandatory)

This template is stored under `onshape/`, but the **real** file is saved at `hardware/mechanical/fabrication/<RELEASE_ID>/manifest.md`. Relative Markdown links copied verbatim from the template directory would be **broken**. When copying, use these links **from the real destination** (after substituting real component/release IDs):

- CAD registry — `../../onshape/cad_registry.md`
- Test report template — `../../reports/TEST_REPORT_TEMPLATE.md`
- Actual report: `../../reports/<COMPONENT_ID>/<RELEASE_ID>/test-report.md` (create only with true evidence; insert validated link when it exists).
- ME1 V2 specification — `../../../../docs/project_management/NCKH_ME1_ONSHAPE_CAD_GITHUB_WORKFLOW_REQUIREMENTS_V2.md`

**Quan trọng:** các relative links trên lấy **vị trí manifest đích** làm chuẩn, nên có thể hiển thị không đúng khi còn ở `onshape/release_manifest_template.md`. Không coi preview tại nơi chứa template là kiểm tra links release. Trước khi tạo release thật, chạy kiểm đường dẫn trên **bản sao đặt trong fabrication/<RELEASE_ID>/** và bổ sung link report thực. Các trường `exported_by`, `exported_at`, export settings và SHA256 phải thuộc **cùng lần export**, không được mượn timestamp tạo Onshape Version để thay.
