# NCKH — PROJECT MASTER CONTEXT
## Adaptive Multimodal TinyML for Drift-Robust Predictive Maintenance on Resource-Constrained Edge Microcontrollers

> Vai trò của file: nguồn ngữ cảnh chung cho toàn bộ các box chat trong Project NCKH.
> Mỗi box thành viên phải ưu tiên nội dung trong file này và các PROJECT UPDATE mới nhất.

## 1. QUY TẮC ƯU TIÊN
1. Lệnh mới nhất của ME1/trưởng nhóm có ưu tiên cao nhất.
2. Nếu lệnh mới mâu thuẫn kế hoạch cũ, dùng lệnh mới và ghi rõ phần bị thay thế.
3. Không tự ý đổi MCU, cảm biến, kiến trúc, fault set hoặc phân công.
4. Không giao lại nhiệm vụ Week 1/Week 2 như nhiệm vụ mới nếu không có mục tiêu nâng cấp rõ ràng.
5. Mọi phần việc phải nối được vào chuỗi:
Motor/Testbench → Fault → Physical Response → Multimodal Sensing → Acquisition → Raw Data → Feature/Representation → TinyML → Drift/Reliability → Adaptive Fusion → Edge Decision → Predictive Maintenance.

## 2. CẤU HÌNH DỰ ÁN ĐANG KHÓA
- MCU chính: ESP32-S3.
- Accelerometer chính: ADXL345.
- Không dùng ADXL355 trừ khi trưởng nhóm chủ động đổi.
- Modalities mục tiêu: vibration + current + temperature + RPM/load/context.
- ADXL345 có thể dùng I2C trong prototype/engineering validation.
- Dataset chính chưa thu cho tới khi fixture/testbench cơ khí ổn định, lặp lại được và được freeze version.
- Dữ liệu trước khi fixture freeze: Engineering Validation Data.
- Engineering Validation Data không trộn trực tiếp vào Dataset v0.1.
- Dataset v0.1 dự kiến bắt đầu với Healthy + Imbalance + Misalignment trước.

## 3. PHÂN VAI CỐ ĐỊNH
### ME1 — Mechatronics / Team Lead
System Architecture, Mechanical Testbench, Motor Physics, Integration, Ground Truth.
- Chốt kiến trúc toàn hệ thống.
- Thiết kế testbench, fixture, fault mechanism, Ground Truth, experiment matrix.
- Kiểm tra Physics ↔ Sensor ↔ Acquisition ↔ Data ↔ ML.
- Quản lý version fixture/dataset/experiment.

### MS2 — MEMS / Semiconductor
ADXL345, MEMS physics, calibration, sensor health, drift/reliability.
- Bias/noise/sensitivity/repeatability.
- Phân biệt noise, bias, sensitivity error, drift, acquisition artifact.
- Tạo sensor-health evidence phục vụ drift/reliability.

### EE2 — Automation / Embedded Acquisition
ESP32-S3 firmware, timing, buffer, synchronization, logging, RPM/control.
- Acquisition ổn định.
- Timestamp, sample count, dropped sample, recovery.
- Driver/interrupt/FIFO/buffer/state machine.
- Data logger và kết nối testbench.

### ET1 — Electronics / Telecommunications
Electronics, signal acquisition, current/temp/RPM interfaces, power and signal integrity.
- Sensor interface chain.
- Power/ground/decoupling/connector/pull-up.
- Current/temp/RPM front-end.
- Noise/SNR/clipping/aliasing/EMI.

### IT2 — Computer Engineering / ML
Data engineering, DSP, ML/TinyML, on-device inference.
- QC, windowing, FFT/features.
- Leakage-safe evaluation.
- Baseline → TinyML → on-device DSP/inference.
- RAM/Flash/latency/resource measurements.

## 4. WEEK 1 — ĐÃ HOÀN THÀNH
Foundation Week.
- ME1: motor physics, 4 fault, rotational frequency/harmonics, System Architecture v0.1.
- MS2: MEMS/ADXL345 fundamentals, sensing mechanism, noise/bias/drift.
- EE2/ET1: MCU/signal acquisition foundations, SPI/I2C/UART/ADC/PWM, ESP32-S3 basics.
- IT2: signal/window/feature/ML/TinyML fundamentals, FFT/features, leakage awareness.
Không giao lại phần lý thuyết cơ bản này như output chính của Week 3.

