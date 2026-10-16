# WEEK 04 — Multimodal Measurement Integrity, 50-Feature Research & Integrated ESP32-S3 Prototype

> **DỰ THẢO V0.1 — CHỜ ME1 DUYỆT — KHÔNG TỰ CÔNG BỐ ACTIVE.**
> Repo đích (sau phê duyệt/PR): `docs/plans/week04/README.md` trên branch `dev`, repository `nkaq/SVNCKH--HUST`.
> Người đề xuất yêu cầu: **ME1 `@nkaq`**. Soạn dự thảo hỗ trợ: AI; **không phải** phê duyệt khoa học/kỹ thuật.
> Ngày soạn: 2026-10-16. Ngày bắt đầu/kết thúc Week 04, hạn nộp từng task, phiên bản protocol và thời điểm hiệu lực: **ME1 PHẢI CHỐT**.
> Mục đích: tạo nền móng đo lường đúng, xử lý nhiễu có chứng cứ, 50 đặc trưng ứng viên và luồng phối hợp ET1–EE2 / MS2–IT2. **Không yêu cầu model chẩn đoán chính xác hoặc công bố % hỏng ngay trong tuần.**

## 0. Quy tắc đọc, thứ tự ưu tiên và quyết định cần phê duyệt

1. **Nguồn chính:** root/scoped `AGENTS.md`, `docs/project_management/CODEX_RESEARCH_STANDARD_V1.md`, `docs/project_management/CODEX_ENGINEERING_STANDARD_V1.md`, `docs/project_management/NCKH_TEAM_CHANGE_UPDATE_WORKFLOW_V1.md`, `CONTRIBUTING.md`, các contract và protocol đã được ME1 phê duyệt; bản Week 03 trên `dev` vẫn áp dụng cho các phần chưa hoàn thành. Nếu có xung đột, **dừng và hỏi ME1 + domain owner**.
2. **Giới hạn phần cứng đã khóa:** ESP32-S3, ADXL345, ACS724, PT100 + MAX31865, US5881 Hall + nam châm, DHT22, OLED. Cụm “ESP32-S3 N16R8” đã được ME1 xác nhận theo thông tin người dùng (thường là **16 MB Flash / 8 MB PSRAM**); **chưa có ảnh/mã PCB, revision, schematic và xác nhận RAM/Flash do firmware đọc được**. N16R8 không chứng minh board có pinout giống DevKitC-1.
3. **Dự thảo không tự kích hoạt:** Chỉ sau khi ME1 sửa/duyệt nội dung, tạo PR riêng `feature/docs → dev`, Codex review + reviewer liên quan, merge, công bố `[PROJECT UPDATE]` có thời điểm hiệu lực và ACK owner mới được coi là task chính thức.
4. **Chưa nghiệm thu fixture:** Trong khi fixture acceptance/freeze chưa có bằng chứng, các bản ghi tuần này là **`data_origin=ENGINEERING_VALIDATION`, `inclusion_status=NOT_ELIGIBLE` cho Dataset v0.1**; kể cả sau này QC PASS cũng **không được** đổi nguồn gốc thành `RESEARCH_COLLECTION` hoặc hợp thức hóa hồi tố. Dataset v0.1 đòi **recording nghiên cứu mới** sau nghiệm thu/freeze đúng protocol, GT từ trạng thái vật lý, QC và Human Gate inclusion đúng dataset version.
5. **Không đồng nhất “nhiễu thấp” với “đo đúng”:** Không được làm mượt một phép đo để trông giống motor khỏe, tự thay số đo thiếu/mất bằng giá trị đẹp, hoặc lọc mất đỉnh phổ mang thông tin lỗi. Tất cả thay đổi raw → calibrated → conditioned → feature phải có version, dấu vết, kiểm định ảnh hưởng và khả năng tái hiện.
6. **Không nhầm khái niệm:** 5 **nhóm đo** (gia tốc, dòng, nhiệt motor, tốc độ quay, nhiệt/ẩm môi trường), **6 module ngoại vi** nếu tính OLED; 50 **đặc trưng/feature**, **không phải** 50 raw sensor và cũng không phải 50 mẫu đo. Một *feature row* chỉ hợp lệ khi đầy đủ điều kiện tính và validity/QC mask.
7. **Chú ý Week 03 hiện hành:** README Week 03 trên `dev` ghi chuyển CAD mới sang **Onshape còn cần hiệu lực Project Update**; không tự diễn giải thành fixture hoàn thành hay tự thay quyết định CAD. ME1 xử lý fixture song song; ET1+EE2 chỉ bench prototype an toàn độc lập cơ khí cho tới khi có duyệt fixture/motor-on.

## 1. Mục tiêu, phạm vi và thước đo hoàn thành

### 1.1. Ba mục tiêu của tuần

- **O1 — Measurement integrity:** Từng kênh có pinout ứng viên, nguồn/level được kiểm tra, cấu hình, timestamp, raw log, calibration & uncertainty plan, lỗi truyền/bỏ mẫu được báo cáo; một vòng tích hợp 5 sensor + OLED *trên bench* chỉ được đánh dấu PASS khi chạy và có chứng cứ.
- **O2 — 50-candidate feature registry:** MS2+IT2 cung cấp **chính xác 50 định nghĩa** F01–F50 dưới đây, mỗi feature có công thức, đơn vị, sensor, cửa sổ, giả định, kiểm thử reference, điều kiện *NOT_COMPUTABLE*, chi phí edge ước lượng và nguồn/giải thích chọn. Không ép cả 50 chạy trên MCU hoặc đưa tất cả vào model.
- **O3 — Healthy-reference prototype:** Xây đường xử lý *raw → QC → calibrated → features → condition-aware healthy baseline (PC)*, dùng Engineering Validation chỉ cho **thử kỹ thuật**; chưa tuyên bố bộ baseline/mô hình nghiên cứu đạt chuẩn Dataset v0.1, fault type hay % severity.

### 1.2. Definition of Done tổng tuần (có thể kiểm tra, không bịa kết quả)

| ID | Bằng chứng cần có trước khi gọi hoàn tất | Điều kiện/giới hạn |
|---|---|---|
| G01 | Danh mục exact board/module part numbers, mặt trước/sau, điện áp/pin maps được ET1+EE2 đối chiếu | Nếu chưa đủ: **pinout DRAFT/NO POWER** |
| G02 | Schematic + BOM/connector + power/ADC/ground review, kiểm điện đo thật | ET1+EE2 cùng ký; motor power chỉ khi ME1 phê duyệt an toàn |
| G03 | Firmware có version/config, raw timestamps, sample index/error counters, OLED không chặn acquisition | Compile PASS chỉ khi build thực; hardware PASS chỉ khi bench log |
| G04 | Calibration report có reference, repeatability, before/after bias/error/uncertainty, fault preservation test | Không gọi “tăng độ chính xác” khi thiếu reference |
| G05 | Feature registry có đủ **50/50 công thức ứng viên**, units, availability/quality và numerical tests | `DEFINED` khác `MEASURED` khác `ON_DEVICE_PASS` |
| G06 | Parser/QC PC xử lý missing, out-of-order, clipping, NaN, wrong version, split theo recording/setup | Không interpolate âm thầm hoặc leak window giữa train/test |
| G07 | PC↔ESP32 equivalence có ít nhất tập feature được chọn và cả hai log so sánh | Nếu không đủ board/toolchain: `NOT_RUN`/`PENDING`, không giả PASS |
| G08 | Bản healthy baseline kỹ thuật phân theo RPM/load/setup; ghi số recording/vùng điều kiện và kết quả holdout thật | Nếu chưa có đủ điều kiện: chỉ `DESIGN/DEMO`, không công bố anomaly score thật |
| G09 | Biên bản demo cặp ET1–EE2 và cặp MS2–IT2, phiếu ACK và nhận xét 5 thành viên | Không bỏ qua blocker interface hoặc phê duyệt GT/safety |

**Không đặt ngưỡng accuracy %, RAM KB, lost-sample %, sensor precision hoặc deadline theo ý AI.** Các ngưỡng số học do ME1+owner đề xuất, dẫn datasheet/điều kiện test, xác nhận trong phiên bản contract và ghi rõ `PROPOSED/APPROVED`. Checklist có bằng chứng thực tế mới được đánh dấu PASS.

## 2. Phân công 5 người: owner, co-owner, task và handoff thật

