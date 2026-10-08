from pathlib import Path
import json, sys, re

root = Path(__file__).resolve().parents[2]
errors = []
warnings = []

required = [
    "README.md", "AGENTS.md", "CONTRIBUTING.md",
    ".github/PULL_REQUEST_TEMPLATE.md",
    "docs/project_management/PULL_REQUEST_GUIDE.md",
]
for rel in required:
    if not (root/rel).exists():
        errors.append(f"Missing required file: {rel}")

for p in root.rglob("*"):
    if not p.is_file() or ".git" in p.parts:
        continue
    rel = p.relative_to(root).as_posix()
    if rel.startswith("data/raw/") or rel.startswith("data/private/"):
        errors.append(f"Tracked raw/private data is forbidden: {rel}")
    if p.stat().st_size > 50 * 1024 * 1024:
        errors.append(f"File > 50 MB: {rel}")
    low = p.name.lower()
    if re.search(r"(final[_ -]?final|final2|final_real_last|copy of|untitled)", low):
        warnings.append(f"Poor versioned filename: {rel}")

schema = root/"experiments/metadata/metadata_schema_example.json"
if schema.exists():
    try:
        json.loads(schema.read_text(encoding="utf-8"))
    except Exception as e:
        errors.append(f"Invalid metadata JSON: {e}")

for w in warnings:
    print("WARNING:", w)
for e in errors:
    print("ERROR:", e)

if errors:
    sys.exit(1)
print("Repository quality checks passed.")
