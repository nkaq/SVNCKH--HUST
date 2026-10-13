# Repository Structure

Tổ chức theo subsystem, không theo tên người.

```text
hardware/       → cơ khí Onshape (CAD TEST/MAIN Version + STEP/Drawing/BOM/report) + điện tử
firmware/       → ESP32-S3
software/       → dashboard/logger
ml/             → DSP/ML/TinyML
experiments/    → protocol + validation + fault
data/           → sample + manifests
simulations/    → MATLAB/ANSYS (existing SolidWorks Motion folder remains legacy)
results/        → figures/tables/benchmarks
assets/         → images/diagrams/posters
presentations/  → slides/PDF/posters
docs/           → architecture/plans/reports
members/        → note/báo cáo cá nhân
```

## Primary ownership

| Folder | Owner |
|---|---|
| `hardware/mechanical/` | ME1 |
| `experiments/sensor_characterization/` | MS2 |
| `firmware/esp32s3/` | EE2 |
| `hardware/electronics/` | ET1 |
| `ml/`, `software/`, `data/` | IT2 |

Ownership không đồng nghĩa độc quyền chỉnh sửa; thay đổi liên subsystem cần Pull Request.

## ME1 mechanical source and legacy policy

- **New CAD native source:** Onshape MAIN immutable Version; experimental work stays in a separate CAD TEST Document.
- **GitHub index:** `hardware/mechanical/onshape/cad_registry.md`, `onshape/release_manifest_template.md`, `reports/TEST_REPORT_TEMPLATE.md` and `hardware/bom/MECHANICAL_BOM_TEMPLATE.csv`.
- **Existing and reusable:** `hardware/mechanical/step/`, `dxf/`, `fabrication/`.
- **Create only when used:** `cad-source/scripts/`, `cad-source/params/`, `drawings/`, `stl/`, `previews/`, report/release subfolders.
- **Legacy, preserve history:** `hardware/mechanical/solidworks/`, `simulations/solidworks_motion/` and past week plans. Do not write new designs into legacy folders.
- No changes to firmware, electronics, experiment Ground Truth or datasets arise from this directory policy alone. Formal Project Update governs cross-team adoption.
