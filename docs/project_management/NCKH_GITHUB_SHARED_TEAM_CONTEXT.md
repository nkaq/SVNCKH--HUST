# NCKH — GITHUB SHARED TEAM WORKFLOW & PROJECT INSTRUCTIONS
## Repository dùng chung cho toàn bộ Project NCKH

> **Mục đích của file này:** dùng làm ngữ cảnh/lệnh chung để tất cả các box chat ME1, MS2, EE2, ET1, IT2 đều hiểu rằng nhóm đã có GitHub repository dùng chung, biết mỗi thành viên phải làm ở đâu, dùng Git/GitHub như thế nào, và không làm sai cấu trúc dự án.

---

# 1. TRẠNG THÁI CHUNG

Nhóm đã tạo GitHub repository chung:

```text
Repository: SVNCKH--HUST
Owner: nkaq
URL: https://github.com/nkaq/SVNCKH--HUST
```

Repository này là **bộ nhớ kỹ thuật/version-control chính thức của nhóm** cho:

- code;
- firmware;
- dashboard;
- ML/TinyML;
- SolidWorks/CAD;
- sơ đồ điện;
- mô phỏng;
- ảnh;
- báo cáo;
- slide;
- poster;
- experiment protocol;
- metadata;
- sample data;
- kết quả và benchmark.

Không dùng GitHub repo như nơi lưu mọi file ngẫu nhiên. Mỗi artifact phải nằm đúng subsystem/folder.

---

# 2. NGUYÊN TẮC GỐC

Mọi box chat trong Project phải mặc định hiểu:

```text
main
 ↑
dev
 ↑
feature/*
```

Ý nghĩa:

```text
main
= bản ổn định / đã được tích hợp

dev
= nhánh tích hợp công việc hiện tại

feature/*
= branch làm task cụ thể
```

Không khuyến khích thành viên làm trực tiếp trên `main`.

Mỗi task kỹ thuật nên có:

```text
Issue
→ Feature Branch
→ Commit
→ Push
→ Pull Request
→ Review
→ Merge vào dev
→ Integration Test
→ Merge dev vào main
```

---

# 3. REPOSITORY STRUCTURE CHÍNH THỨC

```text
SVNCKH--HUST/
│
├── README.md
├── CONTRIBUTING.md
├── CHANGELOG.md
├── LICENSE
├── SETUP_GIT.md
├── .gitignore
├── .gitattributes
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
│   │   ├── solidworks/
│   │   │   ├── parts/
│   │   │   ├── assemblies/
│   │   │   └── drawings/
│   │   ├── step/
│   │   ├── dxf/
│   │   └── fabrication/
│   │
│   ├── electronics/
│   │   ├── schematics/
│   │   ├── pcb/
│   │   ├── wiring/
│   │   └── interface_board/
│   │
│   └── bom/
│
├── firmware/
│   └── esp32s3/
│       ├── drivers/
│       │   ├── adxl345/
│       │   ├── max31865/
│       │   ├── acs724/
│       │   ├── us5881/
│       │   └── dht22/
│       ├── acquisition/
│       ├── logging/
│       ├── dsp/
│       ├── oled/
│       └── tests/
│
├── software/
│   ├── pc_dashboard/
│   ├── serial_logger/
│   ├── visualization/
│   └── utilities/
│
├── ml/
│   ├── preprocessing/
│   ├── features/
│   ├── baselines/
│   ├── tinyml/
│   ├── drift_detection/
│   ├── adaptive_fusion/
│   └── notebooks/
│
├── experiments/
│   ├── protocols/
│   ├── engineering_validation/
│   ├── sensor_characterization/
│   ├── fault_experiments/
│   └── metadata/
│
├── data/
│   ├── sample/
│   ├── manifests/
│   └── processed_sample/
│
├── simulations/
│   ├── matlab/
│   ├── ansys/
│   ├── solidworks_motion/
│   └── other/
│
├── results/
│   ├── figures/
│   ├── tables/
│   ├── benchmarks/
│   └── model_results/
│
├── assets/
│   ├── system_images/
│   ├── experiment_photos/
│   ├── diagrams/
│   ├── posters/
│   └── thumbnails/
│
├── presentations/
│   ├── slides/
│   ├── pdf/
│   └── posters/
│
├── members/
│   ├── ME1/
│   ├── MS2/
│   ├── EE2/
│   ├── ET1/
│   └── IT2/
│
└── .github/
    ├── ISSUE_TEMPLATE/
    ├── PULL_REQUEST_TEMPLATE.md
    └── CODEOWNERS
```

---

# 4. QUY TẮC QUAN TRỌNG: CODE THEO SUBSYSTEM, KHÔNG THEO NGƯỜI

Sai:

```text
members/EE2/code/
members/IT2/model/
members/ME1/solidworks/
```

Đúng:

