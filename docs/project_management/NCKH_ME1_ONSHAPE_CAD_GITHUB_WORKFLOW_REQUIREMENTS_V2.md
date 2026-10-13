# NCKH — YÊU CẦU SỐ 1 (BẢN HOÀN CHỈNH V2.0)
## Chuyển đổi ME1 sang Onshape — CAD TEST / MAIN CAD, quản lý GitHub, kiểm chứng cơ khí và phát hành fabrication package

> **Loại tài liệu:** Yêu cầu đầu vào chính thức để giao Codex lập phương án.  
> **Ngày biên soạn:** 2026-10-13 (UTC+07:00).  
> **Dự án:** *Adaptive Multimodal TinyML for Drift-Robust Predictive Maintenance on Resource-Constrained Edge Microcontrollers*.  
> **Repository:** <https://github.com/nkaq/SVNCKH--HUST> — nguồn đối chiếu là nhánh `dev` đang cập nhật.  
> **Người chịu trách nhiệm:** ME1 (`@nkaq`), System Architecture / Mechanical / Fixture / Ground Truth / Integration.  
> **Chế độ mặc định khi đưa cho Codex:** **GIAI ĐOẠN 1 — READ-ONLY, KHÔNG SỬA REPOSITORY**.  
> **Tình trạng chuyển đổi:** ME1 đã **chọn Onshape làm công cụ CAD duy nhất cho công việc thiết kế mới**; quy định bàn giao và ngày hiệu lực toàn nhóm cần được ghi nhận bằng PR/Project Update trước khi áp dụng chính thức cho cả nhóm.  
> **Đầu ra mong muốn:** bản phân tích có căn cứ → ME1 phê duyệt phạm vi → chỉnh tài liệu trên feature branch → review/test → human Git/PR → phát hành quy trình. **Không được xem tài liệu này là bằng chứng fixture hay CAD đã được nghiệm thu.**

---

## 0. NHIỆM VỤ DÀNH CHO CODEX — ĐỌC TRƯỚC TIÊN

Tôi là **ME1**. Hãy khảo sát repository **THẬT** trên `dev` và chuẩn hóa cách quản lý thiết kế cơ khí từ quy trình trước đây định hướng SolidWorks sang **Onshape-only cho mọi thiết kế mới**. Sử dụng **Onshape + VS Code + Codex/text-to-cad + GitHub**, trong đó:

1. **Onshape** giữ CAD native: Part Studio, Assembly, Drawing, Versions/History và tham chiếu lắp ráp.
2. **GitHub** lưu tài liệu kỹ thuật, registry, script/parameters, bản xuất thích hợp, BOM, test report, bằng chứng kiểm thử, hồ sơ phê duyệt và lịch sử thay đổi.
3. **Codex/text-to-cad** chỉ hỗ trợ sinh/kiểm tra source script và bản xuất hình học theo quyền được cấp; không tự quyết định hình học chính thức hay xác nhận an toàn.
4. **SOLIDWORKS KHÔNG CÒN LÀ CÔNG CỤ CAD SỬ DỤNG CHO DỰ ÁN MỚI.** Không yêu cầu tiếp tục tạo `.SLDPRT/.SLDASM/.SLDDRW`, không tạo tên đường dẫn SolidWorks cho artifact mới. Các thư mục/file SolidWorks có sẵn giữ nguyên ở trạng thái **LEGACY/READ-ONLY** để bảo toàn lịch sử; việc chuyển lưu trữ/xóa cần quyết định và PR riêng.
5. Không sửa các subsystem ngoài ME1, không thay đổi phần cứng đã khóa, dữ liệu Ground Truth hoặc protocol khi chưa được phê duyệt.

**Giai đoạn 1:** chỉ điều tra, đề xuất cây thư mục, mapping file, versioning, registry, CAD test reports, LFS và kế hoạch migration; **không sửa/tạo/xóa file**, không đụng Onshape cloud, không Git commit/push/merge, không tự cài thư viện hay cấp quyền.

**Giai đoạn 2:** chỉ thực hiện **sau khi ME1 duyệt kế hoạch và cho phép rõ phạm vi**; làm trên nhánh task mới, kiểm tra và bàn giao diff để human thực hiện các thao tác Git và phê duyệt.

---

## 1. KIẾN TRÚC CÔNG CỤ CAD MỚI VÀ RANH GIỚI TRÁCH NHIỆM

| Thành phần | Source of truth/giá trị | KHÔNG được suy diễn |
|---|---|---|
| **Onshape ME1_MAIN_CAD** | Native parametric model, Part Studios, Assemblies, Drawings, tham chiếu phiên bản | Bản Workspace đang sửa không phải release bất biến |
| **Onshape CAD TEST** | Thiết kế thử, import STEP, đánh giá concept, mô phỏng thử nghiệm | TEST PASS không tự đồng nghĩa MAIN APPROVED |
| **Onshape Version** | Snapshot được đặt tên/cố định; liên kết tham chiếu release | `V1` của Onshape không mặc nhiên là `fixture_v01` hoặc Drawing revision |
| **GitHub `hardware/mechanical/`** | Registry, releases/manifest, code CAD, STEP/STL/PDF/DXF khi có, report/evidence | STEP không chứa toàn bộ parametric feature history |
| **GitHub `hardware/bom/`** | BOM CSV và bằng chứng/phiên bản dùng cho cơ khí; phối hợp ET1 | Không tự tạo BOM giả hoặc giá mua chưa kiểm chứng |
| **VS Code** | Soạn/sửa script, README, report và xem diff | Không thay Onshape trong việc duy trì native CAD |
| **Codex / text-to-cad** | Tạo proposal/geometry script, đọc source, test script trong môi trường được phép | Không tự đánh dấu kết cấu an toàn, giải phóng release hay phê duyệt GT |
| **GitHub Codex Review + CI** | Review code/tài liệu và phép kiểm đã cấu hình thật | Không có nghĩa đã kiểm hình học binary, solver hay thử nghiệm motor |

### 1.1. Quyết định SolidWorks → Onshape

- **Có hiệu lực về lựa chọn công cụ cho việc ME1 sẽ thiết kế mới:** sử dụng **Onshape**; không lập kế hoạch cài/mua/phụ thuộc SolidWorks cho công việc mới.
- **Có hiệu lực về quy trình chung GitHub:** chỉ sau khi cập nhật hướng dẫn repo, review và công bố theo `NCKH_TEAM_CHANGE_UPDATE_WORKFLOW_V1.md` với owner/Update ID/mốc hiệu lực cụ thể.
- **File SolidWorks cũ:** vẫn tồn tại, lịch sử được bảo lưu. Đánh dấu `LEGACY — DO NOT USE FOR NEW DESIGN` trong tài liệu hướng dẫn, **không rename/delete/move file** ngay; nếu có artifact legacy thực, giữ nguyên đường dẫn và có mapping khi cần.
- **Tài liệu chứa SolidWorks:** sửa **hướng dẫn hoạt động hiện hành** thành Onshape; **không sửa nội dung lịch sử** như báo cáo tuần đã đóng, release note cũ hoặc ghi chú thiết kế quá khứ nhằm xóa dấu vết công cụ trước đây.
- Mọi fabrication dựa vào file cũ chỉ được tiếp tục nếu ME1 kiểm phiên bản/gốc, tính phù hợp và phê duyệt theo ngữ cảnh; không ngầm giả bản cũ thuộc MAIN CAD Onshape.

### 1.2. Cảnh báo tài khoản và quyền truy cập Onshape (Human Gate số 0)

Trước khi tạo/lưu tài liệu có thông tin nghiên cứu, **ME1 phải xác nhận loại tài khoản, chủ sở hữu Document, quyền xem/sửa, khả năng giữ private và khả năng sử dụng Simulation**:

- **Onshape Free:** tài liệu người dùng tạo là **public**; không đưa vào nội dung chưa được phép công khai như thiết kế fixture riêng, thông tin cá nhân/nhạy cảm, kết cấu an toàn chưa duyệt hay tham số nghiên cứu cần giữ riêng.
- Gói/trường hợp **Education/Educator/Standard/Professional** có quyền, công cụ và điều khoản khác nhau; **không tự đoán** quyền private, release management hay Simulation chỉ vì đăng nhập được.
- Nếu không có khả năng tạo Document **private** trong khi dự án yêu cầu riêng tư: ghi **BLOCKED — ACCOUNT/CONFIDENTIALITY**, dừng đưa CAD nhạy cảm lên Onshape; ME1 chọn gói/quyền sử dụng hợp lệ hoặc phạm vi thông tin được phép công khai.
- Không commit API key/token/session/cookie Onshape; không gắn Onshape API với OpenAI API key. Không cấp quyền sửa toàn bộ MAIN CAD cho một script/agent chỉ để thử nghiệm.
- Link CAD trên GitHub **không cấp quyền xem** đối với Document private; phải thử truy cập bằng đúng tài khoản reviewer được cho phép. Không mở public chỉ để một bot đọc được.

**Cần hỏi ME1 trong Giai đoạn 1:** gói Onshape đang dùng là gì, CAD có cần private không, ai có quyền Owner/Edit/View và Simulation có thực sự hiển thị không? Nếu chưa trả lời, đưa vào bảng quyết định PENDING, không đoán.

---

## 2. HAI MÔI TRƯỜNG CAD — TÁCH THỬ NGHIỆM KHỎI SOURCE CHÍNH

### 2.1. `CAD TEST` — Onshape Document thử nghiệm

**Đề xuất mặc định:** một Onshape Document độc lập mang tên gợi ý `NCKH_ME1_CAD_TEST` (tên thật do ME1 xác nhận), với Workspace/Version phục vụ thử nghiệm. Được dùng để:

- Thử part/assembly, kiểm kích thước, dung sai, khoảng hở, tầm với, trục quay, cách đi dây và vị trí lắp sensor.
- Import/bổ sung STEP sinh bởi script Python/text-to-cad; so nguồn script, tham số, đơn vị và hình học với Onshape.
- Phân tích giả định/tải/độ cứng/Modal khi công cụ hỗ trợ; ghi giả định và `SIMULATED`, không biến thành `MEASURED`.
- Tạo phương án, kiểm lỗi và ghi lại lý do chọn/bỏ trước khi chuyển sang MAIN.

