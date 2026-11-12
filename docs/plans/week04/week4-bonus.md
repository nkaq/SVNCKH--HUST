# week4-bonus — NCKH Week 04 Midterm Preparation & One-Day Integration Workshop

> **Tên tài liệu:** `week4-bonus.md`  
> **Dự án:** Adaptive Multimodal TinyML for Drift-Robust Predictive Maintenance on Resource-Constrained Edge Microcontrollers  
> **Chủ trì:** ME1 — `@nkaq`  
> **Ngày soạn:** 19/10/2026 (UTC+07:00)  
> **Trạng thái:** **DRAFT — ME1 REVIEW / CHƯA CÔNG BỐ ACTIVE**. Đây là **nhiệm vụ bổ sung**, không tự sửa hoặc thay thế `docs/plans/week04/README.md`.  
> **Đường dẫn đề xuất khi ME1 duyệt và đưa lên GitHub:** `docs/plans/week04/week4-bonus.md` trên `dev`, thông qua feature branch, PR và quy trình review/Project Update của nhóm.  
> **Thời gian thực hiện:** chuẩn bị linh hoạt trong thời gian thi giữa kỳ; **buổi workshop 01 ngày sau khi cả nhóm thi xong**, ngày/địa điểm/giờ cụ thể do ME1 chốt. **Không tự đặt hạn nộp theo lịch AI.**

---

## 0. Mục đích và quy tắc áp dụng

Kỳ thi giữa kỳ làm giảm khả năng triển khai tập trung. Thay vì ép cả nhóm lên lab, bốn thành viên **MS2, ET1, EE2, IT2** chuẩn bị những đầu ra nhỏ, có thể kiểm chứng và **có tác dụng trực tiếp trong một buổi họp kỹ thuật chung sau thi**. Mục tiêu không phải báo cáo “đã hoàn thành phần cứng/model” mà là **giảm thời gian tranh luận lại từ đầu**, xác định được quyết định nào cần chốt, còn thiếu thông tin gì, ai phụ trách xử lý.

1. **Không thay đổi phần cứng khóa:** ESP32-S3 **N16R8** (chưa xác định chắc PCB/header), ADXL345, ACS724, PT100 + MAX31865, US5881 Hall, DHT22 và OLED. Có **5 nhóm đo + 1 module hiển thị**.
2. **Không thay 50 đặc trưng bằng 20**: F01–F50 là danh mục **ứng viên** từ Week 04; chưa mặc định có 50 giá trị hợp lệ, 50 đặc trưng hữu ích hoặc 50 đặc trưng cùng chạy trên MCU.
3. **Tôn trọng quyền học và lịch thi**: thời lượng gợi ý **2–4 giờ/người, chia thành những phiên ngắn** để chuẩn bị *phần bắt buộc tối thiểu*; phần mở rộng làm khi có thời gian. Đây là **dự toán khối lượng**, không phải deadline hay xác nhận mức độ khó thực tế.
4. **Trong thời gian thi:** ưu tiên đọc tài liệu, lập bảng, vẽ sơ đồ ứng viên, viết test nhỏ **không cần board**. Không yêu cầu họp trực tiếp, chạy motor, hàn mạch, cấp nguồn, flash firmware hoặc train mô hình nặng.
5. **Nguồn ưu tiên:** `AGENTS.md`, `docs/plans/week04/README.md`, Project Master Context Week 04, các research/engineering standards và contract đang có hiệu lực trên `dev`. Khi mâu thuẫn, dừng và hỏi ME1 + owner chuyên môn; file bonus chỉ thêm việc chuẩn bị.
6. **Phân biệt bằng chứng:** `READ/DEFINED/PROPOSED`, `SYNTHETIC_TESTED`, `COMPILED`, `MEASURED`, `NOT_RUN`, `BLOCKED`, `APPROVED` phải ghi đúng. **Không dùng dữ liệu mô phỏng làm số đo sensor thật.**
7. **Raw/GT/dataset:** Ground Truth chỉ từ điều kiện vật lý do ME1 xác minh. Dữ liệu Engineering Validation không được hồi tố thành Dataset v0.1. Chưa có Fixture Acceptance PASS + freeze và Human Gate thì chưa có nhãn dataset nghiên cứu chính thức.
8. **Teamwork bắt buộc:** **MS2 ↔ IT2** đồng kiểm công thức/tính toán; **ET1 ↔ EE2** đồng kiểm pinout/sơ đồ điện/firmware; **EE2 ↔ IT2** đối chiếu data packet, timebase và feature feasibility trong workshop. Không tự tuyên bố đồng đội đã `READY`.

