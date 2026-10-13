# Mechanical review instructions

Applies to `hardware/mechanical/`.

Review:
- fixture versioning;
- BOM/drawing consistency;
- sensor mounting repeatability;
- alignment references;
- adjustable motor mounting;
- cable clearance;
- fault-creation traceability;
- fixture acceptance documentation.

Do not claim CAD geometry is mechanically safe from text review alone. Require ME1 human CAD/fabrication review.

## Onshape-only policy for new ME1 CAD (proposed)

- For new CAD, use **Onshape** as parametric source, with a separate **CAD TEST** Document and **ME1_MAIN_CAD** Document. Existing `solidworks/` paths are **LEGACY/READ-ONLY**; do not delete, move or create new SolidWorks artifacts.
- Check actual **MAIN immutable Version ID and Version URL** rather than a mutable Workspace URL before evaluating any release/export. Distinguish Onshape Version ID from Part ID, engineering Revision, fixture/setup Version, Release ID and Git SHA.
- Cross-check `onshape/cad_registry.md` ↔ release manifest ↔ STEP/Drawing/BOM ↔ mechanical test report and actual evidence. Do not invent links, hashes, solver outputs, CAD geometry, suppliers or safety PASS.
- CAD TEST import/analysis and Onshape Simulation are not mechanical acceptance. Modal eigenmodes are not forced-vibration response. Require explicit loads, boundary conditions, material data, units, acceptance criteria, real tests and ME1 human CAD/safety review when applicable.
- Before exposing CAD in Onshape, verify account privacy/ACL; if research design needs private access but account cannot ensure it, report `BLOCKED — ACCOUNT/CONFIDENTIALITY`.
- Do not alter locked hardware, physical Ground Truth, fixture-fault protocol, post-freeze setup_version or dataset origin without a domain-owner/ME1 Human Gate. Do not auto-commit/push/merge merely because of a review prompt.
- Read `docs/project_management/NCKH_ME1_ONSHAPE_CAD_GITHUB_WORKFLOW_REQUIREMENTS_V2.md` and root `AGENTS.md` where relevant. This file itself does **not** declare a new mechanical release or project-wide ACTIVE date.
