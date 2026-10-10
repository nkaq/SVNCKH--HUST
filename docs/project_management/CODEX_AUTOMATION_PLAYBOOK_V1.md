# NCKH — Codex Task Automation Playbook (v1, no paid API action)

> Mục tiêu: chuẩn hóa **quy trình tự động có người kiểm soát** trong VS Code và Codex, không thiết lập `codex-agent.yml`/`OPENAI_API_KEY`. Không có lệnh nào trong file này tự kích hoạt tác vụ theo lịch.

## 1. Các chế độ Codex

| Chế độ | Codex tự làm được | Cổng dừng bắt buộc |
|---|---|---|
| A — Explore | Đọc tài liệu phù hợp, vẽ dependency, giải thích kiến trúc | Không sửa file |
| B — Plan | Phân rã task, đề xuất API/schema, file sửa, rủi ro, test | Chờ human xác nhận plan nếu phạm vi vượt chỉnh sửa nhỏ |
| C — Implement | Sửa code/file trên **feature branch**, viết unit test, chạy test khả dụng, tự kiểm diff | Human kiểm diff; không commit/push/merge mặc định |
| D — Review | Review diff/PR, phát hiện lỗi thực sự và giải thích bằng tiếng Việt | Không tự tuyên bố PASS khoa học/an toàn |
| E — Report | Tóm tắt evidence, draft báo cáo, checklist, PR description | Human xác thực số liệu/claim |

**Không suy diễn rằng Codex tự chạy 24/7 hay tự nhận task từ README chỉ nhờ có `AGENTS.md`.** Muốn tác vụ khởi chạy cần người giao việc trong Codex, hoặc tính năng scheduling/automation được bật rõ ràng, hoặc trigger hệ thống được cấu hình riêng. GitHub PR auto-review chỉ thực hiện **review**, không tự sửa và push.

## 2. Task intake (đầu vào có cấu trúc)

```text
Task source: docs/plans/weekXX/README.md on dev
Task ID: <ME1-assigned ID>
Owner: ME1|MS2|EE2|ET1|IT2
Goal: <expected behavior and why>
Acceptance criteria: <verifiable PASS conditions>
Scope: <exact subsystem files/folders>
Inputs/contracts: <existing files/schema, version>
Evidence: <tests, log, plots, hardware checks>
Dependencies: <other owner approval>
Human Gate: <yes/no + rationale>
```

Nếu thiếu Goal/acceptance/owner hoặc source không rõ, **hỏi lại**, không tự đặt tiêu chí hay thông số để task có vẻ hoàn thành.

## 3. Quy trình tự động trong một feature branch

1. **Inspect:** đọc root/scoped `AGENTS.md`, task README và tài liệu liên quan trong `CODEX_PROJECT_CONTEXT_V1.md`; kiểm branch, working tree, trạng thái file liên quan.
2. **Plan:** tóm tắt hiện trạng, contract phải giữ, files to change, risks, test strategy, human approval needed. Với thay đổi đáng kể, **chờ duyệt plan**.
3. **Implement:** sửa tối thiểu, ưu tiên interfaces rõ và tests host-side, không tự đổi hardware/sampling/label/dataset.
4. **Verify:** chạy test/syntax/build có sẵn thực sự. Nếu thiếu toolchain hoặc board, ghi `NOT RUN`, lý do, manual validation needed. Không bịa PASS.
5. **Self-review:** xem diff, vi phạm standards, bất biến NCKH, dependency và acceptance criteria.
6. **Handoff:** báo `Files changed / Tests run / Results / Evidence / Risks / Human Gate / Suggested PR message` bằng tiếng Việt.
7. **Human:** người làm kiểm diff → stage đúng file → commit/push → PR vào `dev` → CI/Codex review → domain owner quyết định. Không force-push, xóa lịch sử, tự merge.

## 4. PR preflight automation (không cần API)