**Trạng thái trong CAD TEST:** `DRAFT`, `UNDER_REVIEW`, `REJECTED` hoặc `TESTED` (đã có bằng chứng theo phạm vi). **Không sử dụng `RELEASED`** cho TEST.

Không commit các bản export thử và simulation cache khổng lồ vào bộ `fabrication/` chính thức. Nếu cần giữ minh chứng thử nghiệm nhỏ thì ghi rõ `TEST_ONLY`, phiên bản, người tạo, nơi lưu và chính sách dữ liệu.

### 2.2. `ME1_MAIN_CAD` — Onshape Document chính

**Đề xuất mặc định:** Onshape Document độc lập mang tên gợi ý `NCKH_ME1_MAIN_CAD`, gồm Part Studios, Assemblies, Drawings, BOM liên quan và reference/version kiểm soát. Đây là source CAD native của **các thiết kế mới**.

**Quy tắc đưa từ TEST sang MAIN:**

1. TEST tạo một **Version cố định** của phương án ứng viên và report kiểm tra có nguồn.
2. ME1 kiểm component ID, tọa độ, kích thước, nguồn script/STEP, impact tới fixture/mount/safety và dependency với ET1/MS2/EE2.
3. ME1 duyệt chuyển; lựa chọn thao tác **derive/copy/insert/rebuild** phù hợp tính năng Onshape đang có. Không hứa có một nút “đồng bộ hai Document” tự động.
4. Sau khi vào MAIN, kiểm tra lại **tham chiếu feature/instance, Mate/Assembly, Drawing updates, BOM và version**. MAIN phải có test/evidence riêng; không dùng kết quả TEST thay nghiệm thu MAIN.
5. Tạo **Version của MAIN** để đóng băng ứng viên release, sau đó xuất artifact **từ đúng Version này**; không xuất từ Workspace đang thay đổi rồi dán nhãn cũ.
6. `APPROVED`/`RELEASED` chỉ sau khi đủ test/evidence, kiểm tra cấp có thẩm quyền và ghi nhận trên GitHub; không suy ra từ việc Onshape import thành công.

**Lựa chọn hai Document** là kiến trúc mặc định an toàn, tránh sửa nhầm MAIN. Nếu Giai đoạn 1 phát hiện giới hạn chia sẻ/thiết kế khiến hai Workspace/branch trong một Document hợp lý hơn, Codex chỉ **đề xuất thay thế với phân tích rủi ro**, không tự đổi quyết định.

### 2.3. Phân biệt Workspace, Version, Revision và Git commit

| Khái niệm | Nghĩa | Nơi tham chiếu |
|---|---|---|
| Onshape Document ID | Định danh Document | `cad_registry.md` |
| Onshape Workspace ID | Đầu nhánh chỉnh sửa, thay đổi theo thao tác | Dùng cho công việc đang làm, **không** làm baseline release |
| Onshape **Version ID** | Snapshot bất biến để xác định đúng geometry/drawing | Registry, release manifest, test report |
| Onshape Version **name** | Nhãn thân thiện, vd `ME1-R02-CANDIDATE` | Chỉ hỗ trợ tìm; không thay Version ID |
| Part/Assembly ID | Định danh kỹ thuật do nhóm đặt, vd `FIX-PLT-001` | Drawing/BOM/STEP/report |
| Engineering **Revision** | Bản sửa kỹ thuật được nhóm duyệt, vd `R01`, `R02` | Drawing, BOM, fabrication, manifest |
| Fixture/setup version | Version cấu hình lắp đặt dùng trong experiment/metadata | ME1, MS2, EE2, IT2, Ground Truth |
| Git SHA | Đóng băng lịch sử của artifact/report/script đã nộp lên GitHub | PR, release manifest, changelog |

**Không tự coi các số đếm là giống nhau.** Ví dụ MAIN Onshape `V8` có thể dẫn tới engineering revision `R02`, và một Git commit khác giữ hồ sơ kiểm tra; mối liên hệ bắt buộc được ghi tường minh.

---

## 3. QUY ƯỚC THIẾT KẾ VÀ HỆ TỌA ĐỘ ME1

Các quy ước dưới đây là **đề xuất cần ME1 xác nhận**, không phải thông số đo đã khóa:

### 3.1. Đơn vị và hệ trục

- **Đơn vị chính:** milimét (`mm`) cho hình học/drawing, `kg` hoặc `g` cho khối lượng, `N` cho lực, `N·m` cho moment, `MPa` cho ứng suất, `Hz` cho tần số. Khi report dùng `m`, `rad/s`, `rpm`, ghi conversion/định nghĩa rõ.
- **Hệ trục tổng thể:** đề xuất một hệ Cartesian **right-handed** cố định trong MAIN Assembly, ghi bằng sơ đồ/trong Drawing: ví dụ `+X` dọc trục motor hướng coupling, `+Z` thẳng đứng, `+Y` theo quy tắc bàn tay phải. **Chỉ là ví dụ — ME1 quyết định và khóa bằng tài liệu thực tế.**
- Ghi **datum A/B/C**, mounting/reference plane, gốc tọa độ, độ cao trục, trục motor, coupling/gối đỡ, hướng lắp và bề mặt chuẩn đo.
- Mỗi part/component có transform về hệ assembly; không thay đổi trục hoặc mirror mà không cập nhật drawing/metadata.
- Với **ADXL345** ghi `sensor_id`, `mount_id`, vị trí mount, hướng trục X/Y/Z tương đối hệ máy, `mounting_method`, lực siết nếu được quy định, `setup_version`, `fixture_version` và ảnh vị trí thực; không đoán orientation chỉ từ icon/render.
- Nếu dùng các STEP/STL khác nhau, **kiểm đơn vị export/import**. STEP có thể xuất với tùy chọn đơn vị cụ thể; STL có thể gây hiểu nhầm đơn vị ở công cụ nhận. Không phát hành chỉ vì render trông giống.

### 3.2. Bộ tiêu chí bắt buộc để nhận một component vào MAIN

- Có part/assembly ID, tên, chức năng và mã tham chiếu BOM.
- Có material/specification (hoặc `TBD — ME1 xác nhận` nếu chưa chốt), geometrical constraints, đặc tính sản xuất.
- Có interface/mating/datum rõ, clearance và vùng cấm va chạm/cáp/lưới bảo vệ.
- Các kích thước quan trọng, tolerances, hole patterns, fit/fastener requirements có nguồn quyết định.
- Có giải thích ảnh hưởng tới motor shaft, coupling, shaft alignment, bearing, tải quay, mount cảm biến và cơ chế tạo fault.
- Nếu làm thay đổi mounting/fixture sau khi thu dữ liệu: ghi **setup/fixture/domain version** và đánh giá tính so sánh của phép đo, không trộn dữ liệu cũ như cùng domain.

### 3.3. Quy tắc đặt tên/version/file

- **Component ID duy nhất, ổn định:** ví dụ `FIX-MNT-001` (giá gá), `FIX-PLT-002` (tấm đế), `FIX-ASM-001` (tổng lắp); sơ đồ ID do ME1 duyệt và đăng trong `cad_registry.md`.
- **Engineering revision:** `R01`, `R02`... **không** suy từ `Onshape V#`.
- **Release ID:** đề xuất `ME1-FIXTURE-R01` (định danh ví dụ; ME1 duyệt tên thực tế).
- **File name:** `<COMPONENT_ID>_<REV>_<ARTIFACT>.<ext>`; ví dụ `FIX-MNT-001_R02.step`, `FIX-ASM-001_R02_drawing.pdf`, `FIX-ASM-001_R02_iso.png`.
- **Test report:** `hardware/mechanical/reports/<COMPONENT_ID>/<RELEASE_ID>/test-report.md`; nếu nhiều lần đo trong cùng release, dùng thêm `run-YYYYMMDD-NN.md` và record ID thực.
- **Không dùng:** `final`, `final2`, `test_ok`, `real_latest` hay tên không thể truy vết.
- Sửa một bản **RELEASED**: tạo revision/release mới và liên kết `supersedes`; không overwrite bản cũ.

---

## 4. CẤU TRÚC GITHUB — GIỮ DI SẢN, TẬP TRUNG ONshape CHO DESIGN MỚI

### 4.1. Hiện trạng repo đã được đọc (snapshot `dev` khi biên soạn)

Hiện đã có:

```text
hardware/mechanical/
  AGENTS.md
  README.md
  solidworks/parts/        # legacy folders có sẵn
  solidworks/assemblies/   # legacy folders có sẵn
  solidworks/drawings/     # legacy folders có sẵn
  step/
  dxf/
  fabrication/
hardware/bom/BOM_TEMPLATE.csv
```

Có `.gitignore`, `.gitattributes`, `.github/CODEOWNERS`, cùng các tài liệu SolidWorks trong `docs/project_management/...` và `docs/plans/week03/README.md`. **Codex phải kiểm tra lại live `dev` trước khi lập danh sách diff; snapshot này không thay cho khảo sát thực tế.**

### 4.2. Cây thư mục đích **ĐỀ XUẤT** (không tự tạo ở Giai đoạn 1)

```text
hardware/
├── mechanical/
│   ├── AGENTS.md                       # đã có; chỉ sửa nếu ME1 duyệt
│   ├── README.md                       # cập nhật: Onshape-only cho thiết kế mới
│   ├── solidworks/                     # LEGACY/READ-ONLY; giữ nguyên, không thêm file mới
│   │   ├── parts/
│   │   ├── assemblies/
│   │   └── drawings/
│   ├── onshape/
│   │   ├── cad_registry.md             # Document/Version/ownership/status + ID map
│   │   └── release_manifest_template.md
│   ├── cad-source/
│   │   ├── scripts/                    # Python/text-to-cad scripts có thể tái tạo
│   │   └── params/                     # JSON/YAML/TOML parameters khi thực sự có
│   ├── step/                           # đã có: STEP từ MAIN Version cụ thể
│   ├── stl/                            # preview/prototyping khi thật sự cần
│   ├── drawings/                       # PDF Drawing đã phát hành
│   ├── dxf/                            # đã có: 2D fabrication khi cần
│   ├── fabrication/                    # đã có: manifest/checklist/release package
│   │   └── <RELEASE_ID>/
│   │       └── manifest.md
│   ├── reports/
│   │   ├── TEST_REPORT_TEMPLATE.md
│   │   └── <COMPONENT_ID>/
│   │       └── <RELEASE_ID>/
│   │           └── test-report.md
│   └── previews/                       # PNG preview nhỏ nếu có nhu cầu
└── bom/
    ├── BOM_TEMPLATE.csv               # đã có
    └── mechanical_bom_<REV>.csv        # CHỈ tạo khi BOM có dữ liệu thật
```

