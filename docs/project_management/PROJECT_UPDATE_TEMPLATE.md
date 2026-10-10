# NCKH — PROJECT UPDATE TEMPLATE V1

> Dùng khi thay đổi mức B/C theo [TEAM CHANGE UPDATE WORKFLOW V1](NCKH_TEAM_CHANGE_UPDATE_WORKFLOW_V1.md). Điền dữ liệu thật, để `PENDING` nếu chưa duyệt. Không ghi `ACTIVE` và không tuyên bố PASS nếu chưa có chứng cứ. Giữ hồ sơ tại PR/issue/tài liệu được nhóm chỉ định; chỉ gửi thông báo sau phê duyệt thích hợp.

## Mẫu thông báo (copy để điền)

```text
[PROJECT UPDATE]
Update ID: UPD-WXX-NNN (ME1 cấp, duy nhất)
Status: PROPOSED | APPROVED | ACTIVE | SUPERSEDED | CANCELLED
Impact level: B | C
Week / Task source: docs/plans/weekXX/README.md | NOT APPLICABLE + lý do
Owner / Contact: @github-user
Decision / Human Gate reference: PR/comment/minutes link | PENDING
Merged PR / Commit: https://github.com/nkaq/SVNCKH--HUST/pull/<number> | PENDING
Source document(s) and version(s): path + version

What changed (OLD -> NEW):
Why / expected impact:
Affected roles: ME1 | MS2 | EE2 | ET1 | IT2 (chọn đúng người)
Interface / protocol / artifact affected:
New deliverable / dependency:
Compatibility / migration / tests required:
Action required per member:
  - @username: <action> / <test> / <deadline>
Effective from: YYYY-MM-DD HH:MM UTC+07:00 | condition after approval
Acknowledgement by: YYYY-MM-DD HH:MM UTC+07:00
ACK location: nhóm chat | PR/issue link
ACK status: PENDING (ghi từng người nhận, READY/BLOCKED + lý do)
Blocker / rollback / escalation contact:
Supersedes: Update ID cũ (nếu có, kèm PR/decision ref) | NONE
```

## Quy tắc sử dụng

1. **Đề xuất khác với quyết định đã duyệt.** Chỉ `APPROVED` khi có approval reference và scope hợp lệ; `ACTIVE` phải thỏa điều kiện hiệu lực nêu rõ.
2. **Update ID không thay thế task ID hay recording/dataset version.** Những định danh này phải giữ riêng.
3. **Mọi thay đổi phải dẫn nguồn:** PR merged, commit, tài liệu gốc trên `dev`, version và người phê duyệt khi cần. Không công bố quy định chỉ dựa vào tin nhắn chat.
4. **Không có quy tắc 'update mới nhất tự ghi đè update cũ'.** Nếu thay thế, phải chỉ rõ `Supersedes`, phạm vi, quyền phê duyệt và effective date. Không hồi tố sửa dữ liệu, approval hoặc Ground Truth.
5. **ACK là xác nhận đã đọc và khả năng thực thi, không phải approval khoa học/an toàn.** Nếu chưa rõ hoặc không thể áp dụng, người nhận phản hồi `BLOCKED`.
6. **Không nhắc toàn nhóm mọi PR nhỏ.** Mức B/C mới yêu cầu thông báo riêng; mức A xem trực tiếp lịch sử PR/CHANGELOG.

## Mẫu tin nhắn ngắn gửi nhóm (sau khi PR đã merge và được phê duyệt)

```text
[PROJECT UPDATE] <Update ID> — <1 câu nêu thay đổi>
PR + tài liệu nguồn: <link PR> | <link dev/path + version>
Áp dụng từ: <thời điểm/điều kiện>
Ảnh hưởng: <tag thành viên>
Phải làm: <hành động/test cụ thể, hạn hoàn thành>
Xác nhận: nhắn 'ĐÃ ĐỌC <Update ID> — READY' hoặc 'BLOCKED: <lý do>' trước <deadline>.
Người giải đáp: <ME1 / domain owner>
```
