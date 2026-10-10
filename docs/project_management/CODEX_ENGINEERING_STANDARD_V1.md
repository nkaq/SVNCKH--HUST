# NCKH — Industrial Engineering & Clean Code Standard (v1)

> Bộ tiêu chí áp dụng khi **viết/sửa/review code**. Là chuẩn mục tiêu của nhóm, **không phải tuyên bố code hiện có đã đạt chuẩn hoặc đã có mọi công cụ CI**.

## 1. Nguyên tắc chung

- **Correctness > observability > testability > performance > style** khi đánh đổi. Không tối ưu nóng trước khi có profile/baseline.
- Thay đổi **tối thiểu, có thể review, có thể rollback**; tránh refactor ngoài phạm vi task, đổi API/schema trong âm thầm, hoặc thêm dependency không có lý do.
- Một module/hàm có trách nhiệm rõ ràng, input/output và ownership tường minh; tách hardware acquisition, DSP, persistence, dashboard và control logic.
- Không hard-code số vật lý hoặc unit conversion không rõ nguồn. Hằng số phải có tên, đơn vị, phạm vi hợp lệ và ghi bản revision/interface liên quan.
- API/packet/CSV phải ghi schema/version, unit, timestamp semantics, invalid/missing values và backwards compatibility. Với breaking change phải có dependency approval.
- Có xử lý lỗi và quan sát lỗi; không nuốt exceptions hoặc silently drop samples; lỗi phân biệt transport, sensor, timing và validation.
- Tên biến/hàm và comment mô tả **ý nghĩa**, không chỉ giải thích cú pháp. Comment các giả định, ràng buộc vật lý, lý do đặc biệt; không viết comment trùng code.
- Bảo mật: không commit key/token/secret, không log dữ liệu nhạy cảm; path/input từ ngoài phải validate, file output phải tránh ghi đè vô thức.
- Mọi thay đổi phải đi kèm test/evidence tương ứng hoặc ghi lý do chưa thể test. Không biến test mô phỏng thành kết luận về thiết bị thực.

## 2. Firmware ESP32-S3 / C++ — tiêu chí review

| Nhóm | Điều kiện cần xác minh |
|---|---|
| Timing | Lấy mẫu không phụ thuộc OLED/Serial; nêu rõ producer/consumer, block time và deadline nếu có |
| FIFO/buffer | Bound checks, overflow policy, dropped-sample accounting, data races và head/tail logic |
| ISR | Không cấp phát động, không gọi IO/blocking không phù hợp trong ISR; shared state atomic/critical section theo nền tảng thật |
| Timestamp | Clock source, rollover, monotonic/sample index, đồng bộ các modality và ý nghĩa microsecond |
| Transport | CRC/checksum khi protocol yêu cầu; packet version, framing, lost/corrupt frames, recovery |
| Driver | ADXL345 cấu hình ODR/range/bus được log; I2C/SPI timeout, disconnect, status, retry có giới hạn |
| State machine | Đối chiếu các trạng thái, transitions, timeout, recovery và failure behavior với **firmware contract / system architecture version đã được phê duyệt**. Không áp đặt một chuỗi trạng thái cố định cho mọi phiên bản. |
**Lưu ý về State Machine:**

Chuỗi `INIT → SELF_CHECK → READY → STABILIZE → RECORD → STOP → QC/ERROR` chỉ là ví dụ từ nhiệm vụ Week 3 / Acquisition Firmware v2.

Đây không phải tiêu chuẩn bắt buộc cho mọi phiên bản firmware.

Khi review, Codex phải:
- Đọc firmware contract và architecture version đang có hiệu lực.
- Kiểm tra transitions, timeout, recovery và xử lý lỗi theo contract đó.
- Phát hiện trạng thái không thể truy cập, deadlock hoặc chuyển trạng thái không hợp lệ.
- Không yêu cầu khôi phục state cũ nếu phiên bản kiến trúc mới đã được người phụ trách phê duyệt.

Nếu contract thiếu hoặc mâu thuẫn, yêu cầu EE2/ME1 xác nhận thay vì tự đặt lại state machine.
| Resources | Không giả định RAM, stack, heap, flash, time; nếu claim đã đo phải dẫn đo thật trên target build |
| Tests | Unit/host-side logic khi có thể; build trên board configuration đúng; bench/stress test chỉ PASS với log thật |

