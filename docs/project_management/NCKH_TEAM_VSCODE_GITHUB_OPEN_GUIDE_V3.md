# NCKH — HƯỚNG DẪN MỞ GITHUB REPOSITORY V3 TRÊN VS CODE
## Dành cho ME1 / MS2 / EE2 / ET1 / IT2

> **Phạm vi của file này:** chỉ hướng dẫn thành viên mở repository chung của nhóm trên VS Code và lấy đúng source code từ GitHub.
>
> **Chưa bao gồm:** cách nộp bài bằng Pull Request, Codex Agent Review, cách tạo Issue, cách merge. Các phần đó sẽ có tài liệu riêng.

---

# 1. REPOSITORY CHUNG CỦA NHÓM

Repository chính thức:

```text
https://github.com/nkaq/SVNCKH--HUST.git
```

Tên repository:

```text
SVNCKH--HUST
```

Tất cả thành viên phải làm việc trên repository này.

Không dùng lại repository cũ hoặc folder ZIP cũ làm nguồn chính.

---

# 2. TRƯỚC KHI LÀM — CHỈ CẦN KIỂM TRA 3 THỨ

Mỗi thành viên cần:

```text
1. Đã Accept lời mời Collaborator trên GitHub
2. Đã cài Git
3. Đã cài Visual Studio Code
```

Mở VS Code → `Terminal` → `New Terminal`.

Kiểm tra Git:

```bash
git --version
```

Nếu terminal hiện:

```text
git version 2.x.x
```

thì Git đã hoạt động.

---

# 3. CẤU HÌNH TÊN GIT — CHỈ LÀM LẦN ĐẦU TRÊN MÁY

Trong Terminal của VS Code:

```bash
git config --global user.name "TEN_CUA_BAN"
```

Ví dụ:

```bash
git config --global user.name "Nguyen Van A"
```

Sau đó:

```bash
git config --global user.email "EMAIL_GITHUB_CUA_BAN"
```

Ví dụ:

```bash
git config --global user.email "example@gmail.com"
```

Kiểm tra:

```bash
git config --global user.name
git config --global user.email
```

---

# 4. CLONE REPOSITORY VỀ MÁY — CHỈ LÀM LẦN ĐẦU

## Cách khuyến nghị: dùng Terminal trong VS Code

Chọn một thư mục để chứa project.

Ví dụ trên Windows:

```powershell
cd D:\
mkdir NCKH
cd NCKH
```

Clone repository:

```bash
git clone https://github.com/nkaq/SVNCKH--HUST.git
```

Sau khi clone xong:

```bash
cd SVNCKH--HUST
```

Mở repository bằng VS Code:

```bash
code .
```

Nếu `code .` không hoạt động thì:

```text
VS Code
→ File
→ Open Folder
→ chọn folder SVNCKH--HUST
```

---

# 5. KIỂM TRA ĐÃ MỞ ĐÚNG REPOSITORY CHƯA

Trong terminal:

```bash
git status
```

Sau đó:

```bash
git remote -v
```

Kết quả đúng phải chứa:

```text
origin  https://github.com/nkaq/SVNCKH--HUST.git (fetch)
origin  https://github.com/nkaq/SVNCKH--HUST.git (push)
```

Kiểm tra root repository:

```bash
git rev-parse --show-toplevel
```

Kết quả phải trỏ tới folder:

```text
...\SVNCKH--HUST
```

Không được là folder cha kiểu:

```text
...\NCKH
```

hoặc:

```text
...\SVNCKH--HUST\SVNCKH--HUST
```

---

# 6. CHUYỂN SANG BRANCH `dev`

Sau khi clone, thành viên phải chuyển sang:

```text
dev
```

Chạy:

```bash
git switch dev
```

Sau đó lấy bản mới nhất:

```bash
git pull origin dev
```

Kiểm tra branch:

```bash
git branch
```

Kết quả đúng:

```text
  main
* dev
```

Dấu `*` phải nằm trước:

```text
dev
```

