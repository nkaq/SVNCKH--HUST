# NCKH — MEMBER WORKSPACE & FILE TYPE GUIDE V3
## Phân chia thư mục làm việc và định dạng file cho ME1 / MS2 / EE2 / ET1 / IT2

> **Mục đích:** quy định rõ mỗi thành viên làm việc ở đâu trong repository V3, loại artifact nào thuộc trách nhiệm của ai, định dạng file nào nên dùng, và file nào cần chuyển sang subsystem khác trước khi merge.
>
> File này **không thay thế** `README.md`, `AGENTS.md`, `CONTRIBUTING.md` hoặc hướng dẫn Git. Đây là tài liệu phân quyền công việc theo subsystem.

---

# 1. NGUYÊN TẮC CHUNG

Repository tổ chức theo:

```text
SUBSYSTEM
```

không tổ chức theo:

```text
TÊN THÀNH VIÊN
```

Vì vậy:

```text
SAI:
members/IT2/code/
members/ME1/solidworks/
members/EE2/firmware/
```

Đúng:

```text
ml/
hardware/mechanical/
firmware/esp32s3/
```

Folder:

```text
members/
```

chỉ dùng cho:

- note cá nhân;
- weekly report;
- checklist;
- draft;
- file tạm chưa trở thành artifact chính thức.

---

# 2. BẢNG TỔNG QUAN

| Member | Chức năng chính | Primary folders |
|---|---|---|
| **ME1** | System Architecture / Mechanical Testbench / Ground Truth / Integration | `hardware/mechanical/`, `docs/architecture/`, `experiments/protocols/`, `hardware/bom/` |
| **MS2** | MEMS / ADXL345 / Calibration / Sensor Health / Drift | `experiments/sensor_characterization/`, `results/figures/`, `results/tables/` |
| **EE2** | ESP32-S3 Firmware / Acquisition / Timing / Logging / RPM | `firmware/esp32s3/` |
| **ET1** | Electronics / Sensor Interface / Power / EMI / Current-Temp-RPM interface | `hardware/electronics/`, `hardware/bom/` |
| **IT2** | Data / DSP / ML / TinyML / Dashboard / Visualization | `ml/`, `software/`, `data/`, `results/` |

---

# 3. ME1 — SYSTEM ARCHITECTURE / MECHANICAL / INTEGRATION

## 3.1. Primary folders

```text
hardware/mechanical/
docs/architecture/
experiments/protocols/
hardware/bom/
docs/plans/
docs/reports/
assets/system_images/
assets/diagrams/
```

## 3.2. Công việc chính

ME1 chịu trách nhiệm chính cho:

- thiết kế fixture / gá motor;
- SolidWorks parts / assembly / drawing;
- sensor mounting reference;
- coupling / load interface;
- fixture acceptance;
- system architecture;
- experimental matrix;
- Ground Truth Protocol;
- physical fault definition;
- integration giữa các subsystem;
- BOM cơ khí / tổng thể;
- version của fixture/setup.

## 3.3. Định dạng file ME1 thường dùng

### CAD source

```text
.SLDPRT
.SLDASM
.SLDDRW
```

### File trao đổi / fabrication

```text
.STEP
.STP
.DXF
.PDF
```

### Tài liệu / bảng

```text
.md
.csv
.xlsx
.pdf
```

### Hình ảnh / sơ đồ

```text
.png
.jpg
.svg
```

## 3.4. Ví dụ vị trí file

```text
hardware/mechanical/solidworks/parts/motor_plate_v01.SLDPRT

hardware/mechanical/solidworks/assemblies/
fixture_v01.SLDASM

hardware/mechanical/step/
fixture_v01.step

hardware/mechanical/dxf/
motor_plate_v01.dxf

hardware/bom/
mechanical_bom_v01.csv

docs/architecture/
system_architecture_v03.md

experiments/protocols/
ground_truth_protocol_v01.md
```