**Không quy định một sampling rate/timeout/buffer size cố định tại đây**. Chỉ đưa giá trị từ task được ME1/EE2 phê duyệt và khả năng sensor thực tế, đo/ghi lại mỗi lần thay đổi.

## 3. Python / DSP / ML — tiêu chí review

- Tách IO/parsing, transformations/features, model training/evaluation và presentation; dùng config thay vì path/magic constant cố định.
- Data contracts gồm sample rate, đơn vị vật lý, kênh, timing, NaN/invalid, RPM/load và fixture/setup version.
- Deterministic seeds và version package khi phù hợp; log config, split identifiers, feature version, model version, metrics, code revision.
- Train/validation/test phân theo recording/setup hoặc nhóm độc lập đúng mục đích khoa học; không chia ngẫu nhiên các window chồng lấn giữa tập.
- Fit scaler/imputer/selection/normalization trên train, tái dùng trên val/test; tuyệt đối không fit từ test.
- Reference PC vs edge DSP phải mô tả cùng định nghĩa feature, windowing, units, rounding và tolerance **được phê duyệt**, không tự nhận equivalence.
- ML metrics luôn có split/protocol, đơn vị thống kê, class balance, confusion matrix hoặc evidence phù hợp; không cherry-pick.
- Nếu dùng static tooling như Ruff, pytest, mypy hay C++ lint/build, chỉ chạy khi toolchain/config có thật. Đề xuất bổ sung qua PR riêng khi chưa được thiết lập.

## 4. Electronics / mechanical / simulation / docs

- Kiểm BOM-drawing-schematic-wiring consistency, versioning và assumptions; ghi rõ simulation khác measurement.
- Không phán đoán an toàn điện/cơ khí chỉ từ text; cần bench/assembly/fabrication review bởi owner.
- File CAD/binary phải có nguồn/version/export và tài liệu có thể đọc/review; đừng giả vờ đã kiểm geometry nếu không có công cụ.
- Không sinh dữ liệu thí nghiệm giả để thỏa điều kiện PASS.

## 5. Các mức độ finding

- **BLOCKER:** mất an toàn có căn cứ; vi phạm Ground Truth/dataset boundary; leakage nghiêm trọng; làm hỏng dữ liệu hoặc khóa project architecture.
- **MAJOR:** bug/chênh contract/không xử lý dropped samples/recovery/validity; test hoặc traceability thiếu ở thay đổi có ảnh hưởng lớn.
- **MINOR:** bảo trì, clarity, style có tác động; khuyến nghị công cụ hóa trong CI khi cơ học.
- **QUESTION:** chưa đủ chứng cứ; yêu cầu owner giải thích, không suy đoán.

Mỗi finding: `file:dòng` → bằng chứng → vì sao sai → hậu quả → bản sửa đề xuất → test cách kiểm chứng; **tiếng Việt**, giữ nguyên identifier/code.

## 6. Definition of Done (code task)

- Task ID và acceptance criteria rõ, phạm vi thay đổi không vượt scope.
- Contract/units/schema/version được bảo vệ; rủi ro liên phân hệ đã thông báo.
- Test phù hợp đã chạy với output có thật (hoặc báo `NOT RUN`, lý do và tác động).
- Diff không có secret, data raw/private, code demo blocking hay file không liên quan.
- Nếu phần cứng/GT/dataset/architecture/safety/research claim bị ảnh hưởng: Human Gate trước khi merge.
- Người làm đã tự review, có reviewer độc lập và PR hướng vào `dev`.
- Nếu Codex hoạt động: phiên review phải hoàn tất và findings có căn cứ phải được xử lý trước khi merge.
- Nếu Codex không khả dụng: PR phải ghi rõ lý do, bằng chứng và kết quả human review độc lập thay thế.
- Human Gate bắt buộc vẫn phải hoàn tất dù Codex có hoạt động hay không.
- Không coi Codex Review Completed hoặc repo-quality PASS là bằng chứng tự động về tính đúng đắn khoa học hay an toàn phần cứng.