| Task ID (đề xuất) | Account/primary owner | Co-owner, người bắt buộc phản biện | Deliverable chính (artifact + evidence) | Tiêu chí nhận giao |
|---|---|---|---|---|
| **W04-ME1-01** | ME1 `@nkaq` | Cả nhóm | Healthy-condition matrix, motor/setup/fixture version, test safety/GT approval, project update, tuần acceptance | Ghi điều kiện *motor khỏe* dựa trên vật lý, tải/RPM, phạm vi chạy motor, không suy từ model |
| **W04-MS2-01** | MS2 `@hquanvu12` | IT2 + ET1 | Literature evidence matrix; calibration/noise/uncertainty plan theo 5 nhóm cảm biến; signal-preservation tests | Mỗi correction có reference & công thức, reference instrument, giới hạn hiệu lực, test trước/sau |
| **W04-IT2-01** | IT2 `@lebach181207-dev` | MS2 + EE2 | 50-feature contract, PC reference DSP/QC, feature validity mask, unit tests, exploratory healthy baseline | Chính xác 50 IDs, không hardcode số, version/units/window; group-safe split |
| **W04-ET1-01** | ET1 `@tiendatvu13` | **EE2 bắt buộc** | Sơ đồ nguyên lý, connector/pinout DRAFT→APPROVED, nguồn/ADC level, BOM, bench điện | Đồng ý cả hai bên, chỉ áp dụng sau điện áp và an toàn được duyệt |
| **W04-EE2-01** | EE2 `@dungnguyen13` | **ET1 bắt buộc** + IT2 | Firmware đa sensor + OLED không blocking, config/packet, diagnostics, resource plan | Sơ đồ pin thống nhất, firmware mapping đúng version, test của từng module và integration |
| **W04-PAIR-EF-01** | **ET1 + EE2 cùng nghiệm thu** | ME1, IT2 consumer | **Một** wiring/schematic+pin map hợp nhất và **một** bench integration log, không hai bản độc lập | ET1 đo điện + EE2 xác nhận logic; cả hai `READY`, ME1 Human Gate khi chạm safety/interface |
| **W04-PAIR-SM-01** | **MS2 + IT2 cùng nghiệm thu** | EE2 consumer | PC calibrated stream, feature contract, before/after validation, test-vector fixtures | MS2 xác nhận vật lý/toán; IT2 xác nhận code/timing/units; EE2 xác nhận khả thi on-device |
| **W04-INTEGRATION-01** | ME1 điều phối | **Cả 5** | System-level cross-check: pinmap ↔ driver ↔ stream ↔ QC ↔ 50-feature definitions ↔ baseline; trạng thái G01–G09 | Mỗi owner trình bày evidence thật + RISK/NOT_RUN; ME1 ghi quyết định và blocker |

**Cơ chế teamwork bắt buộc:** Không giao việc “ET1 đưa sơ đồ cho EE2” kiểu chuyển file một chiều. ET1 và EE2 **cùng họp xác nhận pinout trước đấu**, **cùng test từng module**, **cùng xem điện áp/logic khi tích hợp**; MS2/IT2 phải đọc đúng raw stream do EE2 phát và phản hồi nếu đặc trưng không tính được. Thành viên phản biện ký `READY`/`BLOCKED` bằng comment/PR/biên bản có ref — **không được tự ACK thay**.

### 2.1. Ma trận phối hợp theo sản phẩm

| Sản phẩm | Soạn chính | Bắt buộc cùng review | Người dùng tiếp theo | Human Gate |
|---|---|---|---|---|
| Board/power/pin map v0.1 | ET1 | EE2 | MS2, IT2 | ET1+EE2+ME1 khi ảnh hưởng điện/contract |
| Acquisition packet/schema v0.1 | EE2 | ET1, IT2 | IT2/MS2 | EE2+IT2+ME1 nếu breaking changes |
| Calibration rules v0.1 | MS2 | IT2, EE2 | EE2/IT2 | MS2+ME1 nếu đổi protocol, units/label |
| Feature contract 50 v0.1 | IT2 | MS2, EE2 | IT2 TinyML/EE2 Edge | ME1 nếu đổi kiến trúc/định nghĩa chính thức |
| Healthy baseline protocol | ME1+MS2 | IT2, EE2, ET1 | Cả nhóm | ME1 + chuyên môn, và Dataset gate riêng khi nghiên cứu |
| Demo integration | ET1+EE2 | MS2+IT2+ME1 | Week 05 | Chỉ PASS phần thực sự đo/test |

## 3. Lịch 5 phiên làm việc (D1–D5, chưa gán ngày lịch)

| Phiên | ET1 + EE2 (pair work) | MS2 + IT2 (pair work) | ME1 / milestone |
|---|---|---|---|
| **D1: Design freeze candidate** | Chụp/mã board, xác định pin/header, voltage matrix, sơ đồ bus/power, thống nhất bench checklist; NO POWER đến khi sơ đồ và level được duyệt | Tìm datasheet & paper, quyết định feature IDs/units/preprocessing; xác định cần reference instruments nào | Chốt phạm vi và `BLOCKED/READY`; không phê duyệt đo motor đang quay nếu fixture chưa an toàn |
| **D2: Modules in isolation** | Kiểm nguồn cấp trước khi nối IO, read ADXL345/RTD/DHT/Hall/OLED và ADC current **ở chế độ bench an toàn** | Viết raw parser/QC và module calibration giả lập/reference tests, giải thích bias & error | Chốt metadata/protocol & owner các interface |
| **D3: Integrated acquisition** | Đồng bộ bus scheduler/timestamp/error flags; đo nguồn, phát hiện xung đột OLED/I²C/ADC; kiểm thử từng module rồi full suite | Chốt đầy đủ 50/50 công thức và feature conditions; implement tập PC feasible; kiểm thử synthetic (không coi đo thật) | Review pinout, FW packet, safety & blocker |
| **D4: Cross-systems demonstration** | Run firmware/bench smoke, có thể 60 s × số lần được duyệt; lưu log và memory statistics thật | Nhận chính log ET1–EE2, đối chiếu QC, feature PC và subset edge, nghiên cứu normal-reference conditional | Xác nhận mẫu motor khỏe, RPM/load/setup và evidence |
| **D5: Audit & handoff** | Nộp schematic, pin map, firmware revision, test logs; ET1 và EE2 review chéo | Nộp literature/uncertainty report, 50-feature registry, tests/limitations, PC baseline plan/metrics nếu có | Integration review 5 người, fix/blockers, Project Update và task status |

> Phiên D1–D5 là **thứ tự phụ thuộc công việc**, không tuyên bố mỗi nhiệm vụ chắc chắn hoàn thành trong 1 ngày. Nếu thiếu board, reference instrument, fixture hoặc an toàn thì chuyển sang `BLOCKED + mitigation`, không ép làm vật lý hoặc bịa demo.

## 4. Kiến trúc toàn hệ: bốn lớp không được trộn

```text
5 sensing modalities                     OLED (STATUS ONLY)
ADXL345 | ACS724 | PT100/MAX31865 | US5881 | DHT22    ^
     \       |            |              |       /   |
       [ET1: power, connector, ADC front-end, EMI]   |
                     |                                |
        [EE2: driver + acquisition + clock + QC flags]--+
                     |   raw immutable stream + metadata
                     v
          [MS2 + IT2: PC raw QC / calibration / filtering]
                     |   calibrated stream (versioned)
                     v
          [IT2 + MS2: 50 candidate features + validity]
                     |            |              |
               PC baseline    subset edge    feature validation
                     |            |              |
                anomaly risk  MCU benchmark  reference comparison
                     \____________|_____________/
                                  |
                       ME1 human research gate
```

**Data flow:** luôn lưu/giữ tham chiếu tới raw; không ghi đè raw sau hiệu chuẩn. OLED chỉ hiển thị kết quả/trạng thái *đã có*, không được giữ mutex bus lâu, spam I²C, hoặc làm chậm đường acquisition. Rủi ro acquisition phải xuất hiện trong flags/counters, không im lặng làm mượt.

## 5. Board, nguồn và candidate GPIO cho ESP32-S3 N16R8

### 5.1. Vì sao bảng này chỉ là **ứng viên**

`N16R8` mô tả dung lượng Flash/PSRAM của một cấu hình module/board, **không xác nhận tên PCB DevKitC-1, thứ tự chân header, type/revision của OLED hoặc chân module breakout**. GPIO dưới đây chọn từ nhóm khả dụng thường thấy của ESP32-S3-DevKitC-1 **chỉ để ET1+EE2 khởi thảo**. **Tuyệt đối không cắm dây theo bảng trước khi ET1 nhận ảnh 2 mặt/PCB schematic và EE2 xác nhận mapping GPIO thực tế.** Những GPIO bị chiếm bởi flash/PSRAM của board cụ thể phải loại khỏi pinmap.

### 5.2. Bảng GPIO ứng viên (giữ nguyên một nguồn pinmap versioned sau phê duyệt)

