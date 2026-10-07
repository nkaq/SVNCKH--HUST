# Adaptive Multimodal TinyML for Drift-Robust Predictive Maintenance

**HUST Research Team — NCKH / SVNCKH**

Repository chung của nhóm nghiên cứu cho đề tài:

> **Adaptive Multimodal TinyML for Drift-Robust Predictive Maintenance on Resource-Constrained Edge Microcontrollers**

Hệ thống sử dụng **ESP32-S3** và đa cảm biến để giám sát tình trạng thiết bị điện–cơ, hướng tới chẩn đoán lỗi, đánh giá độ tin cậy cảm biến, phát hiện drift/shift và triển khai TinyML trên thiết bị biên.

---

## 1. Research chain

```text
Motor / Testbench
        ↓
Fault / Operating Condition
        ↓
Physical Response
        ↓
Multimodal Sensing
        ↓
ESP32-S3 Acquisition
        ↓
Raw Data + Metadata + Ground Truth
        ↓
DSP / Feature / Representation
        ↓
TinyML
        ↓
Drift / Reliability Detection
        ↓
Adaptive Multimodal Fusion
        ↓
Edge Decision
        ↓
Condition Monitoring / Predictive Maintenance
```

> Trọng tâm hiện tại: **condition monitoring / fault diagnosis**. Prognosis/RUL chỉ là hướng mở rộng sau.

---

## 2. Cấu hình phần cứng hiện tại

### MCU
- **ESP32-S3**

### Hệ cảm biến

| ID | Sensor / module | Chức năng |
|---|---|---|
| S1 | **ADXL345** | Rung / gia tốc 3 trục |
| S2 | **ACS724** | Dòng điện motor |
| S3 | **PT100 + MAX31865** | Nhiệt độ vỏ motor |
| S4 | **US5881 Hall + nam châm** | RPM reference |
| S5 | **DHT22** | Nhiệt độ / độ ẩm môi trường |

Ba modality chính phục vụ condition monitoring:

```text
Vibration + Current + Motor Temperature
```

RPM được dùng làm **operating-condition/reference signal**.  
DHT22 dùng làm **environmental context** cho thermal/drift characterization.

---

## 3. Trạng thái dự án

- Week 1 — Foundation: **COMPLETED**
- Week 2 — System Characterization & First Experiments: **COMPLETED**
- Week 3 — Prototype Integration & Mechanical Testbench Build: **ACTIVE**

Quy tắc dataset:

```text
Engineering Validation Data
        ≠
Dataset v0.1
```

Dataset v0.1 chỉ bắt đầu sau:

```text
Fixture Fabrication
      ↓
Fixture Acceptance Test
      ↓
PASS
      ↓
Freeze Fixture Version
      ↓
Dataset v0.1 Acquisition
```

---

## 4. Vai trò thành viên

| ID | Trách nhiệm chính | Khu vực repo |
|---|---|---|
| **ME1** | System Architecture, Mechanical Testbench, Motor Physics, Integration, Ground Truth | `hardware/mechanical/`, `docs/architecture/`, `experiments/protocols/` |
| **MS2** | MEMS, ADXL345, Calibration, Sensor Health, Drift | `experiments/sensor_characterization/`, `results/` |
| **EE2** | ESP32-S3 Firmware, Timing, Buffer, Logging, RPM | `firmware/esp32s3/` |
| **ET1** | Electronics, Sensor Interface, Power, Current/Temp/RPM, EMI | `hardware/electronics/` |
| **IT2** | Data, DSP, ML/TinyML, Dashboard | `ml/`, `software/`, `data/`, `results/` |

> Code chính phải nằm theo **subsystem**, không nằm trong folder cá nhân. `members/` chỉ để note/báo cáo cá nhân.

---

## 5. Repository structure

```text
SVNCKH--HUST/
├── README.md
├── CONTRIBUTING.md
├── CHANGELOG.md
├── LICENSE
├── .gitignore
├── .gitattributes
├── SETUP_GIT.md
│
├── docs/
│   ├── architecture/
│   ├── plans/
│   ├── reports/
│   ├── meeting_notes/
│   ├── references/
│   └── project_management/
│
├── hardware/
│   ├── mechanical/
│   ├── electronics/
│   └── bom/
│
├── firmware/
│   └── esp32s3/
│
├── software/
├── ml/
├── experiments/
├── data/
├── simulations/
├── results/
├── assets/
├── presentations/
├── members/
└── .github/
```

