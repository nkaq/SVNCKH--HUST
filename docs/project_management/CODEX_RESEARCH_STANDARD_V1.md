# NCKH — Research Integrity, Data & Reproducibility Standard (v1)

> Hướng dẫn xác minh luận điểm nghiên cứu. Dùng khi sửa `experiments/`, `data/`, `ml/`, `results/`, `docs/reports/` và khi PR ảnh hưởng kết luận khoa học.

## 1. Phân tầng bằng chứng

Mỗi phát biểu kỹ thuật phải được gắn một loại sau:

| Trạng thái | Ví dụ diễn đạt đúng |
|---|---|
| `HYPOTHESIS` | "Có giả thuyết rằng sensor mounting ảnh hưởng phổ rung." |
| `PLANNED` | "Dự kiến kiểm tra 3 setup với protocol X." |
| `ENGINEERING_VALIDATION` | "Log kỹ thuật này đo trên prototype, chưa thuộc Dataset v0.1." |
| `MEASURED` | "Theo recording/log được dẫn nguồn, giá trị đo là ..." |
| `SIMULATED` | "ANSYS/MATLAB theo model và boundary condition ..., không phải bench test." |
| `INFERRED` | "Dự đoán model trên split ..., không phải Ground Truth." |
| `APPROVED_CLAIM` | "ME1/domain owner phê duyệt đối chiếu chứng cứ và giới hạn." |

Nếu không có nguồn evidence, **không tự chọn số hoặc nêu đã PASS**.

## 2. Ground Truth và fault labels

- Nhãn lỗi đến từ **điều kiện vật lý được chủ động thiết lập/áp đặt** và ghi trong protocol/metadata: motor, fixture/setup, fault type, severity nếu được xác nhận, RPM/load, time, người lập và version.
- Không gán/relabel dựa trên FFT, RMS, dự đoán mô hình, dashboard hoặc bất kỳ feature nào.
- Không tự gộp các condition `looseness`, `misalignment`, `imbalance`, `healthy` vào tập train trước khi protocol và phạm vi dataset được phê duyệt.
- Thay đổi fault mechanism, mounting hoặc fixture phải ghi setup/domain version; không trộn các setup khác nhau như cùng một cấu hình.

## 3. Engineering Validation Data và Dataset v0.1

- **Hai nguồn này luôn tách biệt về nguồn gốc và manifest**; không tự tái phân loại recording validation thành dataset sau khi fixture acceptance.
- Fixture Acceptance Test PASS và fixture/setup freeze là **điều kiện cần**, nhưng không tự đủ để xác nhận chất lượng của một recording.
- Dataset v0.1 cần recording mới sau freeze, physical condition verified, recording_id, sensor/timing config, firmware/parser version, QC, metadata, approval. Nếu thiếu, giữ ngoài tập nghiên cứu chính và ghi lý do.
- Không lưu raw/private data lớn hoặc secret trong Git. Sample dữ liệu chỉ được commit khi đúng data policy và đủ provenance.

## 4. Truy vết recording và QC

Gợi ý trường cho một log/manifest; **không coi đây là schema đã khóa**:

```text
recording_id, timestamp source, motor_id, fixture_version, setup_version,
physical_condition, severity (if approved), rpm/load, sample rate,
sensor mounting/config, firmware_version, acquisition/parser_version,
recording_duration, sample_count, dropped_samples, error counters,
QC decision + criteria version, reviewer, evidence references
```

Đọc schema thật tại `experiments/metadata/metadata_schema_example.json` và protocol hiện hành. File ví dụ không có nghĩa là đầy đủ hoặc đã chốt contract.

## 5. Đánh giá ML và drift đúng khoa học

- Split độc lập theo recording/setup/motor/time khi cần đánh giá generalization; báo lý do thiết kế split và nhóm bị giữ lại.
- Window có overlap không được gây train-test contamination; preprocessing/feature selection chọn bằng train và validation chứ không fit test.
- Phân biệt **sensor drift**, random noise, temperature effect, mounting change, operating-condition shift, model drift và domain shift; không gọi proxy là measured real drift.
- Mỗi kết quả phải kèm dataset version, code revision, split ID, seed, config, metric definition và hạn chế.
- Adaptive fusion claim phải có baseline, failure/drift scenario được mô tả, ablation phù hợp, cost/latency/RAM/Flash nếu nói "edge feasible". Không suy ra tính khả thi trên ESP32-S3 từ kết quả PC.

## 6. Research review gate

PR ảnh hưởng Ground Truth, dataset inclusion, experiment design hoặc scientific claim phải hỏi ME1 và domain owner. Codex chỉ được:

```text
Read → check traceability → compare with approved protocol
→ flag uncertainty → propose tests → draft report
```

Codex không được xác nhận thay người đo, không cấp nhãn thực nghiệm, không ký PASS của Fixture Acceptance hoặc Dataset Release.
