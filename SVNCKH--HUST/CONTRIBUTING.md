# Contributing

## Nguyên tắc
- Không push trực tiếp lên `main`.
- Task phát triển đi qua `feature/*`.
- Merge qua Pull Request.
- Không commit password, token, Wi-Fi credentials, API key.
- Không commit raw dataset lớn.
- Không commit build/cache.
- Thay đổi pin mapping / dataset schema / fixture geometry phải review.

## Branch naming

```text
feature/me1-fixture-v01
feature/ms2-sensor-health
feature/ee2-acquisition-v2
feature/et1-interface-board
feature/it2-edge-dsp
fix/ee2-i2c-timeout
docs/week3-report
```

## Commit

```text
type(scope): message
```

Ví dụ:

```text
feat(firmware): add acquisition state machine
fix(adxl345): handle I2C timeout
feat(mechanical): add adjustable motor plate
docs(week3): update integration report
feat(ml): add FFT features
data(metadata): update experiment schema
```

## Pull Request checklist
- [ ] Đã cập nhật branch từ `dev`
- [ ] Không chứa secret
- [ ] Không chứa raw dataset lớn
- [ ] Code build/run được
- [ ] Có test/log/plot/ảnh khi cần
- [ ] Docs được cập nhật nếu interface thay đổi
- [ ] Dependency đã ghi rõ