| Module/net | Bus & signal | GPIO ỨNG VIÊN trên SoC | Nguồn/level và kiểm tra bắt buộc | Owner |
|---|---|---|---|---|
| ADXL345 + OLED | I²C `SDA` chung | **GPIO8** | Hai module phải hỗ trợ logic 3.3 V; xác minh breakout pull-up/address (ADXL345 thường `0x53/0x1D`; OLED thường `0x3C/0x3D`, **quét thực tế**) | ET1+EE2 |
| ADXL345 + OLED | I²C `SCL` chung | **GPIO9** | Chỉ 1 mạng pull-up tổng hợp phù hợp; đo bus load/rise time; tốc độ giao tiếp do đo thực tế | ET1+EE2 |
| ADXL345 | `INT1` DATA_READY/FIFO (nếu dùng) | **GPIO7** | Optional; cấu hình interrupt theo datasheet và đo delay/overflow | EE2+ET1 |
| MAX31865 | SPI `SCLK` | **GPIO12** | SPI 3.3 V theo module thật; không dùng lại với chức năng khác | ET1+EE2 |
| MAX31865 | SPI `MOSI` | **GPIO11** | Check breakout `SDI/DIN`; jumper 2/3/4-wire đúng PT100 | ET1+EE2 |
| MAX31865 | SPI `MISO` | **GPIO13** | Check breakout `SDO/DO`; đảm bảo level 3.3 V | ET1+EE2 |
| MAX31865 | SPI `CS` | **GPIO10** | Mỗi slave SPI có CS riêng, bus conflict check | ET1+EE2 |
| ACS724 | `VOUT → analog front end → ADC1` | **GPIO4 / ADC1_CH3** | **ACS724 thường 5 V**, analog output có thể vượt 3.3 V hoặc ADC range: ET1 thiết kế đo/bảo vệ/bù scale; **KHÔNG nối thẳng VOUT khi chưa chứng minh an toàn** | ET1+EE2 |
| US5881 Hall | `OUT` (pulse) | **GPIO5** | US5881 IC cần nguồn trong spec riêng (thường **3.5–24 V**), open-drain cần pull-up **về 3.3 V MCU**; kiểm module có kéo lên 5 V sẵn không | ET1+EE2 |
| DHT22 | `DATA` | **GPIO6** | Khuyến nghị cấp 3.3 V nếu đúng breakout; pull-up DATA về 3.3 V theo module; không poll nhanh hơn giới hạn cảm biến | ET1+EE2 |
| OLED | `VCC/GND` | **Không phải GPIO** | Xác nhận OLED thực là I²C 4 chân (nếu SPI: *dừng, thiết kế lại*), mức logic, dòng nguồn, điện trở kéo lên | ET1+EE2 |
| All modules | `GND, 3V3, 5V` | **Power nets**, không phải GPIO | Kiểm dùng common ground ở phía đo MCU, cách ly/safety đường motor-current primary; không giả định USB cấp đủ tất cả module | ET1+ME1 |

**Cảnh báo ESP32-S3:** tránh GPIO0/3/45/46 (*strapping*), GPIO19/20 (*USB*), GPIO43/44 (*UART0*), GPIO39–42 (*JTAG nếu đang debug*) và **GPIO35–37** (*có thể bị Octal PSRAM chiếm*). Cũng kiểm GPIO48 đèn RGB onboard và các chân mạch thật đã chiếm. Đây không phải danh sách chân “cấm tuyệt đối” cho mọi PCB, mà là **mạng xung đột phải được giải trình**. Ưu tiên ADC1 và chức năng GPIO matrix đã kiểm chứng.

### 5.3. Bảng ET1 phải đo/chốt trước POWER-ON (không được bỏ hàng)

| NET / hạng mục | Phải chứng minh bằng gì | Nếu không chứng minh |
|---|---|---|
| `3V3`, `5V`, current budget, `GND` | Schematic + đồng hồ đo khi motor OFF, nguồn/điện áp theo từng module | `BLOCKED: POWER` |
| ACS724 part suffix/range/sensitivity, sensor primary isolation, VOUT max/min | **Đúng** full part number + datasheet + sơ đồ conditioning/bảo vệ + đo không dòng | `BLOCKED: ADC/SAFETY` |
| US5881 package (UA/SE), VDD, `OUT` pull-up | Full marking/polarity + điện áp đo thực; no 5 V pull-up trực tiếp vào GPIO | `BLOCKED: HALL` |
| DHT22/OLED/ADXL345 breakout voltage tolerance và pull-up | Ảnh module + spec + I²C scan/bench | `BLOCKED: I2C/DATA` |
| MAX31865 variant, `RREF`, PT100 nominal, dây 2/3/4, jumpers | Datasheet/silkscreen/bom, đo RREF/RTD cấu hình | `BLOCKED: RTD` |
| Board N16R8 pins/PSRAM & boot | Ảnh hai mặt PCB, module marking, board schematic, firmware chip/memory report | `BLOCKED: PINMAP` |
| Sensor mount & source fault, moving shaft guard | ME1 fixture concept/safety sign-off riêng | `BLOCKED: MOTOR-ON` |

**Điện áp và an toàn:** không lấy mức cao logic GPIO ESP32-S3 là 5 V tolerant; mọi tín hiệu vào MCU phải ở phạm vi được datasheet/đặc tính ADC cho phép. Đo điện áp/kết nối **khi không cấp điện** trước, rồi cấp nguồn USB/bench giới hạn dòng theo phê duyệt. **Không tự thử trên điện lưới hoặc motor đang quay** khi chưa được người có chuyên môn phê duyệt quy trình an toàn, cách ly và che chắn cơ khí. Không giả định chống nhiễu bằng software thay cho nối đất/cáp/bảo vệ đúng.

### 5.4. Thủ tục ET1–EE2 đồng thiết kế (joint sign-off)

1. ET1 cung cấp `board_photo_front`, `board_photo_back`, `board_revision`, full marking từng module và `power_voltage_matrix`. EE2 map tên **GPIO logic** sang **header pin vật lý** của PCB thật, không nhầm số chân IC/header.
2. Cả hai đồng lập `pinmap_version` ứng viên, nêu chức năng, chiều, voltage, pull-up, routing, alternate và nguy cơ boot/USB/PSRAM. IT2 biết những kênh nào thực sự có thể đồng bộ.
3. ET1 review schematic/power/ground và thử bằng đồng hồ khi motor OFF. EE2 review bus address/firmware mapping, sau đó **đổi vai review lần hai**.
4. Mỗi module vượt qua ID/read/status/error test theo cấu hình thực trước khi chạy cùng OLED. Nếu ADC Hall/power chưa đạt, **không tự bypass** bằng “test nhanh”.
5. Khi joint bench xong mới đánh dấu `PINMAP APPROVED_FOR_BENCH` cùng `approved_by`, `approval_ref`, `date`, `revision`. **APPROVED_FOR_BENCH không đồng nghĩa SAFE_FOR_MOTOR_ON**.

## 6. Sampling / timestamp / buffer — thỏa thuận kỹ thuật trước firmware

### 6.1. Bảng đa tốc độ (giá trị cụ thể TBD, không ép giả tạo)

| Stream | Đặc tính thời gian | Cần log | Hệ quả feature |
|---|---|---|---|
| ADXL345 XYZ | ODR/range/filter/bus theo datasheet, đủ Nyquist cho dải lỗi cần quan sát; I²C FIFO 32 sample sets và throughput phải được xác minh | ODR register, `sample_index`, `timestamp_us`, sensor status, FIFO overrun | FFT/1×/2×/band energy **không có giá trị** nếu fs/ODR và mounting không đạt |
| ACS724 analog qua ADC1 | Cần định nghĩa fs_adc, jitter, ADC calibration và analog anti-alias trước khi lấy waveform | raw code, calibrated mV, current A, ADC status, clipping, timebase | Dòng trung bình/RMS có thể đo ở tốc độ thấp; phổ dòng/coherence cần fs và đồng bộ cao hơn |
| Hall US5881 | Pulse edge event, debounce/glitch rejection dựa trên tốc độ/magnet thật | `edge_timestamp_us`, `pulses_per_rev`, timeout, missing pulse counter | Không đủ xung/không biết số nam châm: RPM **NOT_COMPUTABLE** |
| PT100 + MAX31865 | Chuyển đổi RTD chậm, đo nhiệt theo timepoint riêng | RTD code, fault bit, RREF, time, °C, line config | Trend nhiệt không nên giả là window 1 s nếu mẫu quá thưa |
| DHT22 | Cảm biến chậm, đọc ở chu kỳ hợp lệ (datasheet thường ≥2 s) | temp, RH, timestamp, checksum/status, age | Không tạo “2-second x 1000 Hz” bằng interpolation; trend cần đủ thời gian |
| OLED | Tác vụ hiển thị ưu tiên thấp | `display_update_ms`, timeout nếu có | Không nằm trên critical acquisition loop |

**Bắt buộc phân biệt:** *time of sensor measurement*, *ISR/FIFO/read time*, *serial arrival time*; không dùng giờ nhận serial thay timestamp đo mà không gắn cờ. Nếu FIFO nhiều mẫu, reconstruction timestamp phải có giả định ODR/counter và error bounds, không giả mọi mẫu cùng thời điểm FIFO read.

### 6.2. Buffer/resource và chiến lược clean embedded

- Tách `drivers` → `acquisition` → `buffers` → `packet/logging` → `features` → `oled`; **không dynamic allocation trên đường lấy mẫu nóng**, ưu tiên buffer tĩnh/cấu hình, giới hạn kích thước, timeout/retry hữu hạn, ISR tối thiểu.
- Dùng FIFO ADXL345 và (nếu driver/toolchain hỗ trợ) ADC continuous/DMA có kiểm chứng; `Serial.print` từng mẫu + OLED vẽ liên tục trong ISR/critical acquisition **không được**.
- Khi queue đầy: tăng `dropped_samples`/`overflow_counter`, công bố QC flag và recording validity; **không** silently overwrite mà vẫn nhận `PASS`.
- Log free heap/internal heap/PSRAM và high-water mark stack, flash image bytes, per-task timing. PSRAM 8 MB (nếu thực sự có) **không có nghĩa** mọi DMA/ISR/realtime buffer đều đặt trong PSRAM được; EE2 phải kiểm heap capabilities của SDK và bộ nhớ nội bộ phù hợp.
- Phân chia feature: **Tier E** (streaming O(1)/small buffer: mean, RMS, std, peak, min/max), **Tier W** (FFT/window O(N), ràng buộc fs/N), **Tier L/PC** (multi-minute trend, coherence, cross-sensor/long-horizon) rồi chứng minh subset có ích mới port xuống MCU.
- Formula/config/feature order phải giống PC; số float hay fixed-point, quantization và tolerance cần được duyệt theo test vectors. Benchmark trên board thật mới kết luận đạt RAM/CPU/latency.

