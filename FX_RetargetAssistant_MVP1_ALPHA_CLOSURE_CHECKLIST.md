# FX_RetargetAssistant MVP1 Alpha Closure Checklist

Status: MVP1 Alpha / Guided Auto Setup + Batch Export flow working in UE5.4 / Closure in progress.

Do not begin UE5.5 / UE5.6 adaptation until this checklist is passed.

## Scope Boundary

- Retarget & Export remains pure export.
- Retarget & Export must not auto-create IK Rig / IK Retargeter.
- Retarget & Export must not auto-repair IK Mapping.
- Retarget & Export must not modify user-selected Retargeters.
- Auto Create / Recreate / Auto Repair may save only plugin-generated assets under `/Game/FX_RetargetAssistant/Setups/`.

## Test Matrix

| Case | Status | Notes |
| --- | --- | --- |
| Mixamo -> UE Manny | Pending visual closure | Requires Auto Create, Open Retargeter, verify Pelvis=None if Mixamo -> UE preset triggers, export and preview. |
| Mixamo -> UE Quinn | Pending | Same expected behavior as Manny target if Quinn uses UE Manny-family naming. |
| Manny -> Quinn | Pending | Pelvis=None preset should not be logged as Mixamo -> UE unless source is detected as Mixamo. |
| Use Existing Retargeter / Manual Mode | Pending | Retarget & Export uses manual Retargeter as-is. |
| Auto Create Setup -> Open Retargeter -> manual inspect -> export | Pending | Verify Open button and generated asset paths. |
| Source Mesh changed clears animations | Implemented, pending visual confirmation | Source change clears SelectedAnimations and Anim Clip picker. |
| Add Selected filters non-Source Skeleton animations | Implemented, pending visual confirmation | Non-matching animations log Warning and are skipped. |
| Auto Repair does not modify user Retargeter | Implemented, pending manual check | Non-`/Game/FX_RetargetAssistant/Setups/` path only warns. |
| Output duplicate naming strategy | Existing behavior, pending explicit check | No overwrite; generated unique names expected. |
| Restart persistence | Pending | Generated IK Rig / Retargeter / AnimSequence should load after closing/reopening UE. |

## Implemented For Closure

- Step 3 renamed to `Retarget Setup`.
- Added read-only `Setup Status`: Not Created / Ready / Needs Review / Missing Chains / Invalid.
- Auto Create defaults to reusing existing generated setup assets.
- Added `Recreate Generated Setup` for explicit reconfiguration/save.
- Added `Auto Repair IK Mapping` with generated-asset guard.
- Report.json extended with MVP1 setup fields.
- Mixamo -> UE Pelvis=None rule documented in logs/report warnings only when the preset is detected.

## Latest UX Fix

- Clear Animations now clears both the selected animation list and the Anim Clip picker. Built successfully in UE5.4.

## Closure Tightening - 2026-06-23

- `Setup Status` is now refreshed after Source Mesh changes, Target Mesh changes, Retargeter changes, Auto Create, Recreate, Auto Repair, Add Animation, and Clear Animations.
- Status rules currently applied:
  - `Invalid`: missing Source/Target mesh or setup creation failure.
  - `Not Created`: Source/Target are valid, but no Retargeter is selected.
  - `Missing Chains`: generated setup reported skipped/missing core chain warnings.
  - `Needs Review`: setup can be used but needs attention, including no animations selected, manual/user Retargeter selected, or setup warnings such as Mixamo -> UE preset behavior.
  - `Ready`: generated Retargeter is selected, no setup warnings are active, and at least one matching AnimSequence is selected.
- `Report.json` now explicitly records `namingRule.conflictPolicy = "Create Unique Name"` and `overwrite = false`.
- Batch Retarget log now states: existing assets are not overwritten; unique output names are created.
- UE5.4 compile passed with MSVC 14.38.
- Headless smoke test passed for Mixamo -> UE5 Manny TestSet Preflight: 3 animations ready. The commandlet intentionally skips actual export because UE5.4 batch retarget export touches Content Browser / Slate.

Remaining before marking Closure passed:

- Complete visual/editor matrix for Mixamo -> Manny, Mixamo -> Quinn, Manny -> Quinn, Manual Mode, Auto Repair user-asset guard, duplicate output naming, restart persistence, Add Selected filtering, Source Mesh change clearing, and Open Retargeter manual-adjust/export.

