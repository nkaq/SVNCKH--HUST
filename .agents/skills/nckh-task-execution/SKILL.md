---
name: nckh-task-execution
description: Triển khai task NCKH từ docs/plans/weekXX/README.md trong VS Code/Codex theo đúng subsystem, acceptance criteria, chuẩn công nghiệp và thực nghiệm; dừng ở bước human review trước khi commit/push.
---

# NCKH Task Execution

Chỉ áp dụng khi user yêu cầu triển khai hoặc sửa mã/tài liệu cho một task thuộc NCKH. Không tự bắt đầu task chỉ vì README có nội dung mới.

1. Đọc root và scoped `AGENTS.md`, task source và file thực sự liên quan. Tham khảo `docs/project_management/CODEX_PROJECT_CONTEXT_V1.md` và standards tương ứng, không tự đọc mọi file trong repo.
2. Xác nhận `Task ID`, owner, deliverables, acceptance, evidence; nếu thiếu điều kiện cốt lõi, hỏi ME1/owner.
3. Đối chiếu scope, version, interface và Human Gate. Với thay đổi không tầm thường, trình bày plan, expected diff và tests; chờ người dùng đồng ý.
4. Triển khai thay đổi nhỏ nhất, thêm test thích hợp; không giả định sensor data hoặc hardware thật.
5. Chạy các kiểm tra thực sự khả dụng; nếu không thể, ghi `NOT RUN` và lý do. Tránh tự nhận đã chạy trên board.
6. Tự review against `CODEX_ENGINEERING_STANDARD_V1.md` và `CODEX_RESEARCH_STANDARD_V1.md`; kiểm branch/working tree/diff.
7. Báo kết quả bằng tiếng Việt: files, lý do, tests/evidence, remaining risk, owner, Human Gate. Không tự thực hiện git add, git commit, git push hoặc git merge trong workflow của nhóm, kể cả khi người dùng yêu cầu "commit và push luôn". Sau khi triển khai và kiểm thử, Codex phải dừng lại, bàn giao danh sách file thay đổi, diff, kết quả test và rủi ro để người phụ trách tự kiểm tra, stage, commit, push và tạo Pull Request. Không tự bỏ qua bước Human Review.
