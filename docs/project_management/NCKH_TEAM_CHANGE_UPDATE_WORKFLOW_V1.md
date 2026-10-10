# NCKH — TEAM CHANGE UPDATE WORKFLOW V1

> **Áp dụng cho:** ME1, MS2, EE2, ET1, IT2. **Nguồn làm việc chung:** branch `dev` của repository `nkaq/SVNCKH--HUST`.  
> **Mục đích:** mọi thay đổi ảnh hưởng nhóm phải được phê duyệt, công bố, truy vết và xác nhận đã đọc trước khi áp dụng. Đây là hướng dẫn truyền đạt thay đổi, **không thay thế** nhiệm vụ tuần, contract kỹ thuật, protocol, `AGENTS.md` hay các Human Gate.

## 1. Đọc tài liệu nào là chính?

| Khi cần biết | Nguồn chính thức |
|---|---|
| Task tuần, owner, deadline, deliverables, acceptance | `docs/plans/weekXX/README.md` trên `dev` của tuần đang áp dụng |
| Hướng dẫn nộp task / review PR | `docs/project_management/NCKH_TEAM_SUBMISSION_WORKFLOW_V3.md`, `CONTRIBUTING.md` |
| Chuẩn/giới hạn Codex | Root/scoped `AGENTS.md` và standards được dẫn chiếu |
| Interface, fixture, sensor, ground-truth protocol, metadata | Contract/protocol có **version và Human Gate phê duyệt** trong subsystem tương ứng |
| Thay đổi đã tích hợp | Merge PR trên `dev` và `CHANGELOG.md` |
| Ai bị ảnh hưởng, việc cần thực hiện, ngày hiệu lực | Thông báo theo `PROJECT_UPDATE_TEMPLATE.md`, dẫn tới PR và tài liệu nguồn |
| Bản milestone phát hành | `main` sau PR `dev → main` và kiểm thử tích hợp |

**Nguyên tắc:** Project Update là thông báo, **không tự sửa hoặc ghi đè contract/README/approval**. Khi hai nguồn xung đột, dừng phần việc bị ảnh hưởng, dẫn cả hai nguồn và hỏi ME1 cùng subsystem owner. Một thông báo mới hơn không tự động vô hiệu Human Gate hoặc quy định an toàn.

## 2. Khi nào ME1 phải gửi Project Update?

**Mức A — Thay đổi cục bộ:** typo, mô tả, refactor không đổi hành vi/contract, không đổi nhiệm vụ hoặc phần phụ thuộc. Vẫn cần PR và review; ghi changelog nếu đáng chú ý, **không cần thông báo riêng tới toàn nhóm**.

**Mức B — Thay đổi ảnh hưởng thành viên khác:** đổi task/deadline/deliverable, vị trí file, quy trình Git/Codex, owner, phụ thuộc liên phân hệ, format/parser, packet/units/timestamp, pinout, firmware interface, version setup, QC, artifact handoff. **Bắt buộc** Project Update và tag người bị ảnh hưởng.

**Mức C — Thay đổi nhạy cảm/đã khóa:** phần cứng ESP32-S3/ADXL345 và các module đã chốt; an toàn cơ/điện; experiment protocol/physical Ground Truth; fixture acceptance; dataset inclusion/exclusion; kiến trúc; scientific claims. **Bắt buộc** owner chuyên môn + ME1 xem xét, có hồ sơ Human Gate; chỉ thông báo sau khi đã phê duyệt. Không được tự suy ra PASS từ CI hoặc Codex.

Nếu thay đổi có nhiều mức, dùng mức cao nhất. PR ghi rõ nhóm nào bị ảnh hưởng và version/interface nào phải cập nhật.

## 3. Quy trình bắt buộc: đề xuất → review → công bố → đồng bộ