**Không tạo cả cây rỗng chỉ để đẹp.** Đường dẫn `reports/`, `stl/`, `previews/`, `cad-source/params/` và `onshape/` chỉ tạo sau phê duyệt nếu có artifact/quy trình tương ứng. `solidworks/` và `fabrication/` đã có **không được xóa**, tránh tạo `fabrication_package/` trùng mục đích.

**Đặc biệt:** `hardware/mechanical/fabrication/<RELEASE_ID>/manifest.md` **dẫn tới** STEP/Drawing/BOM/report ở các thư mục chuẩn; không bắt buộc nhân bản mọi binary vào release folder. Chỉ gom bản sao khi nhà gia công cần một package đóng băng và được kiểm soát hash/size, tránh trùng lặp Git không cần thiết.

### 4.3. Ownership và ranh giới

- `hardware/mechanical/` → ME1 `@nkaq`.
- `hardware/bom/` → ME1 và ET1 (`@nkaq`, `@tiendatvu13`); kiểm xung đột trước khi sửa BOM dùng chung.
- `docs/architecture/`, `experiments/protocols/` → ME1; chỉ cập nhật nếu liên quan và đã duyệt.
- `hardware/electronics/` → ET1; `firmware/esp32s3/` → EE2; `ml/`, `software/`, `data/` → IT2; sensor characterization → MS2.
- Không đặt artifact chính thức tại `members/ME1/`; không tạo project Git repo thứ hai cho ME1.
- Ảnh đo/mô phỏng có thể để tại thư mục phù hợp `assets/`, `results/` hoặc `docs/reports/`, nhưng phải có đường dẫn có thật, quyền owner và cross-reference với registry/report.

### 4.4. Các file repo bắt buộc đánh giá trước khi sửa

| File/path hiện có | Hành động đề xuất | Ghi chú |
|---|---|---|
| `hardware/mechanical/README.md` | **Cập nhật** công cụ Onshape, cây thư mục, release gate; đánh dấu legacy | Ưu tiên số 1 |
| `hardware/mechanical/AGENTS.md` | Kiểm tra; **chỉ sửa nhỏ** nếu cần Codex hiểu Onshape/version/safety | Không xóa Human Gate |
| `docs/project_management/NCKH_MEMBER_WORKSPACE_FILETYPE_GUIDE_V3.md` | Thêm `.step/.stp/.stl/.pdf/.dxf/.py` và Onshape link/version; bỏ SolidWorks khỏi **recommended new output** | Có thể cần phiên bản hướng dẫn mới |
| `docs/project_management/REPOSITORY_STRUCTURE.md` | Sửa ví dụ cơ khí hiện hành; phân biệt legacy | Không đổi ownership |
| `.gitattributes` | Kiểm LFS cho STEP/STL/PDF **theo nhu cầu/kích thước**, không bật bừa | Snapshot hiện có chỉ LFS `*.SLD*`, `*.pptx`, `*.psd` |
| `.gitignore` | Thêm ignore chỉ cho **cache/temp/export thừa đã xác minh** | Không ignore nhầm file `.step` cần phát hành |
| `hardware/bom/BOM_TEMPLATE.csv` | Đánh giá cột ID/revision/material/quantity/supplier/status | Không tự gán giá/nhà cung cấp |
| `docs/project_management/NCKH_TWO_STAGE_CODEX_SKILLS_WORKFLOW_HANDBOOK_V1.md` | Chỉnh phần ME1 nếu vẫn ghi SolidWorks là workflow chính | Không ghi đè toàn bộ handbook |
| `docs/plans/weekXX/README.md` | **Chỉ cập nhật plan đang có hiệu lực nếu ME1 giao nhiệm vụ mới**, không đổi ghi chép lịch sử | Không sửa Week 3 chỉ để tẩy tên SolidWorks |
| `CHANGELOG.md` + Project Update | Ghi thay đổi quy trình mức B / cao hơn nếu có Safety/contract | Có Update ID thật, ngày hiệu lực, ACK |

**Quy tắc tìm từ khóa:** tìm mọi `SolidWorks`, `SOLIDWORKS`, `.SLDPRT`, `.SLDASM`, `.SLDDRW`, nhưng **phân loại ACTIVE GUIDE / HISTORICAL / LEGACY / GENERATED / SCOPE-OTHER**; chỉ đề xuất thay đổi ACTIVE GUIDE có liên quan. Việc có chữ SolidWorks không tự có nghĩa file sai.

---

## 5. QUY TẮC ARTIFACT VÀ RELEASE TRACEABILITY

### 5.1. Những artifact tối thiểu của từng release cơ khí

| Loại | Bắt buộc khi nào? | Nguồn và kiểm tra |
|---|---|---|
| **Onshape MAIN Document + Version ID/URL** | **BẮT BUỘC mọi release CAD mới** | Mở được bởi reviewer có quyền; Version cố định |
| **Release Manifest** `.md` | **BẮT BUỘC mọi release** | Map CAD ↔ file export ↔ report ↔ BOM ↔ PR/Git commit |
| **STEP** `.step`/`.stp` | BẮT BUỘC nếu cần trao đổi 3D/chế tạo | Xuất đúng MAIN Version; check units, part count, assembly integrity |
| **Drawing PDF** | BẮT BUỘC với chi tiết cần gia công/kiểm kích thước | Rev, kích thước, dung sai, vật liệu, datum, scale, inspection notes |
| **BOM CSV** | BẮT BUỘC khi release assembly/chế tạo cần vật tư | Part ID, Rev, qty, material/spec, source status |
| **DXF** | CHỈ khi có gia công cắt/laser/CNC 2D | Units, origin, geometry, drawing ref |
| **STL** | TÙY CHỌN: preview/3D print/prototyping | Không thay STEP vì mesh không phải CAD parametric gốc |
| **PNG preview** | TÙY CHỌN | Ảnh có ghi TEST/MAIN, phiên bản, không dùng làm metrology evidence |
| **Python script + param config** | BẮT BUỘC khi part thực sự sinh bằng text-to-cad/script | Phiên bản script, môi trường/dependencies, seed nếu có, mapping input→STEP |
| **Test report** `.md` | **BẮT BUỘC mọi phần cần engineering acceptance** | Test matrix, methods, trạng thái/evidence, reviewer, limitations |
| **Safety/fabrication approval** | Khi có nguy cơ vật lý/gia công/vận hành motor | ME1 + chuyên môn liên quan; không thay bằng AI review |

**Bản TEST:** có thể chưa có đủ các artifact như release, nhưng phải gắn `TEST_ONLY` và phân biệt rõ. **Không tạo placeholder “PASS” hay artifact rỗng để hoàn thành checklist.**

### 5.2. Cấu trúc `cad_registry.md` (đề xuất cột)

Registry gồm phần đầu mô tả Onshape account/document và bảng ánh xạ ít nhất:

| `component_id` | `release_id` | `engineering_rev` | `onshape_document_id` | `onshape_version_id` | `onshape_version_url` | `onshape_element_id` | `state` | `owner` | `drawing_path` | `bom_path` | `report_path` | `manifest_path` | `supersedes` | `superseded_by` |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| `<ID_THẬT>` | `<RELEASE_ID_THẬT>` | `<ENGINEERING_REV_THẬT>` | `<DID>` | `<VID>` | `<URL version>` | `<EID nếu cần>` | `DRAFT/UNDER_REVIEW/APPROVED/RELEASED/...` | `ME1` | `<PATH>` | `<PATH>` | `<PATH>` | `<PATH>` | `<SUPERSEDED_RELEASE_ID/NONE>` | `<REPLACING_RELEASE_ID/NONE>` |

`release_id`, `engineering_rev` và `supersedes/superseded_by` không được đồng nhất: cả hai quan hệ thay thế nhận Release ID, không phải revision; một engineering revision có thể thuộc nhiều package khác nhau. Row lịch sử được giữ ở state `SUPERSEDED` khi release mới được phê duyệt. Không dùng tên Onshape Version làm engineering Revision.
Bảng trên là **schema minh họa**, **không phải registry đã có dữ liệu**. Không đưa thông tin nhạy cảm/access token vào registry; URL của Document private vẫn cần ACL phù hợp.

Tách riêng `document_url`, `workspace_url`, `version_url` nếu cần quản lý nhiều môi trường; **tuyệt đối không dùng workspace URL ở trường version_url**. Nếu link phiên bản Onshape có thay đổi cú pháp, lấy link bằng chức năng copy/share trong UI thực, không tự chế URL.

### 5.3. `fabrication/<RELEASE_ID>/manifest.md` cần gì?

- `release_id`, `component_id(s)`, `fixture_version`, `setup_version`, `engineering_rev`.
- `main_document_url`, `main_document_id`, `main_version_name`, `main_version_id`, `main_version_url` và danh sách element/assembly.
- `CAD TEST candidate/version` (nếu đã đi qua TEST), phương thức đưa qua MAIN và ngày xác nhận.
- Danh sách Drawing PDF / STEP / STL (nếu có) / DXF (nếu có) / BOM CSV / source Python+parameters / report, tất cả **đường dẫn repo thực**, format, đơn vị, kích thước tệp, **SHA256** khi kiểm được.
- Export settings quan trọng: STEP format version, export units, STL resolution/units, Drawing PDF/DXF units, timestamp và người xuất.
- `simulation evidence`, `bench evidence`, acceptance criteria nguồn và các mục `NOT_RUN/BLOCKED`.
- `human reviewer`, `decision reference`, PR URL, Git commit SHA được cập nhật sau merge thật, `supersedes/rollback` nếu có.
- **Trạng thái:** `PROPOSED`/`UNDER_REVIEW`/`APPROVED`/`RELEASED`/...; không tự đánh dấu `RELEASED` ngay khi tạo manifest.

**Hash không chứng minh đúng thiết kế**; hash chỉ chứng minh file không đổi. Nếu chưa chạy lệnh tính SHA256, ghi `PENDING`, không sinh hash giả.

### 5.4. Đường truy vết bắt buộc hai chiều