## 7. Research method: calibration, noise reduction, uncertainty, bảo toàn dấu hiệu lỗi

### 7.1. Đường xử lý được version và tính đúng

```text
raw sample/code + timestamp + sensor status (immutable)
  -> unit conversion theo part number, RREF, sensitivity, ADC calibration
  -> offset/gain/environment correction có reference instrument
  -> QC masks (NaN, saturation, lost/duplicate, stale, out-of-range)
  -> optional filter với cutoff + nhóm baseline/fault-test có chứng cứ
  -> synchronized window/aggregate theo cadence
  -> feature extraction (có version, missing validity)
```

**Ví dụ công thức nguyên lý (không phải hệ số đã đo):**

- **Linear gain/offset:** `y_cal = (y_raw - b(T,setup)) / k(T,setup)` khi có reference để fit và kiểm độc lập; không tự fit `b` để motor khỏe luôn bằng một con số định trước. Trường hợp fit `y_ref ≈ a*y_raw+b` cũng được nếu công bố định nghĩa và chiều biến đổi.
- **ADXL345:** từ LSB → g theo chế độ `FULL_RES`/range được đọc lại thực tế; chuyển `m/s² = g × 9.80665` khi cần; bù offset/scale/orientation riêng từng trục; gravity/DC removal phục vụ *dynamic vibration features* phải được định nghĩa, **không loại tín hiệu thấp tần do lỗi** mà không test.
- **ACS724:** `I(t)=[V_out(t)-V_zero(T,Vcc)]/S(T,Vcc)` theo **đúng suffix** và đơn vị `V/A`; trước đó ADC raw phải dùng calibration và front-end transfer đã đo; với chip 5 V phải bảo vệ logic/ADC 3.3 V. Phân biệt RMS toàn dòng và RMS ripple.
- **PT100/MAX31865:** `R_RTD=(code_RTD/32768)*R_REF` sau xử lý fault/status đúng datasheet; áp dụng phương trình RTD (Callendar–Van Dusen) hoặc LUT với `R0`, `A`, `B`, `C` và phạm vi nhiệt được phê duyệt. Không gán `R_REF=430 Ω` hay `400 Ω` cố định nếu module chưa được đo.
- **US5881:** `RPM = 60/(ppr × Δt_seconds)` cho chu kỳ giữa xung hợp lệ (hoặc `60*N_pulses/(ppr*T_sec)` trên khoảng đủ dài), `ppr` là xung/vòng thực, **không tự mặc định bằng 1**.
- **DHT22:** giá trị nhiệt/ẩm có timestamp và checksum; không giả hiệu chuẩn bằng moving average. Cần đối chiếu nhiệt/ẩm với reference và xét self-heating/airflow; ghi uncertainty thay vì tuyên bố chuẩn tuyệt đối.

### 7.2. MS2 phải nghiên cứu tối thiểu các thuật toán *so sánh*, không áp đặt một bộ lọc chung

| Phương án | Khi cân nhắc | Phải báo cáo/kiểm chứng gì |
|---|---|---|
| Không lọc (raw calibrated) | **Mốc đối chứng bắt buộc** | Sai số so reference, PSD, noise-floor, pulse integrity, clipping, drift |
| Offset/gain/RREF/ADC calibration | Sai số hệ thống rõ, đủ điểm chuẩn | Trước/sau residual, uncertainty, cross-condition validation |
| Analog front-end/anti-alias, power/ground | Nhiễu/sóng ngoài dải và ADC current | Filter magnitude/phase, input range, noise, distortion, bandwidth, part/schematic |
| Robust outlier flag + segmentation | Packet corrupt, glitch, EMI xung | Số mẫu bị flag, nguyên nhân, bỏ hay giữ; không “sửa đẹp” raw không lưu dấu |
| Median / Hampel (có điều kiện) | Glitch rời rạc không mang fault signature | Tác dụng lên peak, kurtosis, spike fault; reject nếu phá signature |
| FIR/IIR low/high/band pass | Có dải tín hiệu vật lý và fs phù hợp | Cutoff/order, group delay/phase, CPU/RAM, PSD/feature before/after, anti-alias |
| Detrend / notch có chứng cứ | DC/gravity, mains/switching interference được phân biệt | Notch không vô tình triệt 1×/2× hay fault harmonics, test over RPM sweep |
| Temperature compensation / model correction | Bias/gain thay đổi theo nhiệt/nguồn | Fit trên calibration-only, kiểm out-of-domain và không dùng test data để fit |
| Cross-sensor quality gating | Một sensor timeout/dropped, timebase lệch | Cách flag và bỏ feature, fail-safe inference; không “đổ” dữ liệu sensor này sang sensor khác |

**Bắt buộc có ba phép so sánh** trước/sau thuật toán: (i) đo reference/uncertainty (nếu có chuẩn), (ii) preservation của tín hiệu chẩn đoán (biên độ/tần số 1×/2×/transient theo điều kiện áp đặt hoặc test synthetic), (iii) repeatability across runs/setup/RPM. Bộ lọc có vẻ “mượt” nhưng giảm peak/1× không giải thích được → **REJECT/REWORK**. Với dữ liệu synthetic, ghi `SIMULATED`, không ghi `MEASURED`.

### 7.3. Ma trận literature review bắt buộc của MS2

Mỗi sensor ít nhất **datasheet đúng part + một nguồn giải thích mô hình đo/hiệu chuẩn**, và cần có **tài liệu peer-reviewed về fault features, fusion hoặc abnormal detection** để đánh giá 50 đặc trưng. Không tự tuyên bố paper “chứng minh” công thức tối ưu cho board của nhóm. Mẫu mỗi dòng:

| Field | Phải ghi |
|---|---|
| Citation | Author, title, venue/year, DOI/URL, loại nguồn (peer-reviewed/vendor/standards) |
| Problem | Sensor/bias/noise/drift/fault class, sampling/mounting/operating condition |
| Equations | Ký hiệu, tham số, đơn vị, phạm vi, giả thiết và cách tìm hệ số calibration |
| Evidence | Dataset/reference sensor của paper, số đo & giới hạn báo cáo gốc, **không chép kết quả thành số đo nhóm** |
| Implementation | Computational cost, window/buffer, fixed/float, risk on ESP32-S3 |
| Validation proposal | Cách nhóm mình tái hiện trên benchmark/synthetic/engineering validation, loss/feature preservation |
| Decision | `CANDIDATE` / `TESTED` / `REJECTED` / `APPROVED` + lý do và reviewer |

## 8. Feature Contract V0.1 — quy ước toán học bắt buộc

### 8.1. Dữ liệu, notation, đồng bộ và validity

- Trục gia tốc động (calibrated, gravity/DC treatment được version): `a_x[n],a_y[n],a_z[n]` đơn vị `m/s²`; `a_v[n]=sqrt(a_x[n]^2+a_y[n]^2+a_z[n]^2)`. Không đổi dynamic acceleration sang gia tốc tuyệt đối không báo version.
- Dòng hiệu chuẩn `i[n]` đơn vị ampere; ripple `i_ac[n]=i[n]-mean(i)`. RPM từ Hall là `r(t)` `rev/min`; nhiệt PT100 `T_m(t)` °C; DHT22 nhiệt `T_a(t)` °C và humidity `H(t)` %RH.
- Mọi feature tính trên `analysis_interval_id` rõ ràng. `W_fast`, `W_rpm`, `W_thermal`, `W_ambient` có thể **khác độ dài**; phép ghép feature phải ghi `window_start_utc`, `window_end_utc`, `source_age_ms`, `alignment_rule` và *data-validity flags*, tránh look-ahead vào dữ liệu tương lai. `fs_a`, `fs_i`, `N`, hop/overlap, RPM load bins và `filter_version` là giá trị **TBD owner approval**, không tự điền sampling rate thật.
- `mean(x)=Σx/N`; `rms(x)=sqrt(mean(x²))`; `std(x)=sqrt(mean((x-mean(x))²))` dùng **population std (`ddof=0`)** theo định nghĩa này; `peak_abs(x)=max|x|`; `ptp(x)=max(x)-min(x)`.
- `crest(x)=peak_abs(x)/rms(x)` khi denominator > epsilon; `skew(x)=mean((x-μ)^3)/σ³`; `kurt(x)=mean((x-μ)^4)/σ⁴` **Pearson kurtosis** (không trừ 3), khi σ>ε. Ghi library reference và epsilon policy/float precision.
- `PSD_x(f)` là ước lượng power spectral density của tín hiệu đã chọn với window/detrend/scaling được khóa; `E_B(x)=∫_{f∈B} PSD_x(f)df` (đơn vị `m²/s⁴` cho accel; nói rõ anti-alias/dải B). `A_x(f)` là **biên độ vật lý đã sửa coherent gain** từ FFT/window, không phải FFT-bin thô.
- `f_rot = median(r_valid)/60` Hz. `A_v` phổ dựa trên `a_v` **hoặc** kênh trục cụ thể theo `spectral_source_axis` phải thống nhất trước baseline. Tại 1×/2×, ghi bin tolerance/resolution `Δf≈fs/N`, interpolation method/version; nếu tốc độ biến thiên lớn hoặc không đủ độ phân giải thì `NOT_COMPUTABLE`.
- `slope(x,t)` là hệ số dốc hồi quy tuyến tính timestamp thật trong vùng thời gian (mặc định báo °C/min hoặc %RH/min, không fit theo chỉ số mẫu). `coh_ai(f)=|P_ai(f)|²/(P_aa(f) P_ii(f))` chỉ được tính khi accel/current đồng bộ và Welch/segment đủ điều kiện; tính trên *đồng pha, cùng clock/alignment*, không dùng sample index không đồng bộ.
- Khi không đủ `N_min`, không đủ pulse, quá nhiều lost samples, signal clipped, stale context, denominator zero, không có reference/filter config hoặc thiếu time sync: output feature **NULL/NOT_COMPUTABLE + quality reason**, *không điền 0 hoặc giá trị trung bình để dashboard đẹp*. `null` trên storage không phải input NaN của model; missing mask/imputation policy do IT2 quyết định **fit chỉ trên train**.
- `motor healthy` phải được ME1 xác định bằng điều kiện/kiểm tra vật lý và metadata, **không do sai khác với baseline hoặc FFT suy ra**.

