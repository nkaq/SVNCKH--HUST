# NCKH — CẨM NANG CODE, CODEX SKILLS & TWO-STAGE AI CODE REVIEW

**Phiên bản:** V1.2 (bổ sung workflow CAD Onshape cho ME1, PR #20) — 13/10/2026  
**Dự án:** *Adaptive Multimodal TinyML for Drift-Robust Predictive Maintenance on Resource-Constrained Edge Microcontrollers*  
**Repository chính:** <https://github.com/nkaq/SVNCKH--HUST>  
**Đối tượng:** ME1, MS2, EE2, ET1, IT2; ưu tiên người trực tiếp viết code (EE2, IT2, MS2 và các phần code liên quan ET1/ME1).  
**Môi trường ví dụ:** Windows + VS Code + PowerShell + Git + Codex Extension; GitHub Codex Code Review tích hợp ChatGPT, không thiết lập OpenAI API key riêng.  
**Tính chất:** Cẩm nang vận hành bổ sung ở trạng thái **PROPOSED / CHỜ PHÊ DUYỆT VÀ CÔNG BỐ**. **Không phải** lệnh thay thế `AGENTS.md`, chính sách dữ liệu, task README hay quyền phê duyệt.

> **Đọc trước:** Đây là hướng dẫn *cách sử dụng* các Skills và quy trình hai vòng review. Việc tạo file Markdown này **không tự cài Skill mới**, không bật GitHub ruleset, không bổ sung unit test CI và không làm Codex tự chạy khi `git push`. Phần nào chưa được cấu hình phải được kiểm tra trước khi dùng.
>
> **Nguyên tắc không thương lượng:** Thành viên viết/duyệt code; Codex được phép phân tích, sửa file theo phạm vi đã duyệt và chạy test; **người thật tự `git add`, `git commit`, `git push`, mở PR và quyết định merge**. AI không phê duyệt thay Human Gate.
>
> **Trạng thái triển khai được theo dõi RIÊNG theo phiên bản:** **V1.1 (Two-Stage Codex Skills, PR #19)** có hồ sơ [`NCKH_TWO_STAGE_CODEX_WORKFLOW_PROJECT_UPDATE_PROPOSAL_V1.md`](NCKH_TWO_STAGE_CODEX_WORKFLOW_PROJECT_UPDATE_PROPOSAL_V1.md). **V1.2 (ME1 Onshape CAD migration, PR #20)** có hồ sơ [`NCKH_ME1_ONSHAPE_PROJECT_UPDATE_PROPOSAL_V1.md`](NCKH_ME1_ONSHAPE_PROJECT_UPDATE_PROPOSAL_V1.md) với scope CAD, owner, blockers, ngày hiệu lực và ACK độc lập. Hai hồ sơ đều có trạng thái **PROPOSED**, chưa đồng nghĩa các thành viên đã ACK hoặc workflow đã `ACTIVE`. Chỉ áp dụng bắt buộc phần tương ứng sau khi đủ review/human decision, merge, ME1 cấp Update ID, công bố ngày/điều kiện hiệu lực và kiểm tra `READY/BLOCKED` phù hợp. Trước đó dùng chuẩn hiện hành trong `AGENTS.md`, `CODEX_REVIEW_WORKFLOW.md` và README nhiệm vụ tuần đang ACTIVE; nếu mâu thuẫn về CAD, dừng task bị ảnh hưởng để ME1 giải quyết nguồn task.

---

## MỤC LỤC

1. [Quy trình một trang](#1-quy-trình-một-trang)
2. [Nguồn quy tắc và thứ tự ưu tiên](#2-nguồn-quy-tắc-và-thứ-tự-ưu-tiên)
3. [Vai trò và các ranh giới khoa học-kỹ thuật](#3-vai-trò-và-các-ranh-giới-khoa-học-kỹ-thuật)
4. [Hai vòng review hoạt động như thế nào](#4-hai-vòng-review-hoạt-động-như-thế-nào)
5. [Danh mục Skills và điều kiện dùng](#5-danh-mục-skills-và-điều-kiện-dùng)
6. [Chuẩn bị một máy lập trình](#6-chuẩn-bị-một-máy-lập-trình)
7. [Git: bắt đầu task, code tay, quản lý nhánh](#7-git-bắt-đầu-task-code-tay-quản-lý-nhánh)
8. [Vòng 1: Local Codex review, tự sửa và kiểm thử](#8-vòng-1-local-codex-review-tự-sửa-và-kiểm-thử)
9. [Git: human review, commit, push và Pull Request](#9-git-human-review-commit-push-và-pull-request)
10. [Vòng 2: Codex trên GitHub, CI và human gate](#10-vòng-2-codex-trên-github-ci-và-human-gate)
11. [Tiêu chuẩn kiểm chứng theo subsystem](#11-tiêu-chuẩn-kiểm-chứng-theo-subsystem)
12. [Thư viện prompt copy-paste](#12-thư-viện-prompt-copy-paste)
13. [Ma trận xử lý các trường hợp phát sinh](#13-ma-trận-xử-lý-các-trường-hợp-phát-sinh)
14. [Sổ tay xử lý lỗi Git và Codex](#14-sổ-tay-xử-lý-lỗi-git-và-codex)
15. [Mẫu báo cáo, checklist và tiêu chí hoàn tất](#15-mẫu-báo-cáo-checklist-và-tiêu-chí-hoàn-tất)
16. [Đường hướng mở rộng workflow và giới hạn](#16-đường-hướng-mở-rộng-workflow-và-giới-hạn)
17. [Nguồn đối chiếu và lịch sử cập nhật](#17-nguồn-đối-chiếu-và-lịch-sử-cập-nhật)

---

## 1. QUY TRÌNH MỘT TRANG

### 1.1. Quy trình chính: thành viên **code tay trước**, AI review sau

```text
Nguồn task hợp lệ: README tuần trên dev / PR finding / hạ tầng được duyệt
    |
    v
Thành viên sync dev sạch -> tạo feature branch riêng
    |
    v
THÀNH VIÊN TỰ CODE (EE2 / IT2 / MS2 / ...)
    |
    v
LOCAL CODEX REVIEW VÒNG 1 — VS Code
  - đọc root/scoped AGENTS.md + task/contract
  - chọn code-review; diagnosing-bugs nếu cần chẩn đoán lỗi
  - dùng tdd nếu cần thêm regression/unit tests
  - review -> đề xuất -> được phép sửa trên feature branch
  - chạy kiểm tra thật -> review lại diff -> báo evidence
    |
    v
CON NGƯỜI REVIEW DIFF, CHỌN FILE, COMMIT, PUSH
    |
    v
PULL REQUEST: feature/... -> dev
    |
    +--> GitHub Actions: repo-quality + các CI kỹ thuật đã thực sự cấu hình
    |
    +--> CODEX GITHUB REVIEW VÒNG 2 (độc lập phiên local)
    |      - đọc PR diff, AGENTS.md, các chuẩn liên quan
    |      - phản hồi bằng review/inline comments
    |
    +--> Reviewer con người + Human Gate theo lĩnh vực
    |
    v
Nếu có lỗi: IT2/EE2/... sửa TRÊN CÙNG feature branch -> test ->
             human commit/push thêm -> chờ review commit mới
    |
    v
CI đạt + findings có căn cứ xử lý + human approval -> squash merge dev
    |
    v
Tích hợp đa subsystem -> khi có milestone mới xét PR dev -> main
```

### 1.2. Quyết định nhanh: việc nào thuộc vòng nào?

| Yêu cầu | Nơi xử lý | Thao tác quyết định |
|---|---|---|
| Code tay xong, cần soi lỗi và tự sửa | Codex Extension trên máy | Dùng prompt **P03**, cho phép sửa sau khi xem plan |
| Bug chưa rõ nguyên nhân | Codex Extension | **P05**: điều tra trước, không vá đoán mò |
| Viết test để khóa regression | Codex Extension | **P06**: `tdd` phù hợp, chứng minh RED/GREEN nếu chạy được |
| Code đã ổn, sắp commit | Codex Extension | **P07** + `nckh-pr-preflight` |
| Mới push lên feature branch | Git/GitHub | Push **không tự kích hoạt Skills ở máy** |
| Mở PR vào `dev` | GitHub | CI theo workflow đã cấu hình; Codex GitHub Review theo quyền/trigger đã bật |
| Codex trên GitHub báo finding | IT2/EE2/... + VS Code | **P22**: kiểm tra finding, sửa local, push thêm |
| Codex GitHub im lặng/không chạy | GitHub + human | Xem trạng thái/giới hạn; dùng `@codex review` nếu hỗ trợ; fallback reviewer độc lập |
| Cần thay sensor, schema, Ground Truth, architecture | Owner + ME1 | **DỪNG và xin Human Gate** |

### 1.3. Lệnh tối thiểu dành cho một task mới

> Chạy **trong thư mục gốc repository**, PowerShell hoặc terminal của VS Code. Thay tên nhánh bằng tên task thật; không copy nguyên ví dụ cho hai task khác nhau. Chỉ dùng khi working tree sạch.

```powershell
# 1) Xác minh đang ở đúng repo và tình trạng sạch
git rev-parse --show-toplevel
git status --short
git remote -v

# 2) Lấy nhánh tích hợp mới nhất
git switch dev
git pull --ff-only origin dev

# 3) Tạo nhánh riêng cho đúng task, ví dụ IT2
git switch -c feature/it2-recording-qc

# 4) TỰ CODE trên VS Code, sau đó dùng Codex review/fix/test
#    (prompt P03/P05/P06/P07 ở Mục 12)

# 5) Human xem thay đổi trước khi stage
git status --short
git diff --check
git diff --stat
git diff

# 6) Stage đúng file, đường dẫn chỉ là ví dụ — xác nhận file tồn tại
# # git add -- ml/preprocessing/recording_qc.py
# git add -- ml/preprocessing/tests/test_recording_qc.py

# 7) Sau khi stage thật, xem lại trước commit
git diff --cached --check
git diff --cached --stat
git diff --cached

# 8) Human commit/push — CHỈ khi đã xác nhận nội dung stage
# git commit -m "test(it2): validate recording-level QC"
# git push -u origin feature/it2-recording-qc
```

Các dòng `# git ...` là **lệnh ví dụ được comment**, cố ý không chạy ngay; khi tới bước đó, bỏ `#` và thay bằng file/branch/message thực tế.

---

## 2. NGUỒN QUY TẮC VÀ THỨ TỰ ƯU TIÊN

Trước khi giao Codex việc quan trọng, bắt buộc đối chiếu nguồn **đang có trên nhánh `dev` mới nhất**; không coi cẩm nang này là nguồn cập nhật nhiệm vụ tuần.

| Nguồn trong repository | Chức năng |
|---|---|
| `AGENTS.md` | Chính sách toàn repository; review tiếng Việt; luật locked hardware, evidence, human gate |
| Scoped `AGENTS.md` như `ml/AGENTS.md`, `firmware/AGENTS.md`, `experiments/AGENTS.md`, `hardware/.../AGENTS.md` | Quy tắc chuyên môn theo vùng file, nếu có |
| `docs/plans/weekXX/README.md` | **Nguồn task chính thức của task tuần**; Task ID, owner, deliverable, acceptance/evidence |
| `docs/project_management/CODEX_PROJECT_CONTEXT_V1.md` và Project Master Context bản hiện hành | Bối cảnh, phiên bản, nguồn đúng |
| `docs/project_management/CODEX_ENGINEERING_STANDARD_V1.md` | Clean Code, interface, test, timing, embedded engineering |
| `docs/project_management/CODEX_RESEARCH_STANDARD_V1.md` | Data provenance, Ground Truth, dataset, reproducibility |
| `docs/project_management/CODEX_AUTOMATION_PLAYBOOK_V1.md` | Inspect -> Plan -> Implement -> Verify -> Review -> Handoff |
| `docs/project_management/CODEX_REVIEW_WORKFLOW.md` | GitHub Code Review, xử lý findings, reviewer độc lập, human fallback |
| `.github/CODEOWNERS` | Người phụ trách file và quy định review theo subsystem |
| `.github/PULL_REQUEST_TEMPLATE.md` | Form nộp Pull Request thực tế |
| `CONTRIBUTING.md` | Quy trình Git/PR/merge, format commit, review policy |
| `docs/project_management/NCKH_TEAM_CHANGE_UPDATE_WORKFLOW_V1.md` | Thông báo và xác nhận thay đổi liên nhóm, nếu áp dụng |

### 2.1. Nguồn task hợp lệ theo loại PR (không bịa Task ID)

Theo `.github/PULL_REQUEST_TEMPLATE.md` hiện hành:

| Loại PR | Nguồn bắt buộc ghi trong PR | Cách ghi Task ID |
|---|---|---|
| **Task tuần** | `docs/plans/weekXX/README.md` trên `dev`, tuần đúng hiệu lực | Dùng Task ID do ME1 giao |
| **Sửa finding review** | PR gốc và URL finding/commit được xác nhận | Dùng PR/finding ref; ghi `NOT APPLICABLE — review finding` nếu không có Task ID tuần |
| **Hạ tầng / tài liệu** | Quyết định hoặc yêu cầu hạ tầng được ME1 phê duyệt, PR/thảo luận/tài liệu nguồn có thể truy vết | Ghi `NOT APPLICABLE — infrastructure/docs` nếu không có Task ID tuần |
| **Milestone / release** | Quyết định integration/milestone, PR `dev → main`, approval và evidence | Ghi release/version hoặc task milestone đã được cấp; không chế ID |

Thiếu *nguồn nhiệm vụ được phê duyệt* hoặc acceptance criteria cốt lõi thì phải hỏi owner; nhưng **không** ép mọi PR có `WXX-ROLE-NN`. Một GitHub Issue không bắt buộc; nếu không có thì ghi `Issue: None`. Với thay đổi quy trình mức B/C, bổ sung Project Update và Human Gate theo chính sách trước khi áp dụng.

**Khi mâu thuẫn:** dừng, trích rõ hai nguồn và hỏi ME1/domain owner. Một prompt của người dùng hoặc một Skill bên thứ ba **không được phép âm thầm ghi đè** repository rules, kế hoạch tuần/contract đã phê duyệt, hoặc Human Gate. Nếu bản kế hoạch thay đổi, phải căn cứ `[PROJECT UPDATE]` và phiên bản có hiệu lực.

> **Cảnh báo lịch sử:** Có tài liệu ban đầu gọi GitHub Codex là “review vòng 1”; **trong cẩm nang này vòng được đánh số theo thời gian làm việc**: Local Codex = vòng 1, GitHub Codex = vòng 2. Đây chỉ là cách đặt tên, **không thay đổi thẩm quyền review**.

---

## 3. VAI TRÒ VÀ CÁC RANH GIỚI KHOA HỌC-KỸ THUẬT

### 3.1. Đúng người — đúng thư mục — đúng artifact

| Vai trò | Vùng sở hữu chính | Nội dung code/review cần quan tâm |
|---|---|---|
| **ME1** | `hardware/mechanical/`, `docs/architecture/`, `experiments/protocols/` | Fixture, Ground Truth, cơ cấu tạo fault, hợp đồng hệ thống, CAD/version/BOM |
| **MS2** | `experiments/sensor_characterization/` (+ phối hợp `ml/drift_detection/`) | ADXL345 calibration, self-test, bias/noise/sensitivity/drift, metadata và sensor health |
| **EE2** | `firmware/esp32s3/` | ESP32-S3, ADXL345 driver, FIFO/ISR/ring buffer, timestamp, dropped samples, logging, recovery |
| **ET1** | `hardware/electronics/`, `hardware/bom/` | Source schematic, nguồn, điện áp, pull-up, ACS724, PT100/MAX31865, US5881, DHT22 |
| **IT2** | `ml/`, `software/`, `data/`, `results/` | Parser/QC, DSP/features, ML/TinyML, leakage-safe split, PC dashboard, reproducibility |

**Cross-team:** `software/serial_logger/` IT2 + EE2; `ml/drift_detection/` IT2 + MS2; `hardware/bom/` ME1 + ET1; một số thư mục `results/` cần chủ sở hữu theo nguồn. Người viết Python chưa chắc là IT2: owner dựa trên **mục đích/subsystem**, không chỉ extension.

**Không để artifact production trong `members/<name>/`**; folder `members/` chỉ dành cho notes/draft. Không commit dữ liệu raw lớn vào `data/raw/`; sample/manifest phải phù hợp chính sách.

### 3.2. Những bất biến bắt buộc

- MCU **ESP32-S3**; accelerometer chính **ADXL345**; current **ACS724**; motor temperature **PT100 + MAX31865**; RPM **US5881 Hall + magnet**; ambient **DHT22**; display **OLED**. Không tự đổi phần cứng đã khóa.
- **Ground Truth** chỉ đến từ điều kiện vật lý được chủ động áp đặt và metadata có thể truy vết; **không** từ FFT/RMS/prediction/dashboard.
- **Engineering Validation Data không phải Dataset v0.1**; tuyệt đối không đổi `data_origin=ENGINEERING_VALIDATION` thành `RESEARCH_COLLECTION` để đưa recording vào dataset, kể cả đã QC PASS hoặc fixture freeze.
- Dataset inclusion phải có thu mới đúng protocol sau Fixture Acceptance/freeze, metadata, QC, physical Ground Truth và Human Gate đúng `approval_scope`; `PENDING`/`EXCLUDED` không phải thành viên dataset.
- Phân biệt `HYPOTHESIS`, `PLANNED`, `MEASURED`, `SIMULATED`, `INFERRED`, test thực sự đã chạy và test chỉ đề xuất.
- Không sửa im lặng ODR, sensor range, pinout, timestamp units, sample/packet/CSV schema, record ID, setup/fixture version, API contract, model architecture.
- Không dùng mô hình AI để tuyên bố an toàn cơ khí/điện, đo đạc phần cứng, công bố khoa học hoặc phê duyệt release.

### 3.3. Khi nào phải dừng xin Human Gate?

**Bắt buộc hỏi ME1 và/hoặc domain owner trước khi áp dụng thay đổi** liên quan: an toàn cơ khí; nguồn/điện áp/dòng; experiment protocol; Ground Truth và fault severity; dataset inclusion/exclusion; thay đổi system architecture/contract; nghiên cứu/scientific claims; `dev -> main` và các vấn đề tương đương. Nếu Codex thấy rủi ro, nó chỉ được **đánh dấu và đề xuất**, không tự quyết.

---

## 4. HAI VÒNG REVIEW HOẠT ĐỘNG NHƯ THẾ NÀO?

### 4.1. Vòng 1 — Codex Extension trên máy tính

- Đầu vào: toàn bộ file local trong workspace, task README, repository instructions, changed diff và test environment được cấp quyền.
- Có thể **sửa file local**, viết/chạy test, review lại, nhưng chỉ khi người thật cho phép theo phạm vi; phải tránh lệnh Git thay người.
- Có thể đọc Skills đặt ở user-level như `$HOME\.agents\skills` (trên máy đã cấu hình, Junction dẫn dữ liệu sang ổ D) và repo Skills `.agents/skills/`.
- Chất lượng phụ thuộc prompt, model lựa chọn, context, thực tế source; **không bảo đảm 100% tự kích hoạt**. Muốn chắc chắn, yêu cầu Codex báo `SKILL.md` đã thực sự đọc, hoặc gọi Skill phù hợp bằng UI/cú pháp extension đang hỗ trợ.
- Kết quả vòng 1 **chưa phải chứng nhận PR PASS**: nó sửa trên working tree, con người phải xem diff và thực hiện Git.

### 4.2. Vòng 2 — Official Codex GitHub Code Review

- Đầu vào: PR diff/metadata và quy tắc có trong repository GitHub, trong đó có `AGENTS.md` (và các tài liệu mà reviewer thực sự đọc).
- Kết quả: review summary, inline findings, hoặc không có nhận xét; Codex GitHub có thể phản hồi khi PR/commit có trigger và quyền thích hợp.
- **Không lấy Skills từ ổ D của thành viên**: máy chủ GitHub/phiên cloud không đọc được junction/local path. Không hứa `tdd`, `diagnosing-bugs`, `code-review` local đã chạy trong vòng 2.
- Review trên GitHub **không tự vá file local của thành viên**; nếu có cơ chế cloud agent chỉnh code, đó là một tác vụ khác phải được duyệt riêng. Quy trình nhóm ưu tiên sửa trên feature branch local rồi human push.
- `Codex Review: Completed` = **review đã hoàn tất**, **không có nghĩa PASS**; vẫn phải đọc findings và xác nhận chúng đã giải quyết trên **commit mới nhất**.
- Tích hợp Codex qua tài khoản **ChatGPT Plus** nếu tài khoản/gói đủ điều kiện; **không triển khai GitHub Action với `OPENAI_API_KEY`**. Hạn mức/5-hour usage là do sản phẩm quyết định, không phải không giới hạn hoặc bảo đảm reset số token cố định.

### 4.3. GitHub Actions là lớp kiểm chứng khác

Trên nhánh `dev` được rà soát ngày 12/10/2026, `.github/workflows/repo-quality.yml` chạy `python .github/scripts/repo_quality.py` khi có PR vào `dev`/`main`, và khi push trực tiếp lên `dev`/`main`. **Nó không tự động chạy pytest, compile firmware, hardware stress test, DSP-equivalence hoặc training benchmark.** Chỉ ghi PASS cho những job đã tồn tại và đã chạy thật. Việc bổ sung CI theo subsystem và branch protection là công việc riêng, phải thông qua PR/human review.

### 4.4. Trạng thái GitHub đã xác minh

- Repo `nkaq/SVNCKH--HUST` có `dev`, `main`, `CODEOWNERS` và `repo-quality.yml`.
- PR [#18](https://github.com/nkaq/SVNCKH--HUST/pull/18) và [#14](https://github.com/nkaq/SVNCKH--HUST/pull/14) ghi nhận reviewer `chatgpt-codex-connector`.
- PR #18 có `Codex Review Summary` với trạng thái `Completed` và trigger `New commits`; vì vậy kết nối/review trên GitHub **đã từng hoạt động**. Không suy ra mọi PR tương lai sẽ tự động được review đúng lịch.

---

## 5. DANH MỤC SKILLS VÀ ĐIỀU KIỆN DÙNG

### 5.1. Những Skills thực sự đã được kiểm chứng

| Skill | Nguồn/vị trí | Dùng khi nào? | Quyền hạn thực tế |
|---|---|---|---|
| `code-review` | Global Matt Pocock, user-level ngoài repo | Code tay đã xong; review diff; soi bug/regression | Đọc/check. Nếu muốn **sửa**, phải giao thêm tác vụ edit |
| `diagnosing-bugs` | Global Matt Pocock | Có lỗi bất thường, runtime failure, mismatch, lỗi khó tái hiện | Điều tra nguyên nhân trước khi sửa |
| `tdd` | Global Matt Pocock | Phát triển tính năng có testability; thêm regression test trước khi fix | RED -> GREEN -> REFACTOR nếu môi trường cho phép |
| `nckh-task-execution` | `.agents/skills/nckh-task-execution/` trong repo | Thực thi một task NCKH từ README tuần, có acceptance/evidence | Plan -> triển khai -> verify -> handoff; human Git |
| `nckh-pr-preflight` | `.agents/skills/nckh-pr-preflight/` trong repo | Kiểm tra trước PR, kiểm tra scope/evidence/Human Gate | Review/handoff; không tự commit/push/merge |

**Trạng thái ở thời điểm viết:** 3 global Skills đầu đã được cài và thử bằng Codex Extension trên **máy chủ dự án** qua ổ D; **không mặc định cả 5 máy thành viên đã cài**. 2 NCKH repo Skills hiện có trên nhánh `dev`. Tên **`nckh-coding-workflow` là Skill điều phối dự kiến, chưa được tạo/cài**. Cẩm nang này giúp cả nhóm thực hành *ngay cả khi chưa có* Skill điều phối mới.

### 5.2. Quy tắc chọn Skill trong vòng local

| Tình huống | Nên ưu tiên | Kết hợp |
|---|---|---|
| Tự code xong, muốn AI rà và sửa | `code-review` | `diagnosing-bugs` nếu tìm ra lỗi; `tdd` thêm regression test |
| Có bug/traceback/firmware timing nghi ngờ | `diagnosing-bugs` | `tdd` để chống tái phát; `code-review` cuối |
| Bắt đầu viết module mới theo task | `nckh-task-execution` | `tdd` nếu có test harness và interface đủ rõ |
| Sửa lỗi ML leakage | `diagnosing-bugs` + `tdd` | `code-review` xác minh train/test isolation |
| Đến trước commit/PR | `nckh-pr-preflight` + `code-review` | Check status/staged diff/test evidence |
| Sửa feedback PR GitHub | `diagnosing-bugs` hoặc `code-review` | `tdd` nếu có lỗi tái hiện được; preflight lại |
| Chỉ hỏi kiến trúc/giải thích code | Đọc instructions/context | **Không bắt buộc** chạy cả ba Skills |

**Không bắt buộc luôn chạy `tdd` + `diagnosing-bugs` + `code-review` cho mọi thay đổi.** Ví dụ chỉnh typo tài liệu không cần đo firmware; review CAD không thể thay bench check bằng pytest. Chọn theo rủi ro, task, acceptance và toolchain thực có.

### 5.3. Skill điều phối tương lai: hành vi mong đợi

Nếu nhóm sau này triển khai `nckh-coding-workflow` tại global user-level ngoài repo, nó chỉ nên làm: **intake -> xác minh scope/branch -> xác định vấn đề -> chọn đúng Skill(s) -> xin phê duyệt sửa -> chạy kiểm tra thật -> tự review -> bàn giao để human Git**. Không override `AGENTS.md`, không giả review GitHub bằng local tools, không tự commit/push/merge, không tự nhận `PASS` khi `NOT RUN`.

---
## 6. CHUẨN BỊ MỘT MÁY LẬP TRÌNH

### 6.1. Điều kiện tối thiểu

1. Có quyền collaborator đối với repository GitHub; đăng nhập đúng tài khoản cá nhân.
2. Cài **VS Code**, **Git**, **Codex Extension**, đăng nhập Codex bằng ChatGPT theo quyền của mỗi thành viên.
3. Có Node.js/npm nếu cần tự cài Matt Pocock Skills. **Không cần Node.js chỉ để Git clone hoặc dùng Codex Extension nếu không cài Skills qua `npx`.**
4. Có Python/toolchain firmware **nếu task yêu cầu chạy**; không cài bừa một stack trên mọi máy.
5. Mở **đúng thư mục root repo**, không mở thư mục cha hoặc ZIP snapshot.

Kiểm tra nhanh trong **PowerShell**:

```powershell
git --version
node --version      # Chỉ cần nếu cài Skills từ npx
npm.cmd --version   # Chỉ cần nếu cài Skills từ npx
python --version    # Nếu làm Python
Get-Location
```

Nếu `node`/`npm`/`python` không được tìm thấy, đừng đoán đường dẫn. Xác nhận phần mềm/PATH và yêu cầu người quản lý máy hỗ trợ.

### 6.2. Clone repository lần đầu (mỗi người tự chọn thư mục)

**Thiết lập Git identity một lần trên máy cá nhân** (chạy khi chưa cấu hình hoặc phải cập nhật thông tin; dùng tên/email GitHub **thật của chính thành viên**):

```powershell
git config --global user.name "TEN_THAT_CUA_THANH_VIEN"
git config --global user.email "EMAIL_GITHUB_CUA_THANH_VIEN"
git config --global --get user.name
git config --global --get user.email
```

`user.name`/`user.email` chỉ là metadata tác giả commit, **không thay cho xác thực GitHub**. Khi Git mở trình duyệt yêu cầu login/Authorize, đăng nhập đúng collaborator; không nhập mật khẩu/token của đồng đội vào terminal.

```powershell
# VÍ DỤ cho máy thành viên mới, không chạy lại nếu đã clone
New-Item -ItemType Directory -Force -Path 'D:\NCKH' | Out-Null
Set-Location 'D:\NCKH'
git clone https://github.com/nkaq/SVNCKH--HUST.git
Set-Location '.\SVNCKH--HUST'
code .
```

**Riêng máy đã thiết lập trong dự án này**, ví dụ đường dẫn thực tế là:

```powershell
Set-Location 'D:\SVNCKH--HUST_v3_Codex_Ready\SVNCKH--HUST'
```

**Không yêu cầu các thành viên khác có cùng đường dẫn.** Kiểm tra trước:

```powershell
git rev-parse --show-toplevel
git remote -v
git status --short
```

`origin` phải là `https://github.com/nkaq/SVNCKH--HUST.git` (hoặc SSH remote chính xác cùng repository). Nếu remote lạ, **dừng**, không push.

### 6.3. Xác minh đã có Global Skills trên máy

```powershell
# User-level Codex Skills (trường hợp đã dùng Junction)
Get-Item "$HOME\.agents\skills" -ErrorAction SilentlyContinue |
  Format-List FullName,LinkType,Target

@('tdd','diagnosing-bugs','code-review') | ForEach-Object {
  $file = Join-Path "$HOME\.agents\skills" "$_\SKILL.md"
  "{0}: {1}" -f $_, (Test-Path $file)
}

# Nếu đã có Node/npm:
npx.cmd --yes skills@1.7.0 list -g -a codex
```

`True` chứng minh file tồn tại, **chưa tự chứng minh Codex sẽ chọn Skill trong mọi chat**. Reload VS Code (`Ctrl+Shift+P` -> `Developer: Reload Window`), mở Codex chat mới, thử yêu cầu liệt kê Skill file thực sự đọc. Không dùng `$tdd` thủ công trong bài thử tự kích hoạt nếu đang kiểm chứng cơ chế chọn tự động.

### 6.4. Cài 3 Matt Pocock Skills **chỉ nếu máy thành viên chưa có**

**Nguồn bên thứ ba:** `mattpocock/skills` — mã/hướng dẫn ngoài OpenAI và NCKH. Đọc nội dung `SKILL.md` trước khi tin dùng, đặc biệt các đoạn đòi commit, sửa `AGENTS.md`, truy cập mạng hoặc chạy lệnh rủi ro. Những lệnh bên dưới là thao tác cài đặt, phải có sự đồng ý của chủ máy.

**Bước A — xem có thư mục user-level cũ không:**

```powershell
Test-Path "$HOME\.agents\skills"
```

Nếu là `True`, **không ghi đè, xóa hay chuyển ngay**; kiểm tra nội dung rồi quyết định. Nếu `False`, có thể đặt dữ liệu trên D bên ngoài repository. Ví dụ chọn `D:\NCKH\AI_Agent_Skills` (nếu D không có quyền ghi thì chọn một thư mục mà user có quyền):

```powershell
# Chạy chỉ trên máy chưa có "$HOME\.agents\skills"
$skillsDir = 'D:\NCKH\AI_Agent_Skills'
New-Item -ItemType Directory -Force -Path $skillsDir | Out-Null
New-Item -ItemType Directory -Force -Path "$HOME\.agents" | Out-Null
New-Item -ItemType Junction -Path "$HOME\.agents\skills" -Target $skillsDir
```

**Không đặt thư mục Skills user-level bên trong `SVNCKH--HUST/` hoặc stage lên Git.** Máy chủ dự án đã có Junction từ `C:\Users\Dell\.agents\skills` sang `D:\SVNCKH--HUST_v3_Codex_Ready\AI_Agent_Skills`; **không tạo Junction trùng**.

**Bước B — khóa nguồn cài (không dùng `latest`):**

Đề xuất baseline **cần ME1 thẩm định trước rollout**: `skills` CLI phiên bản npm `1.7.0` (phiên bản có trên npm tại ngày biên soạn), repository `mattpocock/skills` **commit bất biến** `4588b32ecab9ecc9fc8cc6b6c5e7d675b6004b0d`. Đây là mốc được xác minh tồn tại trên GitHub, **chưa phải** bằng chứng đã audit từng file trên máy của cả 5 thành viên, và có thể khác ba Skills từng cài bằng `latest` trước đây. Không tuyên bố các bản cài cũ đồng nhất. Không tự lấy thêm bản mới khi chưa có PR nâng cấp và ME1 duyệt.

```powershell
# Chỉ áp dụng cho máy chưa cài, sau khi ME1 duyệt baseline.
$approvedCommit = '4588b32ecab9ecc9fc8cc6b6c5e7d675b6004b0d'
$sourceRoot = 'D:\NCKH\Approved_Skill_Sources'     # Đổi sang ổ có quyền ghi nếu cần
$verifiedCheckout = Join-Path $sourceRoot 'mattpocock-skills-4588b32'
New-Item -ItemType Directory -Force -Path $sourceRoot | Out-Null
if (Test-Path $verifiedCheckout) {
    throw 'Thư mục nguồn đã tồn tại; kiểm tra trước, không tự ghi đè.'
}
git clone https://github.com/mattpocock/skills.git $verifiedCheckout
if ($LASTEXITCODE -ne 0) { throw 'git clone thất bại' }
git -C $verifiedCheckout checkout --detach $approvedCommit
if ($LASTEXITCODE -ne 0) { throw 'git checkout thất bại' }
$actualCommit = (git -C $verifiedCheckout rev-parse HEAD).Trim()
if ($actualCommit -ne $approvedCommit) { throw 'Sai commit nguồn, DỪNG.' }
npx.cmd --yes skills@1.7.0 add $verifiedCheckout --list
```

**Bước C — sau khi kiểm duyệt nội dung Skills ở checkout đã khóa, cài đúng ba Skills cho Codex:**

```powershell
# Không chạy nếu máy đã có bản Skills khác mà chưa được ME1 duyệt thay thế.
npx.cmd --yes skills@1.7.0 add $verifiedCheckout -g -a codex --copy -s tdd -s diagnosing-bugs -s code-review
npx.cmd --yes skills@1.7.0 list -g -a codex
```

**Bước D — kiểm chứng bản cài và lưu dấu vết:**

```powershell
foreach ($skill in @('tdd', 'diagnosing-bugs', 'code-review')) {
    $skillFile = Join-Path "$HOME\.agents\skills" "$skill\SKILL.md"
    if (!(Test-Path $skillFile)) { throw "Thiếu $skillFile" }
    Get-FileHash -Algorithm SHA256 -LiteralPath $skillFile |
        Select-Object Path, Hash
}
```

ME1 lưu `CLI version`, `source commit`, **hash thực tế từng `SKILL.md` + supporting files** và kết quả test Codex đọc Skill trên máy pilot; chỉ so khớp với baseline đã duyệt sau khi có bảng hash tham chiếu thật. Hash hiển thị không tự chứng minh an toàn hay nội dung đúng. Sau đó Reload Window, thử P01. Nếu CLI không hỗ trợ local source/flag ở phiên bản đã ghim, dừng và lập PR cập nhật hướng dẫn, **không tùy tiện trở lại `@latest`**.

> **Không cài vội** `implement` của Matt Pocock vì trong bản đã xem Skill này có bước commit; điều đó mâu thuẫn quy trình team. Cũng không chạy `setup-matt-pocock-skills` khi chưa duyệt vì có thể thay đổi instruction/config của repository.

### 6.5. Repo Skills đã có sẵn

Trong repo `dev`, kiểm tra:

```powershell
Test-Path '.\.agents\skills\nckh-task-execution\SKILL.md'
Test-Path '.\.agents\skills\nckh-pr-preflight\SKILL.md'
```

Nếu `False`, kiểm tra đang ở đúng root và branch đã cập nhật chưa. Không tự copy/tạo file khác để “vá” cấu trúc nếu chưa biết nguyên nhân.

### 6.6. Hạn mức ChatGPT Plus và môi trường

- Codex local/GitHub dùng tính năng tích hợp với ChatGPT **nếu tài khoản và repo đã được cấp quyền**; người dùng Plus không phải tạo `OPENAI_API_KEY` riêng cho quy trình chính thức này.
- Hạn mức Codex có thể có **cửa sổ 5 giờ và giới hạn khác**; phụ thuộc gói, tác vụ và thời điểm. Không khẳng định “mọi 5 giờ hoàn lại nguyên một lượng token cố định”.
- Theo dõi trạng thái tại <https://chatgpt.com/codex/settings/usage> hoặc màn hình usage thực tế. Nếu review cloud không chạy, **đừng đổi sang workflow GitHub Action dùng API key khi chưa được duyệt**.
- Không cần để VS Code mở liên tục để GitHub Codex review PR. Ngược lại, local Skills không chạy ngầm theo `git push`.

---

## 7. GIT: BẮT ĐẦU TASK, CODE TAY, QUẢN LÝ NHÁNH

### 7.1. Trước mọi task: kiểm tra 4 điều

```powershell
git rev-parse --show-toplevel
git remote -v
git branch --show-current
git status --short
```

- Nếu `git status --short` **không rỗng**, xem file nào đang sửa; **không tự `switch`/`pull` để mất thay đổi**.
- Với **task tuần**, Task ID/Goal/Acceptance/Evidence lấy từ `docs/plans/weekXX/README.md` có hiệu lực trên `dev`. Với PR finding hoặc hạ tầng/tài liệu, dùng nguồn PR/ME1-approved task tương ứng theo §2.1; không tự bịa số Issue hay ID.
- Kiểm tra đang làm đúng subsystem, xác nhận các dependency với owner trước khi sửa contract dùng chung.
- Không dùng `main` như nhánh phát triển; không sửa trực tiếp `dev`.

### 7.2. Bắt đầu feature branch từ `dev` sạch

```powershell
git switch dev
git pull --ff-only origin dev
git status --short
git switch -c feature/it2-edge-dsp
```

Lệnh tạo nhánh theo vai trò:

```text
feature/me1-<task>       docs/me1-<task>
feature/ms2-<task>       experiment/ms2-<task>
feature/ee2-<task>       fix/ee2-<bug>
feature/et1-<task>       hardware/et1-<task>
feature/it2-<task>       fix/it2-<bug>
```

Chọn tên ngắn, không khoảng trắng, không chứa secret. Nếu nhánh đã tồn tại, Git báo lỗi: **không xóa nhánh để “thử lại”**; kiểm tra `git branch --list` và nhiệm vụ tương ứng.

### 7.3. Thành viên tự viết code trước

Đây là workflow mong muốn của nhóm: **AI không chiếm phần học/làm của sinh viên**. Tự triển khai các phần chính, ghi rõ giả định chưa xác nhận, commit chưa phải yêu cầu ngay. Sau đó:

```powershell
# Chỉ xem; không stage/commit
git status --short
git diff --stat
git diff
```

Lưu ý `git diff` **không hiện nội dung file chưa được Git theo dõi (untracked)**. Xem `git status --short`; nếu có `??`, mở file trong VS Code kiểm tra riêng. Nếu lỡ stage từ trước, `git diff --cached` cho biết phần staged.

### 7.4. Nếu đang có feature branch cũ chưa xong

```powershell
git branch --show-current
git status --short
git log -5 --oneline
```

Ở đúng branch cũ -> tiếp tục task trên branch cũ, không tạo branch cùng nhiệm vụ để né review. Nếu cần bắt đầu task mới nhưng đang có thay đổi dở, **dừng và chọn**: hoàn thiện/test/commit phần hợp lệ trên feature branch cũ; hoặc stash có kiểm soát sau khi đã hiểu trạng thái; hoặc tách worktree với người có kinh nghiệm. **Không `reset --hard`, `clean -fd` hoặc force-push** để xử lý vội.

### 7.5. Muốn lấy cập nhật từ `dev` khi feature branch đang triển khai

Sau khi **working tree sạch và đã bảo toàn thay đổi**, có thể đồng bộ:

```powershell
git fetch origin
git status --short
git log --oneline --decorate -5
git merge origin/dev
```

`git merge origin/dev` chỉ chạy **khi đang đứng tại feature branch**, **đã trao đổi với owner nếu có thay đổi interface**, và hiểu rằng Git có thể tạo merge commit hoặc conflict. Nó không đồng nghĩa với merge feature vào `dev`. Nếu chưa chắc, dừng sau `fetch` và hỏi ME1.

Nếu merge conflict: xem §14. Không dùng `git pull origin dev` tùy tiện trên feature branch vì dễ hiểu sai nhánh nào được merge vào nhánh nào.

### 7.6. Cấm lệnh nguy hiểm trong workflow thông thường

Không chạy theo gợi ý AI nếu chưa có review đặc biệt:

```text
git push --force
git reset --hard
git clean -fd
git rebase --onto ...
git branch -D ...
git push origin --delete dev
git push origin --delete main
```

`git add .` / `git add -A` không bị Git cấm, nhưng **không nên dùng** trong repo NCKH do nguy cơ kéo raw data, binary CAD, test log hoặc secret không chủ ý. Stage **đúng file**.

---

## 8. VÒNG 1: LOCAL CODEX REVIEW, TỰ SỬA VÀ KIỂM THỬ

### 8.1. Hai chế độ phải tách biệt

| Chế độ | Mục đích | Quyền được cấp |
|---|---|---|
| **Review-only** | Tìm vấn đề có chứng cứ, khuyến nghị test/sửa | Không sửa file, không chạy lệnh trừ khi cho phép |
| **Review-and-fix** | Sau human xác nhận phạm vi, sửa bug và viết test | Được edit **file task**; được chạy test hợp lý; **không Git commit/push/merge** |

**Quy trình đề nghị cho code tay:** review-only lần đầu (**P03-A**) -> người làm đồng ý phạm vi -> fix/test (**P03-B**) -> self-review (**P07**) -> human xem diff. Review và sửa không phải cùng một quyền tự động.

### 8.2. Vòng 1 theo từng pha có điểm dừng

**Pha L1 — Intake, đọc quy tắc:** Codex đọc `AGENTS.md`, scoped `AGENTS.md`, task README, contracts, code thật. Kiểm tra branch và danh sách file. Nếu thiếu task hoặc spec, hỏi lại thay vì tự quyết. Không quét toàn bộ repo vô điều kiện.

**Pha L2 — Review code tay:** ưu tiên `code-review`. Finding chuẩn: severity `BLOCKER/MAJOR/MINOR/QUESTION`, file:dòng, lỗi, tại sao xảy ra, ảnh hưởng, bằng chứng, cách sửa, cách test. Không coi lỗi vốn có từ trước là lỗi PR nếu diff không sinh lỗi đó.

**Pha L3 — Phân loại hành động:**

- Không có lỗi rõ: báo review scope, rủi ro, test cần chạy; không tự thay code chỉ để có diff.
- Có bug nhưng nguyên nhân chưa chắc: sử dụng `diagnosing-bugs`, tái hiện/log/test hoặc ghi hypothesis/NOT RUN.
- Có bug đã rõ và sửa được: dùng `tdd` khi phù hợp để thêm regression test; kiểm RED trước fix nếu harness có; sửa tối thiểu; test GREEN; refactor nếu không phá API.
- Có bất biến/human gate: **dừng ở plan**, không sửa đến khi owner phê duyệt.

**Pha L4 — Human chấp thuận bản vá:** member xác nhận cụ thể file/behavior Codex được phép thay; mặc định không thay schemas/pinout/ODR/physical labels. Nếu Codex đề nghị thay lớn, chia PR hoặc review architecture riêng.

**Pha L5 — Verify:** chạy lệnh phù hợp **với toolchain/config thực có**; báo chính xác command, exit code, output và artifact nếu có. Không tự nói “firmware PASS” khi chỉ đọc code hoặc chạy host unit test.

**Pha L6 — Review lại:** `code-review` + `nckh-pr-preflight`; so diff với task, scoped rules, secret/large files, dependency. Nếu vẫn còn `BLOCKER/MAJOR`, vòng lặp về L3-L5.

**Pha L7 — Handoff:** trả cho thành viên báo cáo chuẩn §15 rồi **dừng**, không thực hiện lệnh Git thay người.

### 8.3. Các lệnh kiểm thử tham khảo — CHỈ KHI PROJECT ĐÃ CÓ TOOLCHAIN

**Kiểm tra Git, có thể dùng mọi task:**

```powershell
git status --short
git diff --check
git diff --stat
```

**IT2/MS2 — Python:**

```powershell
# Xem trước có test runner/config không; không tự cài package
git ls-files '*test*.py' 'pyproject.toml' 'pytest.ini' 'requirements*.txt'
python --version
python -m pytest --version   # Chỉ hoạt động khi pytest đã cài

# Nếu repo và môi trường có pytest: chạy đúng tests liên quan
# python -m pytest -q ml/path/to/real_tests.py
# python -m pytest -q experiments/sensor_characterization/path/to/real_tests.py
```

**EE2 — firmware ESP32-S3:**

```powershell
# Xác định project đang dùng toolchain nào
Get-ChildItem firmware/esp32s3 -Recurse -File -Include `
  platformio.ini,CMakeLists.txt,*.ino,arduino-cli.yaml -ErrorAction SilentlyContinue |
  Select-Object FullName

# CHỈ là ví dụ, không chạy bừa nếu không có file cấu hình/toolchain
# pio run -d firmware/esp32s3
# arduino-cli compile ...          # Cần board FQBN và thư mục sketch thực tế
# idf.py build                      # Cần ESP-IDF project và môi trường đã setup
```

**ESP32 hardware validation:** board, fixture, phiên bản firmware, điều kiện test, lệnh đo và log **phải thực tế**. `60 s acquisition PASS`, sample count, dropped samples, jitter, RMS equivalence, heap/Flash/latency đều **không được suy từ source code**. Khi không có board: `HARDWARE TEST: NOT RUN — no board/device access`.

**Không tự tải package/firmware/toolchain, flash board, ghi đè dataset hoặc mở cổng serial thật** chỉ vì prompt “test hết đi”. Phải có quyền và nhận diện môi trường.

### 8.4. Bằng chứng local review có thể tin cậy

Một báo cáo hữu ích tối thiểu phải trả lời:

1. Codex đã đọc **những `SKILL.md` nào**, đường dẫn thật; nếu không đọc thì nói `NOT LOADED`.
2. Chính xác **file nào đã sửa** và thuộc task nào.
3. Finding nào đã **REPRODUCED**, **FIXED**, **UNVERIFIED**, **NOT APPLICABLE**; vì sao.
4. Test nào **EXECUTED PASS/FAIL** với command/log; test nào **NOT RUN** với lý do.
5. Có thay đổi **public interface / packet schema / feature definition / timestamp semantics / data policy** không.
6. Human Gate và owner nào cần phê duyệt.
7. Diff đã được self-review chưa; còn tồn tại blocker nào không.

---

## 9. GIT: HUMAN REVIEW, COMMIT, PUSH VÀ PULL REQUEST

### 9.1. Human kiểm working tree trước commit

```powershell
git branch --show-current
git status --short
git diff --check
git diff --stat
git diff
```

Chắc chắn nhánh có dạng `feature/...`/`fix/...`/`docs/...`, **không phải `dev`/`main`**. Với các file `??`, xem nội dung riêng. Nếu AI thay nhầm file, yêu cầu sửa lại hoặc hoàn tác **từng file có hiểu biết**, không chạy reset/clean toàn bộ.

### 9.2. Kiểm tra secret và file lớn trước khi stage

Với CAD/model/binary lớn nếu repository đang dùng Git LFS, kiểm tra LFS **chỉ đọc**:

```powershell
git lfs version
git lfs ls-files
```

Nếu `git lfs` chưa có hoặc chưa được thiết lập, báo owner; **không tự thêm tracking rules** bằng `git lfs track` hoặc sửa `.gitattributes` ngoài task/approval.

- Mở `git status --short`, tìm `.env`, key, credential, token, `.pem`, log raw, dataset lớn, output build, model artifact cỡ lớn, file nháp.
- `git diff --check` chỉ kiểm whitespace, **không quét secret**.
- File nhị phân, CAD, `.tflite`, dataset phải theo version/data/LFS policy.
- Khi nghi lộ secret: **dừng**, không push; xoay vòng credential theo quy trình và hỏi ME1 nếu đã commit/push.

### 9.3. Stage chính xác file task

```powershell
# Ví dụ IT2 — thay bằng file có thật trong task
# git add -- ml/preprocessing/recording_qc.py
# git add -- ml/preprocessing/tests/test_recording_qc.py

# Xem staged
git diff --cached --name-only
git diff --cached --check
git diff --cached --stat
git diff --cached
```

Nếu stage nhầm một file, **chỉ bỏ stage** (không xóa nội dung file):

```powershell
git restore --staged -- path/to/wrong-file
```

**Không commit nếu danh sách staged không khớp task/scope.** Với file thuộc subsystem khác, xin xác nhận owner hoặc tách PR.

### 9.4. Human commit

```powershell
# Chỉ sau khi staged diff đã được human duyệt
git commit -m "fix(it2): prevent recording-level data leakage"

# Kiểm tra commit vừa tạo
git log -1 --oneline
git status --short
```

Mẫu commit: `type(scope): message`. Ví dụ `feat(ee2): add bounded acquisition buffer`, `test(ms2): cover calibration edge cases`, `fix(it2): guard empty recording`, `docs(et1): clarify connector pinout`. Không bịa Issue ID nếu không có.

### 9.5. Human push feature branch lần đầu

```powershell
git branch --show-current
# Gõ lại đúng tên nhánh do lệnh trên hiển thị
git push -u origin feature/it2-edge-dsp
```

Lần sau trên cùng branch có tracking:

```powershell
git push
```

**Không force push.** Nếu `non-fast-forward`, đọc §14, kiểm tra remote/branch, fetch rồi phối hợp an toàn; không ghi đè commit của người khác.

### 9.6. Mở Pull Request vào `dev`

1. Vào <https://github.com/nkaq/SVNCKH--HUST/pulls> -> **New pull request**, hoặc dùng gợi ý **Compare & pull request** sau khi push.
2. Đặt **base = `dev`**, **compare = `feature/...`** (không nhầm chiều). Mặc định không PR thẳng vào `main`.
3. Điền **đầy đủ `.github/PULL_REQUEST_TEMPLATE.md`**, task ID + path `docs/plans/weekXX/README.md`, các thay đổi, tests/evidence, data/hardware impact, dependency, review focus, Human Gate.
4. Nếu không có GitHub Issue: ghi `Issue: None`/`Không có`, không giả `Closes #...`.
5. Request reviewer **là người khác tác giả PR**. Nếu tác giả là CODEOWNER duy nhất, mời reviewer độc lập và kiểm tra quy tắc CODEOWNER approval; người không phải owner không tự thay thế yêu cầu owner khi ruleset bắt buộc.
6. Khi sẵn sàng review, để PR ở chế độ **Ready for review**; nếu chưa sẵn sàng, dùng **Draft PR** và chuyển Ready sau.

**Ví dụ mô tả PR chuẩn (chỉ là template):**

```markdown
Task source: docs/plans/weekXX/README.md
Task ID / source: <WXX-ROLE-NN + README tuần | PR finding URL | ME1-approved infra task URL>
Owner: IT2
Base: dev
Scope: ml/preprocessing/, ml/features/
Goal: <mục tiêu theo task>
Acceptance: <tiêu chí đo được>

Local Codex review:
- Loaded skills: <chỉ ghi skills thực sự đọc>
- Confirmed findings fixed: <...>
- Open findings: <...>

Tests actually executed:
- Command: <lệnh thật>
- Result: PASS/FAIL/NOT RUN + evidence link

Data provenance: Engineering Validation Data only / ...
Cross-team interface change: No / Yes -> approval reference
Human Gate: No / Yes -> required owner + approval evidence
Codex GitHub review: Waiting / Completed (findings still under review)
```

### 9.7. Sau push bổ sung cho cùng PR

Khi Codex GitHub review tìm lỗi:

```powershell
# Đang ở đúng feature branch
# Thành viên hoặc Codex (đã cấp quyền) sửa local + chạy test
git status --short
git diff --check
git diff

# Human stage đúng file và commit
git add -- path/to/confirmed-file.py
git diff --cached
git commit -m "fix(it2): address PR review finding"
git push
```

Không tạo PR mới chỉ để sửa một finding của PR cũ nếu vẫn cùng phạm vi. **Không đặt quota cứng phải push đúng 2 lần**: có thể 1 lần nếu không có lỗi, nhiều lần nếu có lỗi thật. Với mỗi push, đối chiếu **review có chạy trên commit mới nhất không**.

---

## 10. VÒNG 2: CODEX TRÊN GITHUB, CI VÀ HUMAN GATE

### 10.1. Không cần cấu hình lại kết nối đã hoạt động

Codex GitHub Code Review đã có dấu vết hoạt động trong các PR NCKH, ví dụ PR #18 và #14. Mỗi thành viên không cần cài global Skills trên máy chủ GitHub và không cần tạo `OPENAI_API_KEY` cho chức năng review chính thức. **Quyền tích hợp và Automatic Review setting có thể thay đổi**, nên luôn kiểm tra trên PR thực tế.

### 10.2. Review trigger và cách xác minh

- Mở PR Ready, chuyển Draft -> Ready, hoặc comment `@codex review` **nếu tích hợp hỗ trợ**.
- Trong repo này PR #18 đã cho thấy review có thể ghi trigger `New commits`; **không suy ra mỗi push chắc chắn tạo review mới trên mọi PR**.
- Chờ `Codex Review Summary`/review comment và so sánh **Reviewed commit SHA** với **HEAD commit mới nhất** của PR.
- Nếu review vẫn trên SHA cũ: không coi nó đã xác minh bản sửa mới; chờ/nhờ re-review theo tích hợp.
- Nếu `Completed` nhưng có P1/P2 hoặc finding khác: vẫn **FIX THEN REVIEW** hoặc xin quyết định human nếu là false positive có chứng cứ.
- Nếu Codex trả tiếng Anh: yêu cầu giải thích tiếng Việt, nhưng vẫn đánh giá nội dung kỹ thuật.

**Comment trigger mẫu:**

```text
@codex review
```

Đừng spam lặp lại khi review đang chạy; kiểm tra GitHub UI/trạng thái/quota trước. Reviewer GitHub có thể bỏ qua lỗi nhỏ: human review và CI vẫn bắt buộc theo chính sách nhóm.

### 10.3. Xử lý finding GitHub chuẩn

1. **Đọc** finding, severity, `file:dòng`, commit được review, contract/AGENTS được viện dẫn.
2. **Phân loại**: bug thực tế do PR gây ra; regression có thể tái hiện; vấn đề tồn tại sẵn; thiếu bằng chứng; false positive; yêu cầu Human Gate.
3. **Đối chiếu code và test**. Không sửa mù vì AI gắn nhãn `P1`.
4. **Giao Codex Extension** prompt **P22** khi cần phân tích/sửa; chỉ sửa local trong feature branch, chạy test và báo cáo.
5. **Human** kiểm diff -> commit -> push; phản hồi reviewer với evidence, trỏ commit sửa; xin re-review khi cần.
6. Không tự resolve một thread chưa kiểm bằng chứng; không merge chỉ do comment được resolve.

**Mẫu trả lời finding bằng tiếng Việt:**

```text
Đã kiểm tra finding tại <file:dòng> trên commit <SHA>.
Kết luận: CONFIRMED / PARTIAL / FALSE POSITIVE / NEEDS HUMAN DECISION.
Nguyên nhân: <...>
Thay đổi xử lý (nếu có): <commit SHA và các file>.
Kiểm chứng thực tế: <lệnh, PASS/FAIL, log/artifact>.
Kiểm chứng chưa thực hiện: <NOT RUN + lý do>.
Nhờ Codex/human reviewer kiểm tra lại commit mới nhất nếu cần.
```

### 10.4. CI trên GitHub có vai trò gì?

Theo `.github/workflows/repo-quality.yml` hiện có:

```yaml
name: Repository Quality
on:
  pull_request:
    branches: [dev, main]
  push:
    branches: [dev, main]
jobs:
  repo-quality:
    # thực tế chạy .github/scripts/repo_quality.py
```

**Lưu ý:** block chỉ là *trích ý nghĩa trigger*, không phải YAML copy để ghi đè file workflow. CI chỉ bảo đảm những phép kiểm cụ thể nó đã chạy. Ví dụ IT2 phải có unit test riêng cho parser/ML khi phù hợp; EE2 cần compile, host-side tests và **bench test thật**. Chưa có job thì ghi `NOT CONFIGURED`, không tự nói GitHub đã chạy.

### 10.5. Người có quyền quyết định merge

Yêu cầu vận hành nội bộ trước merge:

- PR base đúng `dev`, nguồn task/acceptance khớp **loại PR** theo §2.1; Task ID tuần chỉ bắt buộc với task tuần.
- Tests/evidence **thực sự** đủ cho phần thay đổi; `repo-quality` PASS nếu workflow có áp dụng.
- Codex review đã được xem ở commit cập nhật hoặc có fallback độc lập khi không khả dụng.
- Findings có căn cứ đã sửa hoặc được owner chấp nhận có lý do; không bỏ qua blocker.
- Reviewer con người **khác tác giả** đã kiểm và approve khi cần; CODEOWNER đúng quyền nếu branch rules yêu cầu.
- Human Gate phải có thông tin phê duyệt truy vết khi liên quan an toàn, Ground Truth, experiment protocol, dataset, architecture hoặc scientific claim.
- Chỉ người được giao quyền GitHub mới thực hiện **Squash and merge** vào `dev`. Sau merge, thành viên khác đồng bộ `dev` và phải đọc `[PROJECT UPDATE]` nếu có thay đổi liên nhóm.

**Branch protection/rulesets:** Nếu chưa bật hoặc chưa cấu hình required checks/approvals, **không được giả định GitHub đang chặn merge tự động**. Đây là quality gate vận hành của team, không nhất thiết là rào chắn kỹ thuật hiện hữu.

### 10.6. Khi Codex GitHub không khả dụng / quota hết

**Theo quy trình hiện hành của NCKH**, có thể thay Codex GitHub review bằng **human review độc lập có chuyên môn**, nhưng phải ghi rõ lý do, bằng chứng kiểm tra và người review; không bỏ CI/test/human gate. Chờ quota reset là tùy chọn; không chuyển sang Action sử dụng API tính phí chỉ để “cứu” một PR. Nếu review bot hoạt động lại, có thể bổ sung review nhưng không giả diễn ra hồi tố.

---

## 11. TIÊU CHUẨN KIỂM CHỨNG THEO SUBSYSTEM

### 11.1. IT2 — Data, DSP, ML, TinyML, Dashboard

**Nơi:** `ml/preprocessing/`, `ml/features/`, `ml/baselines/`, `ml/tinyml/`, `ml/drift_detection/`, `ml/adaptive_fusion/`, `software/pc_dashboard/`, `software/serial_logger/`, `data/manifests/`, `results/`.

**Review bắt buộc theo loại thay đổi:**

- **Serial parser:** packet length/schema/units, `sample_index`, `timestamp_us`, recording ID, EOF/truncated rows, missing/duplicate/out-of-order samples, counters từ EE2; lỗi phải báo thay vì im lặng sửa dữ liệu.
- **Preprocessing/QC:** NaN/Inf, corrupted row, axis scale, timebase, sensor calibration, empty recording, short recording, threshold config/version, provenance và `setup_version`.
- **Windowing/DSP:** window size/hop/overlap, sample rate, normalization, FFT units/scaling, mean/RMS/std/peak/crest factor, reference tests và test degenerate inputs.
- **Train/eval:** **split theo recording hoặc cấp nhóm phù hợp**, không random-window leakage; fit scaler/selector/training transforms **chỉ trên train**; deterministic seed; split rỗng/imbalance cases; metadata/label provenance.
- **Baseline/TinyML:** không tuyên bố model quality từ Engineering Validation Data như official Dataset v0.1; đo Flash/RAM/latency trên hardware hoặc build thực tế khi báo; ghi architecture/config/revision.
- **PC↔ESP32 equivalence:** chung input, config, float tolerance có cơ sở, window definition, feature order, frequency units, overflow/precision; kết quả đo phải có log và firmware SHA.
- **Dashboard:** lỗi kết nối/timeout/reconnect, dữ liệu stale, đơn vị trên plot, không dùng predictions làm Ground Truth.

**Ví dụ tình huống test** (kịch bản cần chạy, không phải dữ liệu thực): 0 mẫu; 1 mẫu; 2 recording; nhiều window cùng recording; cùng `recording_id` ở hai tập; timestamps lặp/nhảy ngược; bytes bị cắt; sample rate thiếu; mixed setup versions; NaN; FFT hằng số/zero; terminal disconnect.

### 11.2. EE2 — Firmware acquisition ESP32-S3

**Nơi:** `firmware/esp32s3/`.

- **Driver:** ADXL345 config, I2C/SPI return code, sensor connection, FIFO mode, DATA_READY behavior, unit/range mapping.
- **Sampling:** ISR ngắn, không blocking trong đường acquisition; ring/double buffer overflow, producer/consumer race, lost-sample accounting; 60 s/multiple-run stress test **trên thiết bị**.
- **Timestamps:** nguồn clock/độ phân giải/overflow, timebase monotonicity, sample-index sequence, FIFO batch timestamp assumptions, sync với serial logger.
- **Control:** START/STOP/STATUS/SET_RECORD_ID/SET_CONDITION theo protocol **đang được phê duyệt**, idempotency/error states; không lấy chuỗi state từ kế hoạch Week 3 làm contract lâu dài nếu architecture mới đã thay thế.
- **Resource:** stack/heap/free memory, watchdog, Serial/OLED không làm hỏng acquisition, compile/link size, error recovery.
- **Evidence:** serial/hardware logs và conditions rõ, số mẫu thực, error counters, board/config/firmware SHA, thời gian stress test. **Host-side mock PASS ≠ hardware PASS**.

### 11.3. MS2 — ADXL345 calibration/sensor health

**Nơi:** `experiments/sensor_characterization/` và phần phối hợp `ml/drift_detection/`.

- Kiểm đơn vị, orientation, bias/noise/sensitivity và repeatability; metadata mounting/temperature/sample rate.
- Phân biệt **sensor drift** với điều kiện vận hành, domain/mounting shift, noise và artifact acquisition.
- Không sinh kết quả self-test/calibration số học giả; thống kê/plot phải có raw evidence; test code synthetic là test thuật toán, không phải đo ADXL345 thật.
- Nếu thay schema sensor-health ảnh hưởng firmware/parser, yêu cầu EE2 + IT2 phối hợp, ME1 nếu là contract chung.

### 11.4. ET1 — Electronics

**Nơi:** `hardware/electronics/`, `hardware/bom/`.

- Schematic/pinout/connector compatibility, power/ground, 3.3V/5V, pull-up và ADC ranges.
- ACS724 current front-end, PT100/MAX31865 wiring, US5881/DHT22 reference, EMI/cable routing.
- CAD/EDA exports và bảng pin cần đồng bộ; yêu cầu kiểm bench về điện áp/dòng trước khi kết luận an toàn.
- Script tính toán/simulation **không thay** phép đo trực tiếp.

### 11.5. ME1 — Mechanical, system architecture, protocol

**Nơi:** `hardware/mechanical/`, `docs/architecture/`, `experiments/protocols/`, phối hợp `hardware/bom/`.

- **Onshape là CAD native duy nhất cho thiết kế ME1 mới**, với CAD TEST tách khỏi ME1_MAIN_CAD; chỉ MAIN immutable Version ID/URL làm baseline. Giữ SolidWorks legacy, không tạo/di chuyển/xóa file cũ. Kiểm fixture/drawing/BOM/STEP/DXF cùng engineering Revision/Release ID, vị trí ADXL345, alignment, fault mechanism và guarding; đối chiếu registry/manifest/test report.
- Ground Truth phải truy xuất về controlled physical condition, operator/protocol/recording metadata; đổi fixture/setup sau freeze phải ghi version/domain.
- AI không tự xác nhận an toàn cơ khí hay acceptance fixture chỉ dựa vào mô tả/file CAD nhị phân; cần ME1/human fabrication review và thực nghiệm.

### 11.6. Tình huống giao diện liên nhóm — không có “owner theo file extension”

| Thay đổi | Cần thống nhất |
|---|---|
| EE2 đổi packet/CSV/timestamp | EE2 + IT2; cập nhật parser, schema, tests, migration/version, thông báo ME1 nếu contract chung |
| MS2 đổi sensor-health metadata | MS2 + EE2 + IT2; xác nhận units, cadence, schema, version |
| IT2 chuyển DSP xuống MCU | IT2 + EE2; ownership `firmware/esp32s3/`, resource budget, equivalence |
| ET1 đổi pin/pull-up/interface | ET1 + EE2 + ME1; duyệt điện và cập nhật hardware/firmware contracts |
| ME1 đổi mounting/fixture/protocol | ME1 + MS2 + EE2 + IT2; đánh dấu setup/domain version, Ground Truth/dataset gate |
| IT2 sửa drift detector | IT2 + MS2; giải thích drift v. operating-condition shift, validation/evidence |

---
## 12. THƯ VIỆN PROMPT COPY-PASTE

### Cách sử dụng

- Thay mọi placeholder như `<TASK_ID>`, `<file:path>`, `<acceptance criteria>`, `<PR_NUMBER>`, `<SHA>` bằng giá trị thật. **Không bỏ placeholder nguyên văn rồi yêu cầu Codex đoán.**
- Các prompt **P01–P36** chủ yếu dành cho Codex Extension trên VS Code; những prompt ghi **GitHub comment** thì dán trong PR GitHub.
- Muốn kiểm tra **tự chọn Skill**, không nêu tên Skill trong prompt thử; khi muốn bảo đảm mô hình thực sự đọc Skill, **có thể gọi tên trực tiếp** và yêu cầu liệt kê đường dẫn `SKILL.md` đã đọc.
- Quy tắc chung mọi prompt: **đọc root/scoped `AGENTS.md` và task README; trả lời bằng tiếng Việt; không bịa test/evidence; không tự Git add/commit/push/merge; không đổi hardware/contract/dataset/human gate**.
- Các prompt “cho phép sửa” chỉ áp dụng **khi ở feature branch và người làm đã đồng ý phạm vi sửa**. Nếu chưa, dùng bản `read-only` trước.

### P01 — Test Codex đã nhận Global Skills và Repo Skills (READ-ONLY)

```text
Đọc repository root và scoped AGENTS.md liên quan. Hãy xác minh trong phiên Codex này:
1) Có nhìn thấy code-review, diagnosing-bugs, tdd ở user-level Skills không?
2) Có nhìn thấy nckh-task-execution, nckh-pr-preflight trong .agents/skills của repo không?
3) Chỉ đọc đúng file SKILL.md của các Skill liên quan; ghi absolute path và kết quả thực sự đọc được.
4) Phân biệt: tìm thấy file / đọc file thành công / chưa sử dụng Skill / Skill không tồn tại.
Không sửa hoặc tạo file; không chạy test; không commit, push, merge.
Trả lời tiếng Việt, không tự tuyên bố Skill điều phối nckh-coding-workflow đã được cài.
```

### P02 — Tiếp nhận task và lập kế hoạch trước khi sửa (READ-ONLY)

```text
Tôi là <ME1/MS2/EE2/ET1/IT2>. Nguồn task hợp lệ: <README tuần + Task ID | PR finding URL | task hạ tầng được ME1 duyệt>.
Yêu cầu: <GOAL>. Acceptance criteria: <ACCEPTANCE>. Phạm vi: <FOLDERS>.
Đọc root/scoped AGENTS.md, task README và các contracts có liên quan.
Phân tích code hiện có, đề xuất file cần sửa, interface cần giữ, regression risks,
unit/build/hardware tests cần chạy, dependency với owner khác và Human Gate.
Nếu task/spec chưa rõ, liệt kê câu hỏi; không tự gán tiêu chí.
READ-ONLY: không sửa file, chạy build/test, cài dependency, commit/push/merge.
Chờ tôi duyệt plan.
```

### P03-A — **TÌNH HUỐNG CHÍNH:** IT2/EE2/... đã **code tay xong**, nhờ review trước (READ-ONLY)

```text
Tôi đã tự viết code cho task <TASK_ID> trên feature branch hiện tại.
Hãy đọc root/scoped AGENTS.md, task README và code/diff trong phạm vi <PATHS>.
Tự sử dụng Skills review phù hợp nếu có; đừng chỉ mô tả lý thuyết.
Review theo thứ tự: correctness -> research/hardware integrity -> contracts ->
error handling -> tests/reproducibility -> maintainability.
Với mỗi finding: BLOCKER/MAJOR/MINOR/QUESTION, file:dòng, chứng cứ,
hậu quả, sửa đề xuất và cách kiểm chứng. Phân biệt lỗi PR mới với lỗi tồn tại sẵn.
Chưa sửa file, chưa chạy test, chưa cài tool, chưa chạy bất kỳ lệnh Git thay người.
Cuối cùng báo Skills đã THỰC SỰ đọc và absolute SKILL.md paths.
```

### P03-B — Sau khi xem P03-A, **cho phép Codex sửa trên máy** (EDIT ĐƯỢC PHÊ DUYỆT)

```text
Tôi chấp thuận sửa các finding sau từ bản review: <FINDING_IDS>.
Giới hạn file được sửa: <ALLOWED_PATHS>. Không sửa các finding chưa duyệt.
Đọc lại root/scoped AGENTS.md và áp dụng workflow Skills phù hợp:
- chẩn đoán nguyên nhân trước khi vá lỗi chưa xác nhận;
- thêm regression/unit test nếu khả thi;
- sửa code tối thiểu, chạy đúng test có sẵn, review diff sau sửa.
Không thay đổi hardware locked, pinout, sampling/schema/units/labels/dataset.
Nếu cần thay contract hoặc phải đo board thật, DỪNG để hỏi.
Không chạy git add/commit/push/merge/reset/clean/force-push.
Báo file sửa, findings đã FIXED/chưa FIXED, test command/result thực tế,
NOT RUN, SKILL.md đã đọc và phần cần human review.
```

### P04 — Một lượt Local Review & Fix đã được phê duyệt sẵn (EDIT)

```text
Tôi cho phép review và sửa lỗi code trong phạm vi <ALLOWED_PATHS> trên feature branch <BRANCH>.
Task <TASK_ID>, acceptance <CRITERIA>; không mở rộng scope.
Đọc root/scoped AGENTS.md và Skills phù hợp với lỗi thực tế.
Trước mỗi sửa: nêu defect + evidence + minimal fix; nếu ảnh hưởng interface/human gate thì hỏi tôi.
Viết/chạy relevant tests nếu môi trường có; không bịa PASS, ghi NOT RUN nếu thiếu.
Làm tối đa các thay đổi liên quan trực tiếp task, rồi self-review toàn bộ diff.
Bàn giao kết quả bằng tiếng Việt. Tuyệt đối không Git add/commit/push/merge.
```

### P05 — Có bug nhưng chưa biết nguyên nhân (DIAGNOSE, chưa sửa)

```text
Hiện tượng: <ERROR/TRACEBACK/LOG/SYMPTOM>. Đường dẫn liên quan: <PATHS>.
Mục tiêu: xác định root cause, KHÔNG đoán sửa vội.
Đọc root/scoped AGENTS.md và dùng quy trình diagnosing-bugs nếu có.
Tái hiện bằng test/log/code-path được phép đọc; tách CONFIRMED và HYPOTHESIS.
Đánh giá input, trạng thái, invariant, concurrency, units, exception, missing data.
Đưa 2-3 phép kiểm chứng phân biệt các giả thuyết; ưu tiên bằng chứng có thể tái hiện.
READ-ONLY hiện tại: không sửa file, không tự cài dependency, không commit/push/merge.
Nếu muốn chạy command, hãy liệt kê command và rủi ro trước để tôi duyệt.
```

### P06 — Viết regression test theo TDD cho lỗi đã xác định (EDIT)

```text
Finding đã xác nhận: <FINDING>. Acceptance: <EXPECTED_BEHAVIOR>.
Phạm vi được sửa: <SOURCE_AND_TEST_PATHS>. Đọc root/scoped AGENTS.md.
Sử dụng tdd phù hợp để:
1) xây test tái hiện lỗi, nêu vì sao test phải FAIL trước bản vá (RED);
2) chạy test thật nếu có runner; ghi log RED, hoặc NOT RUN nếu không có;
3) sửa tối thiểu để test PASS (GREEN);
4) refactor an toàn rồi chạy lại regression tests.
Thêm edge cases phù hợp; không sửa contract để test giả PASS.
Không Git add/commit/push/merge. Kết thúc nêu lệnh/log và điều chưa kiểm chứng.
```

### P07 — Preflight trước khi human commit hoặc mở PR (READ-ONLY)

```text
Review toàn bộ thay đổi hiện tại trước khi đưa vào PR <feature -> dev>.
Đọc AGENTS.md, task <TASK_ID>, CODEOWNERS, project standards và
nckh-pr-preflight/SKILL.md nếu có.
Kiểm git status/diff, danh sách file untracked/staged, wrong-subsystem files,
secret, raw data, accidental binary/log/build files, contract migrations,
unit test evidence, Human Gate và cross-team dependencies.
Xuất BLOCKER/MAJOR/MINOR/QUESTION kèm file:dòng và cách kiểm chứng.
Chỉ đọc/kiểm tra, không sửa code, không stage/commit/push/merge.
Không đánh giá hardware PASS nếu chưa có log đo thật.
```

### P08 — IT2: Review serial parser nhận packet từ EE2 (READ-ONLY hoặc EDIT sau duyệt)

```text
Tôi là IT2. Review <SOFTWARE_SERIAL_LOGGER_PATH> theo packet/schema EE2 đã phê duyệt.
Đọc AGENTS.md, task README, parser và tài liệu giao thức thực tế.
Kiểm truncated frame, partial reads, reconnect, sample_index, timestamp_us,
CRC nếu protocol có, units, record ID, duplicate/missing/out-of-order samples,
error counter và việc báo lỗi thay vì âm thầm làm sạch dữ liệu.
Đề xuất test bằng synthetic fixtures có expected output rõ ràng;
KHÔNG tự chế định dạng frame, field hay sample rate mới.
READ-ONLY trước; báo finding + test cases + dependency với EE2.
```

### P09 — IT2: QC Engineering Validation Data (READ-ONLY trước)

```text
Review code QC ở <PATHS> cho Engineering Validation Data.
Đọc AGENTS.md, CODEX_RESEARCH_STANDARD_V1.md và metadata/protocol thật.
Kiểm empty recording, NaN/Inf, timestamps không đơn điệu, missing sample,
sensor range, số mẫu, setup_version/fixture_version, physical condition metadata.
Tách QC technical status khỏi data_origin/inclusion_status/approval_status.
Không bao giờ biến validation recording thành Dataset v0.1 vì QC PASS.
Báo tests dự kiến, chỗ thiếu metadata, yêu cầu phê duyệt owner.
Không sửa file ở vòng này; không bịa phép đo hoặc nhãn.
```

### P10 — IT2: Kiểm data leakage trong ML split (READ-ONLY trước)

```text
Review toàn bộ đường split/train/evaluate tại <PATHS>.
Tập trung recording-level/group-level leakage, window overlap, recording ID,
train-only normalization/feature selection, cross-validation groups,
random seed, class balance, unseen setup/domain và metric reporting.
Đọc root/ml scoped AGENTS.md + research standard và code thật.
Tạo bảng risk -> file:dòng -> test tái hiện -> hướng fix.
Không coi accuracy đo trên Engineering Validation Data là claim Dataset v0.1.
Chưa sửa file hoặc chạy training, không commit/push/merge.
```

### P11 — IT2: Reference DSP feature pipeline (READ-ONLY trước)

```text
Task <TASK_ID>: review PC reference DSP ở ml/features/ hoặc file <PATH>.
Kiểm RMS, mean, std, peak, crest factor, FFT/Goertzel theo contract hiện có.
Xác minh window size, sample rate, overlap, frequency bins, amplitude scaling,
units, input dtype và edge cases: zero/constant/sine/short/NaN.
Đề xuất exact unit tests, expected math nếu có cơ sở và numeric tolerances.
Nếu thiếu config/ground truth, nêu UNKNOWN thay vì tự đặt thông số.
Chỉ review, chưa sửa hoặc chạy benchmark thật.
```

### P12 — IT2 + EE2: PC DSP và ESP32 DSP equivalence (PLAN/READ-ONLY)

```text
Đối chiếu PC DSP <PYTHON_PATH> với ESP32-S3 DSP <FIRMWARE_PATH>.
Đọc AGENTS.md, scoped rules và cùng một feature contract đã duyệt.
Kiểm window alignment, sample rate, units, sample ordering, numerical precision,
fixed-point/float overflow, RMS/FFT formulas, feature order và tolerance.
Lập kế hoạch test cùng input có provenance, command, output và metric so sánh.
KHÔNG tuyên bố equivalence PASS nếu chưa chạy cả hai và thu log bằng chứng.
Nếu cần sửa firmware (EE2-owned), chờ EE2/ME1 duyệt phạm vi.
Không sửa/flash/commit/push/merge.
```

### P13 — IT2: Baseline ML / TinyML và benchmark resource (READ-ONLY)

```text
Review <MODEL_OR_TRAINING_PATHS> cho baseline/TinyML task <TASK_ID>.
Kiểm train/test leakage, versions, physical labels, seeds, reproducibility,
quantization assumptions, model input shape/schema, class mapping,
Flash/RAM/latency target từ contract và cách thu benchmark thật.
Phân biệt measured board result, desktop simulation và projected estimate.
Không coi unverified model metrics là scientific claims đã chứng minh.
Không tự chạy training nặng, cài libs, thay sensor hoặc upload artifacts.
Báo missing tests, evidence và Human Gate nếu thay architecture/model claim.
```

### P14 — IT2: PC Live Dashboard / Serial visualization (REVIEW)

```text
Review dashboard tại <PATHS> cho Edge Condition Monitor v0.1.
Kiểm parsing, reconnect/backpressure, stale data, frame loss,
units của time/RMS/frequency, trend plots, recording ID và sensor-health flags.
Không hiển thị prediction/FFT thành physical Ground Truth.
Đề xuất unit/integration tests giả lập mất kết nối và dữ liệu cắt đoạn.
Báo lỗi confirmed vs speculative; không tự sửa trước khi tôi chấp thuận.
```

### P15 — EE2: Acquisition firmware theo risk assessment (READ-ONLY)

```text
Tôi là EE2; review firmware <PATHS> cho ESP32-S3 + ADXL345.
Đọc root/firmware AGENTS.md, current acquisition contract và task <TASK_ID>.
Kiểm ISR duration, FIFO, ring/double buffer, Serial/OLED blocking,
producer/consumer race, I2C/SPI failures, recovery, watchdog, memory bounds.
Không tự đổi ODR, range, pins, state machine contract, clock source hoặc schema.
Đề xuất host-side tests và hardware tests riêng; không giả định đã đo 60 giây.
READ-ONLY; các lỗi có bằng chứng ghi file:dòng, severity và cách tái hiện.
```

### P16 — EE2: Timestamp/sample loss bug (DIAGNOSE)

```text
Nghi ngờ: bản ghi 60 s có dropped samples hoặc timestamp bất thường.
Đọc firmware <PATH>, logging/parser schema và root/firmware AGENTS.md.
Điều tra FIFO burst timestamp, ISR/task scheduling, sample_index gaps,
micros() wrap/clock resolution, buffer overflow, serial throughput và STOP flush.
Chỉ xác nhận bug nếu code/log đủ chứng cứ; còn lại đánh dấu HYPOTHESIS.
Thiết kế counters/log/host test và bench protocol 60 s để kiểm chứng.
Không flash board, không sửa source, không chạy stress test ở bước này.
Không commit/push/merge.
```

### P17 — EE2: Lập kế hoạch 60-second bench validation (READ-ONLY)

```text
Thiết kế kế hoạch kiểm thử acquisition 60 s trên ESP32-S3/ADXL345.
Dựa trên contract firmware/metadata thực tế, đề xuất điều kiện board,
sample rate, expected-count math, sample-index/timestamp QC, error counters,
repeats, motor state, log path, firmware SHA, acceptance threshold có nguồn.
Phân biệt host simulation/compile với hardware measurement.
Nếu threshold chưa được phê duyệt, ghi NEEDS OWNER DECISION, không tự đặt PASS.
Chỉ lập kế hoạch; không flash, điều khiển motor hoặc ghi dữ liệu vào dataset.
```

### P18 — MS2: ADXL345 calibration/sensor-health code (REVIEW)

```text
Tôi là MS2; review code ADXL345 characterization <PATHS>.
Đọc root/experiments AGENTS.md, protocol và metadata hiện hành.
Kiểm units, orientation, bias/noise/sensitivity, self-test assumptions,
temperature/mounting confounders, uncertainty/repeatability.
Chỉ dùng số đo có log/recording refs; tests với dữ liệu synthetic phải ghi rõ synthetic.
Không kết luận sensor drift từ một biểu đồ thiếu controls; không tự sửa Ground Truth.
Báo findings, missing evidence và dependency IT2/EE2 nếu đổi schema.
```

### P19 — MS2 + IT2: Phân biệt drift với domain/mounting shift (PLAN)

```text
Review drift detector/sensor-health metrics ở <PATHS>.
Đọc standards NCKH và current measurement/protocol references.
Phân biệt sensor bias drift, noise change, sensitivity change,
operating-condition shift, mounting/setup domain shift và acquisition artifacts.
Kiểm thresholds/config/version, false alarm, split-by-setup/recording và confounders.
Đề xuất controls/test matrix; không gọi classification output là ground truth.
READ-ONLY và nêu số liệu nào chưa đo hoặc chưa được phê duyệt.
```

### P20 — ET1: Schematic / pinout / electronics code (REVIEW)

```text
Tôi là ET1; review <SCHEMATIC_OR_SCRIPT_PATHS> cùng interface contract.
Kiểm ESP32-S3 pin mapping, 3.3V/5V levels, ACS724 current range,
PT100/MAX31865, US5881 Hall, DHT22, I2C pullups, grounding, EMI,
connector IDs, BOM ↔ schematic consistency và datasheet citations có sẵn.
Phân biệt analysis/simulation với bench voltage/current measurements.
Không tự khẳng định điện an toàn, không thay pinout khi EE2 chưa đồng ý.
READ-ONLY; chỉ đề xuất sửa cùng yêu cầu bench/human gate.
```

### P21 — ME1: Onshape CAD, fixture, protocol và architecture (READ-ONLY REVIEW)

```text
Tôi là ME1. Rà tài liệu/registry/exports/report trong <ME1_PATHS>.
Đọc root AGENTS.md, hardware/mechanical/AGENTS.md và
NCKH_ME1_ONSHAPE_CAD_GITHUB_WORKFLOW_REQUIREMENTS_V2.md.
Với CAD mới, Onshape CAD TEST khác ME1_MAIN_CAD:
- xác minh privacy, chủ Document/reviewer access; nếu chưa có: PENDING/BLOCKED;
- chỉ dùng MAIN immutable Version ID/URL cho release, không dùng Workspace URL;
- kiểm Part/Assembly ID, engineering Revision, fixture/setup Version, datum/axis,
  ADXL345 orientation, mating/clearance, Drawing/STEP/BOM/manifest/report backlink;
- đối chiếu source Python+params (nếu thật có) với STEP/version và export units;
- chọn static/modal/dynamic/fatigue/bench tests theo rủi ro, solver có thật.
Modal không xác nhận forced response. Không tạo FEA/hardware/safety PASS giả.
Không sửa SolidWorks LEGACY, Ground Truth hoặc dataset inclusion; nếu cần
đổi fixture/fault protocol, nêu Human Gate và owner dependency.
READ-ONLY: báo finding có chứng cứ file:dòng, NOT_RUN/BLOCKED và
NEEDS HUMAN CAD/FABRICATION REVIEW; không sửa file hoặc Git.
```

### P22 — Sửa finding từ GitHub Codex Review (READ-ONLY rồi EDIT khi duyệt)

```text
PR: <URL>, branch: <BRANCH>, finding: <COPY_FULL_COMMENT_AND_FILE_LINE>,
Reviewed commit: <SHA>.
Đọc AGENTS.md, diff/branch hiện tại, code liên quan và test thật.
Xác nhận finding có còn đúng trên HEAD không; phân loại CONFIRMED/
PARTIAL/FALSE POSITIVE/OUTDATED/NEEDS HUMAN DECISION.
Đưa root cause, minimal patch, regression tests và phạm vi file sẽ sửa.
READ-ONLY đến khi tôi duyệt phương án. Không tự commit/push/merge.
Sau khi duyệt mới edit, run tests, self-review và báo report + suggested PR reply.
```

### P23 — GitHub Actions `repo-quality` hoặc CI khác FAILED (DIAGNOSE)

```text
PR <URL>, failed job <JOB_NAME>, log lỗi <PASTE_LOG_OR_LINK>.
Đọc workflow YAML/commands thật trên branch và file liên quan.
Giải thích lỗi thuộc source/task, dependency, environment hay CI configuration.
Phân biệt failing test với lint/repo policy; tìm bước repro local chính xác.
Trước tiên READ-ONLY, không sửa workflow để làm job 'xanh' giả.
Đề xuất minimal fix + lệnh kiểm chứng; nếu cần đổi .github/, hỏi ME1.
Không commit/push/merge; không mua API credits hay thêm secrets.
```

### P24 — Local review lại sau khi Codex đã sửa (READ-ONLY)

```text
Thực hiện independent-style self-review lần cuối cho diff local vừa sửa.
Kiểm input acceptance, regression, edge cases, tests thực chạy,
new failure introduced, accidental file changes, secret/raw datasets,
public interfaces và Human Gate. Đọc root/scoped AGENTS.md và preflight Skill.
Đặc biệt xác minh bản vá không làm test FAIL bị che bởi mocking quá mức.
Mỗi finding có file:dòng/evidence; không tự khẳng định zero defects.
READ-ONLY: không sửa, không Git add/commit/push/merge.
```

### P25 — Phản biện false positive / finding stale của GitHub Codex (READ-ONLY)

```text
Đánh giá lại GitHub finding <FULL_COMMENT> trên PR <URL>.
So sánh code tại Reviewed SHA và HEAD; đọc contract/test evidence hiện hành.
Nếu false positive, giải thích bằng file:dòng, state assumptions và test/log thật.
Nếu finding đúng một phần, đề xuất fix nhỏ và cách kiểm chứng.
Không tranh luận kiểu 'AI kia sai' không có bằng chứng.
READ-ONLY, chuẩn bị comment phản hồi tiếng Việt; không tự đăng comment.
```

### P26 — Codex soạn handoff và PR description (REPORT-ONLY)

```text
Dựa trên task <TASK_ID>, diff đã duyệt và test thực chạy,
hãy soạn PR description phù hợp .github/PULL_REQUEST_TEMPLATE.md.
Bao gồm owner, paths, mục tiêu, acceptance, test command+result,
evidence refs, data_origin/schema impact, interfaces/dependencies,
Codex local skills thực sử dụng, remaining findings và Human Gate.
Nếu thiếu dữ liệu điền rõ TODO/NOT RUN, không bịa Issue/measurement/approval.
Không sửa source, không git add/commit/push, không mở/merge PR.
```

### P27 — GitHub comment yêu cầu Codex review PR (GITHUB COMMENT)

```text
@codex review
```

*Nhớ:* đây là **comment trên Pull Request GitHub**, **không phải lệnh PowerShell**. Cấu hình tích hợp cần hỗ trợ trigger, và review có thể bị ảnh hưởng bởi quyền hoặc quota.

### P28 — Lỗi hợp đồng chung / cần phối hợp owner (READ-ONLY)

```text
Task <TASK_ID> cần sửa <FILES> và có khả năng đổi <INTERFACE_OR_SCHEMA>.
Đọc source of truth contract, current scoped AGENTS và CODEOWNERS.
Liệt kê consumer/producer, owner liên quan, compatibility, unit/timebase,
version/migration, các test cần cùng chạy và dữ liệu cũ bị ảnh hưởng.
Phân tách sửa nội bộ module với breaking cross-team change.
Chưa sửa file; liệt kê Human Gate và thông báo [PROJECT UPDATE] cần làm.
Không tự quyết định contract mới hoặc thay quyền owner.
```

### P29 — Không có board/device/toolchain (CHỈ PHÂN TÍCH THÍCH HỢP)

```text
Hiện không có <ESP32-S3 BOARD/ADXL345/SENSOR/HARDWARE> hoặc toolchain.
Hãy review code <PATHS> bằng static analysis và host-side tests khả dụng.
Phân loại rõ: STATIC REVIEW, HOST TEST, COMPILATION, HARDWARE BENCH TEST.
Không chạy lệnh cần board, không báo sampling reliability/latency/Flash PASS.
Soạn bench verification checklist để EE2/ME1 chạy khi có thiết bị.
Không tự cài toolchain, flash board, thay config hoặc commit/push/merge.
```

### P30 — Regression nghiêm trọng sau push (READ-ONLY trước)

```text
Sau commit <SHA> trên feature branch <BRANCH>, phát hiện regression <SYMPTOM>.
Đọc diff từ base dev, logs và tests đã từng PASS.
Xác định first bad change, affected interfaces, khả năng rollback bằng patch
nhưng KHÔNG chạy reset, clean, force-push hoặc rewrite history.
Đề xuất minimal corrective commit/test; nếu ảnh hưởng team, yêu cầu owner.
READ-ONLY hiện tại; tôi sẽ duyệt bản sửa và tự thực hiện Git.
```

### P31 — PR chỉ sửa tài liệu, không cần runtime test (REVIEW)

```text
Review tài liệu <FILES> so với AGENTS.md và docs đang authoritative.
Kiểm link/path, version, Task ID, PR template, ownership, Human Gate,
phần code command copy-paste và mâu thuẫn nội dung.
Đánh dấu runtime/hardware tests NOT APPLICABLE (nêu lý do),
không tự đánh dấu tests PASS khi không có việc để chạy.
Không sửa file, không Git add/commit/push/merge.
```

### P32 — Phát hiện secret/token/raw data bị đưa vào working tree (STOP)

```text
Có nghi ngờ repository đang chứa secret/raw data không được phép: <PATH>.
Chỉ kiểm tra metadata/path và diff liên quan một cách hạn chế.
Không in toàn bộ secret vào chat, không copy credentials trong báo cáo.
Đánh giá đã staged/committed/pushed chưa và đề xuất biện pháp cách ly,
rotation/revocation và liên hệ ME1 nếu cần.
KHÔNG tự chạy reset/clean/force-push/history rewrite.
Không commit/push/merge. Chờ người phụ trách phê duyệt xử lý.
```

### P33 — Code tay xong nhưng chưa có bất kỳ test nào (PLAN)

```text
Review <CODE_PATHS>: hiện chưa có test harness phù hợp.
Xác định public behavior, edge cases, nguồn sample test hợp lệ,
những tests đầu tiên tối thiểu có giá trị và runner sẵn có trong repo.
Đề xuất cách bổ sung test theo TDD mà không tạo nhiều framework vô ích.
Không tự thêm package, workflow, file cấu hình hoặc generate fake dataset.
Chờ tôi duyệt test plan trước khi sửa code.
```

### P34 — Kiểm tra model **tự chọn Skill** khi không gọi tên (READ-ONLY)

```text
Tôi vừa viết code trong <PATH> và cần phân tích những lỗi có thể gây regression.
Đọc repo instructions, inspect git diff và file liên quan; đề xuất kiểm tra có bằng chứng.
Không sửa file, không chạy test hay thao tác Git thay tôi.
Cuối câu trả lời: liệt kê chính xác các instruction/workflow files đã đọc,
đường dẫn tuyệt đối và tác động của từng file đến kết luận.
Nếu chưa đọc file nào ngoài AGENTS.md, nói rõ; không nói đã dùng một công cụ chưa dùng.
```

### P35 — Tiếp tục task bị ngắt do Codex hết hạn mức / VS Code đóng (READ-ONLY)

```text
Phiên Codex trước bị ngắt. Không giả định task đã hoàn tất.
Đọc git branch/status/diff, task <TASK_ID>, báo cáo cuối cùng (nếu có),
source/tests liên quan và AGENTS.md.
Đối chiếu: cái gì đã sửa, cái gì còn pending, test nào thực sự chạy,
file nào chưa được commit và có xung đột với changes mới không.
Đề xuất cách tiếp tục an toàn; chờ tôi duyệt trước khi edit/test.
Không commit/push/merge/reset/clean.
```

### P36 — Sau merge: soạn bản thông báo thay đổi liên nhóm (REPORT-ONLY)

```text
PR <URL> đã được human merge vào dev; thay đổi ảnh hưởng <OWNERS/INTERFACES>.
Đọc NCKH_TEAM_CHANGE_UPDATE_WORKFLOW_V1.md, PROJECT_UPDATE_TEMPLATE.md
và diff/commit/contract trên dev.
Soạn [PROJECT UPDATE]: Week, Member, Task changed, New deliverable,
Dependency, Status, Effective from; kèm PR link, version và READY/BLOCKED asks.
Không tự tuyên bố các thành viên đã đọc/đồng ý; không đổi Ground Truth/dataset.
Không sửa file, không gửi thông báo thay người khi chưa duyệt.
```

### P37 — Rà soát quyền hạn trước khi dùng nhiều Skills (PLAN)

```text
Đọc AGENTS.md và task <TASK_ID>. Chọn các Skills phù hợp từ:
code-review, diagnosing-bugs, tdd, nckh-task-execution, nckh-pr-preflight.
Giải thích tại sao chọn/bỏ từng Skill, tình huống cần STOP/Human Gate,
những file SKILL.md đã thực sự load và độ tin cậy của điều phối.
Tạo kế hoạch làm việc, nhưng chưa sửa file/test/Git.
Nếu nckh-coding-workflow chưa cài, nói thật và dùng quy trình thủ công.
```

### P38 — IT2: Review an toàn khi làm notebook/plot/figure (READ-ONLY)

```text
Review notebook/analysis/figures <PATHS> cho task <TASK_ID>.
Kiểm cell execution order, hard-coded local paths, random seeds,
reproducibility, plot axis units, filename/version, evidence provenance,
notebook outputs có raw/private data hoặc artifacts quá lớn.
Không suy diễn correlation/frequency peak thành Ground Truth.
Nếu notebook cần chạy, đề xuất môi trường, data sample hợp lệ và chi phí;
không tự execute notebook/training khi chưa được phép.
Không commit/push/merge.
```

### P39 — IT2: Phát hiện memory/performance regression trên edge (PLAN)

```text
Review <DSP_OR_TINYML_PATHS> vì nghi tăng latency/RAM/Flash.
Đọc firmware+ML contracts và các benchmark có evidence trên repo.
Phân biệt measured-on-board, host simulation, theoretical complexity.
Xác định thuật toán/buffer/tensor allocations có nguy cơ cao và kế hoạch đo:
board/toolchain revision, window size, repeat count, runtime/RAM/Flash method.
Không tự tạo con số benchmark hoặc đổi model architecture.
READ-ONLY, chờ owner duyệt trước khi tối ưu.
```

### P40 — Soát lại phạm vi AI Fix nếu bất ngờ sửa nhiều file (STOP/READ-ONLY)

```text
Codex vừa sửa nhiều file hơn scope đã duyệt.
DỪNG triển khai. Đọc git status, diff và task <TASK_ID>.
Phân nhóm file: in-scope / necessary shared dependency / accidental / unknown.
Giải thích từng thay đổi, dependency, cách thu hẹp patch mà không mất code tay.
Không tự restore/reset/clean/rebase, không stage/commit/push/merge.
Chờ tôi xác nhận file nào được giữ trước khi tiếp tục.
```

---
## 13. MA TRẬN XỬ LÝ CÁC TRƯỜNG HỢP PHÁT SINH

Không thể tiên liệu **mọi** lỗi tương lai của firmware/ML/GitHub, nhưng bảng dưới bao quát những nhánh quyết định thường gặp và các tình huống rủi ro cao trong NCKH. Gặp ca ngoài bảng: **dừng thao tác gây thay đổi, bảo toàn working tree/log, trình bày evidence, hỏi ME1 và domain owner**.

### 13.1. Trong lúc chuẩn bị máy và nhận task

| ID | Trường hợp | Quyết định/hành động đúng |
|---|---|---|
| S01 | VS Code mở nhầm folder cha/ZIP cũ | Kiểm `git rev-parse --show-toplevel`; mở root clone chính thức; không `git init` |
| S02 | Remote không phải `nkaq/SVNCKH--HUST` | Dừng push; kiểm `git remote -v`; hỏi ME1 trước khi sửa remote |
| S03 | Chưa được mời collaborator hoặc chưa accept | Nhờ ME1 mời và accept; không dùng token của người khác |
| S04 | Chưa có `dev` local | `git fetch origin`; kiểm `git branch -a`; nếu có `origin/dev` mới tạo branch tracking đúng |
| S05 | Local `dev` tụt xa remote | Khi sạch: `git switch dev` -> `git pull --ff-only origin dev` |
| S06 | `dev` có thay đổi chưa commit | Dừng; xác định ai sửa và file nào; không pull/switch cưỡng bức |
| S07 | Tạo branch mới trùng tên cũ | Kiểm `git branch --list`, `git branch -r`; chọn tiếp tục branch đúng hoặc tên task mới |
| S08 | Đang ở `main`/`dev` mà đã code tay | Dừng trước commit; kiểm diff; xin hướng dẫn chuyển an toàn sang feature branch |
| S09 | Task thiếu nguồn/acceptance hoặc thiếu Task ID *khi là task tuần* | Hỏi owner/ME1; PR finding/hạ tầng dùng ref hợp lệ thay Task ID theo §2.1 |
| S10 | Week plan cũ mâu thuẫn `[PROJECT UPDATE]` | Xác định version/effective date; dùng nguồn có hiệu lực, báo bất đồng |
| S11 | File cần sửa không có trong repo | Kiểm nhánh, thư mục, owner; đừng tạo file giả chỉ để Codex có cái review |
| S12 | Cần sửa thư mục không thuộc owner | Hỏi owner liên quan; xin task/PR scope và cập nhật interface nếu cần |
| S13 | GitHub Issue không tồn tại | Task README vẫn là nguồn hợp lệ; ghi `Issue: None`, không bịa `Closes #...` |

### 13.2. Local Codex, Skills và phiên làm việc

| ID | Trường hợp | Quyết định/hành động đúng |
|---|---|---|
| S14 | `tdd`/`code-review`/`diagnosing-bugs` chưa cài trên máy thành viên | Kiểm §6.3; cài riêng trên user-level nếu cần và được phép |
| S15 | `SKILL.md` tồn tại nhưng Codex không tự load | Reload Window/chat mới; chỉ định Skill rõ hơn; kiểm file thực đọc, không tuyên bố “auto” chắc chắn |
| S16 | Codex nói dùng Skill nhưng không chứng minh đã đọc | Yêu cầu đường dẫn `SKILL.md` và hoạt động thực tế; đánh dấu UNVERIFIED |
| S17 | Codex đề xuất dùng Skill bên thứ ba có lệnh commit | Không cho thực hiện; repository policy human Git có ưu tiên cao hơn |
| S18 | Codex tìm thấy bug nghi ngờ nhưng không có repro | `diagnosing-bugs`; giữ HYPOTHESIS/NEEDS EVIDENCE, không sửa đoán mò |
| S19 | Codex chỉ chê style không liên quan | Yêu cầu focus correctness/maintainability/risk; không tạo diff vô nghĩa |
| S20 | Codex định sửa ngoài file đã cho phép | DỪNG; dùng **P40**; duyệt scope lại hoặc thu hẹp bản vá |
| S21 | Codex thay pin/ODR/schema/units | DỪNG ngay, chuyển Human Gate/owner, yêu cầu version/migration |
| S22 | Codex muốn cài package, chạy script lạ | Xem command, repo trust, side effects, credential access; human quyết định |
| S23 | Codex dùng dữ liệu synthetic rồi báo đo thật | Từ chối kết quả; yêu cầu phân biệt SIMULATED/MEASURED và evidence refs |
| S24 | Codex hết quota/VS Code crash giữa chừng | Không làm lại mù; đọc `git status/diff`, dùng **P35**; bảo toàn local work |
| S25 | Agent tự sửa đúng nhưng test `NOT RUN` | Không cho PASS phần chưa kiểm; giữ pending bench/CI evidence và reviewer quyết định |
| S26 | Review không phát hiện lỗi | Vẫn chạy relevant tests + human diff review; không suy “không có lỗi” |
| S27 | Cần nhiều lượt debug/review | Cho chạy có giới hạn theo rủi ro; cuối mỗi vòng ghi test/result, không bắt buộc số lượt cố định |
| S28 | Prompt task mới dính context task cũ | Mở chat mới, nêu branch/Task ID, kiểm diff trước khi edit |
| S29 | Codex đề xuất thay đổi kiến trúc lớn để fix nhỏ | Yêu cầu minimal patch, impact analysis, hỏi owner nếu breaking |
| S30 | Không có `nckh-coding-workflow` | Đây là Skill mới **chưa cài**; dùng prompts/5 Skills hiện có, không coi mất workflow |

### 13.3. Code/test/bench/data

| ID | Trường hợp | Quyết định/hành động đúng |
|---|---|---|
| S31 | Unit test trước fix fail đúng như mong đợi | Lưu RED evidence; thực hiện patch; test GREEN rồi chạy regression |
| S32 | Test vẫn fail sau fix | Không commit như PASS; xem traceback/root cause, dừng sau số lượt hợp lý |
| S33 | Test PASS nhưng chưa chạy hardware | Ghi `HOST TEST PASS / HARDWARE NOT RUN`; giữ human bench gate |
| S34 | Thiếu `pytest`/firmware toolchain | Đánh dấu NOT RUN, liệt kê preconditions; không tự cài không phép |
| S35 | Không có một test nào trong repo | Lập test plan P33; không bịa output `pytest` |
| S36 | Build firmware PASS nhưng timing kém | Compile ≠ acquisition reliability; cần 60 s stress/counter bench logs |
| S37 | 60 s log mất mẫu nhưng không biết nguồn | Tách sensor/FIFO/buffer/serial/parser; chẩn đoán theo P16, không gán lỗi cho EE2 khi chưa xác minh |
| S38 | Timestamp negative/duplicate/out-of-order | Kiểm timebase, rollover, packet ordering, event time/arrival time; không sửa dữ liệu im lặng |
| S39 | Parser không khớp packet EE2 | Dừng thay schema đơn phương; đối chiếu approved interface và owner EE2/IT2 |
| S40 | Recording rất ngắn/rỗng | Phải có test, return/error policy rõ theo contract; không chia cho zero |
| S41 | Train/test xuất hiện window cùng recording | BLOCKER leakage; sửa split theo group, kiểm recording ID disjoint |
| S42 | Scaler fit trên toàn dataset | BLOCKER leakage; fit only train, transform val/test; rerun metrics hợp lệ |
| S43 | Khác setup/mounting nhưng gộp domain | Dừng; kiểm setup_version/domain, owner ME1/MS2, evaluate domain shift riêng |
| S44 | QC PASS cho Engineering Validation recording | Vẫn `data_origin=ENGINEERING_VALIDATION`, `NOT_ELIGIBLE` cho Dataset v0.1 |
| S45 | Ground Truth suy từ model/RMS/FFT | BLOCKER research integrity; chỉ dùng physical condition có provenance |
| S46 | Đã đổi fixture sau khi thu data | Version domain/setup; không trộn như cùng setup; ME1 quyết định ảnh hưởng |
| S47 | Model metric thay đổi sau preprocessing | Kiểm seed/split/leakage/config, không cherry-pick metric |
| S48 | RAM/Flash/latency chưa đo trên ESP32 | `NOT RUN`/estimate; không tuyên bố đạt edge resource budget |
| S49 | Raw data/model binary/log quá lớn | Không stage; theo manifest/sample/LFS policy; hỏi owner |
| S50 | ET1/ME1 có nguy cơ an toàn | Dừng điện/motor/CAD physical operation; Human Gate trước khi chạy bench |
| S51 | ADXL345 calibration đổi unit/filter | Kiểm contracts; MS2, EE2, IT2 thống nhất version và test equivalence |
| S52 | Thiếu metadata/recording_id | Không tự đặt nhãn/ID giả; yêu cầu chủ recording bổ sung/cách ly data |

### 13.4. Git, PR, Codex GitHub và merge

| ID | Trường hợp | Quyết định/hành động đúng |
|---|---|---|
| S53 | `git switch dev` bị chặn vì dirty files | Đọc `git status`; bảo toàn thay đổi; không discard |
| S54 | `git pull --ff-only` báo không thể fast-forward | Kiểm local commits/remote; hỏi ME1; không tự force reset |
| S55 | Đã stage nhầm file | `git restore --staged -- <path>`; nội dung working tree còn nguyên |
| S56 | Commit đã tạo nhưng phát hiện còn secret | DỪNG push; xử lý secret và lịch sử với người quản lý; không tự force rewrite |
| S57 | Push bị `non-fast-forward` | `git fetch origin`, kiểm remote history; phối hợp người cùng branch, không force-push |
| S58 | PR base nhầm `main` | Sửa PR base thành `dev` sau khi kiểm diff; không merge nhầm |
| S59 | PR chứa file của hai subsystem | Xác định dependency, owner, cần split PR hoặc cross-owner review |
| S60 | GitHub Actions chưa chạy vì chỉ push feature | Mở PR vào `dev` để kích hoạt `pull_request` workflow hiện cấu hình |
| S61 | GitHub CI đỏ | Xem failed job/log; P23; sửa nguyên nhân, không xóa test để xanh giả |
| S62 | GitHub Codex chưa phản hồi | Kiểm setting/quota/triggers và comment `@codex review` khi phù hợp |
| S63 | Review `Completed` nhưng còn P1/P2 | Không merge vì chữ Completed; xử lý từng finding và human review |
| S64 | Codex review trên SHA cũ | Chờ/re-request review SHA mới; không dùng review cũ làm evidence cho code mới |
| S65 | Codex GitHub tìm false positive | P25; reply với test/file evidence, xin reviewer xác nhận nếu cần |
| S66 | Codex hết quota/permission error | Ghi lý do, chuyển human review độc lập; không bỏ qua CI/Human Gate |
| S67 | Tác giả PR là CODEOWNER duy nhất | Tác giả không tự approve; nhờ reviewer khác; nếu required owner review thì xử lý rule/owner hợp lệ với ME1 |
| S68 | Merge button sáng nhưng thiếu evidence | KHÔNG merge; quality gate vận hành vẫn có hiệu lực dù GitHub UI chưa chặn |
| S69 | Có blocker nghiêm trọng chưa xử lý | FIX THEN REVIEW/DO NOT MERGE; owner phải quyết định ngoại lệ phù hợp, không bỏ qua an toàn |
| S70 | PR chỉ sửa docs | Documentation consistency/link checks; ghi runtime `NOT APPLICABLE`, không bịa test PASS |
| S71 | PR đã merge `dev` nhưng thành viên chưa biết thay đổi | `[PROJECT UPDATE]` có version/effective date, tag owner, yêu cầu READY/BLOCKED |
| S72 | Muốn release `dev -> main` | PR release riêng, integration/milestone evidence, **ME1/human release gate** |
| S73 | Nhánh test cố tình chứa lỗi review | Giữ riêng, không merge production, không kết luận production code bị lỗi |
| S74 | Review GitHub chưa thấy Skills Matt Pocock trên ổ D | Bình thường; GitHub review dùng repo instructions; không tìm cách truyền Junction/local path lên cloud |
| S75 | Muốn chính GitHub tự sửa rồi commit/push | Không thuộc mặc định hai-stage review; cần cơ chế riêng, phê duyệt rõ và đánh giá quyền/budget |
| S76 | Tự động review quá nhiều PR gây hết quota | Kiểm setting/usage, ưu tiên PR quan trọng, dùng human fallback có evidence |

---

## 14. SỔ TAY XỬ LÝ LỖI GIT VÀ CODEX

### 14.1. Bộ lệnh chẩn đoán **chỉ đọc** cần gửi cho ME1

```powershell
git rev-parse --show-toplevel
git remote -v
git branch --show-current
git branch -avv
git status --short
git log -5 --oneline --decorate
git diff --stat
git diff --cached --stat
```

**KHÔNG gửi screenshot terminal đầy đủ hoặc copy nguyên output trước khi rà soát và che bí mật.** `git remote -v` có thể in URL chứa user-info/PAT (ví dụ `https://USER:TOKEN@github.com/ORG/REPO.git`); ảnh cũng có thể chứa email, path riêng, nội dung dữ liệu hoặc token từ log khác. Tốt nhất gửi **chỉ các dòng text liên quan đã được redaction thủ công**, không gửi nguyên màn hình; thay toàn bộ user-info/credential bằng `<REDACTED>`, ví dụ `https://<REDACTED>@github.com/nkaq/SVNCKH--HUST.git`. Không copy giá trị token thật vào prompt, regex, file log hoặc phần ví dụ; không khẳng định Git tự che secrets. Nếu lỡ lộ credential: **dừng chia sẻ, revoke/rotate ngay** và báo owner. 

### 14.2. `fatal: not a git repository`

Nguyên nhân: terminal chưa ở root clone. Thực hiện:

```powershell
Get-Location
Get-ChildItem
# Tìm folder clone đúng rồi Set-Location tới đó
# Set-Location 'D:\...\SVNCKH--HUST'
git rev-parse --show-toplevel
```

Không chạy `git init` tại folder ZIP để biến nó thành repo mới.

### 14.3. `pathspec 'dev' did not match`

```powershell
git remote -v
git fetch origin
git branch -a
```

Nếu `remotes/origin/dev` tồn tại nhưng local `dev` chưa có:

```powershell
git switch --track -c dev origin/dev
```

Chỉ chạy nếu branch local chưa tồn tại và remote chính xác. Nếu Git báo `dev` đã tồn tại, kiểm trạng thái; đừng cưỡng bức.

### 14.4. `git switch`/`git pull` bị chặn vì chưa commit

```powershell
git status --short
git diff --stat
git diff
```

**Không chạy `git reset --hard` hoặc `git clean -fd`.** Chọn bảo toàn thay đổi vào branch/task hiện tại; nếu phải stash, trước tiên hiểu rằng `git stash push` mặc định không bao gồm mọi file untracked, trong khi `-u` sẽ cất thêm file untracked. Chỉ dùng với người hiểu danh sách file và cách `stash list`/`stash show`/`stash pop`; đừng dùng như lệnh “fix tất cả” mặc định.

### 14.5. Nhánh `dev` đang có commit local ngoài ý muốn

```powershell
git switch dev
git status --short
git fetch origin
git log --oneline --left-right --graph dev...origin/dev
```

Nếu thấy commit chỉ có bên `dev`, **không pull/rebase/reset cưỡng ép**. Gửi graph cho ME1; có thể cần giữ commit bằng một feature branch trước khi sửa `dev`. Phục hồi phụ thuộc trạng thái thực tế.

### 14.6. Đang merge nhầm `origin/dev` vào `main` hoặc conflict nghiêm trọng

```powershell
git status
```

Nếu Git xác nhận **merge đang diễn ra** và quyết định hủy merge:

```powershell
git merge --abort
```

Nếu **rebase đang diễn ra** (không phải merge), và quyết định hủy:

```powershell
git rebase --abort
```

Không gọi hai lệnh ngẫu nhiên nếu trạng thái không tương ứng. Sau đó kiểm `git status`, `git branch --show-current` rồi mới quay lại feature/dev chuẩn. Tránh tự giải conflict trong `main` khi lẽ ra phải làm trên feature branch.

### 14.7. Merge conflict khi đưa `origin/dev` vào feature branch

```powershell
git status --short
git diff --name-only --diff-filter=U
```

Mở từng file conflict và **đối chiếu contract mới nhất**, không chỉ chọn “Accept Incoming”/“Accept Current” cho tất cả. Nếu conflict liên quan protocol, schema, pinout hoặc `AGENTS.md`, hỏi owner. Sau khi giải tay và test, human stage đúng file. Nếu muốn hủy merge, chỉ khi merge đang tiến hành mới dùng `git merge --abort`.

### 14.8. Git push bị từ chối `non-fast-forward`

```powershell
git remote -v
git fetch origin
git branch -vv
```

Dùng tên remote branch **thực tế** lấy từ `git branch -vv`, ví dụ nếu nhánh đó tồn tại:

```powershell
git log --oneline --left-right --graph HEAD...origin/feature/it2-edge-dsp
```

Nếu commit của người khác đã có trên remote, phối hợp; dùng merge hợp lý nếu được duyệt, **không `push --force`**.

### 14.9. Lỡ stage file sai nhưng CHƯA commit

```powershell
git diff --cached --name-only
git restore --staged -- path/to/file
git diff --cached --name-only
```

`git restore --staged` bỏ file khỏi index nhưng **không xóa nội dung working file**.

### 14.10. Lỡ commit vào sai branch / commit chứa secrets

**DỪNG push ngay.** Kiểm `git branch --show-current`, `git log -3 --oneline`, `git show --stat HEAD`; thông báo ME1. Nếu secret đã push, cần rotate/revoke và xử lý lịch sử theo quy trình bảo mật; **không tự sửa bằng force push**. Cách khôi phục commit sạch khác nhau theo việc đã push/chưa push nên không có một lệnh tự động an toàn cho mọi trường hợp.

### 14.11. `git diff` rỗng nhưng VS Code có file mới

`git diff` không liệt kê file `??` chưa được theo dõi.

```powershell
git status --short
```

Mở file mới trong editor để xem; không `git add -A` chỉ để “nhìn thấy diff”. Kiểm `git status --short` và nội dung file trước khi stage.

### 14.12. Không biết PR đã bao gồm những commit/file nào

```powershell
git fetch origin
# Khi đang ở feature branch
git log --oneline origin/dev..HEAD
git diff --name-status origin/dev...HEAD
git diff --stat origin/dev...HEAD
git diff origin/dev...HEAD
```

`origin/dev...HEAD` là so sánh từ **merge base** với nhánh feature, thường phù hợp xem tổng diff PR. `git diff` không tự bao gồm uncommitted/untracked: kiểm thêm `git status` và `git diff` hiện tại.

### 14.13. Tạo PR bằng giao diện web và bằng GitHub CLI (tùy chọn)

**Khuyến nghị:** GitHub UI để điền đầy đủ template và chọn reviewers:
<https://github.com/nkaq/SVNCKH--HUST/compare/dev...feature/it2-edge-dsp> (đường dẫn minh họa — chỉ hoạt động nếu branch đó tồn tại).

Nếu đã **cài và đăng nhập `gh`**, có thể dùng terminal:

```powershell
gh --version
gh auth status

# Tạo PR từ feature branch đã push và mô tả đầy đủ đã lưu vào file
# gh pr create --base dev --head feature/it2-edge-dsp `
#   --title "feat(it2): implement edge DSP" --body-file .\PR_BODY.md

# Các lệnh xem thông tin, không tạo PR
gh pr status
gh pr checks
gh pr view --web
```

File `PR_BODY.md` chỉ là **file nháp cục bộ**; không để nó vô tình trở thành commit nếu task không yêu cầu. `gh` không bắt buộc, không giả định đã cài trên tất cả máy.

### 14.14. GitHub Actions hiển thị skipped / chưa chạy

- Kiểm PR có base `dev`/`main` và trigger YAML thật.
- Khi chỉ push feature branch chưa mở PR, workflow hiện tại **không được định nghĩa chạy với `push` feature**; đó không phải lỗi.
- Nếu PR từ fork/permission hạn chế, xem GitHub Actions UI/log và quyền.
- CI `repo-quality` PASS không bao hàm ML tests, firmware compile hay bench tests.

### 14.15. Codex GitHub Review không tự xuất hiện

1. Vào PR -> Conversation/Files changed, tìm `Codex Review Summary`, reviewer `chatgpt-codex-connector` và pending activity.
2. Xác nhận PR là **Ready** và repository integration còn được bật; kiểm Codex Settings của tài khoản có quyền.
3. Kiểm reviewed SHA và quota. Nếu cần và tính năng hỗ trợ, gửi `@codex review` **một lần**.
4. Không tạo GitHub Action chứa `OPENAI_API_KEY` để thay bằng dịch vụ trả phí.
5. Nếu vẫn không khả dụng, ghi lý do và mời **human reviewer độc lập** theo CODEX_REVIEW_WORKFLOW.md.

### 14.16. Codex chỉ review PR nhưng không sửa code

Đây là hành vi **đúng** với mô hình hai vòng. Review trên GitHub không có nghĩa được tự push patch vào branch IT2. Người làm mở comment, xác minh, giao Codex Extension P22 hoặc sửa tay, chạy test, rồi tự commit/push.

### 14.17. Codex Cloud tự động nhận xét trên commit cũ

```powershell
git log -1 --format="%h %H %s"
```

So sánh SHA local vừa push với HEAD của PR trên GitHub; nếu review cũ khác, chờ hoặc yêu cầu review lại. Trong PR reply ghi `Reviewed SHA`, `Fixed SHA`, `Evidence` rõ ràng.

### 14.18. Một reviewer con người không thể approve PR do CODEOWNERS

GitHub không cho **tác giả tự approve** PR. Nếu Code Owners bắt buộc, một collaborator không nằm trong CODEOWNERS **không tự đáp ứng** owner approval. Phối hợp ME1 sửa mô hình CODEOWNER/ruleset thông qua quy trình quản trị phù hợp; **không bật/tắt ruleset để lách human gate**. Nếu không có owner dự phòng đủ quyền, ghi blocker về cấu hình review.

### 14.19. Nhầm review AI là security scan

Review AI có thể bỏ sót hoặc false positive. Với package/dependency/secret/security-sensitive paths, phải có công cụ chuyên dụng và human review; không coi `code-review` hoặc lời khen của bot là bằng chứng bảo mật hoàn chỉnh. Nếu có secrets, xử lý theo §14.10.

### 14.20. Bench/log/report đã được tạo nhưng chỉ một người xác nhận

Không gọi `MEASURED PASS` cho một hạng mục có required repetition/owner gate chưa đạt. Lưu log+fixture/setup_version+firmware SHA/recording_id, chỉ trình bày bằng chứng đúng cấp độ, xin owner/human approval theo protocol. AI được phép tính toán/check consistency nhưng không tự phê duyệt kết quả.

---
## 15. MẪU BÁO CÁO, CHECKLIST VÀ TIÊU CHÍ HOÀN TẤT

### 15.1. Mẫu phiếu nhận task (member điền trước khi code)

```markdown
# NCKH Task Intake

- Task source: <README tuần trên dev | PR finding URL | task hạ tầng được ME1 duyệt | milestone decision ref>
- Task ID: <chỉ task tuần; trường hợp khác NOT APPLICABLE + lý do>
- Owner: <ME1 | MS2 | EE2 | ET1 | IT2>
- Branch: feature/<member>-<task>
- Current approved architecture/contract version: <...>
- Goal: <...>
- Acceptance criteria (verifiable): <...>
- Deliverables: <...>
- Allowed paths: <...>
- Interfaces / downstream consumers: <...>
- Sample data/bench setup available: <...>
- Tests to run (command & expected output source): <...>
- Hardware/measurements needed: <...>
- Dataset / Ground Truth impact: <...>
- Human Gate required? <yes/no + why>
- Cross-team dependency / approval owner: <...>
```

### 15.2. Mẫu báo cáo **Local Codex Review & Fix** để dán vào PR

```markdown
## Local Codex Review — NCKH

### Context
- Task ID / source: <...>
- Owner / branch / base: <...>
- Review timestamp & HEAD SHA: <...>
- Root/scoped instructions actually read: <...>
- Skills actually loaded (SKILL.md absolute paths): <...>

### Findings BEFORE fix
| ID | Severity | File:line | Evidence | Impact | Decision |
|---|---|---|---|---|---|
| F01 | BLOCKER/MAJOR/... | ... | ... | ... | CONFIRMED / ... |

### Files modified by Codex (after approval)
- <path>: <what/why; interface impact>

### Verification actually executed
| Command | Environment/version | Exit/result | Evidence ref |
|---|---|---|---|
| <real command> | <...> | PASS / FAIL | <log> |

### Verification NOT RUN
- <check>: NOT RUN — <reason / who will do it>

### Unresolved findings
- <...>

### Data & Hardware
- Data origin: <ENGINEERING_VALIDATION / RESEARCH_COLLECTION / NOT_APPLICABLE>
- Ground Truth changed? <no/yes, owner approval ref>
- Interface/schema/pin/timing changed? <no/yes, version + approver>
- Hardware bench status: <NOT RUN / MEASURED with evidence>

### Human handoff
- Human must review: <paths, commit/push scope>
- Human Gate: <required owners or not applicable + reason>
- Suggested PR review focus: <...>
- Recommendation: READY FOR HUMAN DIFF REVIEW / FIX THEN REVIEW / STOP FOR HUMAN DECISION
```

### 15.3. Mẫu báo cáo reviewer GitHub (người thật tổng hợp)

```markdown
## PR Verification Summary

- PR URL / base -> head: <...>
- Latest PR HEAD SHA: <...>
- GitHub Codex reviewed SHA: <...> / NOT AVAILABLE
- Codex Review Summary: COMPLETED / RUNNING / NOT AVAILABLE
- Findings: <P1/P2 / BLOCKER/MAJOR/QUESTION + comment URLs>
- Evidence fixes: <commit SHA, test results, reviewer replies>
- GitHub Actions jobs run: <actual job names + links + PASS/FAIL>
- Checks NOT CONFIGURED: <pytest, firmware compile, hardware bench, ...>
- Independent human reviewer: <GitHub account, domain competence>
- CODEOWNER approval satisfied if required? <yes/no/N/A>
- Human Gate items & references: <...>
- Remaining risk accepted? <owner, scope, reason, date/reference>
- Decision: MERGE / FIX THEN REVIEW / HUMAN DECISION REQUIRED / DO NOT MERGE
```

### 15.4. Checklist dành cho **người tự viết code** (trước khi giao Codex)

- [ ] Đã đọc nguồn task đúng loại (§2.1), Goal/Acceptance/Evidence; task tuần phải có Task ID trong README trên `dev`.
- [ ] Đã kiểm đúng repository `origin`, nhánh feature, working tree và owner.
- [ ] Có notes/code tay hợp lý, không copy output AI vô hiểu biết.
- [ ] Các assumptions/chưa rõ được ghi rõ, không tự thay contract.
- [ ] Không đưa secret, raw large data, model/CAD binary sai nơi.
- [ ] Đã xác định test runner/toolchain có thực và board availability.
- [ ] Đã xác định file nào Codex được phép review/sửa.

### 15.5. Checklist dành cho **Codex Extension sau review/fix**

- [ ] Đọc đúng root/scoped `AGENTS.md` và task.
- [ ] Chỉ dùng Skills thực sự tồn tại, không giả `SKILL.md` đã đọc.
- [ ] Nhận diện lỗi có evidence và mức độ; không chỉ nói generic.
- [ ] Với bug, đã tìm root cause trước khi sửa và có regression test khi khả thi.
- [ ] Chỉ sửa file được duyệt, không thay hardware/schema/labels ngoài scope.
- [ ] Test thực chạy ghi command/exit code/log; `NOT RUN` được ghi thật.
- [ ] Đã self-review diff và làm rõ residual risk.
- [ ] Không tự `git add/commit/push/merge`, không xoá lịch sử hay force-push.
- [ ] Bàn giao báo cáo theo §15.2 để người thật kiểm diff.

### 15.6. Checklist dành cho **human trước commit/push**

- [ ] `git branch --show-current` xác nhận feature branch.
- [ ] `git status --short` được giải thích đầy đủ, kể cả `??`.
- [ ] `git diff --check`, `git diff`, `git diff --cached` đã được xem.
- [ ] Staged chỉ có đúng file task; không có private/secret/raw large data.
- [ ] Relevant tests PASS thật hoặc NOT RUN có lý do + người phụ trách.
- [ ] No unapproved cross-subsystem changes; no unapproved breaking contract.
- [ ] Human Gate và approval refs được xác định.
- [ ] Commit message `type(scope): message` rõ và không bịa Issue.
- [ ] Push đúng `origin` feature branch; không force-push.

### 15.7. Checklist dành cho **PR reviewer trước merge `dev`**

- [ ] Base `dev`, PR đúng task/source/acceptance và owner.
- [ ] PR template điền đủ, evidence có thể tái hiện.
- [ ] CODEOWNERS và reviewer độc lập đúng; tác giả không tự approve.
- [ ] CI jobs **thực tế được cấu hình** đã chạy, kết quả được đọc.
- [ ] GitHub Codex review được xem **trên commit mới nhất**, hoặc fallback human được ghi lý do.
- [ ] Tất cả `BLOCKER`, P1 và MAJOR findings có căn cứ được giải quyết; không “resolve” khi chưa test.
- [ ] Scope/contract migration, data version, cross-team dependencies đã được owner xem.
- [ ] Human Gate có approval thật và đúng phạm vi nếu cần.
- [ ] Không có scientific/hardware claim chưa đo/duyệt.
- [ ] Người có quyền chọn **Squash and merge**; không auto-merge chỉ do CI xanh.

### 15.8. Definition of Done theo loại công việc

| Loại task | Tối thiểu để **bàn giao review** | Không được kết luận nếu thiếu |
|---|---|---|
| IT2 parser/QC | Unit tests cho inputs hợp lệ/lỗi, schema và metadata refs | Data capture trên hardware PASS nếu không có log |
| IT2 DSP/features | Tests reference numerical + units/config + reproducible sample | PC↔ESP32 equivalence nếu chưa chạy board |
| IT2 ML baseline | Leakage-safe grouping, train-only transforms, seed/config, metric context | Dataset v0.1 official quality nếu dữ liệu chỉ Engineering Validation |
| IT2 TinyML edge | Model/config/feature contract + edge build/benchmark evidence khi task yêu cầu | RAM/Flash/latency PASS nếu không có đo thật |
| EE2 acquisition | Compile/host tests khi có; error/timing plan và 60 s board evidence theo acceptance | 60 s stability PASS từ code review thuần |
| MS2 calibration | Source recording, metadata, scripts, uncertainty/unit checks | Calibration physical accuracy nếu chỉ synthetic |
| ET1 board/interface | Schematic/pinout/BOM consistency, human measurement plan | Điện an toàn nếu chưa bench verify |
| ME1 fixture/protocol | Versioned CAD/drawing/protocol, physical condition provenance, approvals | Acceptance/safety PASS nếu chưa kiểm thực địa |
| Docs/config only | Link/path/rule consistency, diff review, phù hợp AGENTS | Runtime test PASS (dùng `NOT APPLICABLE` đúng lý do) |

> **Quan trọng:** “Ready for human review” khác với “Ready to merge”; “unit test PASS” khác với “system validated”; “Codex review completed” khác với “approved scientific result”.

### 15.9. Checklist đánh giá Pilot IT2 trước khi rollout cả nhóm

Thử một task nhỏ được giao chính thức (ví dụ parser/QC/DSP reference có fixture test) qua hai vòng:

1. IT2 tự code trên feature branch từ `dev` sạch.
2. Codex local dùng P03-A rồi P03-B (nếu có finding), P07; ghi Skills/file paths.
3. IT2 xác nhận code hiểu được, tests đã chạy, diff trong scope.
4. IT2 human commit/push, mở PR `feature -> dev`.
5. Kiểm CI đã cấu hình, GitHub Codex review SHA mới nhất và review comments.
6. Nếu có finding: sửa cùng branch, chứng minh test, push bổ sung, chờ lại.
7. Independent human reviewer quyết định; không merge test fixture lỗi cố ý.
8. Ghi thực tế: thời gian review, số finding hợp lệ/false positive, lỗi bị bắt bởi tests vs AI, số vòng sửa, hạn mức, các friction về Skills trên máy thành viên.

Nếu pilot không có dữ liệu/hardware đủ, **không chuyển thành claim đã hoàn tất validation AIoT**; vẫn có thể hoàn tất thử nghiệm *workflow* với test scope hạn chế đã ghi rõ.

---

## 16. ĐƯỜNG HƯỚNG MỞ RỘNG WORKFLOW VÀ GIỚI HẠN

### 16.1. Những phần **đã có** và **chưa có**

| Thành phần | Tình trạng đã xác minh đến 12/10/2026 | Cần làm tiếp nếu muốn tự động mạnh hơn |
|---|---|---|
| Root/scoped `AGENTS.md` và NCKH standards | Có, root áp review tiếng Việt | Tiếp tục giữ đồng bộ với task/architecture |
| `.agents/skills/nckh-task-execution` | Có trên `dev` | Sử dụng trong task phù hợp; tránh làm trùng logic |
| `.agents/skills/nckh-pr-preflight` | Có trên `dev` | Dùng cho PR-ready checks |
| Matt Pocock `tdd`, `diagnosing-bugs`, `code-review` | Đã xác minh trên máy được cấu hình ổ D | Mỗi máy khác cần xác minh/cài riêng nếu dùng |
| `nckh-coding-workflow` điều phối global | **CHƯA TẠO/CÀI** | Thiết kế/cài ngoài repo; test tự kích hoạt; đánh giá xung đột Skill |
| Codex GitHub Code Review | Có review thật trong PR #14/#18 | Giám sát usage, trigger và review SHA mới nhất |
| `repo-quality.yml` | Có CI về cấu trúc/data policy | CI Python/firmware/ML tùy subsystem sau human PR |
| GitHub required checks / required CODEOWNER approvals | **Chưa xác minh trạng thái ruleset** | Kiểm tra setting; triển khai có chủ đích, tránh deadlock sole CODEOWNER |
| 60 s firmware hardware stress test tự động | Chưa có bằng chứng CI/HIL chung | Tạo bench protocol và công cụ/log; không giả hardware hiện có |
| OpenAI API action tính phí | **Không dùng trong kiến trúc này** | Không thêm `OPENAI_API_KEY` |

### 16.2. Đề xuất nâng cấp CI bằng PR riêng (không trộn với PR tính năng)

- **Python lint/static/unit:** chỉ thêm khi repository có lựa chọn môi trường, dependency file và tests thực tế. Không bật status check bắt buộc trước khi xác định tên job ổn định và runner chạy được.
- **Firmware compile/host unit tests:** cần xác định Arduino/PlatformIO/ESP-IDF thực tế của EE2, board definition và dependency. Không giả CI cloud có thiết bị ESP32-S3.
- **ML reproducibility/data lint:** test synthetic nhỏ, manifest/schema consistency, forbid leaked recording groups, smoke tests hợp lý; không commit raw dataset.
- **PR governance:** build checks phù hợp scope, reviewer độc lập, CODEOWNER rule phù hợp tình trạng sole owner, bảo vệ `dev` và release `main`.
- **Security/data:** secret scanning và hạn chế raw data/artifact theo chính sách hiện hành.

Các thay đổi `.github/`, `AGENTS.md` và `.agents/` phải được ME1 xem theo `CODEOWNERS`; dùng feature branch + PR riêng, không sửa trực tiếp `dev`.

### 16.3. Nếu sau này xây Skill `nckh-coding-workflow`

**Yêu cầu chức năng đề xuất (CHƯA TRIỂN KHAI):**

```text
TRIGGER (model-driven): user asks to code/review/fix/test in NCKH local repo
GUARD: read AGENTS + task / scope / branch; human consent for editing
ROUTING:
  Existing human-written code -> code-review
  Unknown bug -> diagnosing-bugs
  Reproducible change requiring tests -> tdd
  New official NCKH task -> nckh-task-execution
  Before PR -> nckh-pr-preflight
VERIFY: real command, log, sample data provenance, hardware NOT RUN gates
OUTPUT: report §§15.2-15.3; STOP before human Git
NEVER: auto commit, push, merge, alter locked hardware, forge evidence
```

Không bắt Codex chạy **mọi** Skill cho **mọi** prompt: routing phải linh hoạt. Muốn đảm bảo kiểm tra bắt buộc, đặt điều kiện ở **tests/CI/human approval**, không chỉ đặt trong description của Skill. Mọi phiên cloud GitHub không được coi đã tải những global Skills này từ ổ D.

### 16.4. Hạn chế quan trọng của AI reviewer

1. AI có thể **bỏ sót bug**, hiểu sai requirement hoặc báo false positive; độc lập phiên không bảo đảm độc lập mô hình/cách suy luận.
2. Cloud review không thể thay hardware bench/cảm biến/motor safety, không tự truy cập raw logs riêng trên máy nếu chưa được chia sẻ có quyền.
3. Skills không phải trình thực thi deterministic, không có đảm bảo tuyệt đối rằng mô hình sẽ auto-select từng Skill đúng mọi lần.
4. GitHub review có hạn mức và trigger; `Completed` không có nghĩa quality gate xanh.
5. Test tốt phải bám acceptance đã được phê duyệt, không biến đề xuất do model tạo thành định nghĩa nghiên cứu chính thức.
6. Với tài khoản Plus, ưu tiên tính năng Codex chính thức và quota có sẵn; không cam kết dùng không giới hạn, không tự mua credits/API.

---

## 17. NGUỒN ĐỐI CHIẾU VÀ LỊCH SỬ CẬP NHẬT

### 17.1. Nguồn NCKH làm căn cứ biên soạn

**Tài liệu Project được cung cấp:**

1. `NCKH_PROJECT_MASTER_CONTEXT.md` — cấu hình khóa, phân công, Week 3 / Edge Condition Monitor v0.1, dataset policy.
2. `AGENTS.md` (bản Project đính kèm) — mission, locked hardware, domains, Human Gate. **Root `AGENTS.md` trên `dev` được đối chiếu riêng vì đã bổ sung yêu cầu review bằng tiếng Việt và chuẩn mới.**
3. `NCKH_CODEX_AGENT_WORKFLOW_V3.md` — feature branch, inspect/plan/edit/test/review/human Git, prompt theo vai trò.
4. `NCKH_MEMBER_WORKSPACE_FILETYPE_GUIDE_V3.md` — subsystem owner, file types, location, Git LFS/raw data.
5. `NCKH_TEAM_VSCODE_GITHUB_OPEN_GUIDE_V3.md` — clone/open/sync Git và sự cố cơ bản.
6. `docs/project_management/CODEX_PROJECT_CONTEXT_V1.md` và `docs/project_management/NCKH_TEAM_CHANGE_UPDATE_WORKFLOW_V1.md` (trên nhánh `dev`) — nguồn có version để truy vết task routing và `[PROJECT UPDATE]`; tài liệu không lưu trong Git chỉ là dữ liệu đầu vào biên soạn, **không** được viện dẫn như nguồn chuẩn.

**Đã đối chiếu với nội dung thực có của repo GitHub trên `dev`:**

- <https://github.com/nkaq/SVNCKH--HUST/blob/dev/AGENTS.md>
- <https://github.com/nkaq/SVNCKH--HUST/blob/dev/CONTRIBUTING.md>
- <https://github.com/nkaq/SVNCKH--HUST/blob/dev/.github/CODEOWNERS>
- <https://github.com/nkaq/SVNCKH--HUST/blob/dev/.github/workflows/repo-quality.yml>
- <https://github.com/nkaq/SVNCKH--HUST/blob/dev/.agents/skills/nckh-task-execution/SKILL.md>
- <https://github.com/nkaq/SVNCKH--HUST/blob/dev/.agents/skills/nckh-pr-preflight/SKILL.md>
- <https://github.com/nkaq/SVNCKH--HUST/blob/dev/docs/project_management/CODEX_REVIEW_WORKFLOW.md>
- <https://github.com/nkaq/SVNCKH--HUST/blob/dev/docs/project_management/CODEX_AUTOMATION_PLAYBOOK_V1.md>
- <https://github.com/nkaq/SVNCKH--HUST/blob/dev/docs/project_management/CODEX_ENGINEERING_STANDARD_V1.md>
- <https://github.com/nkaq/SVNCKH--HUST/blob/dev/docs/project_management/CODEX_RESEARCH_STANDARD_V1.md>
- <https://github.com/nkaq/SVNCKH--HUST/pull/18> — Codex GitHub Review có `Completed` và `New commits` trong lịch sử.
- <https://github.com/nkaq/SVNCKH--HUST/pull/14> — Codex GitHub review đã được ghi nhận.

**Nguồn bên ngoài / ghi chú công cụ:**

- Skill collection bên thứ ba: <https://github.com/mattpocock/skills>; nguồn này có thể đổi trong tương lai, nên phải đọc `SKILL.md` bản đang cài.
- Codex Cloud: <https://chatgpt.com/codex>
- Codex Usage: <https://chatgpt.com/codex/settings/usage>

> Khi đường dẫn, branch rules, Codex UX/usage hoặc cấu trúc folder thay đổi sau ngày biên soạn, cần kiểm **repo/GUI thực tế**. Các điểm chưa được xác nhận đã được đánh dấu rõ là “chưa cấu hình”, “chưa kiểm chứng” hoặc “đề xuất”; không tự coi là trạng thái live vĩnh viễn.

### 17.2. Quy trình bảo trì tài liệu này

- **Owner nội dung:** ME1 phối hợp EE2/IT2/MS2/ET1 khi cập nhật nội dung chuyên môn. **Trạng thái V1.2 (ME1 Onshape migration):** PROPOSED; ghi nhận tại [`NCKH_ME1_ONSHAPE_PROJECT_UPDATE_PROPOSAL_V1.md`](NCKH_ME1_ONSHAPE_PROJECT_UPDATE_PROPOSAL_V1.md) và mục Unreleased của `CHANGELOG.md`; chưa tự ACTIVE. **V1.1 Two-Stage Skills** có hồ sơ khác tại [`NCKH_TWO_STAGE_CODEX_WORKFLOW_PROJECT_UPDATE_PROPOSAL_V1.md`](NCKH_TWO_STAGE_CODEX_WORKFLOW_PROJECT_UPDATE_PROPOSAL_V1.md). Không dùng hồ sơ V1.1 làm bằng chứng phê duyệt V1.2.
- **Nơi khuyến nghị đưa vào repo (sau khi human duyệt):** `docs/project_management/NCKH_TWO_STAGE_CODEX_SKILLS_WORKFLOW_HANDBOOK_V1.md` qua **feature branch và PR**, không copy đè trực tiếp `dev`.
- **Trước PR:** cross-check lại `AGENTS.md`, README task tuần hiện hành, CI/ruleset, review trigger, Skills thực cài, source logs và CODEOWNERS.
- **Khi cập nhật:** tăng version, mô tả scope thay đổi, mốc hiệu lực, owner review; thông báo `[PROJECT UPDATE]` nếu thay đổi quy trình cả nhóm.

### 17.3. Lịch sử phiên bản

| Version | Ngày | Thay đổi |
|---|---|---|
| V1.0 | 2026-10-12 | Cẩm nang hợp nhất local Codex Skills + GitHub Codex Review, lệnh Git/PowerShell, 41 mẫu prompt (P01–P40, trong đó P03 có 2 pha), 76 tình huống, bảng xử lý lỗi, checklist và ngưỡng Human Gate |
| V1.1 (PROPOSED) | 2026-10-12 | Sửa 5 finding PR #19: thay tham chiếu ngoài repo, phân nguồn task theo PR template, Project Update mức B/CHANGELOG với trạng thái đề xuất, khóa CLI + Git commit của Skills và cảnh báo redact terminal credentials; đợi human review/ACK trước ACTIVE |
| V1.2 (ME1 CAD migration PROPOSED) | 2026-10-13 | Đổi phần ME1 và prompt P21 từ legacy SolidWorks sang Onshape-only cho CAD mới: tách CAD TEST/ME1_MAIN_CAD, immutable MAIN Version, registry, BOM/report/manifest, privacy gate, giữ legacy. PR #20 / yêu cầu V2; không tuyên bố team rollout ACTIVE hoặc CAD/safety nghiệm thu. |

---

## PHỤ LỤC A — MỘT NGÀY LÀM VIỆC CỤ THỂ CỦA IT2

**Ví dụ minh họa, không phải Task ID/acceptance hoặc test result thật của repository:** IT2 được giao xây Python QC cho Engineering Validation recording.

**Buổi bắt đầu:** IT2 đọc `docs/plans/weekXX/README.md` hiện hành, xác nhận yêu cầu và dữ liệu test hợp lệ, mở VS Code tại repo:

```powershell
git rev-parse --show-toplevel
git status --short
git switch dev
git pull --ff-only origin dev
git switch -c feature/it2-recording-qc
```

**Tự code:** sửa/tạo file **theo cấu trúc thật** trong `ml/preprocessing/`, kiểm `git status`. Giả sử cần sửa Python module và tests; tên `recording_qc.py` trong ví dụ phải được thay bằng file **thực sự có hoặc được task cho phép tạo**.

**Review vòng 1:** dán **P03-A**. Codex đọc task/rules, review code tay, trả finding. Nếu nghi mất sample do parser, dùng **P05**; nếu có bug tái hiện, human duyệt rồi **P03-B/P06**. Sau edit/test, dùng **P07** để kiểm trước PR. Không cho Codex tự commit.

**Human stage/commit/push:**

```powershell
git status --short
git diff --check
git diff
# Thay đúng file thật:
# git add -- ml/preprocessing/<actual_file>.py
# git add -- ml/preprocessing/<actual_test_file>.py
git diff --cached
# git commit -m "feat(it2): add recording QC checks"
# git push -u origin feature/it2-recording-qc
```

**Pull Request:** vào GitHub tạo PR từ `feature/it2-recording-qc` vào `dev`. Ghi tests thật; ghi rõ đây là Engineering Validation, **không** đánh dấu là Dataset v0.1.

**Review vòng 2:** đợi Codex GitHub Review và `repo-quality`. Nếu GitHub Codex có finding liên quan `sample_index`, IT2 dùng **P22** để xác nhận, sửa trên **cùng feature branch**, chạy unit test, human commit/push. Nếu không có finding, vẫn cần human reviewer độc lập và Human Gate khi áp dụng.

**Sau merge:** người có quyền squash merge `dev`, IT2/nhóm đồng bộ dev theo hướng dẫn chính thức; nếu thay đổi packet/schema của EE2 hoặc dùng chung, phát `[PROJECT UPDATE]` và xác nhận các thành viên bị ảnh hưởng.

---

## PHỤ LỤC B — MỘT NGÀY LÀM VIỆC CỤ THỂ CỦA EE2

**Ví dụ minh họa, không phải tuyên bố firmware thật đã hoàn thiện:** EE2 sửa acquisition buffer cho ESP32-S3/ADXL345.

1. Đọc Task ID trong plan tuần; kiểm actual firmware architecture/board/config và scoped `firmware/AGENTS.md`.
2. Tạo `feature/ee2-<task>` từ `dev` sạch; tự code C/C++ trong `firmware/esp32s3/`.
3. Dùng **P15** review static, **P16** khi nghi dropped samples; lập plan thay đổi, rồi mới cho Codex edit phạm vi đã duyệt với **P03-B**.
4. Chạy host tests/build **nếu toolchain có**. Không tuyên bố `60 s PASS` nếu không có board/log.
5. Nếu board/fixture sẵn sàng, EE2/ME1 duyệt điều kiện, chạy stress theo protocol; lưu log `sample_index`, timestamps, error counters, firmware SHA và setup version; Codex chỉ hỗ trợ phân tích evidence.
6. Human diff -> stage đúng firmware files -> commit/push feature -> PR vào `dev`; GitHub Codex Review + `repo-quality` + reviewer có chuyên môn.
7. Finding thật -> sửa trên cùng branch -> test -> human push -> re-review; merge chỉ khi test/human gate phù hợp.

---

## PHỤ LỤC C — HỎI NHANH / TRẢ LỜI NGẮN

**Hỏi:** Codex tự chạy 3 Skills chỉ vì IT2 nhấn `git push`?  
**Đáp:** **Không.** Local Skills được mô hình chọn khi có yêu cầu trong phiên VS Code; Git push không tự gọi Codex.

**Hỏi:** GitHub Codex có đọc Junction `C:\Users\...\.agents\skills` -> D?  
**Đáp:** **Không tự đọc.** Đây là tài nguyên máy cá nhân, không phải nội dung repo/phiên cloud.

**Hỏi:** Codex GitHub review đã hoạt động trong repo chưa?  
**Đáp:** Có evidence từ PR #14/#18; nhưng vẫn kiểm trigger và commit SHA mới nhất trong từng PR.

**Hỏi:** Có cần mua API key dùng theo token mới review PR?  
**Đáp:** Không trong quy trình dùng tích hợp chính thức với ChatGPT đủ quyền. Không thêm GitHub Action gọi API key.

**Hỏi:** `repo-quality PASS` có nghĩa code AIoT chuẩn và an toàn?  
**Đáp:** Không. Job hiện có chỉ thực thi kiểm tra cấu trúc/data policy đã cấu hình.

**Hỏi:** Được nhờ Codex local **tự sửa toàn bộ code** sau review không?  
**Đáp:** Được **trong phạm vi đã duyệt trên feature branch**, có tests/handoff; nhưng không tự Git commit/push/merge.

**Hỏi:** PR luôn phải push hai lần?  
**Đáp:** Không. Số lần push phụ thuộc finding/evidence, không phải quality metric.

**Hỏi:** AI review xong, ai chịu trách nhiệm?  
**Đáp:** Người làm và domain/human gate reviewer; Codex là công cụ hỗ trợ, không phải người ký phê duyệt khoa học/an toàn.

**Hỏi:** Nếu AI nhận xét sai?  
**Đáp:** Đối chiếu diff/code/test/contract, phản hồi có chứng cứ theo P25, nhờ reviewer độc lập khi cần.

**Hỏi:** Nếu hết quota Plus đúng lúc chuẩn bị merge?  
**Đáp:** Chờ quota theo nhu cầu hoặc dùng human review độc lập thay thế theo quy trình repo, **không** bỏ CI/Human Gate.

**Hỏi:** Có thể đưa cẩm nang này vào repo luôn?  
**Đáp:** Có thể đề xuất bằng một **PR tài liệu riêng** sau khi ME1 và những owner liên quan duyệt; không tự ghi vào `dev`.

---

**KẾT LUẬN (đề xuất, chưa ACTIVE):** NCKH dùng **Human Code → Local Codex Skills Review/Fix/Test → Human Git → Official GitHub Codex Review + CI → Human Approval → Merge `dev`**. Mục tiêu là tìm lỗi sớm, giữ kỹ năng tự lập trình của thành viên, bảo vệ nghiên cứu và kiểm soát rủi ro — không “đánh dấu xanh” thay cho phép đo, kiểm thử hoặc quyết định của người phụ trách.
