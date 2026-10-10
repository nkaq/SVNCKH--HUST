# NCKH — TEAM SUBMISSION WORKFLOW V3
## Nhận nhiệm vụ theo kế hoạch tuần → làm việc → nộp bài lên GitHub

**Áp dụng cho:** ME1, MS2, EE2, ET1, IT2 trong repository [`nkaq/SVNCKH--HUST`](https://github.com/nkaq/SVNCKH--HUST).  
**Đường dẫn chuẩn trong repo:** `docs/project_management/NCKH_TEAM_SUBMISSION_WORKFLOW_V3.md`  
**Đối tượng đọc:** Tất cả thành viên và người review.  
**Quy định chính:** `docs/plans/weekXX/README.md` là nguồn **giao nhiệm vụ theo tuần**; GitHub Pull Request (PR) là nơi **nộp và nghiệm thu**. Không bắt buộc tạo GitHub Issue cho mỗi task.

> Tài liệu này hướng dẫn thao tác nộp bài, không thay thế `AGENTS.md`, `CONTRIBUTING.md`, `PULL_REQUEST_GUIDE.md`, `NCKH_MEMBER_WORKSPACE_FILETYPE_GUIDE_V3.md`, hay tiêu chuẩn đo/kiểm định thực tế.

---

## 1. Quy trình tổng thể

```text
ME1 đăng kế hoạch tuần: docs/plans/weekXX/README.md (trên dev)
                      ↓
Thành viên mở README tuần, đọc task của mình và tiêu chí PASS
                      ↓
git switch dev → git pull origin dev → tạo feature branch
                      ↓
Làm artifact trong folder subsystem đúng vai trò
                      ↓
Codex đọc AGENTS.md → hỗ trợ sửa → chạy kiểm tra phù hợp
                      ↓
Người làm xem git diff, kiểm output và evidence
                      ↓
git add đúng file → git commit → git push
                      ↓
GitHub PR: feature branch → dev (ghi mã task + dẫn README tuần)
                      ↓
repo-quality + CODEOWNERS + Codex review (nếu hoạt động)
                      ↓
Khắc phục feedback → push cùng branch → kiểm tra lại
                      ↓
Người có trách nhiệm nghiệm thu; Human Gate nếu cần
                      ↓
Squash merge vào dev → cập nhật trạng thái task tuần
```

`main` là branch ổn định/milestone; **không nộp bài tuần trực tiếp vào `main`**.

---

## 2. Nhận task ở đâu?

1. Vào GitHub repo `nkaq/SVNCKH--HUST`.
2. Đổi branch sang **`dev`** (không đọc phiên bản cũ trên `main`).
3. Mở `docs/plans/weekXX/README.md` của tuần đang triển khai.
4. Tìm mục theo vai trò `ME1 / MS2 / EE2 / ET1 / IT2` và mã task. Nếu README chưa ghi rõ mã task, deadline, output hay tiêu chí nghiệm thu thì **hỏi ME1 trước khi làm**, không tự suy đoán yêu cầu.

### Nội dung tối thiểu ME1 cần đăng trong README mỗi tuần

| Trường | Ý nghĩa |
|---|---|
| `Task ID` | Ví dụ: `W03-EE2-01` (mã do ME1 chốt, không tự chế ra sau khi nộp) |
| `Owner` | Thành viên chịu trách nhiệm chính |
| `Goal` | Mục tiêu cụ thể |
| `Deliverables` | File/folder và định dạng phải nộp |
| `Acceptance criteria` | Những điều kiện để xác định PASS/FAIL |
| `Deadline` | Ngày/giờ nếu ME1 quy định |
| `Dependencies` | Thành viên/hardware/schema cần phối hợp |
| `Evidence` | Test, ảnh, CSV, plot, biên bản đo, log… phải có |

**Ví dụ minh họa mã task**, không thay thế nội dung kế hoạch tuần thật:

```text
W03-EE2-01: Acquisition Firmware v2
Task source: docs/plans/week03/README.md
Owner: EE2
Folder: firmware/esp32s3/
Evidence: build/test results, timestamp gaps, dropped-sample accounting
```

Không đặt sản phẩm code/CAD chính trong `docs/plans/`; folder này chỉ giữ kế hoạch.

---

## 3. Folder nộp theo thành viên

| Thành viên | Phạm vi chính | Artifact điển hình |
|---|---|---|
| **ME1** | `hardware/mechanical/`, `docs/architecture/`, `experiments/protocols/`, `hardware/bom/` | SolidWorks `.SLDPRT/.SLDASM/.SLDDRW`, `.STEP`, `.DXF`, `.md`, BOM `.csv` |
| **MS2** | `experiments/sensor_characterization/`, `results/`; phối hợp `ml/drift_detection/` | ADXL345 calibration, `.py`, `.ipynb`, `.csv`, plot `.png/.svg`, report `.md` |
| **EE2** | `firmware/esp32s3/` | `.cpp`, `.h`, `.ino`, firmware test, acquisition/logging evidence |
| **ET1** | `hardware/electronics/`, `hardware/bom/` | Schematic, wiring, pinout, `.kicad_sch/.kicad_pcb` (nếu dùng KiCad), `.pdf`, BOM |
| **IT2** | `ml/`, `software/`, `data/`, `results/` | `.py`, `.ipynb`, `.json/.yaml`, feature scripts, models `.tflite`, plots, benchmarks |

**Owner theo subsystem, không theo đuôi file.** Thành viên được chỉnh folder phối hợp nếu task cho phép và đã báo người chịu trách nhiệm phần đó. `members/` chỉ dành cho note/draft/report cá nhân, không phải nơi đặt artifact chính.

---

## 4. Chuẩn bị VS Code và tạo branch của task

Mở **đúng thư mục repository đã clone** (thư mục chứa `.git`, `README.md`, `AGENTS.md`). Trong PowerShell terminal:

```powershell
git status
git switch dev
git pull origin dev
git status
```

Nếu có file **modified/untracked không liên quan** (ví dụ `.github/codex/` hoặc `.github/workflows/codex-agent.yml` đang thử nghiệm), **không xóa/reset/`git add .` một cách mù quáng**. Chỉ tiếp tục khi đã xác định rõ các file này và không để chúng lọt vào PR task.

Tạo **một branch riêng cho một task hoặc nhóm artifact gắn chặt với nhau**:

```powershell
# Ví dụ EE2, thay mã/miêu tả task theo kế hoạch tuần thực tế
git switch -c feature/ee2-w03-acquisition-v2
```

Các branch tham khảo:

```text
feature/me1-w03-fixture-v01
feature/ms2-w03-adxl345-calibration
feature/ee2-w03-acquisition-v2
feature/et1-w03-interface-board
feature/it2-w03-edge-dsp
```

Tên branch dùng chữ thường, gạch nối; tránh khoảng trắng, dấu tiếng Việt. Không làm task mới trực tiếp trên `dev` hoặc `main`.

---

## 5. Làm task + Codex + kiểm chứng

1. Mở `AGENTS.md` ở root và `AGENTS.md` chuyên biệt nếu có (`firmware/`, `ml/`, `experiments/`, `hardware/mechanical/`, `hardware/electronics/`).
2. Đọc mục Goal, Deliverables và Acceptance criteria trong README tuần.
3. Dùng Codex **lập kế hoạch trước**, chỉ cho sửa sau khi thành viên xem plan. Codex không tự commit/push/merge.
4. Chạy kiểm tra thực sự phù hợp với artifact. Ghi `command / input / output / kết quả / giới hạn` hoặc bằng chứng vật lý.
5. Kiểm lại nội dung trước commit:

```powershell
git status
git diff
```

**Yêu cầu kỹ thuật đặc thù NCKH:**

- Không đổi mặc định ESP32-S3/ADXL345 và bộ sensor của hệ thống khi chưa có phê duyệt.
- **Ground Truth** bắt nguồn từ tình trạng vật lý áp đặt/quan sát có kiểm soát, không từ FFT hoặc dự đoán model.
- **Engineering Validation Data ≠ Dataset v0.1** khi fixture chưa đạt điều kiện nghiệm thu/version freeze.
- Không bịa kết quả đo, số mẫu, accuracy, latency, current, nhiệt độ hay benchmark.
- Với IT2, chia train/val/test theo **recording/setup** thích hợp; không để cửa sổ từ cùng recording rò rỉ qua các tập.
- Những quyết định về cơ khí an toàn, cấp nguồn/điện áp, Ground Truth, tập dữ liệu, kiến trúc hoặc scientific claim cần **Human Gate**.

---

## 6. Commit và push — ví dụ EE2

Chỉ add **đúng file của task** (không dùng `git add .` khi workspace còn file thử nghiệm):

```powershell
git status
git add firmware/esp32s3/
git diff --cached --stat
git diff --cached
git commit -m "feat(firmware): implement W03-EE2-01 acquisition v2"
git push -u origin feature/ee2-w03-acquisition-v2
```

Nếu bài khác, đổi path, commit message và branch cho phù hợp. Nếu `git diff --cached` hiện file lạ, bỏ staging file đó trước commit:

```powershell
git restore --staged path/to/unrelated-file
```

**Push chỉ tải branch lên GitHub, chưa phải nộp bài hoàn tất.** Phải tạo Pull Request.

---

## 7. Tạo Pull Request để nộp bài

GitHub repo → `Pull requests` → `New pull request`:

```text
base: dev
compare: feature/ee2-w03-acquisition-v2
```

Tiêu đề khuyến nghị:

```text
[W03-EE2-01] Acquisition Firmware v2 — timing, FIFO & logging
```

Trong PR template của repo, **điền thông tin thật**. Ít nhất cần:

```text
Task source: docs/plans/week03/README.md
Task ID: W03-EE2-01
Owner: EE2 (@dungnguyen13)
Subsystem: firmware/esp32s3/
Deliverables: liệt kê file thực nộp
What changed: thay đổi cụ thể
How to test/reproduce: hướng dẫn chạy
Test evidence: PASS/FAIL + bằng chứng có thật
Dependencies / changed interfaces: ghi rõ hoặc None
Human Gate: Needed / Not needed + lý do
```

**PR chỉ được merge khi bài đáp ứng acceptance criteria của kế hoạch tuần**, không phải chỉ vì nút merge đang xanh.

---

## 8. Review và sửa bài trên cùng PR

Các bước kiểm tra áp dụng như sau:

| Thành phần | Công dụng | Trạng thái áp dụng |
|---|---|---|
| `repo-quality` | GitHub Actions kiểm chất lượng repo | Check CI đã được triển khai |
| `CODEOWNERS` | Tự yêu cầu đúng người review theo subsystem | Đã hoạt động, ví dụ firmware → EE2 |
| Codex Code Review | Comment bug/rủi ro, viện dẫn AGENTS khi phù hợp | Có thể yêu cầu bằng `@codex review`; automatic All PRs đang chờ xác nhận cấu hình |
| Human review / Gate | Chốt vấn đề khoa học, cơ khí, điện, GT, dữ liệu, thay đổi kiến trúc | Không được thay bằng AI |

**Không coi Codex là required status check `Codex Agent Checked`** khi chưa triển khai được một GitHub Action riêng. Không coi GitHub Copilot Review là Codex Review.

Nếu reviewer báo lỗi:

1. Đọc finding (file/dòng, bằng chứng, mức độ).
2. Sửa **trên cùng feature branch**, không tạo PR khác chỉ để sửa bài cũ.
3. Test lại; kiểm `git diff`.
4. Commit + push:

```powershell
git add firmware/esp32s3/
git commit -m "fix(firmware): address acquisition review feedback"
git push
```

5. PR tự cập nhật. Kiểm CI mới, phản hồi reviewer, giải quyết conversation **sau khi lỗi được xử lý/đồng ý với lý do rõ ràng**.
6. Nếu ruleset yêu cầu branch up-to-date, có thể bấm `Update branch` trên PR, kiểm conflict và chờ CI chạy lại. Không force push.

**Không merge bài chỉ vì `repo-quality` PASS.** CI không tự xác nhận dữ liệu vật lý hoặc chất lượng khoa học.

---

## 9. Chốt bài, squash merge và đồng bộ máy

Ai merge: người có quyền và được ME1/nhóm phân công, theo ruleset; phải đạt acceptance criteria, kiểm tra feedback và Human Gate nếu áp dụng.

Với PR `feature → dev`, dùng **Squash and merge** theo quy định của repo. Không dùng PR `feature → main`.

Sau khi merge, thành viên chạy:

```powershell
git switch dev
git pull origin dev
git status
```

Có thể xóa branch local cũ khi chắc chắn PR đã merge và không có việc dở:

```powershell
git branch -d feature/ee2-w03-acquisition-v2
```

ME1 hoặc người được giao cập nhật trạng thái task vào **kế hoạch tuần qua PR riêng** nếu README cần bổ sung tiến độ. Các artifact chính vẫn nằm ở folder subsystem. Chỉ tạo PR `dev → main` khi chốt milestone/release, có kiểm thử tích hợp và Human Gate.

---

## 10. Lỗi thường gặp và cách xử lý

| Hiện tượng | Nguyên nhân hay gặp | Cách xử lý an toàn |
|---|---|---|
| `src refspec ... does not match any` | Chưa tạo branch đúng tên hoặc chưa có commit | `git branch --show-current`, `git log -1 --oneline`, kiểm tên branch rồi push lại |
| `Everything up-to-date` nhưng không tạo được PR | Branch không có diff so với `dev` | `git status`, `git log origin/dev..HEAD --oneline`, kiểm file đã commit chưa |
| `fetch first` / `non-fast-forward` | Remote đã thay đổi | `git fetch origin`, xem diff/log; không dùng force push |
| PR bị `out-of-date` | `dev` có commit mới và yêu cầu update | Dùng `Update branch`, xử lý conflict nếu có, đợi CI |
| `review required` | Ruleset branch đang yêu cầu approval | Nhờ người đúng quyền review; không lách ruleset |
| `conversation must be resolved` | Review thread chưa được xử lý | Sửa, trả lời và resolve sau khi thống nhất |
| GitHub không hiện file mới | Đang xem `main` thay vì `dev`, hoặc PR chưa merge | Kiểm dropdown branch + trạng thái PR |
| Commit chứa file thử nghiệm `.github/codex/` | Dùng `git add .` khi có file untracked | Trước commit: `git restore --staged path`; sau commit: hỏi ME1 trước khi sửa lịch sử |

Nếu lệnh Git trả lỗi không rõ, **dừng lại, chụp terminal và hỏi ME1** trước khi dùng `reset --hard`, `clean`, `rebase` hoặc force push.

---

## 11. Checklist nộp bài (thành viên tự tick)

- [ ] Đọc đúng `dev/docs/plans/weekXX/README.md` và ghi task ID thật
- [ ] Có Goal, Deliverables và Acceptance criteria đủ rõ
- [ ] Làm trên feature branch riêng, không phải `dev/main`
- [ ] File nằm đúng subsystem; có nguồn và định dạng hợp lệ
- [ ] Đã đọc AGENTS và dùng Codex đúng phạm vi
- [ ] Đã test/kiểm định phù hợp; evidence là kết quả thật
- [ ] Không commit secret, dữ liệu raw lớn, file test rác
- [ ] Đã xem `git status`, `git diff`, `git diff --cached`
- [ ] Đã commit + push + tạo PR **base = dev**
- [ ] PR ghi task source, task ID, deliverables, test, dependency
- [ ] Đã kiểm `repo-quality`, CODEOWNERS, Codex review khi có
- [ ] Đã xử lý findings và Human Gate nếu có
- [ ] Người có trách nhiệm chấp nhận bài trước khi Squash merge
- [ ] Sau merge đã đồng bộ local `dev`

---

## 12. Câu lệnh ngắn cho thành viên (nhớ thay branch/path thật)

```powershell
# 1) Đồng bộ
git status
git switch dev
git pull origin dev

# 2) Nhận task trong docs/plans/weekXX/README.md
# 3) Tạo branch
git switch -c feature/ee2-w03-acquisition-v2

# 4) Làm việc + test + tự kiểm
git status
git diff

# 5) Chỉ stage file thuộc task
git add firmware/esp32s3/
git diff --cached

# 6) Commit + push
git commit -m "feat(firmware): implement W03-EE2-01 acquisition v2"
git push -u origin feature/ee2-w03-acquisition-v2

# 7) Vào GitHub, tạo PR: feature branch → dev
# 8) Kiểm CI + review; sửa và push bổ sung nếu được yêu cầu
# 9) Sau khi merge
git switch dev
git pull origin dev
```

**Lưu ý cuối:** Người nhận bài cần đọc kỹ kế hoạch tuần gốc. README tuần là yêu cầu công việc; PR + files + evidence là bài nộp; `dev` là nơi tích hợp; `main` chỉ được cập nhật qua quy trình chốt milestone.