```text
MAIN Onshape Document + immutable Version ID
     ↕ cad_registry.md
Part/Assembly ID + Engineering Revision + Fixture/Setup Version
     ↕ release manifest
STEP / Drawing PDF / DXF / STL / Python script + params / BOM
     ↕ test-report.md + simulation/bench evidence
PR review + human decision + Git SHA
     ↕ (nếu ảnh hưởng thu dữ liệu) Experiment Metadata / Ground Truth Protocol Version
```

- Trong Drawing notes/title block: ít nhất Part ID, Revision, Drawing ID, đơn vị, material/spec, release ID hoặc đường đến registry/report khi phù hợp.
- BOM CSV: Part ID/revision/qty/material/supplier hoặc `TBD` theo bằng chứng; kiểm số lượng với Assembly.
- Test report: link ngược từ evidence về version và package; `cad_registry.md` trỏ trực tiếp report/manifest.
- Nếu bản cũ bị `SUPERSEDED`: giữ đường dẫn cũ, link tới release thay thế và ghi tương thích. **Không rewrite Git history để xóa dấu vết**.

---

## 6. BÁO CÁO KIỂM CHỨNG CƠ KHÍ — THEO ĐỐI TƯỢNG, THEO LẦN VÀ THEO RELEASE

### 6.1. Quy tắc đánh giá theo rủi ro thay vì ép chạy mọi mô phỏng

Đối với mỗi component/assembly, ME1 phân loại applicability `REQUIRED`, `CONDITIONAL`, `NOT_APPLICABLE + lý do`. **Không phải part nào cũng cần đủ 10 loại phân tích**.

| Hạng mục | Ví dụ đối tượng/rủi ro | Cần có trước PASS |
|---|---|---|
| Hình học, dung sai, lắp ráp | Đế motor, mặt gá, lỗ/fit, clearance | Drawing/measurements đối chiếu tiêu chí; bản CAD dùng đúng version |
| Vật liệu và chế tạo | Giá đỡ, trục, bulông | Grade/spec, phương pháp chế tạo, thông tin nhà cung cấp khi có |
| Linear Static | Tấm/gá chịu tải, assembly tải tĩnh | Load cases, boundary conditions, material, mesh, convergence/limits, ứng suất/võng có căn cứ |
| Độ cứng và alignment | Trục-coupling-bearing-motor | Reference datum, deflection/misalignment và acceptance nguồn |
| Modal/natural frequency | Fixture và gá ADXL345 | Modes, giả định contacts/mates, dải rpm/harmonics, giới hạn mô hình |
| Động lực học/tải quay | Shaft/coupling/fault mechanism | Load/torque/operating envelope thực, plan phân tích rung cưỡng bức nếu cần |
| Rung vật lý | Gá sensor/fixture thử | Setup, máy đo/board, phương pháp, record ID, điều kiện vận hành, bằng chứng đo |
| Fatigue | Trục, liên kết chịu chu kỳ | Cycles/stress range/source; solver chuyên biệt hoặc test khi cần |
| Thermal/airflow | Chỉ khi nhiệt/làm mát là rủi ro thực | Assumptions và phương pháp đo/solver phù hợp; không tự tính nếu không liên quan |
| Mechanical safety | Vỏ che, bộ phận quay, rủi ro kẹp/văng | Checklist con người, phương án che chắn, thử vận hành có kiểm soát |
| Mount repeatability | ADXL345, vị trí sensor và tháo lắp lại | Datum/orientation, torque/spec nếu có, số lần lặp và bằng chứng |
| Fixture acceptance/GT | Toàn hệ thống và fault mechanism | Protocol nguồn, điều kiện vật lý có kiểm soát, ME1 decision và metadata version |

**Không tạo threshold hay con số vật lý:** RPM, lực, tải, factor of safety, khoảng tách cộng hưởng, kích thước, dung sai, acceptance số học đều phải từ tiêu chuẩn/spec/protocol đã chốt; khi thiếu ghi `TBD — NEEDS ME1 DECISION`.

### 6.2. Onshape Simulation: điều kiện sử dụng và giới hạn

- Kiểm tra **plan/license và tính năng Simulation thực sự khả dụng** trên tài khoản của ME1 trước khi cam kết thực hiện.
- Onshape Simulation (khi có) hỗ trợ **Linear Static** và **Modal (natural frequency)** trong **Assembly** theo hướng dẫn hãng; phải nhập đúng vật liệu, mates/contacts, tải và ràng buộc, ghi version/method/settings/evidence.
- **Modal chỉ cho tần số riêng/mode shapes** trong giả định tuyến tính; **không** là mô phỏng đáp ứng rung cưỡng bức, rotor imbalance theo thời gian, forced frequency response hay damping thực nghiệm. Không kết luận “không cộng hưởng” chỉ vì một mode đầu tiên khác tần số quay.
- Onshape không mặc nhiên có Fatigue/CFD/Transient rotor dynamics/thermal solver đầy đủ. Nếu task yêu cầu, ghi `NOT_RUN`/`BLOCKED` và đề xuất solver hoặc bench validation khác, chờ ME1 duyệt.
- Kiểm convergence/mesh, độ hợp lệ của contacts, boundary conditions và load cases; “solver finished” ≠ engineering PASS.
- Thiết bị quay: an toàn vận hành thực tế không được chứng minh bằng simulation; cần ME1/human physical test plan/acceptance.

### 6.3. Trạng thái báo cáo chính xác

- `PASS`: đủ **criterion đã duyệt + phép kiểm thực chạy + evidence + human review phù hợp**.
- `FAIL`: không đạt criterion hoặc thất bại có chứng cứ.
- `NOT_RUN`: chưa thực hiện; lý do, cần công cụ/người nào, ảnh hưởng release.
- `BLOCKED`: chưa thể đánh giá/triển khai do thiếu quyền, spec, model, board, safety gate hoặc dependency.
- `N/A`: hạng mục không áp dụng, nêu **lý do kỹ thuật** và người xác nhận applicability.
- `SIMULATED`: nhãn nguồn bằng chứng; nếu dùng kèm kết luận thì tách **simulated verification** khỏi **measured/physical acceptance**.
- `INFERRED`/`HYPOTHESIS`: không dùng thay `MEASURED`.

Các phần `REQUIRED` đang `FAIL/NOT_RUN/BLOCKED` **không được release như đã nghiệm thu**. Nếu một kiểm tra `CONDITIONAL` được miễn, phải lưu scope/rationale và quyết định người có quyền.

### 6.4. File `TEST_REPORT_TEMPLATE.md` và mỗi `test-report.md`

Gồm tối thiểu:

1. Header: report ID, component/assembly ID, engineering Rev, release ID, report revision, writer/reviewer/date/timezone/status.
2. Document sources: CAD TEST candidate/version link (nếu có), **MAIN Document/Version ID + URL cố định**, element ID và Git commit/script revision.
3. Hình học: coordinate frame, units, datum, mounting, drawing/STEP/DXF/BOM/preview links và export settings.
4. Vật liệu/khối lượng, fastener/interface và nguồn thông số (TBD nếu thiếu).
5. Operating envelope/motor RPM, load cases, force/torque/constraints, acceptance reference.
6. Bảng applicability/check matrix — geometry, assembly, material, static, modal, dynamics, fatigue, thermal, sensor mount, safety/guarding và fixture acceptance.
7. Kết quả FEA mỗi phép: solver/version/license, material, mesh/convergence, boundary/loads, stress/displacement/FOS, units, image/log links, assumptions và limits.
8. Kết quả modal: frequencies/modes với units, boundary/mates, setup/damping giả định, đối chiếu harmonics/cảnh báo forced-response `NOT_RUN` khi chưa đo.
9. Physical test: máy/fixture/setup version, environmental context, motor condition, repeated trials, raw evidence location, người vận hành và các giới hạn đo.
10. Tổng kết finding, remaining risks, items `NOT_RUN/BLOCKED/N/A`, affected owner và actions.
11. Kết luận **chỉ đúng trong scope**, quyết định HUMAN `APPROVE/REJECT/PENDING`, link PR/approval và release gate.
12. Traceback/backlinks tới `cad_registry.md`, fabrication manifest, Drawing/BOM/version và `supersedes`.

**Không chỉ có một `reports/test-report.md` duy nhất bị ghi đè theo thời gian.** Template chung nằm `reports/TEST_REPORT_TEMPLATE.md`; hồ sơ thật nằm dưới `<COMPONENT_ID>/<RELEASE_ID>/`. Nếu nhiều run, mỗi run có ID và được link trong report cấp release.

---

## 7. QUY TRÌNH RELEASE ĐƯỢC KIỂM SOÁT

### 7.1. State machine CAD/Engineering (không lẫn trạng thái GitHub PR)

```text
CAD TEST: DRAFT -> UNDER_REVIEW -> TESTED hoặc REJECTED
                                     |
                                     v (ME1 cho phép chuyển)
MAIN: DRAFT -> UNDER_REVIEW -> APPROVED -> RELEASED -> SUPERSEDED
          \--------------------------> BLOCKED / REJECTED
```

| Trạng thái | Điều kiện tối thiểu | Ai quyết |
|---|---|---|
| `DRAFT` | Có working proposal | Người thiết kế |
| `UNDER_REVIEW` | Version candidate + diff/report + phạm vi | ME1 yêu cầu review |
| `TESTED` (TEST only) | Kiểm thử đã chạy đúng phần TEST đã khai báo | ME1 xác nhận kỹ thuật phạm vi TEST |
| `APPROVED` | MAIN Version cố định; review kỹ thuật và Human Gate hợp lệ theo scope | ME1, owner liên quan khi cần |
| `RELEASED` | APPROVED + fabrication package/registry/report/BOM/Drawings nhất quán, PR source merged và release decision ghi nhận | ME1 hoặc người được giao quyền có hồ sơ |
| `BLOCKED` | Thiếu spec, an toàn, quyền, phép đo hoặc dependency | Owner xác định và giải quyết blocker |
| `SUPERSEDED` | Có release mới thay thế; bản cũ vẫn tra cứu được | ME1 ghi mapping/reason |

**Không có rule “GitHub Actions PASS ⇒ CAD APPROVED”.** Quyết định phê duyệt CAD phải có bằng chứng/đúng chuyên môn, không chỉ thông qua merge code.

### 7.2. Workflow thực tế từ thiết kế đến fabrication