1. **Đề xuất:** người thực hiện mô tả hiện trạng, thay đổi, lý do, ảnh hưởng, cách kiểm chứng, phương án tương thích/chuyển đổi và Human Gate. Không sửa trực tiếp `dev/main`.
2. **Sửa ở feature branch:** cập nhật chính **tài liệu nguồn** (README tuần, contract, protocol hoặc workflow), code/artifact có liên quan, bằng chứng; không chỉ thay `PROJECT UPDATE`. Mọi thay đổi vào `dev` qua PR.
3. **Review:** `repo-quality` PASS; Codex review commit mới nhất xong và các P1/P2 có căn cứ đã xử lý; nếu Codex không khả dụng thì ghi lý do + human review độc lập. Yêu cầu owner liên quan/human gate hoàn tất **trước merge**. Không coi `Completed` là đồng nghĩa `PASS`.
4. **Merge:** người được giao quyền squash merge `feature → dev` sau review; lưu URL PR/commit và version. PR `dev → main` chỉ cho milestone đã nghiệm thu, không dùng để giao task hằng ngày.
5. **Ghi lịch sử:** cập nhật `CHANGELOG.md` hoặc trạng thái task tuần bằng thay đổi truy vết được. Với thông báo mức B/C, chuẩn bị bản ghi theo `docs/project_management/PROJECT_UPDATE_TEMPLATE.md` có PR nguồn, nội dung, owner, người bị ảnh hưởng, hành động, hạn xác nhận, thời điểm hiệu lực.
6. **Công bố:** ME1 hoặc người được ME1 chỉ định gửi thông báo vào kênh chung của nhóm (Messenger/Zalo/GitHub tùy kênh nhóm đang dùng). Tag đúng ME1/MS2/EE2/ET1/IT2 bị ảnh hưởng và dẫn link `dev` tới tài liệu gốc + PR. Gửi tin nhắn **không tự động chỉ nhờ Git merge**.
7. **Xác nhận:** mỗi người liên quan trả lời `ĐÃ ĐỌC <Update ID>` và báo `READY` hoặc `BLOCKED + lý do` trong kênh thông báo hoặc issue/PR được chỉ định. Người công bố ghi lại ai đã xác nhận. Nếu chưa đủ xác nhận, không coi việc chuyển đổi liên phân hệ đã hoàn tất.
8. **Áp dụng:** người nhận lấy `dev` mới, xem diff/change log, điều chỉnh feature branch theo hướng dẫn tương thích và kiểm thử phần mình. Thay đổi chỉ có hiệu lực theo quyết định phê duyệt, thời điểm đã công bố và các điều kiện tương thích/Human Gate; **không đổi retroactive dữ liệu đã thu**.

**Thời điểm hiệu lực:** phải ghi chính xác `YYYY-MM-DD HH:MM UTC+07:00` hoặc một mốc/điều kiện rõ ràng (`sau PR #... merge và owner xác nhận`). Không đánh dấu `ACTIVE` khi điều kiện chưa hoàn tất. Với thay đổi có ảnh hưởng an toàn hoặc giao diện đang dùng chung, ME1 có thể yêu cầu toàn bộ owner liên quan xác nhận trước khi triển khai.

## 4. Thành viên cập nhật thế nào trong VS Code?

### 4.1. Chưa có công việc dở, muốn bắt đầu task mới

```powershell
git status --short --untracked-files=all
git switch dev
git pull --ff-only origin dev
git status
```

Đọc Project Update mới liên quan, `CHANGELOG.md`, rồi mở `docs/plans/weekXX/README.md` đúng tuần. Chỉ sau đó mới tạo branch task mới từ `dev`.

### 4.2. Đang làm trên feature branch có thay đổi chưa commit

- **Không** chạy `git switch`, `git pull`, `reset --hard`, `clean`, `rebase` hoặc `git add .` theo thói quen; trước hết xem `git status`, bảo vệ và bàn giao những thay đổi đang làm.
- Kiểm tra thông báo để biết interface/version mới có ảnh hưởng task đang làm không; nếu có, báo owner liên quan và thống nhất cách tích hợp.
- Khi working tree an toàn, cập nhật local `dev` rồi tích hợp vào feature branch bằng quy trình review được nhóm đồng ý. `git merge dev` có thể gây conflict; không tự giải quyết conflict về schema, pinout, Ground Truth hoặc dataset mà không hỏi owner.
- Chạy lại test thực tế phù hợp với phần thay đổi; báo `NOT RUN` nếu thiếu toolchain/board/evidence.

### 4.3. Khi gặp xung đột tài liệu hoặc cập nhật chưa rõ