```text
firmware/esp32s3/
ml/
hardware/mechanical/
```

Folder `members/` chỉ dùng cho:

- note cá nhân;
- weekly report;
- draft;
- checklist cá nhân;
- artifact tạm.

Artifact chính thức phải chuyển về đúng subsystem trước khi Pull Request.

---

# 5. PHÂN CÔNG VỊ TRÍ LÀM VIỆC TRONG REPO

## ME1 — System Architecture / Mechanical / Integration

Primary folders:

```text
hardware/mechanical/
docs/architecture/
experiments/protocols/
hardware/bom/
docs/plans/
```

ME1 đưa vào repo:

```text
SolidWorks parts
SolidWorks assemblies
drawings
STEP
DXF
fabrication package
BOM
Fixture Acceptance Checklist
System Architecture
Experimental Matrix
Ground Truth Protocol
```

Branch ví dụ:

```text
feature/me1-fixture-v01
feature/me1-architecture-v03
docs/me1-week3-report
```

---

## MS2 — MEMS / ADXL345 / Sensor Health / Drift

Primary folders:

```text
experiments/sensor_characterization/
results/figures/
results/tables/
docs/reports/
```

Có thể phối hợp code với:

```text
firmware/esp32s3/drivers/adxl345/
ml/drift_detection/
```

MS2 đưa vào repo:

```text
raw characterization samples nhỏ
calibration scripts
self-test result
temperature characterization
bias/noise/repeatability plots
Sensor Health Vector
drift evidence note
```

Branch ví dụ:

```text
feature/ms2-sensor-health
feature/ms2-adxl345-calibration
```

---

## EE2 — ESP32-S3 Firmware / Acquisition / Timing

Primary folders:

```text
firmware/esp32s3/
```

Đặc biệt:

```text
firmware/esp32s3/drivers/
firmware/esp32s3/acquisition/
firmware/esp32s3/logging/
firmware/esp32s3/tests/
firmware/esp32s3/oled/
```

EE2 đưa vào repo:

```text
ESP32-S3 firmware
sensor drivers
timestamp
interrupt/FIFO
ring buffer
state machine
data logger
error counters
RPM handling
OLED status
hardware test code
```

Branch ví dụ:

```text
feature/ee2-acquisition-v2
feature/ee2-adxl345-fifo
fix/ee2-i2c-timeout
```

---

## ET1 — Electronics / Interface Board / EMI / Current-Temperature-RPM

Primary folders:

```text
hardware/electronics/
hardware/bom/
```

ET1 đưa vào repo:

```text
schematic
wiring diagram
connector pinout
Sensor Interface Board
PCB nếu có
power/ground architecture
decoupling
I2C pull-up
current sensor interface
PT100/MAX31865 interface
Hall RPM interface
EMI/cable-routing note
```

Branch ví dụ:

```text
feature/et1-interface-board
feature/et1-wiring-v01
```

---

## IT2 — DSP / Data / ML / TinyML / Dashboard

Primary folders:

```text
ml/
software/
data/
results/
```

IT2 đưa vào repo:

```text
QC script
parser
serial logger
dashboard
FFT
RMS/features
PC reference DSP
baseline ML
TinyML
drift detection
adaptive fusion
benchmark
confusion matrix
plots
```

Branch ví dụ:

```text
feature/it2-edge-dsp
feature/it2-live-dashboard
feature/it2-baseline-ml
```

---

# 6. WORKFLOW MỖI KHI BẮT ĐẦU LÀM VIỆC

Trước khi code/CAD:

```bash
git switch dev
git pull origin dev
```

Tạo branch task:

```bash
git switch -c feature/<member>-<task>
```

Ví dụ:

```bash
git switch -c feature/ee2-adxl345-fifo
```

Không nên làm toàn bộ tuần trên một branch quá lớn nếu có thể tách thành task nhỏ.

---

# 7. SAU KHI LÀM XONG MỘT PHẦN

Kiểm tra:

```bash
git status
```

Stage:

```bash
git add .
```

Commit:

```bash
git commit -m "type(scope): message"
```

Push lần đầu:

```bash
git push -u origin feature/<member>-<task>
```

Sau đó các lần tiếp theo:

```bash
git push
```

---

# 8. COMMIT MESSAGE CHUẨN

Format:

```text
type(scope): message
```

Ví dụ:

```text
feat(firmware): add ADXL345 FIFO acquisition
fix(firmware): handle I2C timeout
feat(mechanical): add adjustable motor plate
feat(electronics): add PT100 interface
feat(ml): add RMS and FFT features
docs(week3): add member report
data(metadata): update recording schema
```

Type dùng:

```text
feat
fix
docs
test
refactor
data
chore
```

Không dùng commit kiểu:

```text
update
final
final2
code moi
abc
fix loi
```

---

# 9. PULL REQUEST

Sau khi push branch:

