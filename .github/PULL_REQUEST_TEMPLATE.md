# NCKH — Pull Request (ME1 / MS2 / EE2 / ET1 / IT2)

> **Mẫu chung cho cả 5 thành viên.** PR task thông thường vào `dev`. PR `dev → main` là **milestone/release**, chỉ thực hiện sau integration test có bằng chứng và Human Gate. PR chỉ sửa tài liệu quản trị/template trực tiếp vào `main` là **ngoại lệ cần ME1 phê duyệt rõ phạm vi và reviewer độc lập**, không thay cho release. Điền thông tin thật; không bịa test, Issue, số liệu hoặc phê duyệt.

## 1. Task và người chịu trách nhiệm

- **Loại PR:** Task tuần / Sửa lỗi review / Hạ tầng-tài liệu / Milestone release / Bảo trì governance trên `main` (ngoại lệ có phê duyệt)
- **Owner:** ME1 / MS2 / EE2 / ET1 / IT2 (`@username`)
- **Task ID / nguồn:** `WXX-ROLE-NN` + `docs/plans/weekXX/README.md` / PR finding `#...` / task hạ tầng có phê duyệt
- **Issue (tùy chọn):** `#...` / Không có
- **Subsystem / file chính:** `path/to/real/file`
- **Base / compare:** `dev ← feature/...` / `main ← dev` (milestone release) / `main ← docs/...` (governance-only có phê duyệt ngoại lệ)
- **Reviewer độc lập:** `@username` / Chưa chỉ định

## 2. Mục tiêu, thay đổi và ảnh hưởng

**Mục tiêu / acceptance criteria:**
- (Điền mục tiêu và tiêu chí nghiệm thu)

**Đã thay đổi:**
- (Điền các thay đổi cụ thể)

**Các interface hoặc phiên bản chịu ảnh hưởng:** packet/schema/units/timestamp/pinout/fixture/firmware/dataset / Không đổi (chọn và mô tả).

**Phụ thuộc liên phân hệ & owner cần xác nhận:** Có / Không; nếu có, ghi owner và tham chiếu contract.

**Project Update — phân loại và điều kiện có thể kiểm tra ngay trên mọi branch:**
- **Mức A:** sửa cục bộ (typo, mô tả, refactor không đổi hành vi), không ảnh hưởng task hoặc interface của người khác; ghi PR/changelog khi phù hợp.
- **Mức B:** đổi task/owner/deadline/deliverable, dependency liên phân hệ, packet/schema/units/timestamp/pinout, parser, firmware interface, QC hoặc artifact bàn giao. Bắt buộc có `Update ID`, PR + contract/version, owner bị ảnh hưởng, hành động, kế hoạch migration/test và thời điểm hiệu lực; thông báo/tag từng owner.
- **Mức C:** thay đổi hardware đã khóa, an toàn cơ/điện, experiment protocol/physical Ground Truth, fixture acceptance, dataset inclusion/exclusion, kiến trúc hoặc scientific claim. Cần thêm approval có thể truy vết của ME1 và domain owner (Human Gate); không dùng CI/Codex thay người phê duyệt.
- **Kích hoạt B/C:** Không đánh dấu `ACTIVE` khi có owner liên quan chưa xác nhận `READY` hoặc đang `BLOCKED`. Ngoại lệ chỉ được áp dụng nếu có phương án version compatibility/feature flag, migration, rollback và kế hoạch rollout **được ME1 + owner liên quan phê duyệt**, có evidence consumer không bị hỏng; mọi Human Gate mức C vẫn bắt buộc.
- **Bản ghi:** Mức A / B / C / Không áp dụng + lý do; `Update ID` (nếu B/C), tài liệu nguồn/PR, approvals, danh sách owner `READY/BLOCKED`, bằng chứng compatibility và điều kiện hiệu lực:

## 3. Kiểm thử và bằng chứng

- **Cách chạy / tái hiện:** command hoặc quy trình thực tế / Không áp dụng + lý do
- **Tests thực sự đã chạy:** tên test, lệnh, phiên bản/môi trường
- **Kết quả:** PASS / FAIL / NOT RUN (ghi rõ từng test và lý do)
- **Evidence:** đường dẫn log, ảnh, báo cáo, commit hoặc biên bản thực tế
- **Rủi ro còn mở / việc chưa kiểm chứng:**

> Không gọi test PC/simulation là test trên ESP32-S3; không tự nhận hardware/CAD/safety/dataset PASS nếu chưa có kiểm chứng thật.

**Nếu PR `dev → main` là milestone/release (bắt buộc):** Ghi `integration_test_plan`, các subsystem/interface đã kiểm, phiên bản và kết quả **PASS thực tế**, link log/biên bản, reviewer và rủi ro còn mở. `FAIL`, `NOT RUN` hoặc thiếu evidence integration test **chặn release**; CI `repo-quality` không thay integration test. Nếu hạng mục không áp dụng, nêu lý do theo phạm vi nhưng không bỏ qua kiểm thử tích hợp các interface thực sự bị ảnh hưởng.