### 8.2. 50 đặc trưng ứng viên, mã ổn định F01–F50

**Ký hiệu Tier:** `E` = có thể tính streaming/nhẹ; `W` = cần window/FFT; `L` = long-horizon/context; `R` = nghiên cứu/offline hoặc điều kiện khó. **Đây là độ phức tạp dự kiến, KHÔNG phải lời cam kết RAM/Flash PASS.** `a*` là dynamic calibrated acceleration; `i_ac = i - mean(i)`; các công thức PSD/chỉ số moment dùng quy ước ở §8.1.

| ID | Nguồn | Feature & công thức định nghĩa | Đơn vị | Tier / điều kiện đặc biệt |
|---|---|---|---|---|
| F01 | ADXL345 | X dynamic RMS — `rms(a_x)` | m/s² | E; offset/gravity strategy |
| F02 | ADXL345 | Y dynamic RMS — `rms(a_y)` | m/s² | E; same calibration |
| F03 | ADXL345 | Z dynamic RMS — `rms(a_z)` | m/s² | E; same calibration |
| F04 | ADXL345 | Vector dynamic RMS — `sqrt(mean(a_x²+a_y²+a_z²))` | m/s² | E; not `rms(a_v-mean(a_v))` |
| F05 | ADXL345 | X absolute peak — `max(abs(a_x))` | m/s² | E; clipping flag |
| F06 | ADXL345 | Y absolute peak — `max(abs(a_y))` | m/s² | E; clipping flag |
| F07 | ADXL345 | Z absolute peak — `max(abs(a_z))` | m/s² | E; clipping flag |
| F08 | ADXL345 | Vector peak — `max(a_v)` | m/s² | E; normalized axis/units |
| F09 | ADXL345 | X crest factor — `crest(a_x)` | 1 | E; rms>eps |
| F10 | ADXL345 | Y crest factor — `crest(a_y)` | 1 | E; rms>eps |
| F11 | ADXL345 | Z crest factor — `crest(a_z)` | 1 | E; rms>eps |
| F12 | ADXL345 | Vector amplitude PTP — `ptp(a_v)` | m/s² | E; motion/axis policy |
| F13 | ADXL345 | X Pearson kurtosis — `kurt(a_x)` | 1 | E; std>eps |
| F14 | ADXL345 | Y Pearson kurtosis — `kurt(a_y)` | 1 | E; std>eps |
| F15 | ADXL345 | Z Pearson kurtosis — `kurt(a_z)` | 1 | E; std>eps |
| F16 | ADXL345 | Dominant vibration frequency — `argmax_f PSD_a*(f)` within `[f_lo,f_hi]` | Hz | W; valid passband/resolution |
| F17 | ADXL345 | Dominant peak amplitude — `A_a*(F16)` corrected coherent gain | m/s² | W; window/scaling defined |
| F18 | ADXL345 | Spectral centroid — `sum(f_k*P_k)/sum(P_k)` on valid bins | Hz | W; denominator>eps |
| F19 | ADXL345 | Normalized spectral entropy — `-sum(p_k*ln p_k)/ln K`, `p_k=P_k/sum(P)` | 1 | W; K>=2, positive total |
| F20 | ADXL345 | Low-band PSD energy — `E_Blow(a*)` | (m/s²)² | W; bands TBD approved |
| F21 | ADXL345 | Mid-band PSD energy — `E_Bmid(a*)` | (m/s²)² | W; bands TBD approved |
| F22 | ADXL345 | High-band PSD energy — `E_Bhigh(a*)` | (m/s²)² | W; bands within Nyquist |
| F23 | ADXL345+US5881 | 1× rotation amplitude — `A_a*(f_rot)` | m/s² | W; RPM time-aligned/resolved |
| F24 | ADXL345+US5881 | 2× rotation amplitude — `A_a*(2*f_rot)` | m/s² | W; f<Nyquist |
| F25 | ACS724 | Mean current — `mean(i)` | A | E; bidirectional/DC semantics |
| F26 | ACS724 | Total current RMS — `rms(i)` | A | E; current circuit type known |
| F27 | ACS724 | Ripple RMS — `rms(i_ac)` | A | E; mean removed only for ripple |
| F28 | ACS724 | Absolute peak current — `max(abs(i))` | A | E; ADC front-end not clipped |
| F29 | ACS724 | Current PTP — `ptp(i)` | A | E; input range validated |
| F30 | ACS724 | Current crest factor — `crest(i)` | 1 | E; rms>eps |
| F31 | ACS724 | Ripple skewness — `skew(i_ac)` | 1 | E; std>eps |
| F32 | ACS724 | Ripple Pearson kurtosis — `kurt(i_ac)` | 1 | E; std>eps |
| F33 | ACS724 | Dominant ripple frequency — `argmax_f PSD_iac(f)` valid band | Hz | W; ADC fs/alias/sync verified |
| F34 | ACS724+US5881 | Current ripple 1× amplitude — `A_iac(f_rot)` | A | W; 1× may be weak; time aligned |
| F35 | US5881 | RPM mean — `mean(r)` | rpm | E; ppr/valid pulse rule |
| F36 | US5881 | RPM median — `median(r)` | rpm | E; valid rpm points |
| F37 | US5881 | RPM population std — `std(r)` | rpm | E; ≥Nmin per interval |
| F38 | US5881 | RPM min — `min(r)` | rpm | E; stalled != missing unless verified |
| F39 | US5881 | RPM max — `max(r)` | rpm | E; glitch pulse filtering |
| F40 | US5881 | RPM variation CV — `std(r)/abs(mean(r))` | 1 | E; mean RPM>eps |
| F41 | PT100+MAX31865 | Motor temp median — `median(T_m)` | °C | L; valid RTD conversions |
| F42 | PT100+MAX31865 | Motor temp max — `max(T_m)` | °C | L; valid thermal interval |
| F43 | PT100+MAX31865 | Motor heating slope — `slope(T_m,t)` ×60 if t in s | °C/min | L; span/min points TBD |
| F44 | PT100+MAX31865 | Motor temp std — `std(T_m)` | °C | L; >1 valid readings |
| F45 | DHT22 | Ambient temp median — `median(T_a)` | °C | L; freshness/valid checksum |
| F46 | DHT22 | Relative humidity median — `median(H)` | %RH | L; sensor sampling limit |
| F47 | DHT22 | Ambient temp slope — `slope(T_a,t)` ×60 if t in s | °C/min | L; sufficiently long horizon |
| F48 | DHT22 | Humidity slope — `slope(H,t)` ×60 if t in s | %RH/min | L; long horizon & valid data |
| F49 | MAX31865+DHT22 | Motor-ambient temperature rise — `median(T_m(t)-T_a_nearest_valid(t))` | °C | L; age/TTL & time-alignment |
| F50 | ADXL345+ACS724+Hall | Magnitude-squared coherence @1× — `coh_ai(f_rot)` | 1 | R/PC-first; synchronized streams & Welch, enough segments |


**Quan trọng về F16–F24/F33–F34/F50:** Không thể cam kết 50 feature có tính hợp lệ khi chưa biết tốc độ motor, `ODR/fs`, frontend ACS724, dải tần, tốc độ cập nhật DHT22/RTD và đồng bộ. Các feature này phải có `quality/availability_reason` và được chứng minh bằng spectrum/timebase; nếu không đáp ứng thì trạng thái **CANDIDATE / NOT_COMPUTABLE**, không ép ra số.

