---
name: nckh-pr-preflight
description: Rà soát thay đổi NCKH trước khi mở/merge Pull Request; kiểm tra task tuần, CODEOWNERS, dữ liệu Ground Truth, clean code, test evidence và rủi ro human gate bằng tiếng Việt.
---

# NCKH PR Preflight

Chạy khi user yêu cầu kiểm diff, chuẩn bị PR hoặc kiểm lần cuối trước merge.

1. Đọc root/scoped `AGENTS.md`, task README tuần, PR description/diff và tài liệu project liên quan theo `CODEX_PROJECT_CONTEXT_V1.md`.
2. Đối chiếu scope, owner và CODOWNERS (`.github/CODEOWNERS`), task ID, artifact, acceptance và evidence. Tác giả không tự approve.
3. Chỉ tìm lỗi có bằng chứng: buffer/timestamp/ISR, schema version, unit, data leakage, Ground Truth, dataset boundary, missing bench gate, secrets và accidental files.
4. Đánh dấu `BLOCKER/MAJOR/MINOR/QUESTION`; mỗi finding có `file:dòng`, hậu quả, cách sửa và phương án kiểm chứng bằng tiếng Việt.
5. Liệt kê tests đã thật sự chạy vs `NOT RUN`, unresolved findings, reviewer/human gate cần thiết. Không phát hành trạng thái PASS nghiên cứu hoặc quyết định merge thay ME1.
6. Soạn PR-ready summary bằng tiếng Việt; không tự commit/push/merge.