**Nếu PR governance-only trực tiếp vào `main` (ngoại lệ):** Ghi `governance_exception_approval_ref` của ME1, phạm vi giới hạn ở tài liệu/quy tắc/template, bằng chứng review/CI. Có thể ghi hardware integration test `NOT APPLICABLE` kèm lý do **vì đây không phải release**; Human Gate cho thay đổi lên `main` vẫn bắt buộc.

## 4. Ground Truth, dữ liệu và Human Gate

- **Ground Truth / physical fault labels:** Không ảnh hưởng / Ảnh hưởng + protocol/approval
- **Engineering Validation vs Dataset v0.1:** Không ảnh hưởng / Ảnh hưởng + provenance, QC, version, inclusion decision và Human Gate
- **Metadata / recording/setup/fixture version:** Không đổi / Nêu thay đổi
- **Experiment protocol (kể cả sampling configuration, repetitions, QC, recording procedure không đổi physical fault label):** Không thay đổi / Có thay đổi + ME1/domain-owner Human Gate, protocol version và approval reference **bắt buộc**.
- **An toàn cơ/điện, kiến trúc, scientific claim:** Không ảnh hưởng / Người có thẩm quyền phê duyệt + evidence
- **Mọi PR có base `main` (milestone hoặc governance-only):** Human Gate bắt buộc cho release lên `main`/ngoại lệ governance; ghi ME1 (hoặc người được phân quyền), phạm vi phê duyệt, quyết định, reference và reviewer độc lập. Không chọn `Không cần`.

> Ground Truth phải từ điều kiện vật lý được chủ động áp đặt và truy vết, không suy nhãn từ FFT/model. Recording Engineering Validation không được tự chuyển thành Dataset v0.1. AI không có quyền quyết định Human Gate.

## 5. Codex và human review

- **Codex:** Chưa chạy / Running / Completed / Không khả dụng + lý do và bằng chứng
- **Reviewed commit:** SHA của phiên review mới nhất
- **Findings theo `AGENTS.md`:** `BLOCKER` / `MAJOR` / `MINOR` / `QUESTION` + file:dòng, link finding, cách xử lý và bằng chứng. Nếu Codex dùng badge P1/P2 thì giữ badge như thông tin bổ sung; **không giả định P1/P2 là ánh xạ một-một** với bốn nhóm trên. Mọi `BLOCKER` phải được giải quyết trước merge; `MAJOR` có căn cứ phải sửa hoặc được owner chấp nhận rủi ro có ghi nhận.
- **Human reviewer độc lập:** `@username` + kết quả / Chưa hoàn tất
- **Human Gate:** BẮT BUỘC nếu sửa experiment protocol/GT/dataset/safety/architecture/research claim hoặc PR có base `main` (kể cả governance-only); ghi approver, phạm vi quyết định, evidence/reference. Các PR khác: Cần + reference / Không cần + lý do.

Yêu cầu Codex review **bằng tiếng Việt**, theo root/scoped `AGENTS.md` và standards liên quan; chỉ rõ file:dòng, bằng chứng, hậu quả, cách sửa, cách kiểm chứng. Trường hợp Codex không khả dụng: ghi nhận và thực hiện human review độc lập thay thế; không giả nhận Codex đã chạy.

## 6. Checklist trước khi merge

- [ ] Đúng base branch, đúng phạm vi; không có secret, raw/private data hay file ngoài task
- [ ] Tests/evidence được báo cáo trung thực; acceptance criteria và dependency được xem xét
- [ ] `repo-quality` PASS trên commit mới nhất
- [ ] Codex review hoàn tất trên commit mới nhất và findings có căn cứ được xử lý; **hoặc** có hồ sơ Codex không khả dụng và human review độc lập thay thế
- [ ] Reviewer độc lập và Human Gate phù hợp đã hoàn tất; tác giả không tự approve PR
- [ ] Nếu thay đổi experiment protocol (sampling/repetitions/QC/recording), ME1/domain owner đã phê duyệt và có approval reference, dù không thay physical label
- [ ] Nếu Project Update mức B/C: mọi owner bị ảnh hưởng `READY` trước khi `ACTIVE`, hoặc có compatibility/rollout/rollback plan đã được phê duyệt; không còn consumer `BLOCKED` thiếu phương án
- [ ] Nếu PR có base `main`: đã có **Human Gate bắt buộc** và reviewer độc lập, ghi phạm vi/approval reference, bất kể file thay đổi
- [ ] Nếu `dev → main` milestone: **integration test PASS thực tế** với plan, log/evidence, version và owner xác nhận; `FAIL`/`NOT RUN` chặn release
- [ ] Nếu governance-only `feature → main`: ME1 phê duyệt ngoại lệ và phạm vi chỉ tài liệu/quản trị, giải thích integration test N/A (không phải release)
- [ ] Chỉ sau đó mới merge: task `feature → dev` dùng **Squash**, PR vào `main` theo ruleset dùng **Merge commit**

**Quyết định:** CHỜ REVIEW / FIX THEN REVIEW / HUMAN DECISION REQUIRED / READY TO MERGE
