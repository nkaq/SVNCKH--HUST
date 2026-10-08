# Pull Request Guide — Hướng dẫn đầy đủ cho thành viên

## Tại sao cần PR?

PR là nơi:
- mô tả thay đổi;
- lưu test/evidence;
- Codex review;
- human review;
- kiểm dependency;
- quyết định merge.

Không coi `git push` là hoàn thành task.

---

## Ví dụ 1 — EE2 làm ADXL345 acquisition

### Bước 1 — cập nhật dev

```bash
git switch dev
git pull origin dev
```

### Bước 2 — tạo branch

```bash
git switch -c feature/ee2-adxl345-acquisition
```

### Bước 3 — làm ở đúng folder

```text
firmware/esp32s3/drivers/adxl345/
firmware/esp32s3/acquisition/
firmware/esp32s3/tests/
```

### Bước 4 — test

Ví dụ phải ghi:
- target ODR;
- duration;
- expected sample count;
- actual sample count;
- timestamp gap;
- I2C error count;
- dropped sample count.

### Bước 5 — commit

```bash
git add .
git commit -m "feat(firmware): add ADXL345 buffered acquisition"
git push -u origin feature/ee2-adxl345-acquisition
```

### Bước 6 — tạo PR

GitHub:
`Pull requests → New pull request`

```text
base: dev
compare: feature/ee2-adxl345-acquisition
```

Điền toàn bộ template.

### Bước 7 — Codex review

Mở PR trong Codex Code Review và yêu cầu review theo `AGENTS.md`.

Prompt gợi ý nằm tại:
`docs/project_management/CODEX_REVIEW_PROMPTS.md`

### Bước 8 — sửa finding

Nếu Codex tìm ra lỗi:
- sửa ở cùng branch;
- test lại;
- commit mới;
- push;
- yêu cầu re-review.

### Bước 9 — merge dev

Chỉ merge khi:
- không còn blocker;
- test pass;
- human gate hoàn thành nếu cần.

---

## Ví dụ 2 — ME1 cập nhật fixture

Branch:

```bash
git switch dev
git pull origin dev
git switch -c feature/me1-fixture-v01
```

Lưu file:

```text
hardware/mechanical/solidworks/parts/
hardware/mechanical/solidworks/assemblies/
hardware/mechanical/solidworks/drawings/
hardware/mechanical/step/
hardware/mechanical/dxf/
hardware/mechanical/fabrication/
hardware/bom/
```

Commit:

```bash
git add .
git commit -m "feat(mechanical): add fixture v0.1 design"
git push -u origin feature/me1-fixture-v01
```

PR phải ghi:
- motor dimensions;
- adjustable range;
- sensor mounting reference;
- coupling/load assumptions;
- BOM;
- drawing/export;
- acceptance criteria.

Codex chỉ hỗ trợ consistency/traceability. ME1 vẫn phải review CAD và safety.

---

## Ví dụ 3 — IT2 thêm feature pipeline

Branch:

```bash
git switch -c feature/it2-feature-pipeline
```

Folder:

```text
ml/preprocessing/
ml/features/
ml/notebooks/
results/
```

PR phải ghi:
- recording split;
- window size;
- normalization;
- feature definitions;
- input/output schema;
- command chạy;
- metrics;
- leakage checks.

Codex review phải tập trung vào data leakage và reproducibility.

---

## Ví dụ 4 — ET1 sửa wiring

Folder:

```text
hardware/electronics/wiring/
hardware/electronics/schematics/
hardware/electronics/interface_board/
```

PR phải ghi:
- voltage level;
- connector;
- power source;
- signal direction;
- changed pinout;
- bench verification còn thiếu.

Không merge wiring quan trọng chỉ vì Codex nói OK.

---

## Ví dụ 5 — MS2 thêm sensor-health analysis

Folder:

```text
experiments/sensor_characterization/
results/figures/
results/tables/
```

PR phải ghi:
- raw source;
- temperature/orientation condition;
- units;
- statistics;
- plots;
- conclusion giới hạn ở evidence.

Không gọi proxy là real drift nếu chưa chứng minh.
