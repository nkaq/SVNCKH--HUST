# Git Workflow

## Clone

```bash
git clone <REPO_URL>
cd SVNCKH--HUST
```

## Bắt đầu task

```bash
git switch dev
git pull origin dev
git switch -c feature/ee2-acquisition-v2
```

## Commit

```bash
git status
git add .
git commit -m "feat(firmware): add acquisition firmware v2"
```

## Push

```bash
git push -u origin feature/ee2-acquisition-v2
```

## Pull Request

```text
feature/... → dev
```

Sau integration test:

```text
dev → main
```

## Cập nhật feature branch

```bash
git switch dev
git pull origin dev

git switch feature/ee2-acquisition-v2
git merge dev
```

## Xem lịch sử

```bash
git log --oneline --graph --decorate --all
```

## Quy tắc
Không dùng `git push --force` trên `main` hoặc `dev`.
