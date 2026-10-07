# Hướng dẫn đưa Starter Repo này lên GitHub

Repo GitHub hiện tại của nhóm đã có commit/README, nên làm theo quy trình an toàn dưới đây.

## 1. Giải nén ZIP

Giải nén `SVNCKH--HUST_Starter_Repo.zip`.

Mở folder `SVNCKH--HUST` bằng VS Code.

## 2. Mở Terminal trong VS Code

Kiểm tra Git:

```bash
git --version
```

Cấu hình danh tính nếu chưa có:

```bash
git config --global user.name "Your Name"
git config --global user.email "your_github_email@example.com"
```

## 3. Khởi tạo Git trong folder

```bash
git init
git branch -M main
git remote add origin https://github.com/nkaq/SVNCKH--HUST.git
```

Kiểm tra:

```bash
git remote -v
```

## 4. Lấy lịch sử remote hiện tại

```bash
git fetch origin
git pull origin main --allow-unrelated-histories
```

Nếu conflict ở `README.md`, merge thủ công và giữ README starter này làm nội dung chính.

## 5. Commit toàn bộ starter repo

```bash
git add .
git commit -m "chore: initialize NCKH repository structure"
git push -u origin main
```

## 6. Tạo branch dev

```bash
git switch -c dev
git push -u origin dev
```

## 7. Cài Git LFS

```bash
git lfs install
```

File `.gitattributes` đã có sẵn cấu hình cho SolidWorks/PPTX.

## 8. Workflow về sau

```bash
git switch dev
git pull origin dev

git switch -c feature/me1-fixture-v01
```

Sau khi làm xong:

```bash
git add .
git commit -m "feat(mechanical): add fixture v0.1"
git push -u origin feature/me1-fixture-v01
```

Mở Pull Request:

```text
feature/... → dev
```

> Không dùng `git push --force` lên `main` hoặc `dev`.
