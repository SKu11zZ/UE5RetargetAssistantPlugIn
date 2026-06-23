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
