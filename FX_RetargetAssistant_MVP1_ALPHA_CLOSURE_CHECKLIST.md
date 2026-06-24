# FX_RetargetAssistant MVP1 Alpha UE5.8 Migration Checklist

Status: MVP1 Alpha UE5.8 Migration / Guided Auto Setup + Batch Export.

Current active target: **Unreal Engine 5.8 only**.

UE5.4 MVP1 Alpha is archived. It proved the workflow but is no longer the active compatibility target.

## Scope Boundary

- `Retarget & Export` remains pure export.
- It must not auto-create IK Rig / IK Retargeter.
- It must not auto-repair IK Mapping.
- It must not Auto Align.
- It must not modify user-selected Retargeters.
- Auto Create / Recreate / Auto Repair may save only plugin-generated assets under `/Game/FX_RetargetAssistant/Setups/`.

## UE5.8 Migration Tasks

| Case | Status | Notes |
| --- | --- | --- |
| Copy plugin into FXRA58 | Passed | Target: `F:\Unreal Projects\FXRA58\Plugins\FX_RetargetAssistant`. |
| Remove target generated artifacts | Passed | Removed plugin `Binaries/`, `Intermediate/`, `Saved`, `.vs` if present before UE5.8 build. |
| Regenerate project files | Passed via UBT | UE5.8 UBT invalidated makefile and generated required build data during compile. |
| Compile FXRA58Editor | Passed | Built with UE5.8 and VS 14.44 toolchain. UBA retried several compile actions due memory pressure but build succeeded. |
| Remove/minimize historical cross-version bridge usage | Passed | Deleted the empty historical bridge stub; removed low-version guard around `bRetainAdditiveFlags`. |
| Verify Auto Create IK Rig + Retargeter | Pending | Prefer UE5.8 native APIs; plugin policy is supplement. |
| Verify Open Retargeter | Pending | Native Retargeter editor opens generated asset. |
| Verify Auto Repair IK Mapping | Pending | Only modifies plugin-generated setup assets. |
| Verify Retarget & Export | Pending | Pure export only. |
| Verify Report.json | Pending | Includes UE5.8 root-family and naming fields. |
| Verify Show Output Folder | Pending | Syncs Content Browser to exported assets. |

## Bring-up Pass 1 - Automated Checks

Status: partial pass. Editor UI and visual retarget cases are still pending manual/visual confirmation.

Completed:

- Documentation audit:
  - README is clean for UE5.8-only mainline.
  - Current technical plan is UE5.8-only.
  - Current Closure Checklist is UE5.8 migration-focused.
  - Remaining historical terms are limited to Archive / Legacy / UE54 archived / Memory contexts, or an Archived UE5.4 Notes section.
- FXRA58 content asset check:
  - Found UE5.8 Mannequin meshes:
    - `/Game/Characters/Mannequins/Meshes/SKM_Manny_Simple`
    - `/Game/Characters/Mannequins/Meshes/SKM_Quinn_Simple`
    - `/Game/Characters/Mannequins/Meshes/SK_Mannequin`
  - Found Mannequin animation assets under `/Game/Characters/Mannequins/Anims/`.
  - Missing Mixamo test assets:
    - `/Game/FXRA_Imported/MixamoShared/Center_Block`
  - Missing old UE5.4-style default retarget assets:
    - `/Game/Characters/Mannequins/Rigs/IK_Mannequin`
    - `/Game/Characters/Mannequins/Rigs/RTG_Mannequin`
- Missing test asset behavior:
  - SmokeTest now logs:
    `[Warning] Required test assets are missing in FXRA58. Please import or migrate test assets.`
  - SmokeTest exits with 0 errors instead of hard failing when those optional validation assets are absent.
- UE5.8 compile:
  - `FXRA58Editor` compile passed after the warning-path update.

Blocked until test assets are imported or migrated:

- Mixamo -> UE Manny visual validation.
- UE Mannequin -> Mixamo visual validation.
- Output duplicate export validation that depends on a complete Retargeter + animation test path.
- Report.json full field validation from an actual export.

## Bring-up Pass 2 - Test Asset Preparation + Automated Checks

Status: partial pass. Test assets are prepared and automated checks pass; Editor UI interaction and visual retarget matrix are still pending.

Completed:

- Prepared Mixamo test assets in FXRA58:
  - `/Game/FXRA_Imported/MixamoShared/Center_Block`
  - `/Game/FXRA_Imported/MixamoShared/Center_Block_Anim`
  - `/Game/FXRA_Imported/MixamoShared/Hip_Hop_Dancing_Anim`
  - `/Game/FXRA_Imported/MixamoShared/Swing_Dancing_Anim`
  - skeleton dependency: `/Game/FXRA_Imported/Mixamo/Center_Block_Skeleton`
- Did not copy UE5.4 generated setup assets:
  - no `/Game/FX_RetargetAssistant/TestSet/IK_Mixamo_FXRA`
  - no `/Game/FX_RetargetAssistant/TestSet/RTG_Mixamo_to_UE5Manny_FXRA`
  - no UE5.4 `/Game/FX_RetargetAssistant/Setups/...`
