# Simulations

- MATLAB
- ANSYS
- SolidWorks Motion (**LEGACY/READ-ONLY** — historical work only; not the CAD tool for new ME1 designs)
- Other

Ưu tiên commit:
- input/setup
- scripts
- screenshots
- plots
- report PDF

Không commit cache/output quá lớn nếu có thể tái tạo.

## Current ME1 CAD analysis policy

ME1 creates **new** parametric models in Onshape CAD TEST/ME1_MAIN_CAD. Where the account and actual analysis type allow, use Onshape Simulation for documented Linear Static or Modal checks; do not assume forced response, fatigue or CFD exists in that tool. Alternative solver or bench evidence must be separately documented if required. A completed simulation is not physical acceptance or safety approval. Reuse `hardware/mechanical/reports/TEST_REPORT_TEMPLATE.md` for evidence/limitations and preserve all legacy simulation outputs.
