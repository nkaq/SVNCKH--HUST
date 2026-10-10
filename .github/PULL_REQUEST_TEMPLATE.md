# NCKH — Pull Request (ME1 / MS2 / EE2 / ET1 / IT2)

> **Mẫu chung cho cả 5 thành viên.** PR task thông thường vào `dev`; `dev → main` chỉ khi chốt milestone/release theo quy trình được duyệt. Điền thông tin thật, ghi `Không áp dụng + lý do` khi cần. Không bịa số liệu, test, Issue hoặc phê duyệt.

## 1. Task và người chịu trách nhiệm

- **Loại PR:** Task tuần / Sửa lỗi review / Hạ tầng-tài liệu / Milestone
- **Owner:** ME1 / MS2 / EE2 / ET1 / IT2 (`@username`)
- **Task ID / nguồn:** `WXX-ROLE-NN` + `docs/plans/weekXX/README.md` / PR finding `#...` / task hạ tầng có phê duyệt
- **Issue (tùy chọn):** `#...` / Không có
- **Subsystem / file chính:** `path/to/real/file`
- **Base / compare:** `dev ← feature/...` / `main ← dev` (milestone)
- **Reviewer độc lập:** `@username` / Chưa chỉ định

## 2. Mục tiêu, thay đổi và ảnh hưởng

**Mục tiêu / acceptance criteria:**
- 

**Đã thay đổi:**
- 

**Các interface hoặc phiên bản chịu ảnh hưởng:** packet/schema/units/timestamp/pinout/fixture/firmware/dataset / Không đổi (chọn và mô tả).

**Phụ thuộc liên phân hệ & owner cần xác nhận:** Có / Không; nếu có, ghi owner và tham chiếu contract.

**Project Update:** Mức A / B / C / Không áp dụng. Với mức B/C: dẫn `Update ID`, quyết định phê duyệt, người bị ảnh hưởng và điều kiện `READY`/tương thích theo `NCKH_TEAM_CHANGE_UPDATE_WORKFLOW_V1.md`. Không tự công bố `ACTIVE` nếu chưa đủ điều kiện.

## 3. Kiểm thử và bằng chứng

- **Cách chạy / tái hiện:** command hoặc quy trình thực tế / Không áp dụng + lý do
- **Tests thực sự đã chạy:** tên test, lệnh, phiên bản/môi trường
- **Kết quả:** PASS / FAIL / NOT RUN (ghi rõ từng test và lý do)
- **Evidence:** đường dẫn log, ảnh, báo cáo, commit hoặc biên bản thực tế
- **Rủi ro còn mở / việc chưa kiểm chứng:**

> Không gọi test PC/simulation là test trên ESP32-S3; không tự nhận hardware/CAD/safety/dataset PASS nếu chưa có kiểm chứng thật.

## 4. Ground Truth, dữ liệu và Human Gate

- **Ground Truth / physical fault labels:** Không ảnh hưởng / Ảnh hưởng + protocol/approval
- **Engineering Validation vs Dataset v0.1:** Không ảnh hưởng / Ảnh hưởng + provenance, QC, version, inclusion decision và Human Gate
- **Metadata / recording/setup/fixture version:** Không đổi / Nêu thay đổi
- **An toàn cơ/điện, kiến trúc, scientific claim:** Không ảnh hưởng / Người có thẩm quyền phê duyệt + evidence

> Ground Truth phải từ điều kiện vật lý được chủ động áp đặt và truy vết, không suy nhãn từ FFT/model. Recording Engineering Validation không được tự chuyển thành Dataset v0.1. AI không có quyền quyết định Human Gate.

## 5. Codex và human review

- **Codex:** Chưa chạy / Running / Completed / Không khả dụng + lý do và bằng chứng
- **Reviewed commit:** SHA của phiên review mới nhất
- **Findings P1/P2:** Không có / Đã xử lý (link commit) / Còn mở (link + lý do)
- **Human reviewer độc lập:** `@username` + kết quả / Chưa hoàn tất
- **Human Gate:** Không cần + lý do / Cần + người phê duyệt + reference

Yêu cầu Codex review **bằng tiếng Việt**, theo root/scoped `AGENTS.md` và standards liên quan; chỉ rõ file:dòng, bằng chứng, hậu quả, cách sửa, cách kiểm chứng. Trường hợp Codex không khả dụng: ghi nhận và thực hiện human review độc lập thay thế; không giả nhận Codex đã chạy.

## 6. Checklist trước khi merge

- [ ] Đúng base branch, đúng phạm vi; không có secret, raw/private data hay file ngoài task
- [ ] Tests/evidence được báo cáo trung thực; acceptance criteria và dependency được xem xét
- [ ] `repo-quality` PASS trên commit mới nhất
- [ ] Codex review hoàn tất trên commit mới nhất và findings có căn cứ được xử lý; **hoặc** có hồ sơ Codex không khả dụng và human review độc lập thay thế
- [ ] Reviewer độc lập và Human Gate phù hợp đã hoàn tất; tác giả không tự approve PR
- [ ] Nếu thay đổi liên phân hệ mức B/C: điều kiện phê duyệt/`READY`/tương thích đã được kiểm chứng trước khi kích hoạt
- [ ] Chỉ sau đó mới merge: task `feature → dev` thường dùng **Squash**, milestone `dev → main` theo ruleset **Merge commit**

**Quyết định:** CHỜ REVIEW / FIX THEN REVIEW / HUMAN DECISION REQUIRED / READY TO MERGE