- Từ task ID trong README, kiểm có deliverable, evidence và changed-file scope khớp PR không.
- Kiểm thay đổi API/schema/unit/timing/pin/fixture/fault/labels có version và thông báo owner tương ứng.
- Kiểm không có secrets, file raw data không phù hợp, file accidental staging, benchmark fabricated.
- Kiểm test có log/chứng cứ, phân biệt bench vs simulator vs unit test.
- **Trường hợp Codex hoạt động bình thường:** Giữ PR ở trạng thái Open cho tới khi xác nhận phiên Codex review hiện tại đã hoàn tất. Đọc và xử lý tất cả findings có căn cứ trước khi merge. `Completed` chỉ là một ví dụ về trạng thái giao diện, không phải điều kiện phụ thuộc tên cố định.

- **Trường hợp Codex không khả dụng:** Nếu Codex không thể chạy do lỗi tích hợp, thiếu quyền hoặc hết hạn mức, phải ghi rõ lý do và bằng chứng trong PR. Khi đó cho phép sử dụng **human review độc lập** thay thế, không được tự coi Codex đã PASS.

- **Điều kiện human review thay thế:** Reviewer phải là người khác tác giả PR, có chuyên môn phù hợp, kiểm tra diff, acceptance criteria, test evidence, rủi ro kỹ thuật/nghiên cứu và ghi rõ quyết định review trên GitHub.

- **Human Gate vẫn bắt buộc:** Việc Codex không khả dụng không được dùng để bỏ qua phê duyệt liên quan đến an toàn cơ khí/điện, Ground Truth, dataset, kiến trúc hoặc scientific claims.

- **Điều kiện merge:** PR chỉ được merge khi `repo-quality` PASS, findings có căn cứ đã được xử lý, reviewer độc lập đã hoàn tất phần kiểm tra theo quy định và Human Gate cần thiết đã được chấp thuận. Không merge chỉ vì Codex chưa đưa ra nhận xét hoặc nút merge đang sáng.
- Codex findings phải bằng tiếng Việt, gồm `file:dòng`, nguyên nhân, rủi ro, cách sửa và kiểm chứng; bot có thể không luôn tuân thủ.

## 5. Bài toán automation đề xuất theo mức ưu tiên

**Có thể áp dụng ngay (Codex VS Code / kỹ năng thủ công):**
- Từ một task tuần → tạo implementation plan, dependency list, test plan và checklist.
- Phân tích diff → review coding standard/research invariants → đề xuất patch và tests.
- Sinh skeleton parser hoặc đơn vị DSP có tests **từ contract thật**; không bịa schema.
- Dự thảo PR description và test-evidence table; yêu cầu human xác nhận.

**Nên bổ sung CI (không gọi OpenAI API, cần duyệt config):**
- Python formatting/lint/unit test/schema validation; C++ formatting/static checks/build khi xác định toolchain của EE2; Markdown/repository checks.
- CI báo FAIL theo bài test chạy thật. Một workflow linter **không thay Codex code review**.
- Trước khi bật required check, xác nhận CI chạy được trên mọi loại PR và không chặn ngõ cụt cho CAD/docs.

**Không bật mặc định:**
- tự tạo/sửa PR không cần người giám sát;
- tự đổi wiring/safety/fixture acceptance, Ground Truth hay dataset;
- tự chạy thực nghiệm hardware, tự xác nhận thông số;
- GitHub Actions dùng `OPENAI_API_KEY` (vì người dùng không muốn phát sinh API cost).

## 6. Kết thúc một task: báo cáo mẫu

```text
Task ID / nguồn:
Trạng thái: READY FOR REVIEW | BLOCKED | IN PROGRESS
Files changed:
Contracts & version affected:
Tests đã chạy (command + output + PASS/FAIL):
Tests chưa chạy (NOT RUN + lý do):
Evidence (recording/log/report):
Rủi ro còn mở và owner:
Human Gate required:
Suggested PR title/description:
```