## 3.5. Không đặt ở ME1

Không đặt firmware chính tại:

```text
hardware/mechanical/
```

Không đặt ML model tại:

```text
docs/architecture/
```

Nếu ME1 viết code test tạm, phải chuyển về subsystem phù hợp trước khi merge.

---

# 4. MS2 — MEMS / ADXL345 / SENSOR HEALTH / DRIFT

## 4.1. Primary folders

```text
experiments/sensor_characterization/
results/figures/
results/tables/
docs/reports/
```

## 4.2. Shared folders khi phối hợp

Nếu MS2 có code calibration / analysis:

```text
ml/preprocessing/
ml/features/
ml/drift_detection/
```

Nếu MS2 phối hợp chỉnh driver ADXL345:

```text
firmware/esp32s3/drivers/adxl345/
```

Phần driver production vẫn cần phối hợp EE2.

## 4.3. Công việc chính

MS2 chịu trách nhiệm cho:

- ADXL345 characterization;
- static bias;
- noise;
- orientation;
- repeatability;
- temperature effect;
- self-test;
- calibration;
- Sensor Health Vector;
- drift indicators;
- phân biệt noise / bias / sensitivity / drift;
- evidence cho sensor reliability.

## 4.4. Định dạng file MS2 thường dùng

### Phân tích / script

```text
.py
.ipynb
```

### Dữ liệu nhỏ / metadata

```text
.csv
.json
.txt
```

### Báo cáo

```text
.md
.pdf
.xlsx
```

### Figure

```text
.png
.svg
.pdf
```

### Nếu phối hợp firmware ADXL345

```text
.ino
.cpp
.h
.hpp
.c
```

## 4.5. Ví dụ vị trí file

```text
experiments/sensor_characterization/
adxl345_static_bias_v01.csv

experiments/sensor_characterization/
adxl345_repeatability_v01.csv

ml/drift_detection/
sensor_health_vector_v01.py

results/figures/
adxl345_bias_vs_temperature_v01.png

results/tables/
adxl345_repeatability_summary_v01.csv

docs/reports/
ms2_sensor_characterization_v01.md
```

## 4.6. Quy tắc riêng

Không đặt label lỗi dựa vào FFT.

Không gọi một controlled proxy là:

```text
real deployment drift
```

nếu evidence chưa đủ.

---

# 5. EE2 — ESP32-S3 FIRMWARE / ACQUISITION / TIMING

## 5.1. Primary folder

```text
firmware/esp32s3/
```

## 5.2. Subfolders chính

```text
firmware/esp32s3/drivers/
firmware/esp32s3/acquisition/
firmware/esp32s3/logging/
firmware/esp32s3/dsp/
firmware/esp32s3/oled/
firmware/esp32s3/tests/
```

## 5.3. Công việc chính

EE2 chịu trách nhiệm cho:

- ESP32-S3 firmware;
- sensor drivers;
- I2C / SPI / ADC acquisition;
- timestamp;
- sample index;
- FIFO / ring buffer;
- interrupt;
- state machine;
- error counters;
- dropped-sample tracking;
- RPM acquisition;
- data logging;
- OLED status;
- firmware test;
- acquisition reliability.

## 5.4. Định dạng file EE2 thường dùng

### Arduino / C / C++

```text
.ino
.cpp
.c
.h
.hpp
```

### Config / metadata

```text
.json
.yaml
.yml
```

### Documentation

```text
.md
.txt
```

### Test log nhỏ

```text
.csv
.log
.txt
```

> Log lớn không commit trực tiếp nếu không cần thiết.

## 5.5. Ví dụ vị trí file

```text
firmware/esp32s3/drivers/adxl345/
adxl345.cpp
adxl345.h

firmware/esp32s3/drivers/max31865/
max31865.cpp
max31865.h

firmware/esp32s3/acquisition/
acquisition_manager.cpp
acquisition_manager.h

firmware/esp32s3/logging/
data_logger.cpp

firmware/esp32s3/oled/
oled_status.cpp

firmware/esp32s3/tests/
test_adxl345_fifo.ino
```

