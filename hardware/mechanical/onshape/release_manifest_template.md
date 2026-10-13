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

| Type | Component ID | Engineering Rev | Source MAIN Onshape Element ID | Immutable MAIN Version + Element URL | Source Part/Assembly/feature ID | Repo path | Export MAIN Version ID | Export units/settings | Exported by (account) | Exported at (ISO 8601 + timezone) | File size bytes | SHA256 (if actually calculated) | Verification/evidence |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| STEP (if required) |  |  |  |  |  |  |  |  |  |  |  |  |  |
| Drawing PDF (fabricated parts) |  |  |  |  |  |  |  |  |  |  |  |  |  |
| BOM CSV (assemblies) |  |  |  |  |  |  |  |  |  |  |  |  |  |
| DXF (if required) |  |  |  |  |  |  |  |  |  |  |  |  |  |
| STL/PNG (optional) |  |  |  |  |  |  |  |  |  |  |  |  |  |
| CAD Python script + parameters (if generated) |  |  |  |  |  |  |  |  |  |  |  |  |  |

**Per-artifact source binding:** mỗi hàng phải có **Element ID** và **đường dẫn bất biến tới đúng Version + Element** (hoặc `N/A + lý do` cho BOM/script không sinh trực tiếp từ Element). Đồng thời ghi Part/Assembly/feature/instance ID đúng output nguồn khi một Element chứa nhiều đối tượng. `Document ID + Version ID` chung là **không đủ** để phân biệt hai STEP hoặc Drawing xuất từ hai Element khác nhau.
Đối chiếu từng hàng với `hardware/mechanical/onshape/cad_registry.md`, Drawing/BOM, file xuất thực tế, SHA256, export settings, người xuất và thời điểm xuất. Nếu Version có nhiều Element, hai hàng artifact khác nhau phải truy ngược được nguồn riêng; không được copy Version URL chung thay cho Element URL.

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
- Source PR URL / human decision record: PENDING — external GitHub record, not evidence of CAD safety
- **artifact_source_commit:** PENDING — immutable commit containing **artifact bytes/exports** whose SHA256 is checked in this manifest
- **release_metadata_commit:** **EXTERNAL REFERENCE ONLY** — identify the Git commit holding the final manifest via the merged PR, immutable tag or GitHub Release metadata **outside this manifest**. Do not write the SHA of the commit containing this file into the same file (self-referential cycle).
- Release metadata decision/tag URL (external reference, after human release approval): PENDING
- Verification: checkout `artifact_source_commit` to validate exported files and SHA256; then resolve external release metadata ref and compare manifest/report/approval. If data is updated later, create a new controlled metadata revision rather than rewriting history.
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