### 8.3. Những trường bắt buộc cho **mỗi** feature khi MS2+IT2 soạn contract chính thức

`feature_id`, `name`, `version`, `source_sensor`, `raw_field`, `unit`, `formula`, `filter_calibration_version`, `window_type`, `window_seconds`, `hop_seconds`, `sample_rate_hz`, `frequency_band_hz`, `rpm_dependency`, `sync_tolerance`, `min_valid_samples`, `min_coverage`, `validity_rule`, `undefined_policy`, `reference_test_vectors`, `PC implementation reference`, `MCU feasibility TIER`, `estimated_time/memory` (chỉ estimate nếu chưa đo), `measured_time/RAM/Flash` (chỉ evidence thật), `literature_ref`, `approver`, `status`.

**Không tự phê duyệt một vector gồm 50 số nếu F47/F48/F50 thiếu điều kiện.** Nên giữ `feature_schema_version` và một tập `feature_subset_id` cho những feature thật sự huấn luyện/triển khai; model phải kiểm đúng feature ordering/units và missingness policy đã học.

### 8.4. Numerical reference tests và kiểm chứng feature

| Test | Input có thể kiểm tra | Kết quả kỳ vọng / tiêu chí |
|---|---|---|
| T01 | Vector zero, constant, linear ramp; các trục đổi dấu | RMS/std/peak/skew/kurt đơn vị đúng; zero denominator → validity flag, không NaN im lặng |
| T02 | Sine một tần số với fs,N được xác định; thử Hann/rectangular | Dominant freq/amplitude và 1×/2× đúng trong **tolerance phê duyệt**, không giả FFT raw là amplitude |
| T03 | Sum of two tones, injected impulse/noise, controlled synthetic drift | PSD energy trong bands, kurtosis/crest preservation, trước/sau filter |
| T04 | Bi-directional current DC+ripple, simulated ADC transfer+clipping | Dòng mean/RMS/ripple RMS phân biệt rõ, clipping flag, không đảo sensitivity |
| T05 | Hall events 1 pulse/rev vs nhiều pulses/rev, glitches, timeout | RPM đúng khi có `ppr`; missing/zero speed không đánh tráo |
| T06 | RTD resistance/code theo datasheet, config RREF khác nhau | RTD °C/fault branches xử lý đúng, không hardcode 430 Ω |
| T07 | DHT22 đọc thiếu, checksum fail, sample stale | Feature temp/RH/trend không dùng stale value như dữ liệu mới |
| T08 | Time series có timestamp lệch/mất mẫu, window overlap | validity/coverage gắn flag; không trộn cửa sổ cùng recording vào train/test |
| T09 | Accel/current sync tốt & cố tình lệch, Welch segments thiếu | F50 chỉ valid khi đủ sync/segments, coherence nằm [0,1] trong tolerance |
| T10 | Bản ghi thật có source/metadata khi bench được phép | PC↔ESP32 selected features cùng input, diff report từng feature, SHA/units/config rõ ràng |

Unit test dùng **synthetic test vector** là test toán/code, không chứng minh sensor độ chính xác vật lý hoặc dataset nghiên cứu. Nếu chưa có toolchain/board: T10 là `NOT RUN`.

## 9. PC calibrated pipeline, baseline và ranh giới ML

### 9.1. Hai nhánh dataset luôn riêng

- **Engineering Validation** (hiện tại nếu fixture chưa freeze): dùng để thử điện, driver, filter, feature, PC↔edge equivalence, *exploratory* healthy baseline (prototype). Luôn giữ `data_origin=ENGINEERING_VALIDATION`, `inclusion_status=NOT_ELIGIBLE`; file raw/manifest truy vết, không copy đổi nhãn thành Dataset v0.1.
- **Research Collection** (chỉ tương lai): thu **recording mới sau** Fixture Acceptance PASS và frozen version, đúng protocol ME1/domain approved, physical condition verified, QC, dataset-specific Human Gate, manifest/version trước `INCLUDED`.

### 9.2. Hướng model hợp lý theo các mốc chín muồi

1. **M0 — Đo và feature engineering:** raw/calibration/QC/feature validity đúng; chưa train nghiên cứu chính thức.
2. **M1 — Healthy anomaly prototype:** thu nhiều recording của motor khỏe theo **motor_id, RPM bin, load condition, fixture/setup, ambient**. Dùng phân bố/mô hình theo điều kiện thay vì một hàng 50 giá trị cố định. Có thể đánh giá robust location/covariance, distance score/one-class detector trên PC **thử kỹ thuật**. Không tự biến một threshold arbitrary thành lỗi thực tế.
3. **M2 — Fault-type classifier:** chỉ khi ME1 đã thiết kế và áp đặt điều kiện vật lý `healthy/imbalance/misalignment/looseness/...` phù hợp protocol và dataset có human approval; không suy nhãn từ FFT, sensor hoặc đầu ra model.
4. **M3 — Severity/regression:** chỉ khi có **độ lớn lỗi vật lý** được định nghĩa/đo/kiểm chuẩn (ví dụ thông số imbalance/misalignment/looseness có protocol) và đánh giá generalization. **Model probability 0.9 không nghĩa “motor hỏng 90%”.**
5. **M4 — Edge TinyML:** chọn feature subset + model đáp ứng chất lượng *và* RAM/Flash/latency thực đo; thử domain shift/healthy drift theo motor/setup/reload/time, không làm lại nhãn bởi AI.

### 9.3. Data leakage và evaluation

- Chia train/validation/test theo `recording_id` và khi cần theo motor/setup/session/date, **không random windows cùng recording qua các tập**.
- Fit calibration learned parameters, scaler, imputation, feature selection, threshold, model **chỉ train (hoặc calibration partition được protocol cho phép)**; khóa config trước test; không tune trên test.
- Healthy baseline tách các điều kiện RPM/load/ambient; so sánh test holdout khác session/setup; báo false-positive rate, missing-feature rate, variability và độ nhạy điều kiện **chỉ khi thật sự đo**.
- `20` hay `50` không phải KPI khoa học tự thân. 50 là **candidate registry**, tập huấn luyện v0.1 có thể nhỏ hơn sau ablation và đánh giá collinearity/compute budget; giữ manifest `selected_feature_ids` và `selection_reason`.
- Những sensor chậm không được “upsample giả” thành tín hiệu nhanh; khi join dùng causal last-valid với TTL phê duyệt; không nhận dữ liệu từ tương lai.

## 10. Recording, metadata, QC và interface contracts (DRAFT, không sửa schema ngầm)

**Tách ba lớp:** (i) recording raw bất biến, (ii) annotation/metadata có evidence, (iii) derived calibrated/features có processing version. Schema sau đây là **danh mục trường phải đối chiếu với schema hiện có**, KHÔNG phải lệnh thêm field vào production JSON ngay.

| Nhóm | Trường bắt buộc/đề xuất đối chiếu |
|---|---|
| Provenance | `recording_id`, `data_origin`, `inclusion_status`, `recording_start_utc`, `operator_ref`, `protocol_version`, `evidence_type`, `evidence_ref` |
| Hardware | `motor_id`, `board_model`, `board_revision`, `module_part_number`, `pinmap_version`, `sensor_mounting`, `fixture_version`, `setup_version` |
| Firmware | `firmware_sha`, `driver_versions`, `acquisition_config_version`, `serial_schema_version`, `boot/reset`, `heap/psram/flash_report` |
| Acquisition | `sensor_id`, `sample_index`, `sensor_timestamp_us`, `timebase`, `effective_fs`, `ODR/registers`, `full_scale`, `ADC calibration`, `ppr`, `RTD_RREF`, `drop/overflow/error counters` |
| Controlled condition | `physical_condition` (ME1 verified), `rpm_setpoint/observed`, `load_condition`, `motor_on/off`, `fault_mechanism_ref`, `severity_physical` nếu đã được duyệt |
| Processing/QC | `raw_manifest_ref`, `qc_criteria_version`, `qc_status`, `calibration_version`, `filter_version`, `feature_schema_version`, `feature_subset_id`, `validity_mask`, `window_start/end` |
| Dataset human gate | Nếu nghiên cứu mới: `target_dataset_version`, quyết định theo dataset version, `included_dataset_version` **chỉ khi INCLUDED**, `approver`, `approval_scope`, `approved_at`, `approval_ref`, `decision_history_ref` |

**Hợp đồng packet/CSV** phải chốt qua EE2+IT2+MS2 (và ET1 nếu pinout) gồm header/version, length/framing, byte order, units, origin/timebase, `sample_index`, sensor ID, error status, CRC/checksum nếu protocol yêu cầu. Không tạo giả packet format “đã chính thức” từ README. Khi lỗi/thiếu packet: parser phải flag/reject, không âm thầm nối mẫu.

**QC status khác dataset membership:** `QC PASS` trên Engineering Validation chỉ có nghĩa kiểm kỹ thuật theo tiêu chí; **không cấp tư cách Dataset v0.1**. Quyết định `EXCLUDED`/`INCLUDED` cho nguồn nghiên cứu phải có lịch sử và người phê duyệt đúng scope/version; không tự đổi nguồn thu hoặc dán `APPROVED` khi thiếu `approval_ref`.

