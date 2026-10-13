# ME1 — Onshape CAD Registry (schema, chưa có bản ghi release)

> **Owner:** ME1 (`@nkaq`). **Tình trạng:** TEMPLATE / PENDING. Onshape là CAD native duy nhất cho **thiết kế mới**; `hardware/mechanical/solidworks/` là **LEGACY/READ-ONLY**. Không có Document ID, Version ID hoặc artifact nào được xác minh ở thời điểm tạo file này.

## Controlled documents

| Environment | Onshape document name | Document ID | Document URL | Owner | Access/privacy verified | Workspace ID (work only) | Current controlled Version ID | Status |
|---|---|---|---|---|---|---|---|---|
| CAD TEST | PENDING | PENDING | PENDING | ME1 | PENDING | PENDING | PENDING | NOT_CONFIGURED |
| ME1_MAIN_CAD | PENDING | PENDING | PENDING | ME1 | PENDING | PENDING | PENDING | NOT_CONFIGURED |

**Điều kiện mở CAD:** ME1 xác minh gói Onshape và quyền private/public trước khi đưa thiết kế nghiên cứu lên cloud. Nếu CAD phải private nhưng tài khoản không đáp ứng, ghi `BLOCKED — ACCOUNT/CONFIDENTIALITY`. Không dùng Workspace URL động làm release baseline; link trong trường `onshape_version_url` phải dẫn tới **Version** cố định thực tế và reviewer được cấp quyền xem.

## Released/controlled component index

| component_id | release_id | engineering_rev | fixture_version | setup_version | onshape_document_id | onshape_element_id | onshape_version_id | onshape_version_url | state | owner | drawing_path | bom_path | report_path | manifest_path | supersedes | human_decision_ref | superseded_by |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|

Các trường `supersedes` (release này thay thế bản nào) và `superseded_by` (release mới đã thay thế bản này) nhận **Release ID** thật, không phải engineering Revision. Ví dụ state transition *chỉ để kiểm quy tắc*: R01 đã RELEASED → khi R02 được duyệt/RELEASED thì R01 = SUPERSEDED, R01.superseded_by = R02, R02.supersedes = R01; một candidate gặp thiếu evidence được BLOCKED hoặc REJECTED thay vì gắn RELEASED. Đây không phải Release ID thực tế.

**Không thêm row ví dụ giả**: chỉ ghi bản ghi khi có Part/Assembly ID thật, Version thực và đường dẫn artifact đang tồn tại. `component_id`, `engineering_rev`, `release_id`, `fixture_version`, `setup_version`, Onshape Version ID và Git SHA là **các loại định danh khác nhau**; không suy diễn từ nhau.

## Registry rules

1. Quy ước Component ID/Revision/Release ID và datum/axis chưa được ME1 khóa: `PENDING`; kiểm trên Drawing thật trước khi release.
2. CAD TEST: `DRAFT → UNDER_REVIEW → TESTED/REJECTED`; **không** `RELEASED`. Chuyển sang MAIN chỉ sau human review; kiểm lại features/mates, drawing/BOM của MAIN.
3. MAIN CAD: `DRAFT → UNDER_REVIEW → APPROVED → RELEASED` sau kiểm chứng, review và Human Gate đúng phạm vi; nhánh `UNDER_REVIEW → REJECTED/BLOCKED` khi không đủ yêu cầu, `BLOCKED → UNDER_REVIEW` chỉ sau khi có quyết định xử lý blocker. Bản `RELEASED → SUPERSEDED` khi một release mới hợp lệ thay thế; **giữ nguyên row lịch sử**, ghi `superseded_by=<NEW_RELEASE_ID>`, lý do/human_decision_ref và liên kết ngược. Release mới ghi `supersedes=<OLD_RELEASE_ID>`; `SUPERSEDED` không còn là baseline hiện hành. `APPROVED` không đồng nghĩa đã fabrication; `RELEASED` không đồng nghĩa fixture acceptance.
4. Khi tạo release: kiểm chứng `onshape_version_id`, mở phiên bản cố định, đối chiếu [release manifest template](release_manifest_template.md), Drawing, BOM và [test-report template](../reports/TEST_REPORT_TEMPLATE.md). Manifest thực nằm trong `../fabrication/<RELEASE_ID>/manifest.md`, **chỉ tạo khi có release thật**.
5. Mọi path phải là relative path từ root repository, dùng link hai chiều Registry ↔ manifest ↔ report ↔ drawing/BOM. Không commit token, private CAD, cache mô phỏng, kết quả giả hoặc thông tin nhạy cảm.
6. Thay geometry/mounting/fault fixture đã khóa → tăng `fixture_version/setup_version` theo quyết định ME1; thông báo MS2/EE2/IT2 và giữ khác biệt Engineering Validation Data ≠ Dataset v0.1.

Nguồn quy trình: [ME1 Onshape Requirements V2](../../../docs/project_management/NCKH_ME1_ONSHAPE_CAD_GITHUB_WORKFLOW_REQUIREMENTS_V2.md).