1. **Intake:** xác nhận Task ID hoặc nguồn task hợp lệ, part ID, yêu cầu, đơn vị, giới hạn tải, fixture/setup version hiện hành và người có quyền.
2. **TEST:** tạo/edit ở `CAD TEST`, log candidate, source Python/STEP và thử hình học/assembly/analysis theo applicability.
3. **Test evidence:** xuất test report/mô phỏng/physical checks có nguồn; tách nguồn SIMULATED/MEASURED.
4. **Preliminary review:** ME1 xác minh concept, điều kiện an toàn, mounting, dependencies và quyết định chuyển vào MAIN.
5. **MAIN:** đưa geometry/feature phù hợp vào `ME1_MAIN_CAD`; tái kiểm assembly/mates/drawing/BOM vì import/derive có thể mất thông tin tham số.
6. **Version freeze:** tạo Onshape MAIN Version; ghi **Document/Version ID và URL thực**, kiểm Drawing cập nhật trước snapshot.
7. **Export:** từ MAIN Version xuất STEP/PDF/DXF/STL khi cần, với units/settings/traces; kiểm import lại nếu quan trọng.
8. **Record:** cập nhật `cad_registry.md`, release manifest, BOM và test report; tính SHA256 cho artifact thực nếu cần.
9. **Local review:** Codex có thể review scripts/Markdown/manifest/cross-links; với binary CAD dùng check theo metadata/preview và yêu cầu human CAD review; không tự nhận FEA PASS.
10. **Human Git:** xem `git diff`, staged files, LFS/size; ME1 hoặc member được phép tự commit/push trên feature branch và PR vào `dev`.
11. **GitHub review:** kiểm `repo-quality` job có thực, Codex findings trên HEAD đúng, owner/human approval liên quan; sửa finding có bằng chứng.
12. **Release decision:** chỉ sau approval thích hợp và merge, ME1 đánh dấu `RELEASED` bằng một **bản ghi thay đổi truy vết** (PR/tài liệu), đảm bảo manifest trỏ commit đã thực sự tồn tại. Nếu phải cập nhật sau merge, dùng PR cập nhật metadata, không tự rewrite commit.
13. **Announce:** với thay đổi quy trình/cấu hình ảnh hưởng nhóm, phát Project Update, cập nhật mốc hiệu lực và ghi ACK/READY/BLOCKED.
14. **Operate:** chỉ dùng release được duyệt cho fabrication/fixture; nếu physical fixture thực khác CAD, mở deviation/inspection report và ME1 duyệt ảnh hưởng đến Ground Truth.

### 7.3. Logic ACCEPT / STOP

- **Không có MAIN Version ID:** STOP release.
- **MAIN Version có nhưng STEP/Drawing/BOM sai revision:** STOP release.
- **Simulation `NOT_RUN` ở hạng mục REQUIRED:** STOP hoặc yêu cầu ME1 quyết định lại phạm vi/criterion bằng hồ sơ riêng; không tự PASS.
- **Safety/rotor hazard chưa human-verify:** STOP vận hành motor/fixture.
- **Onshape private access không cho reviewer mở:** BLOCKED review; không tự publish public.
- **CAD TEST khác MAIN:** cần tái xác minh, không giữ kết quả cũ bằng cách rename.
- **Script thay đổi source nhưng STEP không tái tạo được:** BLOCKED traceability.
- **Phần cứng đã khóa/interface thay đổi:** HUMAN GATE và Project Update; không tự sửa.

---

## 8. GIT, GIT LFS VÀ CHÍNH SÁCH DUNG LƯỢNG

### 8.1. Hiện trạng `.gitattributes` / `.gitignore`

Snapshot đã kiểm tra trên `dev`: `.gitattributes` có LFS cho `*.SLDPRT`, `*.SLDASM`, `*.SLDDRW`, `*.pptx`, `*.psd`; `.png`, `.jpg`, `.jpeg`, `.pdf` được đánh dấu binary; **không thấy LFS rule cho `*.step`, `*.stp`, `*.stl`, `*.pdf`, `*.dxf`**. Root `.gitattributes` còn dùng **`* text=auto eol=lf`**; ASCII STEP/STL và DXF có thể bị Git chuẩn hóa newline/byte khi add/checkout nếu thiếu scoped binary override, dù file vẫn mở được trong CAD. `.gitignore` có nhiều quy tắc cache/temporary/SolidWorks và raw data. Đây là **bằng chứng cần kiểm lại trên HEAD mới nhất**, không phải chỉ đạo tự thêm LFS.

### 8.1.1. BYTE-INTEGRITY GATE cho STEP/STL/DXF (chưa triển khai config trong PR này)

**BLOCKED — CAD EXPORT BYTE INTEGRITY** trước fabrication release nếu file STEP/STP, STL (kể cả ASCII), DXF hoặc binary khác bị quy tắc `* text=auto eol=lf` tự động chuẩn hóa byte. SHA256 tính lúc export có thể khác SHA sau Git checkout. LFS-vs-normal-Git là một quyết định **khác** với text-vs-binary. Bắt buộc ME1 duyệt **PR cấu hình riêng** cho scoped `-text`/binary attributes phù hợp (.step/.stp/.stl/.dxf, và formats khác có căn cứ) **trước khi commit export làm release**; đây là file `.gitattributes` **ngoài danh sách 16 file đã duyệt**, nên không tự sửa trong PR tài liệu này.

Kiểm chứng bắt buộc khi cấu hình được phép: `git check-attr -a -- <ACTUAL_EXPORT_PATH>`, SHA256 của export gốc → add/commit/checkout trên môi trường thực (Windows/Linux nếu nhóm dùng) → compare SHA256 byte-for-byte, xác nhận đúng định dạng/units và reviewer. Không tạo sample CAD giả để tuyên bố PASS. Đến lúc có bằng chứng, ghi `NOT_RUN/BLOCKED` và không coi artifact chưa chắc byte-identical là fabrication release.

### 8.2. Cách quyết định file gì nên đưa lên Git

- **Git thường:** Markdown, CSV nhỏ, YAML/JSON/TOML configs, Python scripts; STEP/STL/PDF/DXF **chỉ khi kích thước và tần suất phù hợp** với chính sách của repo.
- **Git LFS:** cân nhắc khi binary lớn/thay đổi thường xuyên và GitHub LFS quota/team tooling được ME1 kiểm tra; cần **cài và verify Git LFS trên máy thành viên**, thống nhất `.gitattributes` bằng PR trước khi thêm file thuộc diện tracking.
- **External artifact storage:** với bản CAD/CAE quá lớn hoặc nhiều snapshot, giữ URL/checksum/metadata trong GitHub; kiểm quyền truy cập, retention và provenance. Không trỏ sang đường dẫn ổ D cá nhân làm nguồn duy nhất của release.
- **Không commit:** simulation cache, solver scratch, temp, secret/API keys, downloaded materials có license hạn chế, file log/raw dataset lớn và bản TEST lẫn với RELEASED.
- **Không xóa/di chuyển lịch sử LFS SolidWorks** để tiết kiệm chỗ khi chưa lập migration/backup plan; điều này khác với việc không dùng SolidWorks nữa.

### 8.3. Lệnh Git/PowerShell được Codex phép đề xuất **chỉ đọc ở Giai đoạn 1**

```powershell
# Chạy ở đúng root repository đang mở trong VS Code:
git rev-parse --show-toplevel
git branch --show-current
git status --short --untracked-files=all
git fetch origin                 # chỉ cập nhật remote refs, không sửa worktree
git ls-files hardware/mechanical hardware/bom
git ls-files '*SolidWorks*' '*.SLDPRT' '*.SLDASM' '*.SLDDRW'
git check-attr -a -- 'hardware/mechanical/step/example.step'
git lfs version                  # Có thể báo chưa cài; không tự cài
# Đọc .gitattributes và .gitignore từ repo hiện hành, không sửa.
```

`git fetch` là thao tác mạng và cập nhật refs; nếu prompt yêu cầu **READ-ONLY tuyệt đối, không lệnh thay đổi state**, Codex phải **xin phép** trước khi chạy. Nếu không được phép, dùng bản `dev` hiện có và ghi giới hạn snapshot.

**Không gửi nguyên màn hình chứa `git remote -v` cho AI/người khác** trước khi kiểm/redact URL chứa `username:token@`, PAT hay thông tin nhạy cảm. Không cung cấp thông tin đăng nhập Onshape hoặc cloud workspace cho Codex.

### 8.4. Lệnh Git ở Giai đoạn 2 — HUMAN THỰC HIỆN SAU KHI ĐÃ DUYỆT

Chỉ chạy khi working tree sạch và đã xác nhận nhánh/remote; đây là **ví dụ có kiểm soát**, không phải lệnh Codex tự chạy:

```powershell
# Human kiểm trạng thái:
git status --short
git switch dev
git fetch origin
git merge --ff-only origin/dev

# Human tạo branch mới riêng cho sửa quy trình CAD:
git switch -c feature/me1-onshape-workflow

# Sau khi Codex (được phép) chỉ sửa docs/config được duyệt:
git status --short
git diff --check
git diff --stat
git diff

# Human stage CHỈ file đã review (ví dụ; thay danh sách file thật):
# git add -- hardware/mechanical/README.md
# git add -- docs/project_management/NCKH_MEMBER_WORKSPACE_FILETYPE_GUIDE_V3.md
# git add -- hardware/mechanical/onshape/cad_registry.md
# git diff --cached --check
# git diff --cached
# git commit -m "docs(me1): migrate new CAD workflow to Onshape"
# git push -u origin feature/me1-onshape-workflow
```

`git merge --ff-only origin/dev` là cách đồng bộ an toàn từng được kiểm chứng trên máy NCKH; không dùng `git reset --hard`, `git clean -fd`, `git push --force` để “sửa nhanh”. Với PR chỉ về hướng dẫn, ghi runtime CAD/bench `NOT APPLICABLE`, không bịa FEA/test PASS. Nếu sửa policy ảnh hưởng cả nhóm, lập Project Update trước khi tuyên bố ACTIVE.

---

## 9. TUÂN THỦ NCKH: HARDWARE LOCKED, GROUND TRUTH VÀ TEAM WORKFLOW

### 9.1. Cấu hình phần cứng đã khóa — KHÔNG TỰ THAY

