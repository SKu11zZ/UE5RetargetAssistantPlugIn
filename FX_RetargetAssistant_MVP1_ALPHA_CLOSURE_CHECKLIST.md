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

## UE5.8 Functional Validation Pass

Status: automated pass + human visual validation pass. Current stage: `FX_RetargetAssistant MVP1 Alpha / UE5.8 Functional Validation Passed / Alpha Closure Passed`.

Commandlet:

```text
UnrealEditor-Cmd.exe FXRA58.uproject -run=FX_RetargetAssistantSmokeTest -FunctionalValidation -unattended -nop4 -nullrhi -log
```

Latest validation output:

```text
/Game/FX_RetargetAssistant/Exports/FunctionalValidation_20260624_142733/
```

Resolved human visual failure:

- Failed asset:
  `/Game/FX_RetargetAssistant/Exports/FunctionalValidation_20260624_142733/Manny_to_Mixamo/MF_Unarmed_Walk_Fwd_Right_RTG`
- Symptom:
  Manny -> Mixamo exported animation shows root-family deformation: body motion is not correct, pelvis/Hips is pulled away, and visible mesh tearing occurs.
- Resolution:
  UE->Mixamo root-family policy now maps Target Chain `Root` and Target Chain `Pelvis/Hips` to Source Chain `None`.
- Current status:
  `UE5.8 Functional Validation automated PASS + human visual validation PASS.`

Automated pass result:

- `FXRA FUNCTIONAL VALIDATION RESULT: PASS failures=0`
- `REOPEN_VALIDATION PASS`
- Generated IK Rig / Retargeter / AnimSequence assets loaded in a fresh UE commandlet process.
- Report.json required fields were present.
- `retargetOpsStackValid=true`; `retargetOpsStackCount` and `retargetOpsStackOpTypeNames` recorded.

Bug fixed during validation:

- Duplicate export names now use plugin-controlled naming:
  - `Hip_Hop_Dancing_Anim_RTG`
  - `Hip_Hop_Dancing_Anim_RTG_001`
  - `Hip_Hop_Dancing_Anim_RTG_002`
- The previous UE default `RTG1` / `RTG2` style is no longer used by the plugin exporter.

## UE5.8 First Test Matrix

| Case | Status | Expected Result |
| --- | --- | --- |
| Mixamo -> UE Manny | Passed | `Pelvis=None`; no pelvis/body shaking. |
| UE Mannequin -> Mixamo | Passed | `Root=None` and `Pelvis/Hips=None`; no tearing, floating, or global rotation. |
| Manny -> Quinn | Passed | UE -> UE keeps Root/Pelvis exact; no None special rule. |
| Manual Mode / Use Existing Retargeter | Automated Pass | User Retargeter is not modified. |
| Auto Repair IK Mapping | Automated Pass | Only repairs `/Game/FX_RetargetAssistant/Setups/`; user Retargeter gets Warning only. |
| Recreate Generated Setup | Automated Pass | Only clears/rebuilds plugin-generated assets. |
| Duplicate output names | Passed | Generates `_001`, `_002`; no overwrite. |
| Open Retargeter -> manual adjust -> export | Policy Pass / Manual Visual Pending | Auto Create reuse path preserves existing generated RTG; user fine-tuned generated Retargeter can be exported without forced rebuild. |
| Reopen UE5.8 project | Passed | Generated IK Rig / Retargeter / AnimSequence reload. |

## Root Family Directional Policy

- `UEMannequin -> Mixamo`: Target Chain `Root=None`; Target Chain `Pelvis/Hips=None`. Mixamo `Hips` is driven by UE5.8 Pelvis Motion / Root Motion Ops, not normal FK chain mapping.
- `Mixamo -> UEMannequin`: Target Chain `Pelvis=None`; Target Chain `Root=None`.
- `UEMannequin -> UEMannequin`: Root and Pelvis remain exact.
- `Mixamo -> Mixamo`: Retarget Root uses `Hips`; Root defaults to `None`.

These rules are for plugin-generated setup assets only.

## UE5.8 Root-Family Fix Recheck

Status: code fix compiled, automated validation passed, and human visual recheck passed. Closure blocker resolved.

Latest recheck output:

```text
/Game/FX_RetargetAssistant/Exports/FunctionalValidation_20260624_145341/
```

Recheck evidence:

- Manny -> Mixamo `Report.json`:
  - `rootFamilyPolicy = UEMannequin -> Mixamo`
  - `rootChainMapping = Target Chain Root -> Source Chain None`
  - `pelvisChainMapping = Target Chain Pelvis/Hips -> Source Chain None`