- UE5.8 resave passed for Mixamo assets with 0 errors / 0 warnings.
- Asset validation confirmed `Center_Block` and all 3 AnimSequences share `/Game/FXRA_Imported/Mixamo/Center_Block_Skeleton`.
- Updated SmokeTest/TestSet preparation to use UE5.8 Auto Setup instead of old `/Game/Characters/Mannequins/Rigs/IK_Mannequin`.
- `FXRA58Editor` compile passed after the update.
- `FX_RetargetAssistantSmokeTest` passed:
  - generated `/Game/FX_RetargetAssistant/Setups/Center_Block_to_SKM_Manny_Simple/`
  - generated `IK_Center_Block`
  - generated `IK_SKM_Manny_Simple`
  - generated `RTG_Center_Block_to_SKM_Manny_Simple`
  - applied Mixamo -> UE root-family policy
  - Preflight passed for 3 animation(s)
- FXRA58 Editor launches with `-d3d11 -NoLiveCoding -log` and reaches window title `FXRA58 - Unreal Editor`.

Still pending:

- Open FX Retarget Assistant panel and verify UE5.8 UI details.
- Retarget & Export actual output validation.
- Visual checks for Mixamo -> UE Manny, UE Mannequin -> Mixamo, and Manny -> Quinn.
- User Retargeter safety validation.
- Duplicate output naming / Report.json validation.
- Reopen persistence validation.

## Bring-up Fix - UE5.8 Retarget Ops Stack

Status: code fix passed compile and automated smoke test; visual verification still needed on regenerated setup assets.

Issue:

- Generated UE5.8 Retargeter assets could open with an empty Op Stack.
- Auto Align appeared not to persist.
- Exported retargeted animation could be empty.

Fix:

- Generated Retargeter configuration now follows the UE5.8 default-op path:
  - clear existing ops
  - `AddDefaultOps()`
  - Exact Automap
  - target pose reset
  - target Auto Align
  - directional Root/Pelvis policy
  - save generated Retargeter
- Reinitialization now uses `ERetargetRefreshMode::ProcessorAndOpStack`.

Automated verification:

- `FXRA58Editor` compile passed.
- `FX_RetargetAssistantSmokeTest` passed with 0 errors / 0 warnings.
- Generated RTG binary contains UE5.8 op types:
  - `IKRetargetPelvisMotionOp`
  - `IKRetargetFKChainsOp`
  - `IKRetargetRunIKRigOp`
  - `IKRetargetRootMotionOp`
  - `IKRetargetCurveRemapOp`

Manual verification required:

- For any setup generated before this fix, click `Recreate Generated Setup` once.
- Confirm the Retargeter editor shows default Ops in the Op Stack.
- Confirm Auto Align result is visually correct.
- Confirm exported animation is no longer empty.

Closure guardrails:

- Auto Create reuses an existing generated Retargeter without modifying or saving it when its UE5.8 Retarget Ops Stack is valid.
- Auto Create does not update/save existing generated IK Rig assets in the default reuse path.
- Recreate Generated Setup remains the explicit rebuild path for generated assets.
- Existing generated Retargeters missing an Ops Stack are treated as invalid setup and may be repaired only under `/Game/FX_RetargetAssistant/Setups/`.
- User Retargeters outside `/Game/FX_RetargetAssistant/Setups/` are never auto-modified, never Auto Aligned, and never Remove Ops / Add Default Ops / Saved.
- Preflight checks the current Retargeter Ops Stack before export.
- Missing Retarget Ops Stack is a Preflight error and blocks export to avoid empty AnimSequence output.
- Setup Status shows `Invalid` when the selected Retargeter has no valid Retarget Ops Stack.
- Report.json records:
  - `retargetOpsStackValid`
  - `retargetOpsStackCount`
  - `retargetOpsStackOpTypeNames`

## UE5.8 First Test Matrix

| Case | Status | Expected Result |
| --- | --- | --- |
| Mixamo -> UE Manny | Pending | `Pelvis=None`; no pelvis/body shaking. |
| UE Mannequin -> Mixamo | Pending | `Root=None`; no floating/global rotation. |
| Manny -> Quinn | Pending | UE -> UE keeps Root/Pelvis exact; no None special rule. |
| Manual Mode / Use Existing Retargeter | Pending | User Retargeter is not modified. |
| Auto Repair IK Mapping | Pending | Only repairs `/Game/FX_RetargetAssistant/Setups/`; user Retargeter gets Warning only. |
| Recreate Generated Setup | Pending | Only clears/rebuilds plugin-generated assets. |
| Duplicate output names | Pending | Generates `_001`, `_002`; no overwrite. |
| Open Retargeter -> manual adjust -> export | Pending | User fine-tuned generated Retargeter exports correctly. |
| Reopen UE5.8 project | Pending | Generated IK Rig / Retargeter / AnimSequence reload. |

## Root Family Directional Policy

- `UEMannequin -> Mixamo`: Target Chain `Root=None`.
- `Mixamo -> UEMannequin`: Target Chain `Pelvis=None`; Target Chain `Root=None`.
- `UEMannequin -> UEMannequin`: Root and Pelvis remain exact.
- `Mixamo -> Mixamo`: Retarget Root uses `Hips`; Root defaults to `None`.

These rules are for plugin-generated setup assets only.