## 5. WEEK 2 — ĐÃ HOÀN THÀNH
System Characterization & First Experiments.
- ME1: Fault Specification, Experimental Matrix, Ground Truth/Recording Protocol, Architecture v0.2, Physics-vs-Data Review.
- MS2: static/orientation/temperature/repeatability characterization; y(t)=S(t)x(t)+b(t)+n(t); drift evidence/proxy.
- EE2: acquisition stability, timestamp, buffer, dropped sample, synchronization, logging/testbench routine.
- ET1: current/temp characterization, signal quality, sampling/filter recommendation.
- IT2: dataset schema, QC, windowing, FFT/features, leakage-safe split, baseline ML.
Week 3 không lặp lại characterization Week 2 nếu không có mục tiêu nâng cấp.

## 6. WEEK 3 — PROTOTYPE INTEGRATION & MECHANICAL TESTBENCH BUILD

### ME1 — nhiệm vụ đang giao
1. Đo kích thước motor/testbench, chốt design requirements.
2. Thiết kế fixture trên SolidWorks:
   - base plate;
   - adjustable motor mount;
   - slot/rail để phù hợp nhiều motor;
   - shaft/coupling clearance;
   - vị trí ADXL345 cố định;
   - vị trí current/temp/RPM sensors;
   - cable routing;
   - safety/guarding;
   - cơ cấu tạo imbalance/misalignment/looseness có kiểm soát.
3. DFM: vật liệu, dung sai, bulong/ốc, quy trình gia công/lắp ráp, BOM, drawing.
4. Freeze Fixture Concept v0.1 trước gia công.
5. Cập nhật System Architecture v0.3 theo hardware thật.
6. Xây Interface Control fixture ↔ sensor ↔ electronics ↔ firmware.
7. Chuẩn bị Fixture Acceptance Test trước Dataset v0.1.

Deliverables:
- SolidWorks assembly.
- Part drawings.
- BOM.
- Fabrication package.
- Fixture v0.1 specification.
- System Architecture v0.3.
- Fixture Acceptance Checklist.

### MS2 — nhiệm vụ đang giao
Trọng tâm: ADXL345 Calibration & Sensor Health.
1. Xây calibration workflow từ kết quả Week 2.
2. Thực nghiệm built-in self-test nếu module hỗ trợ ổn định.
3. So sánh normal / controlled disturbed mounting / temperature-related condition khi khả thi.
4. Đề xuất Sensor Health Vector: bias, noise, self-test response, temperature...
5. Xây indicator sơ bộ cho sensor quality/reliability.
6. Chuẩn hóa Sensor Health Metadata cho EE2/IT2.

Deliverables:
- Calibration routine.
- Self-test report.
- Sensor Health Vector v0.1.
- Sensor-health acceptance rule.
- Engineering Validation raw data + plots.

### EE2 — nhiệm vụ đang giao
Trọng tâm: Acquisition Firmware v2.
1. Chuyển khỏi polling + Serial.print từng sample.
2. Tích hợp DATA_READY/interrupt khi phù hợp.
3. Khai thác FIFO ADXL345 nếu cấu hình cho phép.
4. Xây ring buffer/double buffer.
5. State machine: INIT → SELF_CHECK → READY → STABILIZE → RECORD → STOP → QC/ERROR.
6. Command protocol từ PC: START, STOP, STATUS, SET_RECORD_ID, SET_CONDITION...
7. Log sample_index, timestamp_us, sensor data, error counters.
8. Error detection/recovery: I2C error, timeout, disconnect, buffer overflow.
9. Stress test 60 s / nhiều lần.
10. Chuẩn hóa stream/file format cho IT2.

Deliverables:
- Firmware v2.
- State machine.
- Buffer/FIFO/interrupt implementation.
- Command protocol.
- Error/recovery report.
- 60 s engineering validation recordings.