- ESP32-S3
- ADXL345 3-axis vibration
- ACS724 current sensor
- PT100 + MAX31865 motor temperature
- US5881 Hall + magnet RPM reference
- DHT22 ambient conditions
- OLED status

Fixture phải xét lắp được cảm biến, hướng/cứng vững gá ADXL345 lặp lại, shaft/coupling, fault mechanism, cable routing và guarding. Nếu cần thay sensor, pinout, fault mechanism, fixture geometry ảnh hưởng acquisition/calibration hoặc chuẩn Ground Truth: **dừng và yêu cầu owner cross-subsystem/ME1 duyệt**.

### 9.2. Ground Truth và dữ liệu

- Physical Ground Truth lấy từ **điều kiện lỗi vật lý được chủ động áp đặt** theo experiment protocol/version có chứng cứ; không dùng FFT, RMS, hình CAD, simulation hoặc model prediction để tự gán fault label.
- Thay fixture/setup/mounting sau nghiệm thu có thể gây **domain shift**; ghi `fixture_version`, `setup_version`, `mount_id`, thời gian và affected recordings; không trộn dữ liệu cũ và mới như một cấu hình nếu chưa chứng minh tính tương đương.
- Engineering Validation Data **không tự thành** Dataset v0.1 vì QC hay ME1 CAD report PASS; dataset inclusion phải có Human Gate riêng.
- Khi đưa drawing/fixture vào testbench thực, ME1 phải quyết định physical acceptance với bằng chứng; Onshape report/PR không tự thay thế nghiệm thu thực tế.

### 9.3. GitHub review và Codex Skills

- Đọc root `AGENTS.md`, `hardware/mechanical/AGENTS.md`, `.github/CODEOWNERS`, `.github/PULL_REQUEST_TEMPLATE.md`, `CONTRIBUTING.md`, task source thực, `docs/project_management/NCKH_MEMBER_WORKSPACE_FILETYPE_GUIDE_V3.md` và docs quản lý thay đổi.
- Nếu có repo Skills `nckh-task-execution` / `nckh-pr-preflight`, dùng khi phù hợp; `code-review`, `diagnosing-bugs`, `tdd` user-level chỉ đọc được trên máy đã cài, **GitHub Codex Review không tự đọc Skills ở ổ D**.
- Review CAD bằng AI tập trung source script, path, config, registry, traceability, version, test-report consistency. **Không coi review Markdown/code là xác nhận an toàn cơ khí.**
- **Người thật** review diff, stage, commit, push, PR, quyết định approval/merge. AI không tự commit/push/merge/force-push/xóa branch.
- Nếu yêu cầu PR nguồn: với **Task tuần** dẫn README tuần và Task ID; với **finding từ PR** dẫn comment/PR; với **hạ tầng/tài liệu** dẫn task/decision phê duyệt. Không tạo Task ID tuần giả cho migration docs.

### 9.4. Project Update khi chuyển sang Onshape

Chuyển công cụ chính và tiêu chuẩn bàn giao CAD ảnh hưởng thành viên khác → **mức B** theo `NCKH_TEAM_CHANGE_UPDATE_WORKFLOW_V1.md`. Nếu đồng thời đổi interface vật lý, fixture locked, safety, Ground Truth protocol hoặc kiến trúc → phân loại **mức C**, có Human Gate chuyên môn tương ứng.

**Trong Giai đoạn 1**, Codex phải đề xuất **Project Update** với thông tin dự kiến (không tự phát):

- `Update ID`: **PENDING — ME1 cấp**; không tự tạo ID giả.
- `Status`: `PROPOSED` trong lúc chưa duyệt; `APPROVED` khi có decision/Human Gate thích hợp **trước merge**; `ACTIVE` chỉ sau khi các điều kiện đã công bố đạt (merge, thông báo, mốc hiệu lực, compatibility/tests/migration và blockers được xử lý).
- `Old → New`: SolidWorks legacy → Onshape new design only; fixed MAIN Version; STEP/Drawing/BOM/report mapping.
- `Affected roles`: ME1, ET1, MS2, EE2, IT2 theo đúng interface/cách nhận artifact.
- `Effective from`: ngày giờ rõ `YYYY-MM-DD HH:MM UTC+07:00` hoặc điều kiện sau phê duyệt/rollout; **không tự đặt ngày**.
- `ACK`: kênh, deadline, từng người `READY/BLOCKED`, escalation, rollback nếu có blocker chưa xử lý; ACK không tự bằng approval khoa học/safety.
- `Supersedes`: link phiên bản/hướng dẫn cũ khi có quyết định thực; lịch sử vẫn giữ.

---

## 10. CHI TIẾT HAI GIAI ĐOẠN GIAO CHO CODEX

### 10.1. GIAI ĐOẠN 1 — READ-ONLY AUDIT + PLAN (BẮT BUỘC DỪNG)

**Không sửa/tạo/xóa file, không tự cài toolchain, không gọi Onshape API, không tự tạo Document/Version, không commit/push/PR/merge.** Chỉ dùng công cụ đọc nguồn; xin phép nếu cần chạy lệnh có side effect như `git fetch`.

**Phải trả lại đầy đủ các mục sau bằng tiếng Việt:**

1. **Repo inventory:** cây thư mục ME1/BOM; file chính; `AGENTS.md`/scoped/CODEOWNERS; file đang còn SolidWorks; mỗi file là ACTIVE GUIDE/HISTORICAL/LEGACY.
2. **Gap analysis:** hiện trạng → tiêu chuẩn Onshape-only; lỗi/thiếu của version/registry/export/report/quality gate; không bịa feature đã có.
3. **Privacy/licensing gate:** tài khoản Onshape, private/public, owner/collaborators, Simulation availability và blocker.
4. **CAD environments:** xác nhận kiến trúc hai Document `CAD TEST` và `ME1_MAIN_CAD`, quy tắc chuyển, workspace vs immutable version.
5. **Folder plan:** so sánh cây hiện có với cây đề xuất; giữ/cần tạo/tránh tạo; tránh trùng `solidworks/`, `fabrication/`, `step/`, `dxf/`.
6. **Source-to-export mapping:** Part/Assembly ID, Onshape Document/Version/Element, Python script, STEP/STL/PDF/DXF, BOM, test report, manifest, PR.
7. **Coordinates & design rules:** units, axis convention (chờ ME1), datums, mount orientation ADXL345, bolt/coupling, setup/fixture version.
8. **Testing strategy:** applicability/risk matrix và `test-report` template; Linear Static/Modal limits; no fake numbers; physical safety and acceptance gates.
9. **Git/LFS/storage plan:** kích thước/quy định file, `.gitattributes`, `.gitignore`, khả năng Git LFS, quota, binary/source retention.
10. **File-by-file change plan:** file cần `CREATE`, `UPDATE`, `KEEP`, `DO NOT TOUCH`, `HUMAN DECISION`; đường dẫn thật và tác động owner.
11. **Migration plan:** ưu tiên chỉnh hướng dẫn đang active; giữ tài liệu lịch sử và legacy SolidWorks; xử lý BOM/cross-team/Project Update.
12. **Verification plan:** file/link/version/schema checks, report consistency, PR checks, Codex review, handoff; phân biệt automated static tests và human CAD/bench.
13. **Open decisions:** những giá trị ME1 phải chốt (tài khoản/quyền, Document links, release IDs, naming, coordinate frame, tiêu chí acceptance, LFS policy, effective date).
14. **Phạm vi Stage 2:** danh sách file sẽ tạo/sửa/xóa (**mặc định DELETE = NONE**) và thứ tự; cách rollback, checklist nghiệm thu.
15. **Kết luận GATE:** `READY_FOR_ME1_PLAN_REVIEW` / `BLOCKED` kèm lý do, **KHÔNG TỰ CHUYỂN STAGE 2**.

**Định dạng xuất kết quả Stage 1:**

```text
A. Snapshot/branch/commit và instruction files thực sự đọc
B. Current inventory & SolidWorks reference classification
C. Onshape architecture and license/privacy constraints
D. Gap analysis + minimum-change recommendation
E. Proposed file tree + file-by-file change matrix
F. Registry, release manifest, report, BOM and naming contracts
G. Risk-based mechanical verification, interfaces and Ground Truth gates
H. Git LFS, storage, .gitignore/.gitattributes recommendations
I. Project Update/human decisions/owner dependencies
J. Exact stage-2 plan, verification checklist and STOP status
```

**Kết thúc:** `STOP — WAITING FOR ME1 APPROVAL`. Không coi câu “tạo bản hoàn chỉnh” là quyền triển khai repo hay Onshape.

### 10.2. GIAI ĐOẠN 2 — EDIT CHỈ KHI ME1 CHO PHÉP

ME1 sẽ xác nhận bằng lời rõ ràng: nhánh task, danh sách file được phép sửa, trạng thái hồ sơ và những quyết định còn PENDING. Nếu chưa có, Codex dừng.

Trong phạm vi duyệt:

1. Tạo/chỉnh **chỉ** những file docs/registry/template/schema đã duyệt; không tạo CAD/FEA/GT giả.
2. Cập nhật các hướng dẫn active từ SolidWorks sang Onshape và giữ giải thích legacy.
3. Nếu Giai đoạn 2 chỉ là migration **quy trình**: không tạo STEP/Drawing/BOM/report có số liệu tưởng tượng.
4. Nếu cần sửa `hardware/mechanical/AGENTS.md`, `.gitattributes`, `.gitignore` hoặc `docs/project_management` có tác động nhóm: tách hoặc ghi scope/owner rõ trước khi sửa.
5. Kiểm Markdown links, path existence, report/registry sample schema, commands, `.gitattributes` và status assumptions; ghi script/test chạy thật hoặc `NOT_RUN`.
6. Nếu kiểm tra với Onshape không có quyền hoặc không có Document thật: `NOT_VERIFIED`, không tự mở public hay tạo version.
7. Self-review toàn diff bằng `code-review`/`nckh-pr-preflight` nếu tồn tại, báo các findings.
8. Báo cáo file created/updated, evidence, unresolved risks, owner decisions, Proposed Project Update; **DỪNG trước Git add/commit/push/merge**.

**Giai đoạn 2 chưa tương đương phát hành phần cứng:** chỉ áp dụng cách tổ chức/hướng dẫn; một bản CAD cụ thể cần đi qua quy trình riêng ở §7.

---

## 11. BỘ PROMPT CHO ME1 — COPY/PASTE VÀO CODEX EXTENSION

