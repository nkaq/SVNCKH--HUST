# Pull Request — SVNCKH--HUST

> PR mặc định merge vào `dev`. Chỉ tạo PR `dev → main` sau integration test/milestone review.

## 1. Thông tin task

**Member:** ME1 / MS2 / EE2 / ET1 / IT2  
**Issue:** Closes #___  
**Branch:** `feature/...` / `fix/...` / `docs/...`  
**Subsystem:** mechanical / sensors / firmware / electronics / data-ml / docs  
**Target:** `dev` / `main`  
**Setup / Fixture version (nếu liên quan):**  
**Firmware / dataset version (nếu liên quan):**

## 2. Mục tiêu

Mô tả 2–5 câu:
- vấn đề cần giải quyết;
- lý do cần thay đổi;
- output mong muốn.

## 3. Thay đổi cụ thể

- [ ] Code
- [ ] CAD / mechanical
- [ ] Electronics / wiring
- [ ] Experiment / metadata
- [ ] Data / DSP / ML
- [ ] Documentation
- [ ] Slide / figure / asset

**Files / folders chính đã thay đổi:**

```text
path/to/file
path/to/folder
```

**Tóm tắt:**
- 
- 
- 

## 4. Cách tái hiện / sử dụng

Viết đủ để thành viên khác có thể chạy lại.

Ví dụ firmware:

```bash
# board / environment
# build
# flash
# run
```

Ví dụ Python:

```bash
python ...
```

Ví dụ CAD/electronics:
- file nguồn;
- version;
- drawing/export;
- assumptions cần biết.

## 5. Test & Evidence

### Test đã chạy

- [ ] Build/compile PASS
- [ ] Unit/script test PASS
- [ ] Hardware bench test
- [ ] 60 s acquisition/stress test
- [ ] Sample-count/timestamp QC
- [ ] Plot/FFT/figure checked
- [ ] CAD/drawing consistency checked
- [ ] Electronics interface checked
- [ ] Data leakage/reproducibility checked
- [ ] Không áp dụng

### Evidence

Ghi kết quả thật, không dự đoán:

```text
Command/test:
Result:
Artifact/log/figure:
```

Ảnh/plot nên lưu tại `assets/` hoặc `results/`, không chèn file tạm không truy vết.

## 6. Ground Truth / Data impact

- [ ] Không ảnh hưởng Ground Truth / dataset
- [ ] Có thay đổi metadata schema
- [ ] Có thay đổi acquisition format
- [ ] Có thay đổi fixture/sensor mounting
- [ ] Có thay đổi label/protocol
- [ ] Có thể tạo domain/setup mới

Nếu có, mô tả:

```text
Impact:
Required version bump:
Migration / re-recording required:
```

## 7. Interface / Dependency

**Phụ thuộc vào:**
- ME1:
- MS2:
- EE2:
- ET1:
- IT2:

**Interface thay đổi:**
- pin mapping?
- packet/file schema?
- units?
- sampling rate?
- filenames?
- fixture geometry?
- connector?

Nếu có, cập nhật docs liên quan trước khi merge.

## 8. Codex Review

PR này phải được review theo `AGENTS.md` và:

```text
docs/project_management/CODEX_REVIEW_WORKFLOW.md
```

### Review focus đề nghị cho Codex

- 
- 
- 

### Codex result

- [ ] Chưa review
- [ ] Không có blocker
- [ ] Có blocker/major finding cần sửa
- [ ] Đã sửa và re-review
- [ ] Không áp dụng / lý do:

**Finding còn mở:**
- 

> Codex là reviewer kỹ thuật vòng 1. Kết quả AI phải được đối chiếu với code/test; không thay thế human gate cho safety, Ground Truth, architecture hoặc research claim.

## 9. Human Gate

Yêu cầu người thật review nếu PR ảnh hưởng:

- [ ] Mechanical safety / CAD / fabrication
- [ ] Electrical power / current / voltage
- [ ] Ground Truth / experiment protocol
- [ ] Dataset inclusion / exclusion
- [ ] System architecture
- [ ] Scientific claim
- [ ] Release `dev → main`
- [ ] Không thuộc nhóm trên

**Required reviewer:** ME1 / MS2 / EE2 / ET1 / IT2 / other

## 10. Merge Checklist

- [ ] Đúng folder/subsystem
- [ ] Branch cập nhật từ `dev`
- [ ] Commit message rõ
- [ ] Không có password/token/API key
- [ ] Không commit raw dataset lớn
- [ ] Binary lớn dùng Git LFS khi phù hợp
- [ ] README/docs cập nhật nếu interface thay đổi
- [ ] Test/evidence được ghi
- [ ] Không bịa measured result
- [ ] Ground Truth không suy từ FFT/model
- [ ] Codex findings đã xử lý hoặc được chấp nhận có lý do
- [ ] Human gate hoàn thành nếu cần

## 11. Reviewer Decision

```text
[ ] MERGE
[ ] FIX THEN REVIEW
[ ] HUMAN DECISION REQUIRED
[ ] DO NOT MERGE
```

**Reviewer note:**
