# Quy trình Codex Code Review — NCKH

> Codex là reviewer kỹ thuật vòng 1; đây là **quy trình nội bộ** chứ không phải GitHub Action dùng API key. Review của Codex không thay thế kiểm thử và Human Gate.

## 1. Nguồn task chính thức

ME1 giao nhiệm vụ trong `dev/docs/plans/weekXX/README.md`, ghi rõ Task ID, owner, deliverables, acceptance criteria và evidence. **GitHub Issue là tùy chọn** để theo dõi, không phải nơi bắt buộc để nhận nhiệm vụ.

```text
ME1 đăng README kế hoạch tuần trên dev
→ thành viên đọc task, tạo feature branch từ dev
→ làm artifact đúng subsystem + thực hiện kiểm thử thực tế
→ commit, push và mở PR vào dev
→ repo-quality chạy; Codex review khi trigger khả dụng
→ xử lý findings trên cùng branch, xem xét re-review
→ reviewer độc lập và Human Gate (nếu cần)
→ Squash merge vào dev
→ kiểm thử tích hợp; chỉ dev → main khi chốt milestone
```

**Không bấm merge chỉ vì `repo-quality` PASS hoặc nút merge sáng.** Đợi `Codex Review Summary` chuyển sang `Completed` và đọc các findings. `Completed` chỉ có nghĩa review đã kết thúc, **không có nghĩa là không còn lỗi**.

Nếu Codex chưa chạy do giới hạn sử dụng, quyền hoặc lỗi hệ thống, ghi rõ lý do và có **human review độc lập** thay thế trước khi merge. Đây là quy tắc vận hành nhóm, chưa phải required GitHub status check có tên `Codex Agent Checked`.

## 2. Người review độc lập và CODEOWNERS

Tác giả PR **không thể approve PR của chính mình**. Nếu tác giả cũng là CODEOWNER duy nhất của file, phải request reviewer là collaborator khác có năng lực liên quan (chẳng hạn ME1). Nếu ruleset bật **Require review from Code Owners**, một reviewer không thuộc CODEOWNERS **không tự đáp ứng được** điều kiện đó: cần thiết lập code owner dự phòng đủ quyền hoặc xem lại rule trước khi áp dụng.

## 3. Ngôn ngữ review

**Codex phải viết bằng tiếng Việt** cho tóm tắt, inline comments, nguyên nhân, hậu quả, hướng sửa, code review suggestions và cách kiểm tra. Giữ nguyên tên file, biến, API, code, log và nhãn kỹ thuật như `P1/P2`, `BLOCKER/MAJOR/MINOR`, `PASS/FAIL`.

Mỗi finding cần:

1. **Mức độ và vị trí `file:dòng`**.
2. **Lỗi và bằng chứng** từ diff/ngữ cảnh (có thể dẫn chiếu `AGENTS.md`).
3. **Hậu quả/rủi ro** đối với hệ thống hoặc nghiên cứu.
4. **Cách sửa cụ thể**, nêu code minh họa khi có đủ ngữ cảnh.
5. **Cách kiểm chứng**; đánh dấu rõ test nào chưa chạy.

Đây là hướng dẫn cho reviewer; nền tảng Codex không được bảo đảm luôn tuân thủ tuyệt đối. Khi bot trả bằng tiếng Anh, yêu cầu giải thích lại bằng tiếng Việt nhưng vẫn phải xử lý nội dung kỹ thuật.

## 4. Trọng tâm review theo thành viên

| Thành viên | Folder | Kiểm tra |
|---|---|---|
| EE2 | `firmware/esp32s3/` | Sampling/timing, FIFO, ring buffer, ISR, timestamp, dropped samples, I2C/SPI, RAM, Serial/OLED blocking |
| IT2 | `ml/`, `software/`, `data/` | Recording-level leakage, preprocessing fit từ train, metadata, metric, reproducibility, ESP32-S3 resource cost |
| MS2 | `experiments/sensor_characterization/` | ADXL345 calibration, bias/noise/sensitivity/drift, đơn vị, thống kê, evidence |
| ET1 | `hardware/electronics/` | Schematic/pinout, nguồn, điện áp, pull-up, ADC, ACS724, PT100/MAX31865, US5881 |
| ME1 | `hardware/mechanical/`, `docs/architecture/`, `experiments/protocols/` | Fixture version, BOM/drawing, mounting, alignment, fault mechanism, Ground Truth, acceptance |

**Không vi phạm bất biến:** Ground Truth chỉ từ **điều kiện vật lý chủ động áp đặt** có metadata, không lấy từ FFT/model. **Engineering Validation Data luôn tách biệt với Dataset v0.1**, kể cả khi fixture đã freeze.

## 5. Human Gate và sửa lỗi

Human Gate bắt buộc khi ảnh hưởng an toàn cơ khí, nguồn/dòng/điện áp, Ground Truth, protocol thí nghiệm, inclusion/exclusion dataset, kiến trúc, scientific claim và release `main`. Không tuyên bố phần cứng an toàn chỉ nhờ file và AI review.

Khi có finding, thành viên xác minh bằng code và test rồi sửa **trên cùng feature branch**. Nếu yêu cầu Codex triển khai code, cần kiểm tra diff, quyền, test và Human Gate trước khi push/merge; tác vụ review **không tự động đồng nghĩa với tự sửa**. Có thể comment `@codex review` khi cần review thủ công nếu integration hỗ trợ; không giả định mọi lần push đều kích hoạt lại khi cấu hình chưa xác nhận.

## 6. Prompt ngắn

```text
Review PR này theo root AGENTS.md và scoped AGENTS.md.
Trả lời hoàn toàn bằng tiếng Việt, trừ code, file paths và tên kỹ thuật.
Mỗi finding ghi mức độ P1/P2, file:dòng, bằng chứng, hậu quả,
hướng sửa cụ thể và cách kiểm chứng. Không bịa kết quả đo.
Nếu liên quan an toàn phần cứng/nhãn/dataset, nêu yêu cầu Human Gate.
```

Xem thêm `CODEX_REVIEW_PROMPTS.md` và `NCKH_TEAM_SUBMISSION_WORKFLOW_V3.md`.