## 5.6. Không đặt ở EE2

Không để Python dashboard chính trong:

```text
firmware/
```

Dashboard thuộc:

```text
software/
```

Không lưu raw dataset lớn trong:

```text
firmware/esp32s3/tests/
```

---

# 6. ET1 — ELECTRONICS / SENSOR INTERFACE / SIGNAL INTEGRITY

## 6.1. Primary folders

```text
hardware/electronics/
hardware/bom/
```

## 6.2. Subfolders

```text
hardware/electronics/schematics/
hardware/electronics/pcb/
hardware/electronics/wiring/
hardware/electronics/interface_board/
```

## 6.3. Công việc chính

ET1 chịu trách nhiệm cho:

- sensor interface;
- power rails;
- grounding;
- connector / pinout;
- ACS724 interface;
- PT100 + MAX31865 interface;
- US5881 Hall interface;
- DHT22 interface;
- decoupling;
- pull-up;
- ADC/input-range documentation;
- EMI;
- cable routing;
- interface board / PCB nếu có.

## 6.4. Định dạng file ET1 thường dùng

### Tài liệu / pinout / BOM

```text
.md
.csv
.xlsx
.pdf
```

### Schematic / wiring export

```text
.pdf
.png
.svg
```

### Nếu dùng KiCad

```text
.kicad_pro
.kicad_sch
.kicad_pcb
```

### Nếu dùng phần mềm EDA khác

Giữ:

```text
native source file
+
PDF/PNG export
```

Không ép thành viên phải đổi phần mềm chỉ để giống đuôi file.

## 6.5. Ví dụ vị trí file

```text
hardware/electronics/schematics/
sensor_interface_v01.pdf

hardware/electronics/wiring/
esp32_sensor_pinout_v01.md

hardware/electronics/interface_board/
interface_board_v01.kicad_sch

hardware/electronics/pcb/
interface_board_v01.kicad_pcb

hardware/bom/
electronics_bom_v01.csv
```

## 6.6. Quy tắc riêng

Không chỉ ghi:

```text
"cắm được"
```

Phải lưu được:

- voltage;
- signal direction;
- connector;
- pin;
- power source;
- ground;
- verification note.

---

# 7. IT2 — DATA / DSP / ML / TINYML / DASHBOARD

## 7.1. Primary folders

```text
ml/
software/
data/
results/
```

## 7.2. Subfolders chính

```text
ml/preprocessing/
ml/features/
ml/baselines/
ml/tinyml/
ml/drift_detection/
ml/adaptive_fusion/
ml/notebooks/

software/pc_dashboard/
software/serial_logger/
software/visualization/
software/utilities/

data/sample/
data/manifests/
data/processed_sample/

results/figures/
results/tables/
results/benchmarks/
results/model_results/
```

## 7.3. Công việc chính

IT2 chịu trách nhiệm cho:

- serial parser;
- QC;
- dataset loader;
- preprocessing;
- synchronization/alignment;
- windowing;
- RMS / STD / peak / crest / kurtosis;
- FFT / band energy;
- baseline ML;
- TinyML;
- drift detection;
- adaptive fusion;
- dashboard;
- visualization;
- model/resource benchmark;
- reproducibility;
- leakage-safe split.

## 7.4. Định dạng file IT2 thường dùng

### Python

```text
.py
```

### Notebook

```text
.ipynb
```

### Config

```text
.json
.yaml
.yml
.toml
```

### Data nhỏ / manifest

```text
.csv
.json
.parquet
```

### Model / edge artifacts

```text
.tflite
.onnx
.pkl
.joblib
```

> Chỉ commit model artifact khi kích thước hợp lý và thực sự cần version-control.

### Nếu convert model sang firmware

```text
.h
.hpp
```

