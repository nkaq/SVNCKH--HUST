# NCKH — Project Update Proposal: Two-Stage Codex Review V1.1

> **PROPOSED / PENDING HUMAN APPROVAL — NOT ACTIVE.** Hồ sơ đề xuất theo `NCKH_TEAM_CHANGE_UPDATE_WORKFLOW_V1.md`, không phải thông báo đã gửi, approval hay xác nhận đã đọc.

- **Update ID:** PENDING — do ME1 cấp, không tự bịa.
- **Impact level:** B — thay đổi hướng dẫn quy trình Git/Codex ảnh hưởng ME1, MS2, EE2, ET1 và IT2.
- **Owner:** ME1 (`@nkaq`); yêu cầu independent human reviewer trước merge.
- **Source PR:** https://github.com/nkaq/SVNCKH--HUST/pull/19
- **Merged PR / commit:** PENDING; chỉ cập nhật sau merge thật.
- **Decision / Human Gate ref:** PENDING.
- **Source document:** `docs/project_management/NCKH_TWO_STAGE_CODEX_SKILLS_WORKFLOW_HANDBOOK_V1.md` nội dung V1.1.
- **Effective from:** PENDING — thời gian `YYYY-MM-DD HH:MM UTC+07:00` hoặc điều kiện được ME1 duyệt và công bố.
- **Acknowledgement deadline/channel:** PENDING — ME1 lựa chọn và thông báo chính thức.
- **ACK status:** ME1 PENDING; MS2 PENDING; EE2 PENDING; ET1 PENDING; IT2 PENDING.
- **Supersedes:** NONE; bản hướng dẫn bổ sung, chưa thay thế `AGENTS.md` hoặc `CODEX_REVIEW_WORKFLOW.md`.

## Nội dung đề xuất (OLD → NEW)

- OLD: `AGENTS.md`, `CONTRIBUTING.md` và `CODEX_REVIEW_WORKFLOW.md` định nghĩa các bước hiện hành; tài liệu cũ có thể gọi GitHub Codex review là “vòng 1”.
- NEW (chỉ nếu duyệt): Local Codex Skills review/fix/test là **vòng 1** theo thời gian; official GitHub Codex review trên PR là **vòng 2**. CI, independent review và Human Gate vẫn giữ nguyên.
- Task tuần lấy ID từ README tuần trên `dev`; PR fix finding/hạ tầng được duyệt lấy nguồn PR/quyết định hợp lệ, không chế Task ID.
- Matt Pocock Skills: đề xuất pin npm CLI `skills@1.7.0`, source commit `4588b32ecab9ecc9fc8cc6b6c5e7d675b6004b0d` và ghi nhận SHA256 sau pilot/audit. Chưa tuyên bố mọi máy đã đồng nhất.
- Git output/screenshot: phải redaction mọi credential/PAT/user-info trước khi chia sẻ.

## Nhóm bị ảnh hưởng và việc dự kiến (CHƯA GỬI THÔNG BÁO)

| Vai trò | Hành động sau khi được công bố | ACK |
|---|---|---|
| ME1 `@nkaq` | Quyết định, cấp Update ID, phê duyệt baseline Skills, xác định hiệu lực và kênh thông báo | PENDING |
| MS2 `@hquanvu12` | Xem phần calibration/drift, evidence và reviewer constraints | PENDING |
| EE2 `@dungnguyen13` | Xem firmware/timestamp, host test và hardware bench gates | PENDING |
| ET1 `@tiendatvu13` | Independent PR review, rà quyền sở hữu và electronics safety | PENDING |
| IT2 `@lebach181207-dev` | Pilot Python/DSP/ML local review/test trên task thật; báo lại khả năng vận hành | PENDING |

## Điều kiện chuyển trạng thái

1. PR #19 có CI `repo-quality` PASS trên commit mới nhất; GitHub Codex re-review; findings có căn cứ được xử lý hoặc phản hồi evidence; independent human reviewer approve.
2. ME1 cấp Update ID và ghi quyết định đúng phạm vi, thời điểm/điều kiện hiệu lực; giải quyết nếu có mâu thuẫn với hướng dẫn cũ.
3. Sau khi merge hợp lệ, ME1 hoặc người được giao gửi `[PROJECT UPDATE]` đến kênh chung, tag nhóm liên quan, ghi PR/commit/tài liệu nguồn, ngày áp dụng và deadline ACK.
4. Thành viên xác nhận `ĐÃ ĐỌC <Update ID> — READY` hoặc `BLOCKED + lý do`; ghi trạng thái thực, không điền sẵn.
5. Chỉ ghi `APPROVED`/`ACTIVE` sau khi điều kiện thực tế đáp ứng; `PR merged` không tự đủ chứng minh mọi người đã đọc hoặc workflow đã triển khai.

## Giới hạn

- GitHub Codex không đọc global Skills trên ổ D của thành viên; PR review độc lập phiên local.
- Không triển khai `OPENAI_API_KEY` có phí cho tác vụ này; quota ChatGPT Plus không phải vô hạn.
- `repo-quality` không tự thực hiện firmware compile, unit tests theo subsystem, hardware bench hoặc scientific validation.
- Ground Truth, dataset, protocol, kiến trúc, an toàn và release `dev → main` vẫn cần Human Gate riêng.
