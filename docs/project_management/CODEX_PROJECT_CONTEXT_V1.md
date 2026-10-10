# NCKH — Codex Project Context & Source Map (v1)

> Mục đích: bản đồ ngữ cảnh, không thay thế các tài liệu chuyên môn đã được chấp thuận. Đọc theo **mức độ liên quan với task**; không tải tất cả tài liệu cho một chỉnh sửa nhỏ.

## 1. Mục tiêu nghiên cứu

**Adaptive Multimodal TinyML for Drift-Robust Predictive Maintenance on Resource-Constrained Edge Microcontrollers**.

Mục tiêu gần: condition monitoring/fault diagnosis trên ESP32-S3, có đánh giá sensor reliability, nhận diện drift/shift có bằng chứng, fusion thích ứng và đo chi phí triển khai edge. Prognosis/RUL là hướng tương lai, **không tự coi là output hiện tại**.

Chuỗi hệ thống và giả thuyết nghiên cứu:

```text
Motor + fixture + physical condition (Ground Truth)
  → multimodal sensors & signal conditioning
  → ESP32-S3 acquisition + timestamp + health/QC
  → engineering validation / approved dataset with provenance
  → DSP/features / PC reference / edge equivalence
  → baseline ML / TinyML and deployment cost
  → sensor reliability, drift/shift detection
  → adaptive fusion and robust evaluation
  → integration evidence / research claims
```

Chuỗi trên mô tả **mục tiêu/roadmap**, không khẳng định mọi giai đoạn đã được triển khai hoặc xác nhận.

## 2. Cấu hình đã khóa

- ESP32-S3 (MCU); ADXL345 XYZ (vibration); ACS724 (current).
- PT100 + MAX31865 (motor temperature); US5881 Hall + magnet (RPM).
- DHT22 (ambient context); OLED (status/display).
- Vibration, current và motor temperature là các modality chính; RPM là reference/operating condition, DHT22 là environmental context.
- Không tự thay ADXL345 thành ADXL355, tự đổi MCU, sensor, sampling spec hoặc kiến trúc.
- Ground Truth đến từ tình trạng vật lý **được chủ động áp đặt theo protocol**, có truy vết; không lấy từ FFT/model output.
- Engineering Validation Data và Dataset v0.1 là **hai tập tách biệt**. Không tái phân loại hoặc trộn recording validation vào dataset sau acceptance.
- Kịch bản lỗi, mức độ lỗi và lớp dataset chỉ chốt theo protocol/experiment matrix và phiên bản được phê duyệt. Không suy nhãn chỉ từ README tổng quan.

## 3. Trạng thái dự án phải tra cứu tại nguồn sống

- `docs/plans/weekXX/README.md` trên `dev`: nguồn giao nhiệm vụ chính thức **theo tuần** do ME1 cập nhật. Chọn **tuần đang triển khai** theo tài liệu và thông báo mới nhất; không mặc định Week 3 mãi mãi.
- `docs/project_management/NCKH_PROJECT_MASTER_CONTEXT_FULL.md`: nền tảng nghiên cứu, vai trò, quy tắc khoa học, roadmap theo snapshot (có thể cũ hơn kế hoạch tuần).
- `docs/project_management/NCKH_PROJECT_MASTER_CONTEXT.md`: bản tóm tắt đã được lưu; có thể thiếu chi tiết.
- `README.md`: mục tiêu chung, kiến trúc khái niệm và các milestone.
- `docs/architecture/`: tài liệu system/interface theo phiên bản được ME1 chấp thuận. Không coi hình khái niệm là wiring thực tế.
- `experiments/protocols/`, `experiments/metadata/`: Ground Truth, recording protocol, metadata và QC nếu đã được phê duyệt.
- `AGENTS.md` và các scoped `AGENTS.md`: giới hạn hành vi và chuẩn review của Codex.
- `.github/CODEOWNERS`, `.github/PULL_REQUEST_TEMPLATE.md`: chủ sở hữu code và bằng chứng cần khi nộp PR.

**Khi nguồn mâu thuẫn:** không tự chọn giá trị thuận tiện. Nêu rõ hai nguồn/path/version, mức ảnh hưởng và xin ME1 hoặc subsystem owner xác nhận. Quy tắc bảo vệ dữ liệu, Ground Truth, bảo mật và an toàn không tự bị vô hiệu bởi một task không rõ ràng.

## 4. Team và ownership

| Vai trò | Trách nhiệm kỹ thuật | Phạm vi chính/phối hợp |
|---|---|---|
| ME1 | System architecture, fixture, motor physics, experiment protocol, integration | `hardware/mechanical/`, `docs/architecture/`, `experiments/protocols/`, `hardware/bom/` |
| MS2 | ADXL345, MEMS/calibration, sensor health/reliability | `experiments/sensor_characterization/`, `results/`, phối hợp `ml/drift_detection/` |
| EE2 | ESP32-S3 drivers, acquisition timing, FIFO/buffer, logging, errors | `firmware/esp32s3/`, phối hợp `software/serial_logger/` |
| ET1 | Sensor interface, power, pinout, EMI, wiring | `hardware/electronics/`, phối hợp `hardware/bom/` |
| IT2 | Data, parser, DSP/features, ML/TinyML, dashboard | `ml/`, `software/`, `data/`, `results/`; phối hợp MS2/EE2 |

Danh mục owner chi tiết và phối hợp phải đối chiếu `.github/CODEOWNERS`; người tạo PR không thể tự duyệt chính PR mình.

## 5. Cổng trưởng thành (không được nhảy cóc)

1. **Engineering validation:** cảm biến, timestamp, dropped-sample/error reporting, parser và testbench có bằng chứng thực tế.
2. **Fixture acceptance:** chạy checklist; ME1 xác nhận PASS, khóa fixture/setup version và vị trí gắn cảm biến.
3. **Dataset v0.1:** chỉ recording mới sau acceptance/freeze, đủ physical labels, metadata, QC và phê duyệt; tuyệt đối không tự chuyển dữ liệu validation thành dataset.
4. **ML/TinyML:** baseline tái lập được, chống leakage theo recording/setup, benchmark có thiết bị và cấu hình đo.
5. **Drift/fusion:** phân biệt sensor drift, operating-condition shift, mounting/domain shift; chỉ khẳng định những gì đã chứng minh.
6. **Research claim:** báo cáo hypothesis/plan/proxy/measured result rõ ràng, có phương pháp, evidence và hạn chế.

**Không được bịa acceptance threshold, target sampling rate, pin assignment, accuracy, latency, RAM/Flash/energy, device safety, số mẫu hoặc kết quả đo.** Những giá trị chưa được xác minh phải là `TBD — owner xác nhận`.

## 6. Quy tắc làm việc với tài liệu lớn

- Nhận task firmware → đọc root + `firmware/AGENTS.md`, contract acquisition hiện hành, tài liệu sensor liên quan.
- Nhận task DSP/ML → đọc root + `ml/AGENTS.md`, dataset provenance, split policy, feature schema và evidence.
- Nhận task experiment → đọc root + `experiments/AGENTS.md`, protocol, metadata, fixture/setup versions.
- Nhận task mechanical/electronics → đọc root + scoped `AGENTS.md`, drawing/pinout, và chờ human validation khi cần.
- Task liên phân hệ → đọc các interface giữa **những subsystem thực sự bị thay đổi**; không mặc định toàn bộ repo đã có contract bất biến.