Báo ngay `BLOCKED`, kèm path, version, PR, mô tả ảnh hưởng. Không tự chọn phiên bản thuận tiện. ME1/owner phải ghi quyết định chính thức trong PR/tài liệu nguồn trước khi thành viên làm tiếp phần bị chặn.

## 5. Phân công theo vai trò

| Vai trò | Trách nhiệm khi nhận update |
|---|---|
| **ME1 (`@nkaq`)** | Duyệt thay đổi liên phân hệ, nguồn GT/fixture/architecture; công bố mốc hiệu lực; theo dõi ACK |
| **MS2 (`@hquanvu12`)** | Rà ADXL345, calibration, sensor reliability, setup/metadata liên quan |
| **EE2 (`@dungnguyen13`)** | Rà firmware acquisition, sample rate, packet/timestamps, phiên bản code và test |
| **ET1 (`@tiendatvu13`)** | Rà wiring/pinout, power/interface, schematic/BOM và bench safety |
| **IT2 (`@lebach181207-dev`)** | Rà parser, units/schema, DSP/ML, dữ liệu, leakage, dashboard/model compatibility |

**Thông báo không thay quyền phê duyệt.** Thành viên xác nhận đã đọc không có nghĩa là đã phê duyệt đổi contract, dataset hoặc an toàn.

## 6. Quy tắc dữ liệu và nghiên cứu không được bị ghi đè bởi Project Update

- Cấu hình sensor khóa theo `AGENTS.md`: ESP32-S3, ADXL345, ACS724, PT100+MAX31865, US5881 Hall, DHT22, OLED. Nếu có đề xuất đổi, phải có phê duyệt/version trước khi coi là cấu hình hiện hành.
- Physical Ground Truth chỉ từ condition **được chủ động áp đặt và ghi chứng cứ**; không lấy từ FFT/RMS/model/dashboard.
- Engineering Validation Data **không được đổi nguồn gốc** để thành Dataset v0.1; việc xem xét dataset cần recording/protocol/version/QC/approval theo Research Standard đã phê duyệt.
- Không gửi qua thông báo các mật khẩu, API key, raw/private dataset, thông số đo chưa xác minh hoặc tuyên bố PASS khoa học chưa được ký duyệt.
- Trong thời gian còn P1/P2 về dataset inclusion/approval, không dùng hướng dẫn mới như lý do tự quyết định đưa vào/loại khỏi dataset.

## 7. Kiểm tra trước khi đánh dấu update hoàn tất

- [ ] PR nguồn merged vào `dev` sau CI/Codex/human review; Human Gate được ghi nhận khi cần
- [ ] `CHANGELOG.md`/README tuần/contract liên quan phản ánh quyết định thật
- [ ] Project Update có `Update ID`, PR/commit, mức ảnh hưởng, owner, version và action cụ thể
- [ ] Tất cả thành viên bị ảnh hưởng đã được tag, có deadline và cách ACK
- [ ] Người nhận đã xác nhận `READY` hoặc báo `BLOCKED`; owner xử lý blocker
- [ ] Tests và các bước migration tương ứng đã được thực hiện hoặc ghi `NOT RUN`
- [ ] Chỉ ghi `ACTIVE` khi điều kiện hiệu lực được thỏa; có hướng rollback/escalation

## 8. Ví dụ chỉ để minh họa (không phải thay đổi đã được phê duyệt)

**Giả sử EE2 đề xuất đổi packet timestamp format:** EE2 mở PR cập nhật firmware packet contract + parser notes; IT2 cùng kiểm schema/units; Codex và CI review; ME1/owner duyệt; PR merge vào `dev`; ME1 phát hành `[PROJECT UPDATE]` mức B, dẫn PR và yêu cầu IT2 ACK, cập nhật parser/test trước khi đổi firmware nguồn gửi. Nếu có dữ liệu cũ, phải xác định version/backward compatibility; không tự sửa recording cũ.

**Ví dụ scientific:** một recording có QC FAIL không tự chuyển trạng thái dataset `EXCLUDED` khi quy định phê duyệt hiện hành chưa được thỏa. Đây là điều kiện đang được Codex yêu cầu làm rõ, không phải phán quyết thay ME1.