## 11. Thử nghiệm ET1–EE2: từ không cấp motor đến đủ 6 module

| Stage | Điều kiện và thao tác | Evidence & verdict |
|---|---|---|
| **S0 — Document/bench preflight** | Board/module ID, schematic, GPIO mapping, nguồn/level/pull-up; đo continuity/short khi power OFF | Signed preflight; chưa duyệt thì `BLOCKED` |
| **S1 — Power only** | Dùng nguồn phù hợp có bảo vệ, đo 3V3/5V, quiescent current, GND; motor supply disconnected | DMM/log thật, PASS/FAIL |
| **S2 — I²C** | ADXL345 đọc ID/config, OLED xác nhận address/display, thử bus shared và update không blocking | Address/config/error logs, timing |
| **S3 — RTD/SPI** | MAX31865 read register/fault, known resistor/PT100 (nếu có), không đấu mù dây 2/3/4 | RTD code/RREF/fault/test results |
| **S4 — ADC current** | Zero-current output, frontend transfer, reference current bench **chỉ khi người có chuyên môn duyệt** | ADC input safe proof, raw/mV/A plots; không nhảy tới motor mains |
| **S5 — Hall/DHT** | Hall test controlled magnet/known pulse generator, DHT update interval/checksum, stale flags | RPM/ppr test, checksum/errors |
| **S6 — Concurrency** | Chạy 5 sensing groups + OLED, kiểm reset/USB/PSRAM, measure 60 s và repeats **khi được duyệt**, monitor lost samples, buffer, latency/heap | Per-channel logs, resource stats, `NOT RUN` nếu chưa bench |
| **S7 — Motor-on (CONDITIONAL)** | Chỉ khi ME1 đã ký electrical + mechanical safety, fixture/guarding, power/current path đủ; dừng theo E-stop protocol | Signed approval + controlled run log; không thuộc default Week 04 |

**Stop rules:** Nếu nóng bất thường, wiring/supply không rõ, ADC input > allowed, boot fail, I²C stuck, sensor data sai pin, motor unguarded hoặc mất xung/timestamps không giải thích được: dừng cấp nguồn và đánh `BLOCKED`, không workaround mù.

### 11.1. Bảng demo hội đồng cuối tuần phải cho thấy

- Với mỗi cảm biến, trạng thái `[connected / configured / valid / stale / error]`, units, actual cadence/timebase, calibration status và evidence.
- OLED hiển thị tối thiểu `system status`, `rpm` nếu có Hall hợp lệ, `vibration RMS`, `current` **chỉ khi ADC calibration valid**, `T_motor`, `T_ambient/RH` và `error flag`. Nếu không đủ diện tích OLED, nhiều màn trang luân phiên **không chặn acquisition**; không dán `Healthy 95%` giả.
- Một file recording/manifest kỹ thuật có raw + QC/errors và một bảng features subset đã được chứng minh; 50-feature registry có đầy đủ định nghĩa và trạng thái `DEFINED/TESTED/NOT_COMPUTABLE/EDGE_PASS` cho từng F.

## 12. Trách nhiệm tối ưu embedded và benchmark của EE2+IT2 (ET1 tham gia)

| Hạng mục | Phép đo / cách chứng minh | Chưa có thì ghi |
|---|---|---|
| Flash image | Build report cho đúng N16R8 board target/toolchain/partition | `NOT RUN` |
| Internal heap / PSRAM | Free heap min, allocation caps, PSRAM init result, fragmentation/counters | `NOT RUN` |
| Stack | Per task high-water mark qua test chu kỳ/stress | `NOT RUN` |
| Acquisition jitter/loss | timestamp delta stats, expected vs observed, FIFO overflow, ring overrun, serial dropped | `NOT RUN` |
| CPU time & latency | profile tasks/per feature group, worst-case, inference/OLED path | `NOT RUN` |
| ADC accuracy | reference current/voltage, error and uncertainty at multiple points | `NOT RUN` |
| PC↔edge equivalence | shared binary/CSV test vectors, feature-by-feature signed/relative/absolute error and tolerance owner-approved | `NOT RUN` |
| Reliability | restart, disconnect, bad packet, bus stuck, power variability (chỉ an toàn) | `NOT RUN` |

**Chiến lược tối ưu:** chạy đúng trước → đo bottleneck → chọn streaming stats / reuse FFT buffers / precomputed coefficients / decimate sensor chậm hợp lệ / partition model → tối ưu có benchmark → regression check. Không “clean code, ít RAM” bằng xóa error reporting hoặc đổi precision không test.

## 13. Cấu trúc sản phẩm trong repo (đề xuất; kiểm path trước, không tạo hàng loạt theo thói quen)

```text
docs/plans/week04/README.md                   # CHỈ sau ME1 approve + PR
docs/project_management/                      # project update, contracts (nếu được giao/versioned)
hardware/electronics/schematics/               # ET1: source schematic nếu tool có + export kiểm chứng
hardware/electronics/wiring/                   # ET1+EE2: bench pinmap/connector/power chart
hardware/electronics/interface_board/          # ET1: front-end, BOM, integration notes
firmware/esp32s3/drivers/{adxl345,acs724,max31865,us5881,dht22}/
firmware/esp32s3/acquisition/                  # EE2: scheduling/timebase/status
firmware/esp32s3/oled/                         # EE2: UI status, nonblocking
firmware/esp32s3/tests/                        # EE2: host/hardware evidence when available
experiments/sensor_characterization/          # MS2: lit review, calibration protocol/scripts
ml/features/                                   # IT2+MS2: 50 feature registry/PC reference/tests
ml/baselines/                                  # IT2: exploratory normal-only prototype
software/                                      # IT2: parser/logger nếu có contract
results/                                       # Reports & evidence *nhỏ*, không raw/private dump
```

Đường dẫn mang tính **dự kiến**, phải kiểm repo/`AGENTS.md` scoped trước khi tạo; không commit raw recordings lớn, secrets, build artifacts, binary CAD/FEA chưa có policy. Raw Engineering Validation lưu ở nơi kiểm soát truy cập và đưa lên GitHub **manifest/redacted sample hợp lệ** theo quy định. Repo hiện tại có thư mục `wiring/`, `schematics/` và các nhóm driver, nhưng trong lần kiểm tra lập draft các thư mục wiring/schematic chỉ chứa `.gitkeep`, **không có pinout đã được duyệt**.

## 14. Cơ chế PR/Codex/ACK/Human Gate của Week 04

1. ME1 chốt draft này thành task source có ngày hiệu lực; không sửa trực tiếp `dev`/`main`. Khi đổi pinout/schema/units/sampling/GT/protocol, Project Update B/C, owner/ME1 approval trước kích hoạt.
2. Mỗi owner làm feature branch riêng từ `dev` sạch, chỉ stage đúng file; PR mặc định vào **`dev`**; task ID và acceptance/evidence theo template hiện hành. Một task cặp có thể có primary implementer nhưng cần comment/review của đồng owner. Không dùng chung branch tùy tiện và không để hai PR ghi đè contract.
3. Hai checkpoint bắt buộc: **EF-ACK (ET1+EE2)** cho pinmap/power và **SM-ACK (MS2+IT2)** cho calibration/feature schema. EE2+IT2 phải ACK cross-schema; ME1 quyết định hiệu lực. `BLOCKED` chưa xử lý → không `ACTIVE`.
4. Local review mã nguồn: người làm tự viết/kiểm, Codex xem diff theo root/scoped `AGENTS.md`, có thể sửa trong phạm vi được phép nhưng không được bịa test/human review; con người xác nhận commit/push.
5. GitHub PR: chờ `repo-quality`, GitHub Codex review **latest HEAD SHA** và các finding; human independent reviewer xét tests/bench/physical gate; không merge do nút xanh nếu còn blocker.
6. Merge `feature → dev` theo ruleset/nhóm; không đưa toàn bộ `dev` vào `main` khi chưa có integration release evidence. Giữ `CHANGELOG`/Project Update/version/owners ACK khi thay đổi liên phân hệ.

## 15. Risk register, nghiệm thu và phương án fallback

| Rủi ro | Dấu hiệu | Mitigation/owner | Điều kiện STOP |
|---|---|---|---|
| Board N16R8 không khớp DevKitC header | Gắn GPIO đúng số nhưng pin header sai | ET1+EE2 xác nhận ảnh/schematic trước power | Chưa xác minh mapping |
| ACS724 5 V vượt ADC/không đạt tuyến tính | VOUT ngoài phạm vi, clipping/nonlinear | ET1: front-end/protection, EE2: calibration ADC1 | Không nối ADC trước level safety |
| Hall open-drain bị kéo lên 5 V | Logic > allowed | ET1: pull-up 3.3 V/module audit | 5 V trên GPIO |
| Shared I²C + OLED gây missed FIFO | FIFO overrun, frame timing/jitter xấu | EE2: priority + DMA/FIFO + OLED low priority; ET1: pull-up/rise | Unreported sample loss |
| Filter làm mất fault peaks/harmonics | 1×/2×/crest down bất hợp lý | MS2+IT2: preservation tests; compare raw | Không có evidence tín hiệu được bảo toàn |
| 50 feature quá nặng/không tính được | RAM/latency hoặc long-term data missing | IT2+EE2: tier/subset, keep missing flags | Không ép 50 MCU features |
| Nhầm GT hoặc dữ liệu | FFT/model gán nhãn hoặc reclass validation | ME1+IT2: provenance/version/human gate | Dataset/source conflict |
| Fixture chưa hoàn thiện | Mounting không lặp, safety chưa rõ | ME1 tiếp tục CAD/fixture task | Không motor-on/dataset |
| Paper không đúng part/condition | Công thức offset/thermal từ sensor khác | MS2 kiểm datasheet/suffix và đo ref | Không áp hệ số không nguồn |
| Train/test leakage | windows overlapping cùng recording 2 tập | IT2 split group, test disjoint | Không công bố metric |