### ET1 — nhiệm vụ đang giao
Trọng tâm: Sensor Interface Board & Robust Hardware Integration.
1. Thiết kế Sensor Interface Board v0.1 hoặc prototype board.
2. Chuẩn hóa 3.3 V/5 V rails, ground, decoupling, I2C pull-up, connector pinout, current/temp/RPM inputs.
3. Kiểm tra power integrity và wiring khi motor OFF/ON.
4. So sánh communication/signal behavior với routing khác nhau.
5. Xây checklist EMI/grounding/cable routing cho fixture ME1.
6. Chuẩn bị sơ đồ đấu dây chính thức.
7. Tạo fault-safe wiring.

Deliverables:
- Interface Board/Prototype v0.1.
- Schematic/wiring diagram.
- Connector/pinout table.
- Power/ground/decoupling note.
- EMI/routing checklist.
- Harness concept.

### IT2 — nhiệm vụ đang giao
Trọng tâm: On-device DSP & Live Engineering Monitor.
1. Nhận Engineering Validation Data từ EE2/MS2.
2. Viết PC reference pipeline: RMS, mean, std, peak, crest factor, selected FFT/Goertzel.
3. Port tập feature tối thiểu xuống ESP32-S3.
4. Benchmark execution time, RAM, Flash, window size.
5. Làm live monitor/dashboard: vibration time-domain, RMS, dominant frequency, sensor health/status.
6. Parser tương thích firmware EE2.
7. So sánh PC feature vs ESP32 feature.
8. Chưa tối ưu CNN/TinyML nặng nếu acquisition/feature equivalence chưa PASS.

Deliverables:
- PC reference DSP.
- ESP32-S3 DSP prototype.
- Feature equivalence report.
- Latency/RAM/Flash benchmark.
- Live dashboard.
- Parser/visualization tool.

## 7. INTEGRATION GOAL CUỐI WEEK 3
Edge Condition Monitor v0.1:
ADXL345/current/temperature/RPM
→ Sensor interface
→ ESP32-S3 Acquisition Firmware v2
→ Sensor Health + QC
→ On-device DSP
→ PC Live Dashboard.

Đây là integration demo, chưa phải final fault classifier.

## 8. QUY TẮC DATASET
- Week 3: Engineering Validation Data.
- Không gọi là Dataset v0.1 nếu fixture chưa freeze.
- Sau gia công:
  Fixture Acceptance Test → PASS → freeze fixture version → Dataset v0.1.
- Nếu fixture/mounting/sensor position thay đổi sau khi dataset bắt đầu:
  tăng setup_version và không trộn như cùng một domain.

## 9. ROUTING THEO BOX CHAT
- Box ME1: ưu tiên ME1; chỉ nhắc thành viên khác khi có dependency.
- Box MS2: mặc định dùng nhiệm vụ Week 3 của MS2 ở trên.
- Box EE2: mặc định dùng nhiệm vụ Week 3 của EE2 ở trên.
- Box ET1: mặc định dùng nhiệm vụ Week 3 của ET1 ở trên.
- Box IT2: mặc định dùng nhiệm vụ Week 3 của IT2 ở trên.

Từ khóa mặc định:
- "việc tuần này" = Week 3 nếu không nói khác.
- "dự án" = đề tài NCKH này.
- "sensor rung" = ADXL345.
- "MCU" = ESP32-S3.
- "dataset chính" ≠ Engineering Validation Data.
- "bệ/gá/fixture" = adjustable motor fixture do ME1 thiết kế.

## 10. GIAO THỨC PROJECT UPDATE
Mỗi thay đổi lớn dùng format:

[PROJECT UPDATE]
Week:
Member:
Task changed:
New deliverable:
Dependency:
Status:
Effective from:

PROJECT UPDATE mới nhất luôn ghi đè phần thông tin cũ liên quan.

## 11. NGUYÊN TẮC TRẢ LỜI
- Ưu tiên thực hành, đo, code, CAD, hardware, data thật.
- Không bịa kết quả thí nghiệm.
- Phân biệt hypothesis với measured result.
- Phân biệt sensor drift với noise, operating-condition shift và mounting/domain shift.
- Ground Truth phải đến từ condition vật lý, không suy ngược từ FFT/model prediction.
- Mỗi nhiệm vụ phải có output kiểm chứng được: CAD, firmware, schematic, CSV, plot, script, benchmark, report hoặc checklist.