Chi tiết: `docs/project_management/REPOSITORY_STRUCTURE.md`

---

## 6. Git workflow

```text
main
 ↑
dev
 ↑
feature/*
```

- `main`: bản ổn định.
- `dev`: nhánh tích hợp.
- `feature/...`: nhánh cho từng task.

Ví dụ:

```text
feature/me1-fixture-v01
feature/ms2-sensor-health
feature/ee2-acquisition-v2
feature/et1-interface-board
feature/it2-edge-dsp
```

Workflow cơ bản:

```bash
git switch dev
git pull origin dev

git switch -c feature/ee2-acquisition-v2

# làm việc

git status
git add .
git commit -m "feat(firmware): add acquisition firmware v2"
git push -u origin feature/ee2-acquisition-v2
```

Sau đó mở Pull Request:

```text
feature/... → dev
```

Khi `dev` ổn định:

```text
dev → main
```

Chi tiết: `docs/project_management/GIT_WORKFLOW.md`

---

## 7. Commit convention

```text
type(scope): message
```

Ví dụ:

```text
feat(firmware): add ADXL345 FIFO acquisition
fix(firmware): handle I2C timeout
feat(mechanical): add adjustable motor plate
docs(week3): update integration report
feat(ml): add RMS and FFT feature extraction
data(metadata): update recording schema
```

Type khuyến nghị:
- `feat`
- `fix`
- `docs`
- `test`
- `refactor`
- `data`
- `chore`

---

## 8. Data policy

Không push raw dataset lớn trực tiếp lên GitHub.

Trong repo chỉ giữ:

```text
data/
├── README.md
├── sample/
├── manifests/
└── processed_sample/
```

Raw dataset lớn nên lưu trên:
- Google Drive / OneDrive
- Hugging Face Dataset
- Zenodo
- NAS / lab server

Repo giữ:
- manifest
- metadata
- checksum
- dataset version
- fixture version
- sample nhỏ

---

## 9. SolidWorks / ANSYS / Slide

File binary lớn nên dùng **Git LFS**:
- `*.SLDPRT`
- `*.SLDASM`
- `*.SLDDRW`
- `*.pptx`

Không commit cache/result ANSYS rất lớn nếu có thể tái tạo.

Với SolidWorks nên export thêm:
- STEP
- DXF
- PDF drawing

---

## 10. Current Week — Week 3

### ME1
- Fixture v0.1 trên SolidWorks
- DFM / fabrication package
- System Architecture v0.3
- Fixture Acceptance Checklist

### MS2
- ADXL345 calibration
- self-test
- Sensor Health Vector v0.1

### EE2
- Acquisition Firmware v2
- FIFO / IRQ / buffer
- state machine
- error recovery

### ET1
- Sensor Interface Board v0.1
- power / ground / connector / EMI
- current / temp / RPM interfaces

### IT2
- PC reference DSP
- on-device DSP
- feature equivalence
- live dashboard
- resource benchmark

Integration target:

```text
5 Sensors
   ↓
Interface Electronics
   ↓
ESP32-S3 Acquisition
   ↓
Sensor Health + QC
   ↓
Edge DSP / Logging
   ↓
OLED / PC Dashboard
```

---

## 11. Research rules

- Không bịa kết quả thí nghiệm.
- Phân biệt `hypothesis` với `measured result`.
- Ground Truth phải đến từ condition vật lý.
- Không suy Ground Truth ngược từ FFT/model prediction.
- Phân biệt sensor drift với noise, operating-condition shift và mounting/domain shift.
- Không chia random-window khiến cùng recording lọt vào cả train và test.
- Mỗi experiment phải có metadata, firmware version và setup version.

---

## 12. Quick start

```bash
git clone <REPO_URL>
cd SVNCKH--HUST
git switch dev
git pull origin dev
```

Task mới:

```bash
git switch -c feature/<member>-<task>
```

Hoàn thành:

```bash
git add .
git commit -m "feat(scope): short description"
git push -u origin feature/<member>-<task>
```

Sau đó mở Pull Request vào `dev`.

---

**HUST Research Team — Adaptive Multimodal TinyML**