## 16. Template báo cáo cuối tuần và đánh giá chéo

**Mỗi task nộp theo mẫu:**

```text
Task ID / owner / co-owner / PR URL / latest SHA:
Goal / acceptance / relevant approved contract versions:
Artifacts changed (paths), Handoff consumers:
Board/module/fixture/setup versions & data_origin:
What was DEFINED / SIMULATED / MEASURED / INFERRED:
Tests EXECUTED: command/bench procedure; PASS/FAIL, raw log/ref:
Tests NOT RUN: exact reason, blocking effect, who follows up:
Measured errors, uncertainty, feature preservation (MS2):
Raw timing/loss, electrical level, flash/heap/stack evidence (ET1+EE2):
Feature registry status (count DEFINED/PC_TESTED/EDGE_TESTED/NOT_COMPUTABLE):
Open findings: BLOCKER/MAJOR/MINOR/QUESTION, owner, action:
Cross-owner ACK: ET1+EE2 / MS2+IT2 / EE2+IT2:
Human Gate approvals (if applicable): approver, scope, timestamp, evidence_ref:
Final status: PASS / PARTIAL / BLOCKED / NOT RUN — no fabricated value.
```

**Week-end quyết định ME1:** hoàn thành từng G01–G09 theo evidence, giữ issues chưa giải quyết, phân biệt “50 feature công thức đã chốt” với “50 feature đo đúng trên board”. Nếu bất kỳ phụ thuộc nào `BLOCKED`, đưa theo dõi Week 05; không tuyên bố model đã xác định % hỏng.

## 17. Các quyết định mở cần ME1 phê duyệt **trước khi triển khai thành lệnh chính thức**

| Decision ID | Nội dung cần chốt | Trạng thái khi soạn draft |
|---|---|---|
| **D01** | Ngày Week 04 bắt đầu/kết thúc, deadline theo cặp và chế độ PR/ACK | `PENDING ME1` |
| **D02** | Ảnh 2 mặt ESP32-S3 N16R8, PCB revision/full module ID và GPIO↔header | `PENDING ET1+EE2`, GPIO chỉ ứng viên |
| **D03** | Exact ACS724 suffix, current range, mức nguồn/mạch primary, phép đo ADC conditioning, voltage rails | `BLOCKED UNTIL ELECTRICAL REVIEW` |
| **D04** | Exact OLED type (I²C/SPI), voltage, pin names/address; DHT22/ADXL345/MAX31865 breakout markings, US5881 package/pull-up | `PENDING ET1` |
| **D05** | Fs/ODR/filter/window for vibration/current, min pulse/thermal horizon, time-sync tolerance, quantization | `PENDING MS2+IT2+EE2` |
| **D06** | Motor healthy physics check, RPM/load conditions, mounting/version, guard/E-stop, fixture acceptance status | `PENDING ME1`, MOTOR-ON BLOCKED |
| **D07** | Reference instruments, calibration points, acceptance tolerances, min repetitions, reference/uncertainty budget | `PENDING MS2+ET1+ME1` |
| **D08** | Feature subset edge, 50 registry validation policy, model evaluation/ablation protocol | `PENDING MS2+IT2+ME1` |
| **D09** | Contract migration/Update ID, ME1 approval scope, EF/SM ACK, hiệu lực | `PENDING CROSS-OWNER ACK` |
| **D10** | Nền tảng firmware/toolchain thực đang sử dụng (ESP-IDF / PlatformIO / Arduino), flash partition/PSRAM reported | `PENDING EE2` |

## 18. Nguồn tham khảo gốc và nội dung MS2 phải kiểm lại

**Nguồn kỹ thuật/nhà sản xuất (cho part và pin safety; không phải đo kiểm của nhóm):**

- [R1] Espressif, **ESP32-S3 Series Datasheet** — GPIO/ADC/strapping/USB/PSRAM cautions. https://documentation.espressif.com/esp32_s3_datasheet_en.pdf
- [R2] Espressif, **ESP32-S3-DevKitC-1 Hardware/User Guide** — pinout **chỉ cho DevKitC-1**, phân biệt PCB thực. https://documentation.espressif.com/esp-dev-kits/en/latest/esp32s3/esp32-s3-devkitc-1/user_guide_v1.1.html
- [R3] Espressif, **ESP-IDF ADC Oneshot/Calibration Guides** — ADC1 config, calibration và noise, ADC2/Wi-Fi conflict. https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/api-reference/peripherals/adc/adc_oneshot.html ; https://documentation.espressif.com/projects/esp-idf/en/latest/esp32s3/api-reference/peripherals/adc/adc_calibration.html
- [R4] Analog Devices, **ADXL345 Datasheet Rev G; AN-1025 FIFO** — ODR, scale, interrupt, FIFO 32 sample sets. https://www.analog.com/media/en/technical-documentation/data-sheets/adxl345.pdf ; https://www.analog.com/en/resources/app-notes/an-1025.html
- [R5] Allegro MicroSystems, **ACS724/ACS725 family** — ACS724 thường cấp 5 V; *phải tra đúng part suffix/full datasheet* cho sensitivity/range. https://www.allegromicro.com/en/products/sense/current-sensor-ics/integrated-current-sensors/acs724-5
- [R6] Analog Devices, **MAX31865 Datasheet/Product** — SPI, fault, RTD code/reference resistor, PT100 2/3/4-wire. https://www.analog.com/media/en/technical-documentation/data-sheets/MAX31865.pdf
- [R7] Melexis, **US5881 Datasheet** — 3.5–24 V, open-drain, package UA/SE polarity. https://www.melexis.com/-/media/files/documents/datasheets/us5881-datasheet-melexis.pdf
- [R8] Aosong, **AM2302/DHT22 Datasheet** — minimum ~2 s polling, humidity/temp timing. https://blog.eletrogate.com/wp-content/uploads/2019/01/Datasheet_DHT22_AM2302.pdf

**Nghiên cứu/metrology (gợi ý đọc; MS2 phải tự tìm, đọc chính văn và so sánh, không bê nguyên thuật toán):**

- [R9] F. Kibrete, D. E. Woldemichael & H. S. Gebremedhen, **Multi-Sensor data fusion in intelligent fault diagnosis of rotating machines: A comprehensive review**, *Measurement*, vol. 232 (2024), Article 114658, DOI: https://doi.org/10.1016/j.measurement.2024.114658 . Dùng để lập bản đồ phương pháp fusion, không coi là chứng minh model nhóm PASS.
- [R10] **Vibration-Based Anomaly Detection for Induction Motors Using Machine Learning** (2025), bài công khai về thống kê rung động và anomaly detection: https://pmc.ncbi.nlm.nih.gov/articles/PMC11820584/ . MS2 xác minh author/venue/DOI từ bài trước khi đưa vào bibliography chính thức.
- [R11] NIST, **Technical Note 1297 / Uncertainty Budget** — repeatability, calibration uncertainty Type A/B và reporting: https://www.nist.gov/pml/nist-technical-note-1297 ; https://www.itl.nist.gov/div898/handbook/mpc/section5/mpc56.htm . Không lấy uncertainty của chuẩn thành accuracy đo nhóm.

> Các liên kết trên là **nguồn khởi đầu đã được kiểm tra sơ bộ để thiết kế task**, không phải “đã đọc toàn văn 11 paper”, không thay báo cáo literature của MS2. Nhóm cần lưu date accessed, version/revision, module part number, assumptions và những bài peer-reviewed mới thực sự dùng.

## 19. Trạng thái xuất bản và lịch sử

- **Version:** `W04_README_DRAFT_0.1` (proposal). `Approved protocol version`, `feature schema version`, `pinmap version` và `effective date` vẫn `PENDING`.
- **GitHub:** **CHƯA TẠO FILE TRÊN REPO; CHƯA COMMIT/PUSH/PR/MERGE.** Đây là bản chuẩn bị cho ME1 kiểm tra.
- **Không có dữ liệu đo/thí nghiệm, test trên board, mô hình huấn luyện hoặc Human Gate nào được tự tuyên bố PASS trong tài liệu này.**
- Khi ME1 duyệt: rà D01–D10 → sửa thành task chính thức/đủ acceptance → tạo branch tài liệu từ `dev` sạch → PR `dev` → review Codex + owners + Human Gate → merge → phát Project Update mức B/C với điều kiện hiệu lực và ACK.