### 0.1. Quan hệ với Week 04 chính

| Nhóm công việc Week 04 | Nhiệm vụ bonus giúp chuẩn bị | Không tự thay thế |
|---|---|---|
| `W04-MS2-01` + `W04-PAIR-SM-01` | Bảng tham khảo, phép đo/sai số, filter và các câu hỏi khoa học | Báo cáo hiệu chuẩn có phép đo/reference thật, signal-preservation PASS |
| `W04-ET1-01` + `W04-PAIR-EF-01` | Candidate schematic, voltage/pinout risk checklist, BOM/part confirmation | Schematic đã duyệt, đo điện an toàn hoặc bench PASS |
| `W04-EE2-01` + `W04-PAIR-EF-01` | Rà PR #22, test plan, shared-bus/firmware architecture | Hardware build/stress/integration PASS hoặc quyền tự merge PR |
| `W04-IT2-01` + `W04-PAIR-SM-01` | Feature review, PC numerical test plan, schema và baseline design | Official Dataset v0.1, classifier đã train/đánh giá, PC↔ESP32 PASS |

**Điểm xuất phát đã đối chiếu GitHub ngày 19/10/2026:** `docs/plans/week04/README.md` đã merge `dev` qua PR [#21](https://github.com/nkaq/SVNCKH--HUST/pull/21), nhưng còn các góp ý Codex về F49 causal alignment, nguồn phổ rung F16–F24, RPM mean F35, timestamp rollover/reset và trạng thái xuất bản. PR firmware [#22](https://github.com/nkaq/SVNCKH--HUST/pull/22) của EE2 còn đang mở với nhận xét P1/P2 và câu hỏi I²C sharing. **Các lỗi/câu hỏi này là đầu vào thảo luận và kiểm thử, không coi như đã sửa.**

---

## 1. Tổng hợp nhiệm vụ bonus, owner và bàn giao

| Bonus Task ID | Owner | Co-review bắt buộc | Bài chuẩn bị trước workshop | Bàn giao cho ai |
|---|---|---|---|---|
| **W04-BONUS-MS2-01** | MS2 `@hquanvu12` | IT2 | Literature/Calibration Decision Sheet + Filter Preservation Questions | IT2, EE2, ET1 |
| **W04-BONUS-ET1-01** | ET1 `@tiendatvu13` | EE2 | Pinout Candidate & Electrical Risk Sheet + Schematic Sketch | EE2, ME1 |
| **W04-BONUS-EE2-01** | EE2 `@dungnguyen13` | ET1 + IT2 | PR #22 Fix/Test Plan + Embedded Integration Sketch | ET1, IT2, ME1 |
| **W04-BONUS-IT2-01** | IT2 `@lebach181207-dev` | MS2 + EE2 | Feature Audit & Numerical Test Plan + Baseline Design | MS2, EE2, ME1 |
| **W04-BONUS-WORKSHOP-01** | ME1 `@nkaq` | Cả 4 thành viên | 01 ngày họp để chốt vấn đề liên phân hệ và kế hoạch thử nghiệm | Cả nhóm / Week 04–05 handoff |

**Cách nộp đơn giản trước khi họp:** mỗi người chuẩn bị **1 tệp Markdown gọn, 1 bảng quan trọng, 3–5 câu hỏi phải được nhóm quyết định và link bằng chứng/tài liệu**. Có thể đính kèm hình/sơ đồ hoặc source test nếu thực sự có. **Không cần hoàn thiện code production mới đủ điều kiện tham gia buổi họp.**

---

## 2. MS2 — W04-BONUS-MS2-01: Từ lý thuyết cảm biến đến phương trình có thể kiểm chứng

**Câu hỏi phải trả lời:** “Tín hiệu của 5 nhóm cảm biến lệch do đâu? Phải đo bằng chuẩn nào mới chứng minh được việc hiệu chuẩn/lọc nhiễu hữu ích, mà vẫn giữ được dấu hiệu lỗi motor?”

### A. Việc bắt buộc có thể làm trong kỳ thi

1. **Lập bảng 5 nhóm cảm biến** với đúng part/module nếu đã biết: ADXL345; ACS724 (full suffix chưa xác minh); PT100/MAX31865; US5881; DHT22. Với mỗi nhóm: đại lượng vật lý, đơn vị gốc, lỗi thường gặp, model đo/sai số, cần instrument/reference gì, điều kiện đo nào chưa biết. Phân biệt phép đo và tính toán số liệu.
2. **Gắn công thức dự kiến có nguồn/giả định**: tối thiểu 01 mô hình hiệu chuẩn/bù sai số có ý nghĩa cho từng *nhóm đo*; ví dụ ADXL345 bias/scale, ACS724 zero-offset/sensitivity/ADC transfer, RTD resistance→temperature, Hall pulse interval→RPM, DHT22 checksum/freshness hoặc so sánh reference. Không ép DHT22 có một “công thức lọc” nếu không đủ căn cứ.
3. **Chọn 2 phương pháp xử lý tín hiệu cần so sánh** (ví dụ correction offset/gain với lọc FIR/IIR; hoặc notch với xử lý outlier) và lập bảng `benefit / damage risk / latency / memory / missing reference`. Khuyến nghị **có nhánh raw không lọc làm đối chứng**, đặc biệt với ADXL345 và ACS724.
4. **Chuẩn bị ít nhất 2 tài liệu nguồn có thể mở lại**: datasheet/app note đúng sensor và một paper/standard phù hợp (nếu tìm được). Với mỗi nguồn ghi URL/DOI, tên tài liệu, mục/trang, phương trình nào lấy từ đó, hạn chế ứng dụng; nếu mới đọc abstract phải ghi `ABSTRACT_ONLY`.
5. **Đọc góp ý Codex của PR #21 liên quan khoa học**: nguồn FFT F16–F24 và weighted RPM F35; chuẩn bị giải thích ngắn tại sao FFT của vector magnitude có thể làm sai phổ, tại sao tốc độ trung bình phải nêu rõ trọng số thời gian. Chưa sửa contract khi MS2 + IT2 chưa thống nhất.

### B. Phần mở rộng nếu còn thời gian

- Viết/pseudo-code một ví dụ **test synthetic** cho sine 1 trục, chuyển động tròn 2 trục, RPM đổi tốc độ. So sánh trước/sau xử lý; ghi `SYNTHETIC`, không khai là test sensor thật.
- Phác họa error budget Type A/Type B cho 1 sensor mà nhóm có thiết bị tham chiếu phù hợp.

### C. Deliverable và acceptance

**Tệp gợi ý:** `MS2_BONUS_CALIBRATION_REVIEW.md` (nháp cá nhân hoặc sau này đề xuất đưa vào `experiments/sensor_characterization/` bằng PR). Có 5 hàng sensor, ít nhất 2 nguồn có reference thật, bảng 2 thuật toán so sánh, 3 câu hỏi cho IT2/ET1/ME1.

**Đạt phần bonus khi:** công thức có đơn vị/giả định; không có thông số sensor tự đoán; có cách kiểm chuẩn trước/sau; không đề xuất “lọc cho tín hiệu giống healthy” bằng mọi giá. Trạng thái đo thực = `NOT_RUN` nếu chưa có thiết bị/log.

**Ba câu hỏi cần đưa ra workshop:** chọn sampling/bandwidth thế nào; loại filtering nào được phép trước feature F16–F24; cần thiết bị tham chiếu nào để đo accuracy thật.

---

## 3. ET1 — W04-BONUS-ET1-01: Bản đề xuất sơ đồ nối chân an toàn để cùng EE2 chốt

**Câu hỏi phải trả lời:** “Với ESP32-S3 N16R8 thực tế, 5 nhóm cảm biến + OLED sẽ dùng những rail nguồn, giao tiếp và chân nào, nguy cơ điện áp/EMI ở đâu?”

### A. Việc bắt buộc có thể làm trong kỳ thi

1. **Tạo bảng danh mục module thật (`BOM/PART ID CHECK`)**: ảnh/marking board ESP32-S3 N16R8 nếu có; part suffix ACS724, module ADXL345, MAX31865 jumper/RTD 2-3-4 dây, DHT22 breakout, US5881 package, OLED controller/I²C-or-SPI. Mọi phần chưa nhìn thấy phải ghi `UNKNOWN/TBD`, không suy từ N16R8 ra board DevKitC-1.
2. **Vẽ sơ đồ khối 6 ngoại vi**: ADXL345 (I²C/SPI), ACS724 (analog→front-end→ADC), PT100/MAX31865 (SPI), US5881 (GPIO event), DHT22 (digital single-wire), OLED (I²C/SPI theo module); đánh dấu rail 3V3/5V, GND, signal direction, pull-up, dòng tiêu thụ và nơi cần decoupling/protection.
3. **Đề xuất 01 `GPIO Candidate Matrix`** theo đúng header board sau khi có bằng chứng: `Module | Signal | Candidate GPIO | Bus/channel | Vlogic | Vpower | Pull-up/level shifter | Shared device | Source | Status`. Có thể dùng pinmap ứng viên từ README Week 04 **chỉ để khảo sát**, không phát lệnh đấu dây từ bảng đó.
4. **Lập bảng ít nhất 5 rủi ro điện quan trọng**: ACS724 output có thể không tương thích ADC 3V3 nếu thiếu front-end; Hall US5881 open-drain/pull-up, source voltage; I²C ADXL345–OLED bus chung; MAX31865 RTD wiring/RREF; DHT22 mức logic/pull-up/chu kỳ đọc; nguồn/ground/EMI khi motor ON.
5. **Gửi EE2 3 câu hỏi bắt buộc**: GPIO nào của board đang dành cho boot/USB/PSRAM; ai sở hữu và recover shared `Wire`/I²C; làm sao chứng minh OLED không gây treo hoặc chậm acquisition.

### B. Phần mở rộng nếu còn thời gian

- Vẽ sơ đồ nguyên lý draft trên KiCad/draw.io/Fritzing **nếu có công cụ phù hợp** và link source; không nộp hình AI tạo làm sơ đồ điện có thể lắp thật.
- Lập phương án kiểm bench từng module theo thứ tự `NO POWER → nguồn/logic đo độc lập → từng module → tích hợp`, có yêu cầu đồng kiểm ET1+EE2.

### C. Deliverable và acceptance

**Tệp gợi ý:** `ET1_BONUS_WIRING_REVIEW.md` kèm ảnh module (nếu được phép) và một schematic **DRAFT/NO POWER**. Bảng pinmap, voltage/logic matrix, risk list và ít nhất 3 câu hỏi dành cho EE2.

**Đạt phần bonus khi:** có cả 5 nhóm đo và OLED; không có chỉ dẫn nối 5V vào GPIO/ADC 3V3; phân biệt candidate vs approved pin; chưa có schematic thực thì ghi `PENDING` và đưa list cần xác minh. **Không yêu cầu tự cắm/hàn/motor-on khi đang thi.**

---

## 4. EE2 — W04-BONUS-EE2-01: Làm rõ rủi ro firmware trước khi tích hợp đa cảm biến

**Câu hỏi phải trả lời:** “Driver ADXL345 hiện tại có thể tạo dữ liệu đo giả hay trạng thái ready giả không? Cách tích hợp với OLED, cảm biến khác mà không nghẽn, không mất mẫu, không dùng nhiều RAM là gì?”

### A. Việc bắt buộc có thể làm trong kỳ thi

1. **Đọc PR [#22](https://github.com/nkaq/SVNCKH--HUST/pull/22) trên đúng commit hiện hành**, tổng hợp ba finding đã được review: **P1** `readFifo()/readRaw()` có nguy cơ trả `XYZ=0` hợp lệ khi không có hardware; **P2** `begin()/selfCheck()` có thể báo physical-ready khi ở nhánh mock; **QUESTION** `recover()` dùng `Wire.end()` global ảnh hưởng bus I²C chung với OLED. Viết `root cause → hành vi mong muốn → test tái hiện → cách sửa tối thiểu → ảnh hưởng API` cho từng finding.
2. **Lập 01 sơ đồ phân luồng firmware nhẹ**: sensor drivers (multi-rate) → event/FIFO/ISR ngắn → buffer giới hạn → packet+timestamp/quality flags → PC; OLED hiển thị ở task/interval ưu tiên thấp. Chỉ đề xuất buffer sizing sau khi đã xác định sample rate/packet size và có giới hạn RAM, không bịa số KiB.
3. **Thiết kế bảng `Measurement-vs-Mock`**: trạng thái absent, hardware-present, bus-error, simulation, self-check, FIFO-empty, timestamp invalid; trường hợp chưa có sensor thì **không phát mẫu như physical measurement**. Giữ raw source provenance.
4. **Đọc lỗi timestamp trên PR #21** và chuẩn bị 01 đề xuất contract về counter width, timebase, boot/session identity, rollover và reset (ví dụ `boot_id` như *candidate*, không tự sửa packet/schema đang phê duyệt). Thảo luận với IT2 trước khi code.
5. **Kiểm nhu cầu toolchain thật**: đã có PlatformIO/ESP-IDF/Arduino config trên branch chưa? Nếu chưa có thì ghi `BUILD NOT RUN` và đề xuất bước tạo config qua task/PR riêng; không ghi build/hardware PASS dựa trên project bring-up khác.

### B. Phần mở rộng nếu còn thời gian

- Nếu sẵn thời gian và **đang ở feature branch của PR #22**, có thể tự viết patch nhỏ và host-side test cho P1/P2, review cùng Codex local; human tự commit/push sau khi hiểu diff. **Không cần hoàn thành patch để tham gia workshop; không merge PR khi finding còn tồn tại**.
- Phác thảo xử lý lỗi I²C đồng sở hữu bus để trao đổi ET1; không tự đổi pinmap.

### C. Deliverable và acceptance

**Tệp gợi ý:** `EE2_BONUS_FIRMWARE_REVIEW.md`; cần bảng 3 finding/test cases, sơ đồ producer–consumer + OLED, bảng mock/physical status, 3 câu hỏi cho ET1/IT2.

**Đạt phần bonus khi:** phân biệt thật/giả rõ, không có thay đổi production pinmap trái phép; chỉ báo test đã chạy khi có command và log thật; nêu chính xác trạng thái PR #22, đặc biệt nếu vẫn chưa sửa/chưa build.

---

## 5. IT2 — W04-BONUS-IT2-01: Kiểm toán 50 đặc trưng và chuẩn bị bản PC reference có thể kiểm thử

**Câu hỏi phải trả lời:** “Trong F01–F50, công thức nào cần sửa nghĩa/kiểm điều kiện, đặc trưng nào tính được ngay, đặc trưng nào cần dữ liệu/clock/RPM/long-horizon và đặc trưng nào chỉ nghiên cứu offline?”

### A. Việc bắt buộc có thể làm trong kỳ thi

1. **Tạo một bảng `F01–F50 Feature Audit` đủ 50 ID** (không sửa đổi số lượng): `ID | Sensor | Time scale | Candidate Tier E/W/L/R | Need fs/sync/RPM? | Undefined condition | Reference test | PC/Edge plan | Status`. Có thể nhóm nhiều ID cùng đặc tính trong tài liệu giải thích, nhưng bảng registry vẫn phải đủ 50 hàng.
2. **Ưu tiên rà soát 4 vấn đề toán/timebase** từ PR #21: F16–F24 phải xem lại nguồn phổ từng trục/tổng PSD thay vì magnitude phi tuyến; F35–F40 phải nói rõ time-weighted RPM vs pulse-weighted; F49 ambient join phải causal last-valid với TTL hoặc đánh dấu offline; timestamp/sample-index rollover/reset phải có policy để dữ liệu window/feature không bị đảo thứ tự. Đây là **đề xuất sửa**, cần MS2/EE2 cùng xác nhận và ME1 duyệt khi thay shared contract.
3. **Thiết kế tối thiểu 4 test vector synthetic** để trình bày trong workshop: (i) sine một trục + chuyển động tròn, (ii) motor RPM chuyển 600→1200 rpm theo thời gian, (iii) DHT22 có dữ liệu trước/sau thời điểm nhiệt motor và TTL, (iv) counter rollover/reboot/sample gap. Mỗi test ghi input, expected property, `FAIL` symptom cũ, expected fix. Không giả làm dữ liệu motor đã đo.
4. **Lập `PC reference architecture` ở mức module**: decoder/quality → immutable raw → calibration config/version → causal time alignment → DSP/features → validity mask → healthy baseline theo RPM/load/setup. Chỉ fit scaler/selector bằng train; không random-window split gây leakage.
5. **Đề xuất tập con ưu tiên demo** (ví dụ nhóm time-domain + dòng/RPM/nhiệt) dựa trên data availability và CPU/RAM, **không khóa số lượng hay claim ESP32 đã đáp ứng**. Đồng review cùng MS2 về tính vật lý và EE2 về resource/packet.

### B. Phần mở rộng nếu còn thời gian

- Viết 1 script Python test synthetic ngắn, có assertion cho ít nhất 2/4 test vector. Nếu không có Python/toolchain thì cung cấp pseudocode và expected output không giả test đã chạy.
- Thiết kế giao diện feature validity/quality flags ở mức đề xuất, chưa đổi schema chính thức.

### C. Deliverable và acceptance

**Tệp gợi ý:** `IT2_BONUS_FEATURE_AUDIT.md` + bảng 50 dòng, 4 test cases, sơ đồ PC pipeline, top risks và 3 câu hỏi cho MS2/EE2.

**Đạt phần bonus khi:** đủ 50 feature IDs; không dùng dữ liệu tương lai cho edge; không ép missing vào `0`; không lấy accuracy trên Engineering Validation làm claim chính thức; không coi xác suất classifier là phần trăm hỏng vật lý.

---

## 6. Quy tắc nộp bài bonus trước khi cả nhóm gặp nhau

### 6.1. Gói bàn giao tối thiểu (mỗi thành viên 01 gói)

```text
TASK ID: W04-BONUS-<ROLE>-01
OWNER / REVIEW PARTNER:
FILE / LINK:
SUMMARY (tối đa 5 dòng):
WHAT IS GROUNDED? (nguồn/datasheet/PR/code):
WHAT IS A PROPOSAL? (giả định/chưa kiểm chứng):
TEST STATUS: NOT_RUN / SYNTHETIC_TESTED / COMPILED / MEASURED + evidence
TOP 3 QUESTIONS FOR THE ONE-DAY MEETING:
DEPENDENCY / OWNER / READY or BLOCKED:
```

- **Nháp cá nhân:** có thể chuẩn bị Markdown trên máy/Project mà chưa phải đẩy PR trong giai đoạn thi; **không upload dữ liệu raw lớn, secret, token, CAD/private files sai nơi**.
- **Khi được ME1 duyệt để đưa vào repo:** mỗi member tạo nhánh riêng từ `dev` sạch; sửa đúng subsystem được giao hoặc file handoff dưới task được duyệt; local Codex review nếu phù hợp; human kiểm diff và tự commit/push; mở PR vào `dev`, chờ `repo-quality` + findings/human review. Không sửa trực tiếp `dev`.
- **Không tự nhận bài đã nộp đủ/chưa đủ chỉ dựa vào GitHub**: nhóm có thể lưu nháp tại máy cá nhân, ME1 cần đối chiếu link thực tế.
- **Thiếu thông tin** (board photo, suffix ACS724, datasheet, motor fixture, instrument reference, clock, etc.) thì nộp một bảng `UNKNOWN / nguồn cần có / ai cung cấp / cần lúc nào` còn hữu ích hơn tự điền số.

### 6.2. Bộ tiêu chí đánh giá phần chuẩn bị

| Gate | Đạt khi | Nếu không đạt |
|---|---|---|
| B01 — Relevant | Nội dung phục vụ trực tiếp 01 task Week 04 và người nhận khác hiểu được | Yêu cầu nối lại với task/owner |
| B02 — Traceable | Có link/ID nguồn và phân biệt giả định vs đã đo | Giữ `PROPOSED`, chưa được ký `READY` |
| B03 — Complete | Có artifact tối thiểu + 3 câu hỏi rõ + trạng thái thiếu | `PARTIAL`, không ép thành viên bịa evidence |
| B04 — Cross-team | Câu hỏi đã hướng đúng reviewer cặp và interface | `AWAITING_PAIR_REVIEW` |
| B05 — Safe | Không tự chốt pin/điện motor/safety/GT/dataset; không thay code ngoài phạm vi | `BLOCKED — ME1/DOMAIN GATE` |

**Phần bonus hoàn thành ≠ G01–G09 của Week 04 đạt PASS.** B01–B05 đo **mức sẵn sàng cho cuộc họp**, không phải chất lượng phần cứng/model.

---

## 7. W04-BONUS-WORKSHOP-01 — HỌP KỸ THUẬT CHUNG 01 NGÀY SAU THI

> **Mục tiêu:** một ngày làm việc **5 người ME1 + MS2 + ET1 + EE2 + IT2** để chốt các interface còn mâu thuẫn, xác định điểm an toàn/truy vết, phân rõ ai code/đo/test gì sau buổi họp. **Không hứa sẽ giải quyết tất cả lỗi hay motor-on trong một ngày.**
>
> **Lịch bên dưới là khung gợi ý**, chỉ áp dụng khi ME1 và cả nhóm thống nhất ngày, địa điểm và điều kiện thiết bị. Nếu ai chưa sẵn sàng, ME1 đánh dấu `BLOCKED` và điều chỉnh.

| Khung giờ tham khảo | Chủ trì / tham gia | Nội dung phải giải quyết | Artifact trong buổi |
|---|---|---|---|
| 08:30–09:00 | ME1 + 4 người | Mỗi người mở gói bonus, xác nhận thiếu/đủ; chốt phạm vi/nguy cơ, không tự động `ACTIVE` | Attendance, agenda, blocker list |
| 09:00–10:15 | **ET1 + EE2** chủ trì | Board N16R8 thật, ACS724/ADC, Hall pull-up, ADXL345/OLED shared I²C, SPI RTD, DHT22, pin candidate | `Joint_Pinout_Decision_DRAFT`, power & risk matrix |
| 10:15–10:30 | Cả nhóm | Nghỉ giải lao | — |
| 10:30–11:30 | **EE2 + IT2** chủ trì | PR #22 P1/P2/mock/`Wire.end()`; packet/timestamp/boot/rollover; recording/QC handoff | Firmware defect disposition + Packet Contract TODO |
| 11:30–12:30 | Cả nhóm | Nghỉ trưa | — |
| 12:30–13:45 | **MS2 + IT2** chủ trì | Calibration/source/range/filter; bảo toàn 1×/2×; F16–F24, F35–F40, F49; validity của 50 features | `Feature_Correction_Decisions_DRAFT`, literature refs |
| 13:45–14:45 | **MS2 + IT2 + EE2** | PC reference test vectors, subset thử nghiệm, data sync, RAM/Flash/latency cần đo; baseline khỏe theo condition | Reference Test Matrix + PC↔Edge Handoff |
| 14:45–15:00 | Cả nhóm | Nghỉ giải lao | — |
| 15:00–16:00 | ME1 điều phối | Ghép hoàn chỉnh `fixture/mounting → circuit → firmware → raw/schema → calibration → F01–F50 → baseline`; xác định bước lab an toàn | Interface Dependency Map; G01–G09 status |
| 16:00–16:30 | ME1 + 4 người | Ra quyết định, phân task sau thi, chủ sở hữu, acceptance, thời hạn do người thật chốt, ACK | Meeting Minutes + Decision Log + `[PROJECT UPDATE]` proposal |

### 7.1. 10 quyết định cần đem ra thảo luận (không phê duyệt trước bằng file)

| ID | Quyết định cần có | Người chuẩn bị | Ai chốt/review |
|---|---|---|---|
| M01 | Đúng model/revision/header của board ESP32-S3 N16R8, danh mục module/ACS724 suffix/OLED controller | ET1 + EE2 | ET1+EE2, ME1 khi liên quan kiến trúc/an toàn |
| M02 | Power rails, I²C/SPI/GPIO candidate, ADC/front-end, Hall pull-up và sơ đồ đấu an toàn | ET1 + EE2 | ET1+EE2 + ME1 Human Gate liên quan |
| M03 | Quyền sở hữu shared I²C, OLED refresh, recovery và buffering khi mất kết nối | EE2 + ET1 | EE2+ET1 |
| M04 | Disposition 3 finding PR #22 và hướng test hardware/mock tách biệt | EE2 | EE2+ME1; reviewer độc lập cho PR |
| M05 | Packet/timebase, rollover, boot/session ID, sync và trường bắt buộc | EE2 + IT2 | EE2+IT2+ME1 nếu đổi contract |
| M06 | Phương trình hiệu chuẩn và reference instrument cho mỗi nhóm cảm biến | MS2 + ET1 | MS2+ET1+ME1 nếu đổi protocol |
| M07 | FFT/1×/2× spectral source, time-weighted RPM, causal F49; bốn lỗi toán/timebase Week 04 | MS2 + IT2 + EE2 | MS2+IT2; ME1 khi khóa shared definition |
| M08 | Feature subset PC/edge để thí điểm, validity/missing policy, test vectors | IT2 + MS2 + EE2 | IT2+MS2+EE2 |
| M09 | Healthy-reference condition, fixture, Ground Truth, phạm vi lab/motor-on | ME1 | **ME1 Human Gate**, người có chuyên môn an toàn nếu cần |
| M10 | Ngày làm lab sau thi, owner từng phần, deadline, PR, review và `[PROJECT UPDATE]` | ME1 + tất cả | ME1 công bố sau các approval có thật |

### 7.2. Kết quả buổi workshop bắt buộc bàn giao

1. **Meeting Minutes:** ai có mặt, link 4 bộ bonus, vấn đề đã bàn, ý kiến khác nhau, quyết định và vấn đề còn chờ.
2. **Decision Log M01–M10:** mỗi dòng `APPROVED / PROPOSED / BLOCKED / DEFERRED`, owner, điều kiện áp dụng, evidence, ngày/giờ xác nhận. **Không đánh dấu APPROVED chỉ bằng đồng thuận miệng thiếu hồ sơ** đối với Human Gate.
3. **Pair Handoff:** ET1+EE2 một pinout/schematic version; MS2+IT2 một feature/calibration specification; EE2+IT2 một packet/timebase interface; ghi `READY/BLOCKED` có lý do, không ký thay nhau.
4. **Action Plan tiếp theo:** Task ID, priority, owner, co-owner, expected output, evidence/test, dependency, due date do nhóm tự chốt sau thi; những nhiệm vụ Week 04 chưa PASS tiếp tục ở trạng thái mở, không “đóng tuần” để đẹp báo cáo.
5. **Project Update** nếu có thay đổi mức B/C: ME1 dẫn PR/version, scope, effective-from, người liên quan, yêu cầu ACK; không cho thay đổi contract/pinout an toàn tự có hiệu lực chỉ do file được tạo.

### 7.3. Mẫu biên bản ngắn để nhóm điền trong ngày

```markdown
# NCKH — Week 04 Bonus Workshop Minutes
Date/time (UTC+07:00): TBD
Location / mode: TBD
Chair: ME1 @nkaq
Participants: MS2 / ET1 / EE2 / IT2 (presence to confirm)
Inputs: MS2 link | ET1 link | EE2 link | IT2 link

| Decision ID | Decision / proposed change | Owner | Status | Evidence / approval ref | Next task |
|---|---|---|---|---|---|
| M01–M10 | TBD | TBD | PROPOSED/BLOCKED/APPROVED | PENDING | TBD |

Pair readiness:
- ET1 + EE2: READY / BLOCKED — evidence ref:
- MS2 + IT2: READY / BLOCKED — evidence ref:
- EE2 + IT2: READY / BLOCKED — evidence ref:

Unresolved risks:
- Hardware/power/fixture:
- Measurement/calibration/feature math:
- Firmware/acquisition/packet:
- Data provenance / QC / ML:

Next actions: owner + task ID + deliverable + evidence + human-approved deadline.
ME1 decision / Project Update ID / effective date: PENDING
```

---

## 8. Nhiệm vụ của ME1 trong thời gian thi và ngay sau thi

ME1 không bắt buộc hoàn thiện fixture ngay trong tuần thi. Trách nhiệm điều phối: xác nhận mọi người có thể học thi; thu bốn gói bonus khi họ sẵn sàng; xin ảnh/part number board và module nếu có; chuẩn bị phòng/thiết bị, hướng dẫn an toàn và các câu hỏi M01–M10; chốt ngày gặp **chỉ sau khi hỏi lịch thi của nhóm**. Nếu tài liệu liên quan fixture, CAD Onshape hoặc motor-on chưa có evidence, giữ `BLOCKED/PENDING`, không ép pair ET1–EE2 cấp nguồn hay chạy động cơ.

**Sau khi ME1 duyệt file này:** có thể đưa lên repo bằng PR tài liệu riêng, không cần sửa README gốc ngoài thay đổi bắt buộc có căn cứ. Việc merge bonus không tự biến các kế hoạch, pinout hay thực nghiệm đề xuất thành `APPROVED`.

---

## 9. Nguồn để từng bạn đọc đúng trước khi làm

- [Week 04 README trên `dev`](https://github.com/nkaq/SVNCKH--HUST/blob/dev/docs/plans/week04/README.md): task chính W04, 50 feature definitions, G01–G09, source refs.
- [PR #21 — Codex review Week 04](https://github.com/nkaq/SVNCKH--HUST/pull/21): 5 góp ý về tài liệu/toán/timebase cần kiểm lại theo code/contract.
- [PR #22 — EE2 ADXL345 modular driver](https://github.com/nkaq/SVNCKH--HUST/pull/22): review/mock/physical measurement/I²C bus, build `NOT_RUN` trên commit hiện hành theo phần mô tả PR.
- [`AGENTS.md`](https://github.com/nkaq/SVNCKH--HUST/blob/dev/AGENTS.md) và các scoped `AGENTS.md`: phạm vi owner, review và Human Gates.
- [Research Standard](https://github.com/nkaq/SVNCKH--HUST/blob/dev/docs/project_management/CODEX_RESEARCH_STANDARD_V1.md) và [Engineering Standard](https://github.com/nkaq/SVNCKH--HUST/blob/dev/docs/project_management/CODEX_ENGINEERING_STANDARD_V1.md): metadata/provenance/algorithm validation và firmware correctness.
- [Team Change/Project Update Workflow](https://github.com/nkaq/SVNCKH--HUST/blob/dev/docs/project_management/NCKH_TEAM_CHANGE_UPDATE_WORKFLOW_V1.md): cách thông báo, điều kiện ACTIVE, owner ACK.

**Tình trạng file:** Soạn để **ME1 duyệt và phân công bonus**; chưa tạo branch, commit, push, PR hay merge cho file này. Không tuyên bố các thành viên đã nhận việc, đã nộp bài, đã đồng ý ngày họp hoặc đã thực nghiệm.