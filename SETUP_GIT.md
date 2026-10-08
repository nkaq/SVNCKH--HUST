# Setup Git — clean repository v3

Khuyến nghị: tạo GitHub repository **trống** tên `SVNCKH--HUST`.
Không chọn “Add README”, `.gitignore`, hoặc license trên GitHub vì repo local đã có sẵn.

## 1. Giải nén

Mở đúng folder:

```text
SVNCKH--HUST_v3/
```

trong VS Code.

## 2. Init

```bash
git init
git branch -M main
git remote add origin https://github.com/<OWNER>/SVNCKH--HUST.git
```

## 3. Git LFS

```bash
git lfs install
```

`.gitattributes` đã chứa rule cho SolidWorks/PPTX.

## 4. Initial commit

```bash
git add .
git status
git commit -m "chore: initialize NCKH repository v3"
git push -u origin main
```

## 5. Create dev

```bash
git switch -c dev
git push -u origin dev
```

## 6. Thành viên bắt đầu làm

```bash
git switch dev
git pull origin dev
git switch -c feature/<member>-<task>
```

Không dùng `git push --force` lên `main` hoặc `dev`.

## 7. Sau khi repository ổn

Thiết lập:
- collaborators;
- branch protection/ruleset;
- Pull Request review;
- Codex Code Review;
- CODEOWNERS usernames.

Chi tiết nằm trong `docs/project_management/`.