### P-ME1-01 — Dùng ngay: AUDIT REPOSITORY + LẬP PHƯƠNG ÁN (READ-ONLY)

```text
Tôi là ME1 dự án NCKH. Tôi CHỈ dùng Onshape cho thiết kế CAD mới;
SolidWorks là LEGACY lịch sử, không sử dụng nữa cho design/release mới.
Hãy dùng toàn bộ yêu cầu trong file NCKH_ME1_ONSHAPE_CAD_GITHUB_WORKFLOW_REQUIREMENTS_V2.md
làm specification để thực hiện GIAI ĐOẠN 1.

Kiểm repo dev và snapshot/HEAD thực, đọc root AGENTS.md,
hardware/mechanical/AGENTS.md, CODEOWNERS, CONTRIBUTING.md,
NCKH_MEMBER_WORKSPACE_FILETYPE_GUIDE_V3.md, Project Update workflow,
Mechanical README, BOM Template, .gitignore, .gitattributes và docs liên quan.

Phân loại mọi chỗ dùng SolidWorks: CURRENT GUIDE/HISTORICAL/LEGACY.
Đề xuất architecture 2 Onshape Documents, fixed Version references,
CAD registry, per-release test report, source-script mapping, units/datum/axis,
STEP/Drawing/BOM/manifest, LFS, Human Gate, roadmap migration nhỏ nhất.
Nếu chưa biết Onshape plan/private access/Simulation, ghi PENDING/BLOCKED.
Không bịa Document/Version ID, simulation, số đo, test PASS hoặc task ID.

READ-ONLY TUYỆT ĐỐI: không sửa/tạo/xóa file; không thay branch,
không cài dependency, không tự đăng nhập Onshape, không commit/push/merge.
Nếu cần git fetch hoặc command đổi state, xin phép trước.
Xuất theo 15 mục Giai đoạn 1 §10.1, rồi STOP chờ tôi duyệt.
```

### P-ME1-02 — Kiểm tài khoản Onshape & môi trường CAD (không đăng nhập thay người)

```text
Tôi đã quyết định Onshape là CAD chính. Hãy lập checklist để TÔI tự kiểm:
plan, khả năng private/public, owner/collaborators, quyền View/Edit,
Simulation (Linear Static/Modal) có xuất hiện hay không,
2 Documents CAD TEST/MAIN, Workspace và Version snapshot.
Giải thích rõ nếu dùng Free thì nội dung có thể public; yêu cầu BLOCKED
cho CAD nhạy cảm khi không có private hợp lệ.
Không dùng/cất API key, password, token; không giả đã vào tài khoản tôi.
Đưa bảng PASS/FAIL/UNKNOWN và thông tin tôi phải cung cấp; READ-ONLY.
```

### P-ME1-03 — Phương án cây thư mục và mapping file (READ-ONLY)

```text
Đọc cấu trúc thật hardware/mechanical/, hardware/bom/ trên dev.
Giữ solidworks/ như LEGACY, giữ step/, dxf/, fabrication/ đã có.
Đề xuất những folder nhỏ nhất cho onshape/registry, cad-source/scripts,
reports/template/per-release, drawings/, stl/ nếu cần, và naming/version.
Liệt kê CREATE/UPDATE/KEEP/DO-NOT-TOUCH, path cụ thể, owner CODEOWNERS,
không tự tạo thư mục rỗng hoặc sửa repository. So diff dự kiến.
```

### P-ME1-04 — Thẩm định cơ khí và report (READ-ONLY)

```text
Lập checklist review cho testbench motor NCKH: mounting ADXL345,
shaft/coupling/bearing, motor plate, alignment, guard, fault mechanism,
units/datum/axis, fixture/setup version, physical Ground Truth.
Phân loại REQUIRED/CONDITIONAL/N/A theo từng component và risk,
Linear Static/Modal capability và giả định, forced vibration/damping limits.
Đề xuất TEST_REPORT_TEMPLATE.md có nguồn, criterion, evidence,
PASS/FAIL/NOT_RUN/BLOCKED/N/A, human CAD/bench/safety approval.
Không tự giả Onshape có solver hoặc tạo simulated/measured output.
```

### P-ME1-05 — Chỉ sau phê duyệt Stage 1: triển khai tài liệu (EDIT)

```text
ME1 đã duyệt kế hoạch Stage 1 tại <LINK/DECISION>, các file được phép sửa:
<EXACT_PATHS>. Nhánh feature: <EXACT_BRANCH>. Những quyết định PENDING:
<LIST>; không tự giải quyết.

Hãy chỉ thực hiện Giai đoạn 2 của
NCKH_ME1_ONSHAPE_CAD_GITHUB_WORKFLOW_REQUIREMENTS_V2.md.
Không thay đổi subsystem khác, không tự tạo CAD/FEA/BOM số liệu giả,
không xóa solidworks legacy, không thay locked hardware, Ground Truth.
Chạy các static/link/schema tests có sẵn, ghi PASS/FAIL/NOT_RUN thực,
review lại diff, báo file changes/human gates và DỪNG TRƯỚC Git add/commit/push/PR/merge.
```

### P-ME1-06 — Review một CAD release thực (READ-ONLY)

```text
Review candidate release <RELEASE_ID> của <COMPONENT_ID>.
Onshape MAIN immutable Version URL/ID: <REAL_URL_AND_ID>.
Đối chiếu cad_registry.md, fabrication manifest, STEP, Drawing PDF,
DXF/STL nếu có, source Python/parameters, BOM, test-report và Git SHA.
Kiểm units/frame/datum, Rev, fixture/setup, export settings/hash,
material/load/Modal/Static/physical test evidence thực.
Các file CAD binary không đọc/giải mã được thì nêu giới hạn và yêu cầu
ME1 human CAD review; KHÔNG tuyên bố safety, Modal, FOS, fixture PASS khi thiếu chứng cứ.
Chỉ đưa findings và các thiếu sót; không sửa bất cứ file nào.
```

### P-ME1-07 — Phân loại SolidWorks references (READ-ONLY)

```text
Quét SOURCE trên dev để tìm 'SolidWorks', '.SLDPRT', '.SLDASM', '.SLDDRW'.
Với từng kết quả, đánh nhãn ACTIVE_INSTRUCTION / HISTORICAL_RECORD /
LEGACY_ARTIFACT / EXAMPLE / UNRELATED. Chỉ đề xuất chuyển đổi active instructions
cho Onshape; giữ lịch sử; không xóa/rename thư mục/file cũ.
Đưa bảng file:dòng -> lỗi/hệ quả -> proposed replacement -> cần ME1 approval.
Không sửa file, không commit/push/merge.
```

---

## 12. ACCEPTANCE CHECKLIST CHO VIỆC CẬP NHẬT QUY TRÌNH

### 12.1. Điều kiện hoàn thành Stage 1 (PLAN)

- [ ] Xác nhận snapshot repo `dev` và instruction files đã đọc; không tự sửa file.
- [ ] Có inventory SolidWorks **active vs legacy**, không hứa xóa legacy.
- [ ] Có quyết định kiến trúc CAD TEST / ME1_MAIN_CAD với immutable Version.
- [ ] Có bảng account/privacy/simulation `VERIFIED/PENDING/BLOCKED`.
- [ ] Có cây thư mục ưu tiên reuse, danh sách CREATE/UPDATE/KEEP/DO-NOT-TOUCH.
- [ ] Có schema registry, release manifest, BOM/test report và naming/revision.
- [ ] Có đề xuất đơn vị/axis/datum/mount ADXL345, chờ ME1 khóa bằng bản vẽ.
- [ ] Có ma trận kiểm tra theo rủi ro, Solver limits và Human Gate.
- [ ] Có kế hoạch Git LFS/quota/secret và cách không nhân bản binary lớn.
- [ ] Có kế hoạch Project Update và danh sách owner affected; không giả ACK.
- [ ] Có Stage 2 scope, thứ tự, evidence, rollback; **STOP chờ phê duyệt**.

### 12.2. Điều kiện hoàn thành Stage 2 (REPO DOCS)

- [ ] Chỉ sửa path đã duyệt trên feature branch; `dev/main` không bị sửa trực tiếp.
- [ ] README/hướng dẫn active coi Onshape là công cụ duy nhất cho CAD mới.
- [ ] Các file SolidWorks legacy vẫn nguyên vẹn; mọi tham chiếu lịch sử được giữ.
- [ ] Registry/template/manifest/BOM schema dùng ID/Rev/Version nhất quán.
- [ ] Không có Document URL/Version ID, FEA output, số đo, nhà cung cấp/hardware PASS giả.
- [ ] Static link/path/lint/schema check đã được ghi đúng status.
- [ ] Không có secrets/private CAD được công khai không chủ đích.
- [ ] PR review/CI và Human Gate được xử lý/ghi bằng chứng; chưa dùng bot để approve safety.
- [ ] Project Update có nguồn, cấp ảnh hưởng, mốc hiệu lực PENDING/APPROVED/ACTIVE đúng thực tế.
- [ ] Human review diff/commit/push và quyết định release theo quy trình repo.

### 12.3. Điều kiện riêng để release một fixture thực

- [ ] MAIN Onshape immutable Version ID/URL đã khóa và reviewer mở được hợp lệ.
- [ ] Part/Assembly/fixture ID, engineering revision, mounting axis/datum/units hợp lệ.
- [ ] STEP/Drawing/DXF/BOM được xuất **cùng đúng Version/Rev** và kiểm tra.
- [ ] Report theo `<COMPONENT_ID>/<RELEASE_ID>/`, đầy đủ applicability/test evidence.
- [ ] Phép kiểm bắt buộc có PASS thật + logs; `NOT_RUN/BLOCKED` không bị tô xanh.
- [ ] An toàn bộ phận quay/gá sensor/cơ cấu fault đã có human review thực tế.
- [ ] Source Python (nếu có) liên kết tới export, parameters và script revision.
- [ ] Registry ↔ manifest ↔ report ↔ drawing/BOM liên kết hai chiều.
- [ ] PR/approval và trạng thái RELEASED/SUPERSEDED có lịch sử truy vết.
- [ ] Nếu fixture/setup/domain ảnh hưởng recording, ME1/MS2/EE2/IT2 đã được thông báo và metadata version đã kiểm.

---

