# NCKH — Research Integrity, Data & Reproducibility Standard (v1)

> Hướng dẫn xác minh luận điểm nghiên cứu. Dùng khi sửa `experiments/`, `data/`, `ml/`, `results/`, `docs/reports/` và khi PR ảnh hưởng kết luận khoa học.

## 1. Phân loại bằng chứng, nguồn recording và trạng thái phê duyệt

**Ba chiều độc lập, không được thay thế cho nhau:**

1. `evidence_type`: bản chất của một phát biểu (đo thật, mô phỏng, suy luận...).
2. `data_origin`: **mục đích/nguồn thu ban đầu** của recording, bất biến sau khi tạo; không phải nhãn thành viên dataset.
3. `inclusion_status`: trạng thái xét đưa recording vào một phiên bản dataset; có lịch sử quyết định và không thay đổi `data_origin`.

Ngoài ra, `approval_status` biểu diễn phê duyệt **một đối tượng và phạm vi cụ thể** (claim hoặc quyết định dataset); `evidence_ref` chỉ đến bằng chứng thật. Những trường dưới đây là **quy ước truy vết/review**, không tự thay thế schema hay protocol đã được phê duyệt.

### 1.1. Bản chất bằng chứng — `evidence_type`

| Giá trị | Ý nghĩa |
|---|---|
| `HYPOTHESIS` | Giả thuyết chưa kiểm chứng. |
| `PLANNED` | Thử nghiệm/công việc dự kiến, chưa thực hiện. |
| `MEASURED` | Kết quả đo thật có recording/log và metadata có thể kiểm tra. |
| `SIMULATED` | Mô phỏng có model, điều kiện biên, cấu hình và phiên bản. |
| `INFERRED` | Kết quả suy luận/dự đoán của mô hình; không phải Ground Truth. |

`SIMULATED` và `INFERRED` không được trình bày thành `MEASURED`. Phê duyệt một claim không được đổi `evidence_type`, cũng không chứng minh rằng mô phỏng đã được đo trên thiết bị thật.

### 1.2. Trạng thái phê duyệt và hồ sơ Human Gate

| `approval_status` | Ý nghĩa |
|---|---|
| `PENDING` | Chưa có phê duyệt hợp lệ cho đối tượng/phạm vi đang xét. |
| `APPROVED` | Có quyết định phê duyệt **có thể truy vết và đúng phạm vi**. |
| `REJECTED` | Có quyết định không chấp nhận, giữ lý do/bằng chứng. |
| `NOT_APPLICABLE` | Không thuộc trường hợp cần phê duyệt; **không** được dùng để bỏ qua Human Gate. |

**Điều kiện bắt buộc để chấp nhận `APPROVED`:** phải có hồ sơ phê duyệt thật, ít nhất gồm `approver` (danh tính và vai trò có thẩm quyền), `approval_scope` (claim/recording/manifest, ID và phiên bản được duyệt), `approved_at` (thời điểm có múi giờ) và `approval_ref` (đường dẫn/ID đến biên bản hoặc quyết định kiểm chứng được). Nếu hồ sơ đặt trong một tài liệu khác, các trường trên có thể nằm trong hồ sơ **được tham chiếu**; không nhất thiết phải chép vào từng bản ghi. Reviewer phải kiểm tra tham chiếu tồn tại, người duyệt đúng thẩm quyền và phạm vi/version khớp. Thiếu một trong các điều kiện này thì **không công nhận `APPROVED`**, ghi thiếu bằng chứng và yêu cầu owner bổ sung; không tự sinh hồ sơ hoặc tự phê duyệt.

Với dataset, quyết định `approval_scope` phải chỉ rõ **dataset inclusion/release**, recording ID (hoặc manifest ID có danh sách recording) và dataset version; approval cho một mô phỏng/claim khác **không** thay thế approval đưa recording vào dataset. Các quyết định, ngày giờ và reference cần giữ được lịch sử/audit, không ghi đè làm mất quyết định cũ.

### 1.3. Nguồn thu bất biến — `data_origin`

| Giá trị | Ý nghĩa |
|---|---|
| `ENGINEERING_VALIDATION` | Recording tạo ra để kiểm thử kỹ thuật/prototype; **không đủ điều kiện chuyển thành Dataset v0.1**, ngay cả sau freeze. |
| `RESEARCH_COLLECTION` | Recording **mới**, chủ đích thu theo protocol nghiên cứu đã phê duyệt sau Fixture Acceptance PASS và freeze setup/fixture; vẫn chỉ là ứng viên cho tới khi QC và Human Gate hoàn tất. |
| `NOT_APPLICABLE` | Phát biểu không gắn với một recording thực nghiệm cụ thể. |