```text
GitHub
→ Pull Requests
→ New Pull Request
```

Chọn:

```text
base: dev
compare: feature/...
```

Pull Request phải ghi:

```text
Task
What changed
Files changed
Test performed
Evidence
Dependencies
Known problems
```

Ví dụ:

```text
[EE2] ADXL345 FIFO Acquisition

Changes:
- DATA_READY interrupt
- FIFO read
- timestamp
- sample counter

Test:
- 60 s acquisition
- expected 48000 samples
- dropped samples checked

Dependency:
- IT2 parser update
```

---

# 10. MERGE RULE

Feature branch:

```text
feature/*
   ↓ PR
dev
```

Sau khi integration PASS:

```text
dev
 ↓ PR
main
```

`main` phải được hiểu là bản ổn định.

Không dùng `git push --force` lên:

```text
main
dev
```

---

# 11. GITHUB ISSUES = GIAO VIỆC

Mỗi task quan trọng nên tạo GitHub Issue.

Ví dụ:

```text
Title:
[EE2] Implement ADXL345 FIFO Acquisition

Goal:
Stable vibration acquisition using FIFO + interrupt.

Deliverables:
- driver
- 60 s test
- sample-count report

Acceptance:
- timestamp available
- sample count checked
- dropped sample tracked
```

Branch nên gắn Issue number:

```text
feature/12-adxl345-fifo
```

---

# 12. DATA POLICY

Không commit:

```text
data/raw/
data/private/
dataset vài GB
```

GitHub chỉ giữ:

```text
sample
manifest
metadata
checksum
dataset version
fixture version
```

Raw dataset lớn lưu ở storage riêng:

```text
Google Drive
OneDrive
Hugging Face Dataset
Zenodo
NAS
```

---

# 13. ENGINEERING VALIDATION DATA VS DATASET

Hiện tại:

```text
Engineering Validation Data
```

không phải:

```text
Dataset v0.1
```

Chỉ bắt đầu Dataset v0.1 sau:

```text
Fixture Acceptance Test
        ↓
PASS
        ↓
Fixture Freeze
        ↓
Dataset v0.1
```

Mọi box chat phải giữ quy tắc này khi hướng dẫn thành viên lưu file.

---

# 14. SOLIDWORKS / PPTX / FILE BINARY

Các file lớn/binary:

```text
*.SLDPRT
*.SLDASM
*.SLDDRW
*.pptx
```

phải ưu tiên Git LFS.

Không đặt tên:

```text
final
final2
final_real
final_real_last
```

Dùng version rõ:

```text
fixture_v01.SLDASM
fixture_v02.SLDASM
```

Ngoài SolidWorks source nên export:

```text
STEP
DXF
PDF drawing
```

---

# 15. ANSYS / MATLAB / SIMULATION

Lưu:

```text
simulations/matlab/
simulations/ansys/
simulations/solidworks_motion/
```

Ưu tiên commit:

```text
script
input
settings
screenshots
plots
report
```

Không commit cache/output cực lớn nếu có thể tái tạo.

---

# 16. IMAGE / PHOTO / DIAGRAM

Lưu theo:

```text
assets/system_images/
assets/experiment_photos/
assets/diagrams/
assets/posters/
assets/thumbnails/
```

Tên file phải mô tả nội dung:

```text
testbench_v01_isometric.png
adxl345_mount_v01.jpg
system_architecture_v03.png
```

---

# 17. SLIDE / POSTER / PRESENTATION

Lưu:

```text
presentations/slides/
presentations/pdf/
presentations/posters/
```

PPTX nên export PDF.

Poster/hình dùng cho báo cáo phải nằm trong:

```text
assets/posters/
```

hoặc:

```text
presentations/posters/
```

tùy mục đích.

---

# 18. README.md LÀ CỬA VÀO DỰ ÁN

Mọi box chat khi đề xuất thay đổi lớn cho:

```text
hardware
sensor set
folder structure
current project stage
dataset version
release
```

phải nhắc cập nhật README nếu thay đổi đó ảnh hưởng toàn dự án.

---

# 19. CURRENT SENSOR / SYSTEM CONFIGURATION

Cấu hình mặc định hiện tại:

```text
MCU:
ESP32-S3

S1:
ADXL345
vibration / acceleration

S2:
ACS724
motor current

S3:
PT100 + MAX31865
motor housing temperature

S4:
US5881 Hall + magnet
RPM reference

S5:
DHT22
ambient temperature / humidity

Display:
OLED
```

Không tự đổi sensor nếu chưa có PROJECT UPDATE.

---

# 20. CÁCH BOX CHAT PHẢI TRẢ LỜI

Nếu chat hiện tại là:

```text
ME1
```

ưu tiên:

```text
hardware/mechanical/
docs/architecture/
experiments/protocols/
```

Nếu là:

```text
MS2
```

ưu tiên:

```text
sensor_characterization
sensor health
ADXL345
drift
```

Nếu là:

```text
EE2
```

ưu tiên:

```text
firmware/esp32s3/
```

Nếu là:

```text
ET1
```

ưu tiên:

```text
hardware/electronics/
```

Nếu là:

```text
IT2
```

ưu tiên:

```text
ml/
software/
data/
results/
```

Mỗi box chat phải nói rõ:

```text
file nên nằm ở đâu
branch nên tên gì
output là gì
test thế nào
dependency với ai
```

khi giao task kỹ thuật.

---

# 21. KHI THÀNH VIÊN HỎI “TÔI PHẢI LÀM GÌ?”

Box chat phải trả lời theo format logic:

```text
1. Mục tiêu task
2. Folder làm việc
3. Branch cần tạo
4. File cần tạo/sửa
5. Cách test
6. Deliverable
7. Dependency
8. Cách commit
9. Cách push
10. Pull Request vào dev
```

---

# 22. VÍ DỤ CHO EE2

Task:

```text
ADXL345 FIFO Acquisition
```

Làm ở:

```text
firmware/esp32s3/drivers/adxl345/
firmware/esp32s3/acquisition/
firmware/esp32s3/tests/
```

Branch:

```text
feature/ee2-adxl345-fifo
```

Commit:

```text
feat(firmware): add ADXL345 FIFO acquisition
```

PR:

```text
feature/ee2-adxl345-fifo → dev
```

---

# 23. VÍ DỤ CHO ME1

Task:

```text
Fixture v0.1
```

Làm ở:

```text
hardware/mechanical/solidworks/
hardware/mechanical/step/
hardware/mechanical/fabrication/
hardware/bom/
```

Branch:

```text
feature/me1-fixture-v01
```

Commit:

```text
feat(mechanical): add fixture v0.1 design
```

---

# 24. VÍ DỤ CHO IT2

Task:

```text
Live Dashboard
```

Làm ở:

```text
software/pc_dashboard/
software/serial_logger/
software/visualization/
```

Branch:

```text
feature/it2-live-dashboard
```

Commit:

```text
feat(software): add live engineering dashboard
```

---

# 25. PROJECT UPDATE LIÊN QUAN GITHUB

Khi workflow/folder/branch/subsystem thay đổi, trưởng nhóm dùng:

```text
[PROJECT UPDATE]
Area: GitHub
Change:
Affected members:
New folder:
New branch rule:
Dependency:
Effective from:
```

Update mới nhất ghi đè quy tắc cũ liên quan.

---

# 26. QUY TẮC AN TOÀN

Tuyệt đối không commit:

```text
password
Wi-Fi password
GitHub token
API key
private credential
personal secret
```

Không commit dữ liệu cá nhân không cần thiết.

---

# 27. DEFINITION OF DONE CHO MỘT TASK

Task chưa được coi là hoàn thành chỉ vì code “chạy trên máy cá nhân”.

Task hoàn thành khi:

```text
artifact đúng folder
+
code/CAD/docs có version
+
test evidence
+
commit rõ
+
push branch
+
Pull Request
+
review
+
merge dev
```

Nếu là subsystem quan trọng:

```text
integration test
```

phải PASS trước khi merge `dev → main`.

---

# 28. QUICK COMMAND CHEATSHEET

```bash
git status

git switch dev
git pull origin dev

git switch -c feature/<member>-<task>

git add .

git commit -m "type(scope): message"

git push -u origin feature/<member>-<task>

git branch

git log --oneline --graph --decorate --all
```

---

# 29. QUY TẮC DÙNG FILE NÀY TRONG PROJECT

Mọi box chat của Project NCKH phải coi file này là **GitHub Shared Team Context**.

Khi người dùng hỏi:

```text
"đẩy cái này lên github thế nào?"
"file này để đâu?"
"branch nào?"
"member này làm ở đâu?"
"task tuần này lưu ở đâu?"
```

phải trả lời dựa trên cấu trúc và workflow trong file này.

Không tự phát minh folder mới nếu folder hiện tại đã phù hợp.

Nếu cần thay đổi cấu trúc repository, phải nói rõ đó là đề xuất mới và yêu cầu PROJECT UPDATE trước khi coi là chuẩn chung.

---

# 30. KẾT LUẬN VẬN HÀNH

GitHub repo được dùng như:

```text
README
= cửa vào dự án

Issues
= giao việc

Feature Branch
= không gian làm task

Commits
= lịch sử thay đổi

Pull Request
= review + tích hợp

dev
= phiên bản đang phát triển

main
= phiên bản ổn định

Tags / Releases
= milestone dự án
```

Mọi thành viên phải làm việc theo cùng cấu trúc này để đảm bảo:

```text
traceability
reproducibility
team integration
version control
research quality
```
