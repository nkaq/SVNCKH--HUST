# NCKH — Research Integrity, Data & Reproducibility Standard (v1)

> Hướng dẫn xác minh luận điểm nghiên cứu. Dùng khi sửa `experiments/`, `data/`, `ml/`, `results/`, `docs/reports/` và khi PR ảnh hưởng kết luận khoa học.

## 1. Phân loại bằng chứng và trạng thái phê duyệt

Mỗi phát biểu nghiên cứu phải phân biệt độc lập:

1. `evidence_type`: nguồn gốc và bản chất của bằng chứng.
2. `approval_status`: trạng thái phê duyệt của người có thẩm quyền.
3. `evidence_ref`: đường dẫn tới dữ liệu, log, mô phỏng hoặc tài liệu chứng minh.
4. `data_origin`: nguồn dữ liệu, nếu có liên quan.

### 1.1. Loại bằng chứng — evidence_type

| Loại | Ý nghĩa |
|---|---|
| `HYPOTHESIS` | Giả thuyết chưa được kiểm chứng. |
| `PLANNED` | Thí nghiệm hoặc công việc dự kiến thực hiện. |
| `MEASURED` | Kết quả đo thật có recording/log và metadata. |
| `SIMULATED` | Kết quả mô phỏng có model, tham số và điều kiện biên. |
| `INFERRED` | Kết quả suy luận hoặc dự đoán từ mô hình/thuật toán. |

Không được trình bày `SIMULATED` hoặc `INFERRED` như
kết quả `MEASURED`.

### 1.2. Trạng thái phê duyệt — approval_status

| Trạng thái | Ý nghĩa |
|---|---|
| `PENDING` | Chưa được người có thẩm quyền phê duyệt. |
| `APPROVED` | Đã được phê duyệt trong phạm vi xác định. |
| `REJECTED` | Không được chấp nhận. |
| `NOT_APPLICABLE` | Không thuộc trường hợp yêu cầu phê duyệt. |

Không dùng `NOT_APPLICABLE` để bỏ qua Human Gate
bắt buộc của dự án.

Phê duyệt không làm thay đổi bản chất bằng chứng.
Kết quả mô phỏng được phê duyệt vẫn là `SIMULATED`,
không tự trở thành `MEASURED`.

### 1.3. Nguồn dữ liệu — data_origin

Khi phát biểu sử dụng dữ liệu thực nghiệm, cần xác định:

- `ENGINEERING_VALIDATION`: dữ liệu kiểm thử kỹ thuật.
- `DATASET_V0_1`: dữ liệu thuộc dataset nghiên cứu đã được
  chấp nhận theo protocol và tiêu chí hiện hành.
- `NOT_APPLICABLE`: không áp dụng.

`ENGINEERING_VALIDATION` luôn tách biệt với `DATASET_V0_1`.
Không tự tái phân loại recording validation thành dataset
sau khi fixture được nghiệm thu hoặc freeze.

### 1.4. Ví dụ truy vết

**Trường hợp A — Mô phỏng đã được phê duyệt**

- `evidence_type: SIMULATED`
- `approval_status: APPROVED`
- `data_origin: NOT_APPLICABLE`
- `evidence_ref`: mô hình, cấu hình và kết quả mô phỏng thực tế.

**Trường hợp B — Kết quả đo thật chưa phê duyệt**

- `evidence_type: MEASURED`
- `approval_status: PENDING`
- `data_origin: ENGINEERING_VALIDATION`
- `evidence_ref`: recording, log và metadata thực tế.

Các ví dụ trên chỉ minh họa cách phân loại,
không khẳng định dự án đã có kết quả đo hoặc mô phỏng tương ứng.

Nếu không có bằng chứng, không tự tạo số liệu,
không tự gán `APPROVED` và không tuyên bố `PASS`.
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
