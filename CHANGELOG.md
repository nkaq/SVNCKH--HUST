# Changelog

## [Unreleased]

### ME1 CAD workflow proposal — 2026-10-13 (handbook V1.2)
- **PROPOSED / NOT ACTIVE:** Onshape is ME1's chosen native CAD tool for new designs; introduces CAD TEST/ME1_MAIN_CAD separation, fixed-Version registry, mechanical release/verification/BOM templates and traceability documentation. SolidWorks legacy remains read-only. **Week 3's original task wording is preserved as a historical quotation**; the updated Week 3 README and submission guide propose Onshape-only deliverables after formal Project Update/activation, not retroactive substitution.
- Scope: `hardware/mechanical/`, ME1 portions of active workflow guides, and proposed Project Update `docs/project_management/NCKH_ME1_ONSHAPE_PROJECT_UPDATE_PROPOSAL_V1.md` (ME1 owner; review/announcement/ACK pending).
- No fabricated CAD, fabrication release, solver outcome, hardware or physical Ground Truth change; effective date and Onshape account/privacy decisions remain PENDING.
- **Approved expansion (PR #20):** 16 original files + Week 3 README, Team Submission Workflow V3 and `.gitattributes` = **19 files**. The `.gitattributes` neutral CAD `-text -eol` rule passed a synthetic Git byte/hash round-trip test; real Onshape exports, team Windows checks and Git LFS/quota remain `NOT_RUN/PENDING`.


### Added
- Initial monorepo structure
- Git workflow
- GitHub templates
- Subsystem READMEs
- Data and experiment policy
- Team change notification and acknowledgement workflow (`docs/project_management/NCKH_TEAM_CHANGE_UPDATE_WORKFLOW_V1.md`)
- **PROPOSED (not ACTIVE):** Two-stage Codex Skills & AI Code Review handbook V1.1 (`docs/project_management/NCKH_TWO_STAGE_CODEX_SKILLS_WORKFLOW_HANDBOOK_V1.md`) and level-B Project Update proposal (`docs/project_management/NCKH_TWO_STAGE_CODEX_WORKFLOW_PROJECT_UPDATE_PROPOSAL_V1.md`). Requires independent human approval, ME1 announcement and team ACK before activation.

### Changed
- Expanded `PROJECT_UPDATE_TEMPLATE.md` to require source PR, affected members, explicit effective date, approval references, action items and acknowledgement tracking.

### Current
- Week 3 — Prototype Integration & Mechanical Testbench Build

## Suggested releases
- `v0.1-prototype`
- `v0.2-testbench`
- `v0.3-dataset`
- `v0.4-tinyml`
- `v1.0-final`

<!-- PR #20 includes the ME1-approved Week 3 task-source amendment and Team Submission Workflow V3 Onshape proposal. The original Week 3 SolidWorks wording is preserved in historical context. PR merge alone does not activate the workflow: effective date, notification, ACK and remaining scope-specific gates still require ME1 decisions. -->