Gán `data_origin` theo provenance **tại thời điểm thu**, lưu recording ID, thời điểm, protocol và fixture/setup version. Không gán `DATASET_V0_1` cho `data_origin`: đó là **tư cách thành viên dataset**, không phải nguồn gốc. Không được biến `ENGINEERING_VALIDATION` thành `RESEARCH_COLLECTION` vì QC PASS, fixture freeze, rename, sao chép hoặc ME1 phê duyệt về sau. Nếu thiếu bằng chứng thời điểm/nguồn thu, đánh dấu chưa xác minh và giữ recording **ngoài dataset**, không suy đoán nhãn có lợi.

### 1.4. Trạng thái xét vào dataset — `inclusion_status`

| Giá trị | Khi nào dùng |
|---|---|
| `NOT_ELIGIBLE` | `ENGINEERING_VALIDATION` hoặc recording không đáp ứng điều kiện nguồn thu; tuyệt đối không chuyển thành `INCLUDED` bằng cách đổi provenance. |
| `PENDING` | `RESEARCH_COLLECTION` đang chờ QC, metadata, kiểm tra Ground Truth và/hoặc Human Gate. |
| `EXCLUDED` | `RESEARCH_COLLECTION` đã bị loại, có lý do, tiêu chí QC/protocol và tham chiếu quyết định. |
| `INCLUDED` | `RESEARCH_COLLECTION` đạt đủ điều kiện, có approval đúng phạm vi và manifest thành viên ở dataset version xác định. |
| `NOT_APPLICABLE` | Không áp dụng cho phát biểu không xét recording vào dataset. |

`PENDING` và `EXCLUDED` **không có tư cách thành viên Dataset v0.1**. Trường `target_dataset_version` có thể ghi mục tiêu xét duyệt khi còn `PENDING/EXCLUDED`, nhưng **chỉ** `INCLUDED` mới được ghi `included_dataset_version` và xuất hiện trong manifest dataset đã phê duyệt. `QC PASS` là điều kiện kỹ thuật, **không tự tương đương** `INCLUDED` hoặc `APPROVED`.

Để chuyển `PENDING → INCLUDED` phải kiểm tra: (i) recording mới có nguồn thu nghiên cứu đúng protocol **sau** Fixture Acceptance PASS/freeze; (ii) physical Ground Truth được chủ động áp đặt và xác minh; (iii) metadata, timing/sensor/config/version đầy đủ; (iv) QC đạt theo phiên bản tiêu chí áp dụng; (v) hồ sơ Human Gate của ME1/domain owner có `approval_scope` đúng recording/manifest và dataset version; (vi) manifest dataset ghi lại quyết định và evidence. Nếu bất kỳ điều kiện nào không đạt, tiếp tục `PENDING` hoặc chuyển `EXCLUDED` có lý do, không tự hợp thức hóa bằng lời mô tả. Xét lại `EXCLUDED` phải có review/approval và lịch sử quyết định mới; không xóa quyết định cũ.

### 1.5. Tình huống kiểm chứng dành cho reviewer (giả định, không phải kết quả dự án)

| Trường hợp | `evidence_type` | `data_origin` | `inclusion_status` | Kết luận review |
|---|---|---|---|---|
| A. Engineering Validation đo thật, kể cả QC PASS sau freeze | `MEASURED` | `ENGINEERING_VALIDATION` | `NOT_ELIGIBLE` | Không được đưa vào Dataset v0.1. |
| B. Recording nghiên cứu mới, chờ QC/duyệt | `MEASURED` | `RESEARCH_COLLECTION` | `PENDING` | Chưa phải thành viên dataset. |
| C. Recording nghiên cứu không đạt QC | `MEASURED` | `RESEARCH_COLLECTION` | `EXCLUDED` | Ghi rõ lý do/QC ref; không vào manifest. |
| D. Recording nghiên cứu đạt QC, GT, metadata; có approval đúng scope | `MEASURED` | `RESEARCH_COLLECTION` | `INCLUDED` | Chỉ hợp lệ nếu có `included_dataset_version`, manifest và approval record kiểm chứng được. |
| E. Claim mô phỏng được người có thẩm quyền duyệt | `SIMULATED` | `NOT_APPLICABLE` | `NOT_APPLICABLE` | Vẫn là `SIMULATED`; `APPROVED` đòi đủ hồ sơ phê duyệt. |
| F. Claim/recording ghi `APPROVED` nhưng thiếu `approval_ref`, approver, scope hoặc time | Theo bằng chứng | Giữ nguyên | Không tự đổi thành `INCLUDED` | **Không công nhận `APPROVED`**; yêu cầu hồ sơ thật. |
| G. Recording `INCLUDED` nhưng chỉ có approval cho claim khác hoặc QC PASS | `MEASURED` | `RESEARCH_COLLECTION` | Không hợp lệ | Yêu cầu approval dataset đúng phạm vi trước khi công nhận. |

