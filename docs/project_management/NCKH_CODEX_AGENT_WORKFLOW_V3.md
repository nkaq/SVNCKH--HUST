# NCKH — CODEX CODING AGENT WORKFLOW V3
## Hướng dẫn dùng Codex trong VS Code cho ME1 / MS2 / EE2 / ET1 / IT2

> **Mục đích:** chuẩn hóa cách cả team dùng Codex Coding Agent trong VS Code để đọc repo, hỗ trợ code/tài liệu, chạy kiểm tra và chuẩn bị thay đổi trước khi nộp Pull Request.
>
> **Không bao gồm:** quy trình nộp bài/Pull Request chi tiết và Codex GitHub Code Review. Hai phần đó có tài liệu riêng.

---

# 1. VAI TRÒ CỦA CODEX TRONG DỰ ÁN

Codex là:

```text
Coding assistant
Technical reviewer vòng đầu
Repository-aware agent
```

Codex có thể:

```text
READ
ANALYZE
PLAN
EDIT
RUN TESTS
CHECK DIFF
SUGGEST FIXES
```

Codex **không phải** người quyết định cuối cùng cho:

```text
mechanical safety
electrical safety
Ground Truth
dataset inclusion/exclusion
system architecture changes
research claims
release to main
```

Các quyết định trên cần Human Gate.

---

# 2. CODEX PHẢI ĐỌC AGENTS.md

Trước khi sửa file, Codex phải đọc:

```text
/AGENTS.md
```

và nếu làm trong subsystem thì đọc thêm scoped instruction tương ứng.

Ví dụ:

```text
firmware/esp32s3/
→ /AGENTS.md
→ /firmware/AGENTS.md
```

```text
ml/
→ /AGENTS.md
→ /ml/AGENTS.md
```

```text
experiments/
→ /AGENTS.md
→ /experiments/AGENTS.md
```

```text
hardware/electronics/
→ /AGENTS.md
→ /hardware/electronics/AGENTS.md
```

```text
hardware/mechanical/
→ /AGENTS.md
→ /hardware/mechanical/AGENTS.md
```

---

# 3. TRƯỚC KHI MỞ CODEX

Luôn bắt đầu từ `dev` mới nhất:

```bash
git switch dev
git pull origin dev
```

Sau đó tạo branch riêng cho task.

Ví dụ:

```bash
git switch -c feature/ee2-adxl345-acquisition
```

Không dùng Codex để sửa trực tiếp trên:

```text
main
dev
```

---

# 4. QUY TẮC BRANCH THEO THÀNH VIÊN

## ME1

```text
feature/me1-...
docs/me1-...
```

Ví dụ:

```bash
git switch -c feature/me1-fixture-v01
```

## MS2

```text
feature/ms2-...
experiment/ms2-...
```

Ví dụ:

```bash
git switch -c feature/ms2-sensor-health
```

## EE2

```text
feature/ee2-...
fix/ee2-...
```

Ví dụ:

```bash
git switch -c feature/ee2-adxl345-acquisition
```

## ET1

```text
feature/et1-...
hardware/et1-...
```

Ví dụ:

```bash
git switch -c feature/et1-interface-board
```

## IT2

```text
feature/it2-...
fix/it2-...
```

Ví dụ:

```bash
git switch -c feature/it2-edge-dsp
```

---

# 5. PROMPT KHỞI ĐỘNG CHUNG CHO MỌI TASK

Dùng prompt này trước khi Codex sửa file:

```text
Read the repository instructions before making changes.

1. Read the root AGENTS.md.
2. Read the scoped AGENTS.md that applies to the files in this task.
3. Inspect the relevant existing files and repository structure.
4. Summarize:
   - task understanding;
   - files you expect to modify;
   - risks;
   - tests/checks you plan to run.
5. Do not modify files yet.
6. Do not change locked project hardware, Ground Truth, dataset policy,
   or repository architecture unless explicitly requested.
7. Do not invent measured results.
```

Chỉ khi plan hợp lý mới cho Codex sửa.

---

# 6. PROMPT CHO CODEX BẮT ĐẦU SỬA

Sau khi xem plan:

```text
Proceed with the implementation.

Constraints:
- Modify only files needed for this task.
- Follow root and scoped AGENTS.md.
- Keep changes minimal and reviewable.
- Do not edit main/dev branch history.
- Do not commit, push, merge, force-push, or delete branches.
- Do not add secrets or large raw datasets.
- Run relevant tests/checks after editing.
- At the end, report:
  1. files changed;
  2. what changed;
  3. tests run;
  4. actual results;
  5. remaining risks or human-gate items.
```

---

# 7. ME1 — CODEX WORKFLOW

## Primary folders

```text
hardware/mechanical/
docs/architecture/
experiments/protocols/
hardware/bom/
docs/plans/
```

## Prompt mẫu ME1

```text
Act as a technical assistant for ME1.

Read:
- /AGENTS.md
- /hardware/mechanical/AGENTS.md
- relevant architecture/protocol files.

Task:
<describe task>

Focus on:
- fixture versioning;
- BOM/drawing consistency;
- sensor mounting repeatability;
- alignment references;
- acceptance criteria;
- traceability to experiment protocol.

Do not claim physical safety from repository text alone.
Flag all items requiring ME1 human CAD/fabrication review.
```

---

# 8. MS2 — CODEX WORKFLOW

## Primary folders

```text
experiments/sensor_characterization/
results/
ml/drift_detection/   [when applicable]
```

## Prompt mẫu MS2

```text
Act as a sensor-characterization assistant for MS2.

Read:
- /AGENTS.md
- /experiments/AGENTS.md
- /ml/AGENTS.md if ML/drift files are involved.

Task:
<describe task>

Requirements:
- Never fabricate measurements.
- Keep measured data separate from hypotheses.
- Do not infer Ground Truth from FFT/model output.
- Preserve recording/setup metadata.
- Report assumptions and missing evidence explicitly.
```

---

# 9. EE2 — CODEX WORKFLOW

## Primary folder

```text
firmware/esp32s3/
```

## Prompt mẫu EE2

```text
Act as an embedded firmware assistant for EE2.

Read:
- /AGENTS.md
- /firmware/AGENTS.md

Task:
<describe task>

Hardware is locked unless explicitly changed:
- ESP32-S3
- ADXL345
- ACS724
- PT100 + MAX31865
- US5881 Hall
- DHT22
- OLED

Focus on:
- non-blocking acquisition;
- FIFO/ring-buffer safety;
- timestamp/sample-index monotonicity;
- I2C/SPI/ADC error handling;
- dropped-sample accounting;
- memory use;
- logging/OLED not disturbing sampling.

Do not silently change ODR, range, pins, packet schema,
sampling rate, or timestamp semantics.
```

---

# 10. ET1 — CODEX WORKFLOW

## Primary folders

```text
hardware/electronics/
hardware/bom/
```

## Prompt mẫu ET1

```text
Act as an electronics documentation assistant for ET1.

Read:
- /AGENTS.md
- /hardware/electronics/AGENTS.md

Task:
<describe task>

Check:
- voltage rails;
- interface levels;
- power/ground;
- pull-ups;
- ADC/input ranges;
- connector pinout;
- ACS724 interface;
- PT100 + MAX31865 interface;
- US5881 interface;
- DHT22 interface;
- EMI/cable-routing documentation.

Flag all assumptions requiring bench verification.
Do not declare electrical safety without human measurement.
```

---

# 11. IT2 — CODEX WORKFLOW

## Primary folders

```text
ml/
software/
data/
results/
```

## Prompt mẫu IT2

```text
Act as the data/ML/TinyML assistant for IT2.

Read:
- /AGENTS.md
- /ml/AGENTS.md
- /experiments/AGENTS.md if experiment metadata is involved.

Task:
<describe task>

Requirements:
- split by recording, not random windows;
- never fit preprocessing using test data;
- labels must come from physical Ground Truth;
- preserve reproducibility;
- record random seeds/configs when relevant;
- report model/resource cost for edge deployment;
- do not invent benchmark results.

Run relevant unit/script tests and report actual results.
```

---

# 12. SAU KHI CODEX SỬA XONG

Yêu cầu Codex tự review lại diff:

```text
Review your own changes against the repository instructions.

Check:
1. root AGENTS.md;
2. applicable scoped AGENTS.md;
3. task requirements;
4. unintended file changes;
5. test coverage;
6. unsupported assumptions;
7. human-gate requirements.

Return:
- Summary
- Blockers
- Major findings
- Minor findings
- Tests/evidence checked
- Human gate required?
- Recommendation
```

---

# 13. CON NGƯỜI KIỂM TRA DIFF TRƯỚC KHI COMMIT

Sau Codex, thành viên phải tự chạy:

```bash
git status
git diff
```

Kiểm tra:

```text
Codex có sửa nhầm file không?
Có thay hardware locked không?
Có thêm dữ liệu lớn không?
Có secret/token không?
Có bịa test result không?
Có file ngoài subsystem không?
```

---

# 14. CODEX KHÔNG ĐƯỢC TỰ PUSH TRONG WORKFLOW TEAM

Mặc định team dùng:

```text
Codex edits
Human reviews
Human commits
Human pushes
Human opens PR
```

Không dùng:

```text
Codex → direct push dev
Codex → direct push main
Codex → merge PR automatically
```

---

# 15. CÁC LỆNH GIT DO CON NGƯỜI CHẠY

Sau khi review thay đổi:

```bash
git status
git add <files>
git commit -m "type(scope): message"
git push -u origin <branch-name>
```

Phần tạo PR/nộp bài được mô tả trong tài liệu riêng.

---

# 16. CẤM / HẠN CHẾ

Codex không được tự ý:

```text
git push --force
git reset --hard
git clean -fd
git rebase --onto
delete branches
rewrite shared history
```

Không được:

```text
commit passwords
commit API keys
commit tokens
commit raw/private large datasets
```

Không được thay đổi:

```text
ADXL345 → sensor khác
ESP32-S3 → MCU khác
Ground Truth definition
Dataset v0.1 criteria
research scope
```

nếu task không yêu cầu và Human Gate chưa duyệt.

---

# 17. HUMAN GATE

Bắt buộc người thật quyết định nếu Codex đụng tới:

```text
Mechanical safety
Electrical voltage/current compatibility
Ground Truth
Dataset inclusion/exclusion
System architecture
Scientific claims
Release dev → main
```

Codex chỉ được:

```text
flag
analyze
suggest
```

không được coi là final authority.

---

# 18. DEFINITION OF DONE CHO MỘT CODEX TASK

```text
[ ] Đúng feature branch
[ ] Codex đã đọc AGENTS.md
[ ] Chỉ sửa đúng subsystem
[ ] Human đã xem git diff
[ ] Relevant tests/checks đã chạy
[ ] Test result là kết quả thật
[ ] Không có secret/raw large data
[ ] Không có unsupported claim
[ ] Human Gate đã xác định nếu cần
[ ] Commit message rõ ràng
```

---

# 19. FLOW CHUẨN

```text
dev mới nhất
    ↓
feature branch
    ↓
Codex đọc AGENTS.md
    ↓
Codex lập plan
    ↓
Human kiểm plan
    ↓
Codex edit
    ↓
Codex test
    ↓
Codex self-review
    ↓
Human git diff
    ↓
Human commit
    ↓
Human push
    ↓
Pull Request → dev
```

---

# 20. PROMPT NHANH DÙNG HÀNG NGÀY

```text
Read root and applicable scoped AGENTS.md first.

Task:
<task>

Before editing:
- inspect relevant files;
- state your plan;
- list expected files to modify;
- list risks and tests.

Then wait for approval.

After approval:
- make minimal changes;
- run relevant tests;
- do not commit/push/merge;
- do not invent measured results;
- flag human-gate items.

At the end provide:
Summary
Files changed
Tests run
Actual results
Remaining risks
Human gate required?
```

---

# 21. NGUYÊN TẮC CUỐI

Codex giúp team:

```text
làm nhanh hơn
kiểm tra kỹ hơn
giảm lỗi lặp lại
giữ repo nhất quán
```

nhưng không thay thế:

```text
engineering judgement
physical measurement
experiment Ground Truth
human responsibility
```
