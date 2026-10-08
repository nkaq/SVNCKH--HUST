# Repository Structure

Tổ chức theo subsystem, không theo tên người.

```text
hardware/       → cơ khí + điện tử
firmware/       → ESP32-S3
software/       → dashboard/logger
ml/             → DSP/ML/TinyML
experiments/    → protocol + validation + fault
data/           → sample + manifests
simulations/    → MATLAB/ANSYS/SolidWorks Motion
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