Với các trường hợp A–G, chỉ dùng **placeholder mô tả logic**. Không bịa `recording_id`, số đo, `approval_ref`, thời điểm hay danh tính người phê duyệt để làm như đã có evidence. Nếu không có bằng chứng, không tự gán PASS hoặc phê duyệt.

## 2. Ground Truth và fault labels

- Nhãn lỗi đến từ **điều kiện vật lý được chủ động thiết lập/áp đặt** và ghi trong protocol/metadata: motor, fixture/setup, fault type, severity nếu được xác nhận, RPM/load, time, người lập và version.
- Không gán/relabel dựa trên FFT, RMS, dự đoán mô hình, dashboard hoặc bất kỳ feature nào.
- Không tự gộp các condition `looseness`, `misalignment`, `imbalance`, `healthy` vào tập train trước khi protocol và phạm vi dataset được phê duyệt.
- Thay đổi fault mechanism, mounting hoặc fixture phải ghi setup/domain version; không trộn các setup khác nhau như cùng một cấu hình.

## 3. Engineering Validation Data và Dataset v0.1

- **Engineering Validation Data và nguồn thu nghiên cứu luôn tách biệt về provenance và manifest**; không tự tái phân loại hoặc trộn recording validation vào Dataset v0.1 sau fixture acceptance/freeze, kể cả khi QC PASS.
- Fixture Acceptance Test PASS và fixture/setup freeze là **điều kiện cần để bắt đầu thu recording nghiên cứu mới**, nhưng không tự chứng minh chất lượng, tính hợp lệ của Ground Truth hoặc tư cách thành viên dataset.
- Dataset v0.1 chỉ xét recording **mới** thu theo protocol sau acceptance/freeze, physical condition verified, recording_id, sensor/timing config, firmware/parser version, metadata và QC thực tế. Phải có **Human Gate phê duyệt inclusion đúng recording/manifest và phiên bản dataset**, kèm approval record kiểm chứng được, trước khi đánh dấu `INCLUDED`.
- Ghi `data_origin` bất biến, `inclusion_status` và lịch sử quyết định riêng theo mục 1.3–1.4. Recording đang `PENDING` hoặc `EXCLUDED` chỉ là ứng viên/recording không được nhận, không phải Dataset v0.1. Không đổi nguồn gốc để né điều kiện QC hoặc phê duyệt.
- Không lưu raw/private data lớn hoặc secret trong Git. Sample dữ liệu chỉ được commit khi đúng data policy và đủ provenance.

## 4. Truy vết recording và QC

Gợi ý trường cho một log/manifest; **không coi đây là schema đã khóa**:

```text
recording_id, timestamp source, motor_id, fixture_version, setup_version,
physical_condition, severity (if approved), rpm/load, sample rate,
sensor mounting/config, firmware_version, acquisition/parser_version,
recording_duration, sample_count, dropped_samples, error counters,
QC decision + criteria version, reviewer, evidence references,
data_origin + acquisition intent/protocol + recorded_at,
inclusion_status + target_dataset_version (nếu ứng viên),
included_dataset_version + dataset_manifest_ref (chỉ khi INCLUDED),
approval_status + approver + approval_scope + approved_at + approval_ref (khi APPROVED),
inclusion decision/reason/review history references
```

`qc_status: PASS` (nếu có) chỉ chứng minh điều kiện QC được ghi, **không** thay thế `inclusion_status: INCLUDED` hoặc Human Gate. Các trường bổ sung là **đề xuất truy vết khái niệm**; không tự thay đổi schema JSON, parser, manifest hay contract đang dùng. Đọc schema ví dụ tại `experiments/metadata/metadata_schema_example.json` và protocol đang được phê duyệt; nếu cần hiện thực các trường mới, phải có task/version/migration riêng được ME1 và owner liên quan duyệt. File ví dụ không có nghĩa là đầy đủ hoặc đã chốt contract.

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