File model-header cuối cùng có thể cần phối hợp EE2 để đưa vào:

```text
firmware/esp32s3/
```

### Figures / report

```text
.png
.svg
.pdf
.md
```

## 7.5. Ví dụ vị trí file

```text
ml/preprocessing/
recording_loader.py

ml/features/
vibration_features.py

ml/baselines/
baseline_random_forest.py

ml/tinyml/
motor_fault_model_v01.tflite

ml/drift_detection/
drift_detector_v01.py

ml/adaptive_fusion/
reliability_fusion_v01.py

software/serial_logger/
serial_logger.py

software/pc_dashboard/
dashboard.py

results/figures/
confusion_matrix_v01.png

results/benchmarks/
esp32_resource_benchmark_v01.csv
```

## 7.6. Quy tắc riêng

Không:

```text
random-window split
```

nếu các window cùng recording có thể lọt vào cả train và test.

Không fit normalization/preprocessing bằng test set.

---

# 8. CROSS-TEAM FOLDERS

Một số folder có nhiều thành viên cùng dùng.

## `results/`

Có thể có output từ:

```text
MS2
IT2
EE2
ET1
ME1
```

nhưng mỗi result phải có tên rõ nguồn và version.

Ví dụ:

```text
adxl345_repeatability_v01.png
firmware_sampling_benchmark_v01.csv
fixture_repeatability_v01.csv
```

---

## `assets/`

Dùng cho:

```text
system_images/
experiment_photos/
diagrams/
posters/
```

Không phải nơi lưu source code.

---

## `docs/reports/`

Có thể lưu report chung.

Tên file nên:

```text
week03_me1_report.md
week03_ms2_report.md
system_integration_report_v01.md
```

---

# 9. FILE EXTENSION KHÔNG QUYẾT ĐỊNH OWNER

Ví dụ:

```text
.py
```

không có nghĩa tự động thuộc IT2.

Nếu `.py` dùng để:

```text
ADXL345 calibration
```

thì MS2 có thể là owner.

Nếu `.py` dùng để:

```text
ML pipeline
```

thì IT2 là owner.

Nếu `.py` dùng để:

```text
firmware log verification
```

thì có thể là EE2 + IT2 phối hợp.

Quyết định owner dựa vào:

```text
MỤC ĐÍCH + SUBSYSTEM
```

không chỉ dựa vào extension.

---

# 10. QUY TẮC TÊN FILE

Khuyến nghị:

```text
<subsystem>_<artifact>_vXX.<ext>
```

Ví dụ:

```text
fixture_v01.SLDASM
system_architecture_v03.md
adxl345_repeatability_v01.csv
sensor_interface_v01.pdf
acquisition_manager_v02.cpp
feature_pipeline_v01.py
```

Không dùng:

```text
final
final2
final_real
new
test123
abc
```

---

# 11. VERSIONING

Khi thay đổi đáng kể:

```text
v01
v02
v03
```

Ví dụ:

```text
fixture_v01.SLDASM
fixture_v02.SLDASM
```

Với code đã quản lý bằng Git, không cần tạo vô hạn:

```text
code_v1.py
code_v2.py
code_v3.py
```

Git đã giữ lịch sử.

Tên version file chỉ dùng khi artifact thực sự có:

```text
protocol version
fixture version
drawing release
model release
```

---

# 12. GIT LFS / FILE LỚN

Các file binary lớn như:

```text
.SLDPRT
.SLDASM
.SLDDRW
.pptx
```

được quản lý theo Git LFS của repo.

Không commit raw dataset lớn vào GitHub.

---

# 13. RAW DATA

Không lưu raw dataset lớn tại:

```text
data/raw/
```

trong GitHub.

Repository chỉ giữ:

```text
data/sample/
data/manifests/
data/processed_sample/
```

và metadata cần thiết.

---

# 14. AI / CODEX KHÔNG QUYẾT ĐỊNH OWNER