---

# 7. LỆNH CHUẨN MỖI KHI MỞ PROJECT LẠI

Sau lần clone đầu tiên, **không clone lại**.

Mỗi buổi làm việc chỉ cần mở folder:

```text
SVNCKH--HUST
```

bằng VS Code.

Sau đó chạy:

```bash
git switch dev
git pull origin dev
```

Đây là 2 lệnh quan trọng nhất trước khi bắt đầu công việc.

---

# 8. NẾU ĐANG Ở FEATURE BRANCH CŨ

Kiểm tra:

```bash
git branch
```

Nếu thấy ví dụ:

```text
* feature/ee2-adxl345-acquisition
  dev
  main
```

và muốn quay lại bản tích hợp mới nhất:

```bash
git switch dev
```

Sau đó:

```bash
git pull origin dev
```

---

# 9. KHÔNG ĐƯỢC LÀM

Không chạy:

```bash
git init
```

sau khi đã clone repo.

Không clone repository vào bên trong chính repository:

```text
SAI:
SVNCKH--HUST/
    SVNCKH--HUST/
```

Không làm việc trực tiếp trên:

```text
main
```

Không chạy:

```bash
git push --force
```

Không tự xóa:

```text
main
dev
```

Không gửi folder project qua Messenger rồi dùng folder đó thay GitHub repository.

---

# 10. NẾU GIT YÊU CẦU ĐĂNG NHẬP GITHUB

Khi `clone`, `pull` hoặc `push`, GitHub có thể yêu cầu xác thực.

Đăng nhập bằng **đúng GitHub account đã được ME1 mời làm Collaborator**.

Nếu trình duyệt mở cửa sổ GitHub:

```text
Sign in
→ Authorize
→ quay lại VS Code
```

Không nhập mật khẩu GitHub trực tiếp vào terminal nếu Git yêu cầu password cho HTTPS.

---

# 11. CÁCH KIỂM TRA MỌI THỨ ĐÃ SẴN SÀNG

Chạy lần lượt:

```bash
git --version
```

```bash
git status
```

```bash
git remote -v
```

```bash
git branch
```

```bash
git pull origin dev
```

Nếu không có lỗi và branch hiện tại là:

```text
dev
```

thì máy đã sẵn sàng làm việc với repository của nhóm.

---

# 12. QUICK START — COPY NGUYÊN CỤM NÀY

## Lần đầu trên máy

```powershell
cd D:\
mkdir NCKH
cd NCKH

git clone https://github.com/nkaq/SVNCKH--HUST.git

cd SVNCKH--HUST

code .

git switch dev

git pull origin dev
```

## Những lần sau

```bash
git switch dev
git pull origin dev
```

---

# 13. FLOW CẦN NHỚ

```text
GitHub repository
      ↓
git clone (chỉ lần đầu)
      ↓
VS Code
      ↓
git switch dev
      ↓
git pull origin dev
      ↓
Sẵn sàng nhận task
```

---

# 14. NẾU CÓ LỖI

Khi lỗi, **không tự xóa `.git` hoặc chạy lại `git init`**.

Gửi cho ME1:

```text
1. Ảnh terminal
2. Kết quả git status
3. Kết quả git branch
4. Kết quả git remote -v
```

Các lệnh:

```bash
git status
git branch
git remote -v
```

Từ đó mới xử lý lỗi.

---

# 15. TÓM TẮT CHO TOÀN TEAM

### Lần đầu

```bash
git clone https://github.com/nkaq/SVNCKH--HUST.git
cd SVNCKH--HUST
code .
git switch dev
git pull origin dev
```

### Mỗi lần mở lại

```bash
git switch dev
git pull origin dev
```

**Chỉ cần hoàn thành bước này trước.**

Tài liệu tiếp theo của nhóm sẽ hướng dẫn riêng:

```text
1. Cách nhận task và tạo feature branch
2. Cách commit/push/nộp bài bằng Pull Request
3. Cách dùng Codex Agent Review
```
