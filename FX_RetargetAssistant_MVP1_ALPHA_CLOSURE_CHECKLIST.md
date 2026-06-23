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
| Remove/minimize VersionBridge usage | Passed | Deleted empty `FX_RetargetAssistantVersionBridge.h`; removed low-version guard around `bRetainAdditiveFlags`. |
| Verify Auto Create IK Rig + Retargeter | Pending | Prefer UE5.8 native APIs; plugin policy is supplement. |
| Verify Open Retargeter | Pending | Native Retargeter editor opens generated asset. |
| Verify Auto Repair IK Mapping | Pending | Only modifies plugin-generated setup assets. |
| Verify Retarget & Export | Pending | Pure export only. |
| Verify Report.json | Pending | Includes UE5.8 root-family and naming fields. |
| Verify Show Output Folder | Pending | Syncs Content Browser to exported assets. |

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
