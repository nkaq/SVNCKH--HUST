# NCKH — Project Update Proposal: ME1 CAD Migration (Onshape)

> **PROPOSED / PENDING** — Đây là hồ sơ chuẩn bị thay đổi quy trình mức **B**, không phải thông báo đã gửi và không phải bằng chứng nhóm đã ACK. ME1 đã chọn Onshape cho thiết kế ME1 mới; quy trình repo áp dụng toàn nhóm cần duyệt/công bố trước khi gọi `ACTIVE`.

- **Update ID:** PENDING — ME1 cấp ID chính thức, không bịa.
- **Impact:** B — công cụ và chuẩn bàn giao CAD/fixture, tài liệu GitHub và quy trình liên quan.
- **Owner:** ME1 (`@nkaq`); BOM phối hợp ET1 (`@tiendatvu13`).
- **Scope PR:** chỉ **10 UPDATE + 6 CREATE** thuộc kế hoạch đã duyệt; không sửa SolidWorks legacy, firmware, data hoặc Ground Truth.
- **Source:** [ME1 Onshape Requirements V2](NCKH_ME1_ONSHAPE_CAD_GITHUB_WORKFLOW_REQUIREMENTS_V2.md).
- **PR / merged commit:** PENDING (ghi URL/SHA thật sau khi có).
- **Decision / Human Gate:** ME1 đã duyệt phạm vi chuẩn hóa tài liệu; việc khóa geometry, safety, experiment/GT và fixture release là gate riêng, chưa được cấp.
- **Effective from:** PENDING — ME1 xác định `YYYY-MM-DD HH:MM UTC+07:00` hoặc điều kiện sau công bố.
- **Announcement channel and ACK due:** PENDING — ME1 lựa chọn; chưa gửi.
- **Affected:** ME1; MS2, EE2, ET1, IT2 cần biết tác động tới fixture, mounting, BOM, setup metadata/thu dữ liệu khi liên quan.
- **ACK:** ME1 PENDING; MS2 PENDING; EE2 PENDING; ET1 PENDING; IT2 PENDING.
- **Supersedes:** quy ước *thiết kế mới* dựa trên SolidWorks trong các hướng dẫn chủ động; **không** sửa hồi tố kế hoạch Week 3, CAD lịch sử hoặc protocol đã nghiệm thu.

## OLD → NEW

- OLD: `hardware/mechanical/solidworks/` và `.SLDPRT/.SLDASM/.SLDDRW` còn được liệt kê là workflow chủ đạo cho thiết kế mới.
- NEW: **Onshape-only cho CAD native mới**; hai Document `CAD TEST` và `ME1_MAIN_CAD` (định danh thật PENDING), nguồn release bất biến là **MAIN Version ID/URL**, GitHub quản lý registry, script (nếu có), STEP/Drawing PDF/DXF/BOM, evidence và report.
- Bảo toàn SolidWorks legacy/LFS cũ; không bắt buộc conversion hay xóa file.
- Không thay locked hardware, fixture approval, dataset/GT protocol hoặc quyền chuyên môn.

## Action after a reviewed merge

1. **PROPOSED → APPROVED:** trước khi merge, ME1 ghi decision và scope review hợp lệ cho *thay đổi tài liệu quy trình*; trường hợp CODEOWNERS yêu cầu reviewer hợp lệ, thực hiện theo GitHub rules.
2. **APPROVED → ACTIVE:** chỉ sau merge, ME1 công bố chính thức bản Project Update với Update ID, PR/commit, mốc hiệu lực, phiên bản nguồn, hành động và hạn ACK. Kiểm tương thích quyền Onshape, hỗ trợ migration có liên quan, xử lý mọi thành viên `BLOCKED`; không đánh dấu ACTIVE vì chỉ nhận được `BLOCKED`.
3. Nếu có `BLOCKED` về private/public, access, LFS, safety hoặc migration: dừng phần bị ảnh hưởng; ME1 xác định rollback/escalation, thông báo chủ subsystem và cập nhật quyết định.
4. Giữ báo cáo ACK tại kênh nhóm/PR/issue thật được chỉ định; không tự điền `READY`, không tự cho là thông báo đã gửi.
5. **Onshape account/privacy gate số 0:** xác minh license, privacy, owner/editor/viewer và Simulation trước khi đưa bất kỳ CAD nghiên cứu cần private lên cloud; chưa xác minh → PENDING/BLOCKED.

## Acceptance and limits

- Onshape native Version được kiểm chứng qua UI thật và reviewer hợp lệ, không qua URL giả.
- Mọi CAD/fixture safety, linear static, modal, fatigue, vibration, fit và fatigue approval đều `NOT_RUN` đến khi có evidence.
- Engineering Validation Data không thành Dataset v0.1 từ QC PASS/fixture acceptance.
- `repo-quality` chỉ là CI cấu trúc/chính sách hiện được bật; không xác nhận CAD/FEA safety.
- ME1 ghi quyết định, ngày hiệu lực và thông báo theo [Team Change Update Workflow](NCKH_TEAM_CHANGE_UPDATE_WORKFLOW_V1.md) và [Project Update Template](PROJECT_UPDATE_TEMPLATE.md).
