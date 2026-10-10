# Thư viện prompt Codex Review — Tiếng Việt

Dùng các prompt dưới đây khi cần review sâu theo nhiệm vụ. Root `AGENTS.md` và scoped `AGENTS.md` vẫn là nguồn chỉ dẫn chính. **Mọi lời nhận xét lỗi và đề xuất sửa viết bằng tiếng Việt**; giữ nguyên code, tên biến, đường dẫn và P1/P2.

## EE2 — Firmware ESP32-S3

```text
Review PR theo AGENTS.md và firmware/AGENTS.md.
Kiểm sampling timing, timestamp, sample index, FIFO/ring buffer,
mất mẫu, ISR/shared state, I2C/SPI error recovery, RAM/stack,
Serial/OLED blocking trong đường lấy mẫu.
Chỉ rõ P1/P2, file:dòng, bằng chứng, rủi ro, hướng sửa khả thi,
và bài test cần chạy. Không phỏng đoán kết quả đo.
Giải thích hoàn toàn bằng tiếng Việt.
```

## IT2 — DSP, ML, TinyML và dữ liệu

```text
Review PR theo AGENTS.md và ml/AGENTS.md.
Kiểm data leakage khi chia train/val/test theo recording/setup,
cửa sổ chồng lấn, preprocessing chỉ fit trên train,
Ground Truth/metadata, metric, random seed, reproducibility,
và resource cost trên ESP32-S3.
Nêu P1/P2, file:dòng, lỗi, nguyên nhân, cách sửa và cách kiểm chứng.
Không suy nhãn từ FFT hay model. Trả lời bằng tiếng Việt.
```

## MS2 — ADXL345 và Sensor Health

```text
Review calibration, sensor characterization và sensor-health.
Kiểm đơn vị, offset, baseline, bias/noise/sensitivity/drift,
thống kê, metadata, nhiệt độ/ngữ cảnh và độ tin cậy chứng cứ.
Phân biệt sensor drift, domain shift và đo đạc với giả thuyết.
Với mỗi lỗi: mức độ, file:dòng, tác động, hướng sửa, test cần thêm.
Trả lời bằng tiếng Việt.
```

## ET1 — Electronics

```text
Review theo AGENTS.md và hardware/electronics/AGENTS.md.
Kiểm schematic/pinout, power/ground, voltage compatibility,
pull-up, ADC range, ACS724, PT100/MAX31865 và US5881.
Nêu các điểm phải bench-test; không tuyên bố an toàn khi
chưa có thực nghiệm. Ghi P1/P2, file:dòng, bằng chứng
và phương án sửa bằng tiếng Việt.
```

## ME1 — Mechanical, Protocol và Integration

```text
Review theo AGENTS.md và hardware/mechanical/AGENTS.md.
Kiểm fixture/setup version, BOM/drawing, mounting, alignment,
fault creation, acceptance criteria và traceability.
Ground Truth phải do nhóm chủ động áp đặt điều kiện vật lý.
Engineering Validation Data không được đổi thành Dataset v0.1.
Nêu P1/P2, file:dòng, tác động, cách sửa và yêu cầu Human Gate
bằng tiếng Việt. Không bịa kết quả CAD/bench-test.
```