- Mixamo -> Manny `Report.json`:
  - `rootChainMapping = Target Chain Root -> Source Chain None`
  - `pelvisChainMapping = Target Chain Pelvis -> Source Chain None`
- Manny -> Quinn `Report.json`:
  - `rootChainMapping = Target Chain Root -> Source Chain Root`
  - `pelvisChainMapping = Target Chain Pelvis -> Source Chain Pelvis`

Manual visual check required:

- `/Game/FX_RetargetAssistant/Exports/FunctionalValidation_20260624_145341/Manny_to_Mixamo/MF_Unarmed_Walk_Fwd_Right_RTG`
- Result: no tearing, no floating, no global rotation; acceptable.

Closure result:

- `FX_RetargetAssistant MVP1 Alpha / UE5.8 Functional Validation Passed / Alpha Closure Passed`

## UE5.8 Packaged Plugin Clean Project Validation

Status: automated packaged-plugin validation passed and user UI visual confirmation passed. Current stage: `FX_RetargetAssistant MVP1 Alpha / UE5.8 Packaged Validation Passed`.

Important scope note:

- This is not Beta.
- This is not a Release Candidate.
- It means MVP1 Alpha packaged clean-project validation has passed.

Stage:

```text
UE5.8 Packaged Plugin Clean Project Validation
```

BuildPlugin:

- Output path:
  `C:\FXRA_UE58_Build\FX_RetargetAssistant`
- Result:
  `BUILD SUCCESSFUL`
- Packaged plugin does not include a `Content` folder, so it does not carry FXRA58 temporary setup/export assets.

Clean project:

- New UE5.8 Blank C++ validation project:
  `F:\Unreal Projects\FXRA58PackagedValidation`
- Packaged plugin installed under:
  `F:\Unreal Projects\FXRA58PackagedValidation\Plugins\FX_RetargetAssistant`
- Project compile:
  `FXRA58PackagedValidationEditor Win64 Development` passed.

Prepared test assets:

- Copied minimal Mixamo test content:
  `/Game/FXRA_Imported/`
- Copied UE Manny / Quinn test content:
  `/Game/Characters/Mannequins/`
- Did not copy generated setup/export assets from FXRA58.

Functional validation:

- Commandlet:
  `FX_RetargetAssistantSmokeTest -FunctionalValidation -nullrhi`
- Result:
  `FXRA FUNCTIONAL VALIDATION RESULT: PASS failures=0`
- Output:
  `/Game/FX_RetargetAssistant/Exports/FunctionalValidation_20260624_152424/`

Verified:

- Generated IK Rig / RTG assets were created under `/Game/FX_RetargetAssistant/Setups/`.
- Retarget Ops Stack exists and Report.json records:
  - `retargetOpsStackValid`
  - `retargetOpsStackCount`
  - `retargetOpsStackOpTypeNames`
- Root Family Policy in clean project:
  - `UEMannequin -> Mixamo`: Root=None; Pelvis/Hips=None.
  - `Mixamo -> UEMannequin`: Root=None; Pelvis=None.
  - `UEMannequin -> UEMannequin`: Root->Root; Pelvis->Pelvis.
- Duplicate outputs exist:
  - `Hip_Hop_Dancing_Anim_RTG`
  - `Hip_Hop_Dancing_Anim_RTG_001`
  - `Hip_Hop_Dancing_Anim_RTG_002`
- Report.json records:
  - `rootFamilyPolicy`
  - `rootChainMapping`
  - `pelvisChainMapping`
  - `retargetOpsStackValid`
  - `retargetOpsStackCount`
  - `namingRule.conflictPolicy = Create Unique Name`
- Reopen validation:
  - A second UE commandlet process loaded generated IK Rig / RTG / AnimSequence assets and Report.json successfully.

UI status:

- Clean project Editor was launched.
- Packaged plugin source confirms panel title:
  `FX Retarget Assistant - MVP1 Alpha / UE5.8`
- User visual confirmation passed:
  - In clean UE5.8 C++ project `FXRA58PackagedValidation`, `Window -> FX Retarget Assistant` opened the panel.
  - Panel title correctly displayed `FX Retarget Assistant - MVP1 Alpha / UE5.8`.

Project-name note:

- UE reported that project names must not be longer than 20 characters for `FXRA58PackagedValidation`.
- This warning is about the temporary validation project name, not the plugin.
- Future clean validation projects should use a shorter name such as `FXRA58PkgVal`.

Final status:

- `FX_RetargetAssistant MVP1 Alpha / UE5.8 Packaged Validation Passed`