## 13. CÁC QUYẾT ĐỊNH PHẢI HỎI ME1 — KHÔNG ĐƯỢC ĐOÁN

| ID | Nội dung cần xác nhận | Mặc định nếu chưa có |
|---|---|---|
| D01 | Gói Onshape, private/public và quyền nghiên cứu | `PENDING`; BLOCKED nếu CAD cần private mà tài khoản không có |
| D02 | Tên/URL/ID thật của CAD TEST và ME1_MAIN_CAD | `PENDING`; không chế link |
| D03 | Người sở hữu Document, ai Edit/View, ai được export | `PENDING`; không tự mở public |
| D04 | Hệ trục toàn máy, units/datum, sensor mount frame | `PENDING`; chỉ đề xuất sơ đồ, không khóa giả |
| D05 | Component/assembly ID, engineering Rev và Release ID | Đề xuất format ở §3; chưa coi là final |
| D06 | Material, load, RPM, fatigue life, tolerance, acceptance | `TBD`; không tự tạo số |
| D07 | Simulation license/solver khả dụng và scope cần chạy | `UNKNOWN`; `NOT_RUN` nếu không thể thực hiện |
| D08 | Kích thước artifact, quota Git LFS, retention và storage | `PENDING`; không tự thêm LFS |
| D09 | Những module nên kiểm như Stage 2 doc-only hay thêm sample thật | Doc-only là mặc định an toàn |
| D10 | Project Update ID, effective date, người nhận/ACK | `PROPOSED/PENDING`, không ACTIVE |
| D11 | Chênh lệch giữa cấu hình SolidWorks lịch sử và fixture physical hiện hành | Giữ lịch sử, ME1 kiểm trước mọi fabrication |
| D12 | Những thay đổi liên phòng ảnh hưởng ET1/MS2/EE2/IT2 | `PENDING OWNER REVIEW` |

---

## 14. NGUỒN CHÍNH THỨC, GIỚI HẠN VÀ TÍNH HIỆU LỰC

### 14.1. Tài liệu trong NCKH cần đọc bản **trên `dev` hiện hành**

- `AGENTS.md`, `hardware/mechanical/AGENTS.md`, `README.md`.
- `.github/CODEOWNERS`, `.github/PULL_REQUEST_TEMPLATE.md`, `CONTRIBUTING.md`.
- `hardware/mechanical/README.md`, `hardware/bom/BOM_TEMPLATE.csv`, `.gitattributes`, `.gitignore`.
- `docs/project_management/NCKH_MEMBER_WORKSPACE_FILETYPE_GUIDE_V3.md`.
- `docs/project_management/NCKH_TEAM_CHANGE_UPDATE_WORKFLOW_V1.md`, `docs/project_management/PROJECT_UPDATE_TEMPLATE.md`.
- `docs/project_management/NCKH_TWO_STAGE_CODEX_SKILLS_WORKFLOW_HANDBOOK_V1.md` (nội dung V1.1 trên `dev` sau PR #19, không mặc định mọi finding được giải quyết).
- `docs/project_management/CODEX_PROJECT_CONTEXT_V1.md`, `CODEX_ENGINEERING_STANDARD_V1.md`, `CODEX_RESEARCH_STANDARD_V1.md`, `CODEX_AUTOMATION_PLAYBOOK_V1.md` khi phù hợp.
- `docs/plans/weekXX/README.md` của tuần đang làm; **không dùng kế hoạch cũ làm tiêu chuẩn hiện hành nếu đã có thay đổi được phê duyệt**.

### 14.2. Tài liệu hãng Onshape dùng để xác minh giới hạn

- **Versions and history:** <https://cad.onshape.com/help/Content/Document/versions_and_history.htm> — Version bất biến, Workspace có thể tiếp tục sửa.
- **Export 3D:** <https://cad.onshape.com/help/Content/File/exporting_files.htm> — STEP/STL và các tùy chọn export/units.
- **Export drawings:** <https://cad.onshape.com/help/Content/Drawing/exporting_a_drawing.htm> — PDF/DXF/DWG cùng options.
- **Simulation (Assembly):** <https://cad.onshape.com/help/Content/Assembly/simulation.htm> — Linear Static/Modal trong giới hạn tính năng.
- **Modal limitations:** <https://cad.onshape.com/help/Content/Assembly/modal_simulation.htm> — natural frequencies, không mô phỏng forced dynamic loads/damping đầy đủ.
- **Plan/account and privacy:** <https://cad.onshape.com/help/Content/Plans/managing_your_onshape_account_and_subscription.htm> và <https://www.onshape.com/en/pricing> — Free public; cần kiểm plan thực tế.

**Chỉ các đường dẫn trên là tài liệu tham khảo; không chứng minh tính năng đã bật trong tài khoản của ME1.**

### 14.3. Lời nhắc bắt buộc cho Codex

> Hãy tối thiểu hóa thay đổi, bám đúng repository và yêu cầu ME1, không tự sáng tạo workflow trái `AGENTS.md`. Chỉ nói `PASS` khi có kiểm chứng thực. Không tự truy cập Onshape, không sửa xóa SolidWorks legacy, không đổi locked hardware, experiment Ground Truth hay dataset approval. **Giai đoạn 1 luôn dừng để ME1 duyệt.**

---

## PHỤ LỤC A — MẪU `test-report.md` (MINH HỌA CẤU TRÚC; KHÔNG PHẢI KẾT QUẢ THỰC)

> Khi thực hiện Stage 2 nếu ME1 duyệt, dùng mục này tạo `reports/TEST_REPORT_TEMPLATE.md`; mỗi báo cáo release phải copy mẫu và điền dữ liệu thật. Không lưu dấu `<...>` như thông tin đã được xác minh.

```markdown
# ME1 Mechanical Verification Report — <REPORT_ID>

- Component/Assembly: <COMPONENT_ID> / <NAME>
- Engineering revision / release: <R##> / <RELEASE_ID>
- Fixture/Setup version: <FIXTURE_VERSION> / <SETUP_VERSION>
- State: DRAFT | UNDER_REVIEW | APPROVED | RELEASED | BLOCKED
- Operator / Reviewer / Date: <...>
- CAD TEST (nếu có): <DOCUMENT_VERSION_URL>
- MAIN Onshape Document ID / Version ID / URL: <...>
- Onshape Element ID: <...>
- Git script/config SHA (nếu có): <...>
- Files: STEP <...>; Drawing PDF <...>; DXF <...>; BOM <...>; Manifest <...>
- Unit / datum / global frame / ADXL345 local frame: <...>
- Material/spec: <... hoặc TBD>
- Load cases / RPM / constraints / acceptance reference: <... hoặc TBD>

## Applicability and evidence
| Check | Required/Conditional/N/A | Criterion/source | Method | Status | Evidence | Owner |
|---|---|---|---|---|---|---|
| Geometry/dimensions | <...> | <...> | <...> | NOT_RUN | <...> | ME1 |
| Assembly/mates/clearance | <...> | <...> | <...> | NOT_RUN | <...> | ME1 |
| Material/manufacturability | <...> | <...> | <...> | NOT_RUN | <...> | ME1 |
| Linear Static | <...> | <...> | <...> | NOT_RUN | <...> | ME1 |
| Modal / natural frequencies | <...> | <...> | <...> | NOT_RUN | <...> | ME1 |
| Forced vibration / dynamics | <...> | <...> | <...> | NOT_RUN | <...> | ME1 |
| Fatigue | <...> | <...> | <...> | NOT_RUN | <...> | ME1 |
| ADXL345 mounting repeatability | <...> | <...> | <...> | NOT_RUN | <...> | ME1/MS2 |
| Motor guarding/physical safety | <...> | <...> | <...> | NOT_RUN | <...> | ME1 |
| Physical fixture acceptance | <...> | <...> | <...> | NOT_RUN | <...> | ME1 |

## Simulation settings and limitations
- Solver/license/version: <...>
- Geometry/material/mates/mesh/convergence: <...>
- Boundary/load cases: <...>
- Static results: <NOT_RUN hoặc bằng chứng thật>
- Modal frequencies/mode shapes: <NOT_RUN hoặc bằng chứng thật>
- Forced-response/damping limitation: <...>

## Physical test evidence
- Board/motor/fixture/setup/record IDs: <... hoặc NOT_RUN>
- Repetitions/environment/operator/log paths: <... hoặc NOT_RUN>

## Findings and decisions
- FAIL/BLOCKED/NOT_RUN/N/A & reasons: <...>
- Risk/next steps: <...>
- Reviewer decision + approval reference: PENDING
- Registry/manifest/drawing/BOM backlinks: <...>
- Status: PENDING — NOT A RELEASE APPROVAL
```

## PHỤ LỤC B — MẪU HỒ SƠ `cad_registry.md` (MINH HỌA; KHÔNG PHẢI ONshape ID THẬT)

```markdown
# ME1 CAD Registry (Onshape-first)

> IMPORTANT: No workspace link is considered an immutable release.
> Owner: ME1. New CAD: Onshape only. Old `solidworks/`: LEGACY/READ-ONLY.

## Controlled documents
| Purpose | Owner | Plan/privacy verified | Document ID | Version URL | Access checked |
|---|---|---|---|---|---|
| CAD TEST | ME1 | PENDING | PENDING | PENDING | PENDING |
| ME1_MAIN_CAD | ME1 | PENDING | PENDING | PENDING | PENDING |

## Controlled components/releases
| Component ID | Revision | Release ID | Main Version ID + URL | Fixture/Setup | Export/Report/BOM/Manifest | Status |
|---|---|---|---|---|---|---|
| <real-id> | <real-rev> | <real-release> | <immutable-version-link> | <...> | <repo-paths> | PROPOSED |

## Status/decision rules
- Never mark RELEASED without approved physical/safety and release gate evidence.
- Preserve legacy and superseded revisions for traceability.
```

**KẾT LUẬN/ĐIỂM DỪNG:** ME1 lựa chọn **Onshape-only cho thiết kế mới**; `CAD TEST` và `ME1_MAIN_CAD` là hai môi trường cần quản trị chặt, nhưng repo chỉ được cập nhật theo **hai giai đoạn**. Codex **phải xuất phân tích Giai đoạn 1 và dừng**, không tự sửa Github/Onshape cho tới khi ME1 phê duyệt. Một CAD Version, test report hoặc Git PR tự nó không phải bằng chứng an toàn/fixture acceptance.