`AGENTS.md` dùng để Codex hiểu subsystem và review.

Nhưng owner cuối vẫn theo phân công:

```text
ME1 → Mechanical / Architecture / Ground Truth
MS2 → Sensor Reliability
EE2 → Embedded Firmware
ET1 → Electronics
IT2 → Data / AI / TinyML
```

---

# 15. MA TRẬN NHANH

| Artifact | Owner chính | Folder |
|---|---|---|
| SolidWorks fixture | ME1 | `hardware/mechanical/solidworks/` |
| STEP / DXF / fabrication | ME1 | `hardware/mechanical/` |
| System Architecture | ME1 | `docs/architecture/` |
| Ground Truth Protocol | ME1 | `experiments/protocols/` |
| ADXL345 characterization | MS2 | `experiments/sensor_characterization/` |
| Sensor Health analysis | MS2 | `experiments/sensor_characterization/`, `ml/drift_detection/` |
| ESP32 firmware | EE2 | `firmware/esp32s3/` |
| Sensor driver | EE2 | `firmware/esp32s3/drivers/` |
| Acquisition/logging | EE2 | `firmware/esp32s3/acquisition/`, `logging/` |
| Schematic / Wiring | ET1 | `hardware/electronics/` |
| Interface Board / PCB | ET1 | `hardware/electronics/interface_board/`, `pcb/` |
| Dataset loader / QC | IT2 | `ml/preprocessing/`, `data/` |
| DSP / features | IT2 | `ml/features/` |
| Baseline ML | IT2 | `ml/baselines/` |
| TinyML | IT2 | `ml/tinyml/` |
| Drift algorithm | IT2 + MS2 | `ml/drift_detection/` |
| Adaptive Fusion | IT2 | `ml/adaptive_fusion/` |
| Dashboard | IT2 | `software/pc_dashboard/` |
| Serial logger | IT2 + EE2 | `software/serial_logger/` |
| Research figures | owner theo nguồn | `results/figures/` |

---

# 16. KHI KHÔNG BIẾT FILE NÊN ĐỂ ĐÂU

Không tạo folder mới ngay.

Hỏi theo thứ tự:

```text
1. File này phục vụ subsystem nào?
2. Ai là owner của subsystem đó?
3. Repo đã có folder phù hợp chưa?
4. Nếu chưa có, hỏi ME1 trước khi tạo folder mới.
```

---

# 17. TÓM TẮT CHO TỪNG THÀNH VIÊN

## ME1

```text
hardware/mechanical/
docs/architecture/
experiments/protocols/
hardware/bom/
```

File thường gặp:

```text
.SLDPRT .SLDASM .SLDDRW .STEP .DXF .PDF .MD .CSV .PNG
```

---

## MS2

```text
experiments/sensor_characterization/
results/
ml/drift_detection/   [khi cần]
```

File thường gặp:

```text
.py .ipynb .csv .json .md .png .svg .pdf
```

---

## EE2

```text
firmware/esp32s3/
```

File thường gặp:

```text
.ino .cpp .c .h .hpp .json .md .csv .log
```

---

## ET1

```text
hardware/electronics/
hardware/bom/
```

File thường gặp:

```text
.md .csv .xlsx .pdf .png .svg
.kicad_pro .kicad_sch .kicad_pcb   [nếu dùng KiCad]
```

---

## IT2

```text
ml/
software/
data/
results/
```

File thường gặp:

```text
.py .ipynb .json .yaml .yml .toml
.csv .parquet
.tflite .onnx .pkl .joblib
.png .svg .pdf .md
```

---

# 18. FINAL RULE

Artifact phải trả lời được 4 câu hỏi:

```text
Nó thuộc subsystem nào?
Ai chịu trách nhiệm?
Nó nằm ở folder nào?
File này dùng để làm gì?
```

Nếu không trả lời được 4 câu trên thì chưa nên commit vào repository.
