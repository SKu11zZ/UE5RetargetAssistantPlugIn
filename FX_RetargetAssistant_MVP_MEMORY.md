# FX_RetargetAssistant MVP Memory

## Current Truth - UE5.8 Pivot (2026-06-23)

Current active target:

- Unreal Engine 5.8 only
- Branch: `ue58-main`
- Active project: `F:\Unreal Projects\FXRA58`
- Active plugin destination: `F:\Unreal Projects\FXRA58\Plugins\FX_RetargetAssistant`
- Source repository plugin folder: `G:\UE5重定向插件开发\插件\FX_RetargetAssistant`
- Current phase: MVP1 Alpha UE5.8 Migration / Guided Auto Setup + Batch Export

UE5.4 status:

- UE5.4 MVP1 Alpha is archived.
- It proved the workflow, but it is no longer the active development target.
- UE5.4 / 5.5 / 5.6 / 5.7 compatibility work, VersionBridge expansion, and multi-version BuildPlugin packaging are frozen.

Important boundary:

- A separate UE5.8 project named `ResearchOnUENewTools` may be running in `F:\Unreal Projects\ResearchOnUENewTools`.
- Do not close, kill, modify, or otherwise affect that project or its running editor process.
- Only operate on `FXRA58` for FX_RetargetAssistant migration work.

## Phase 27 - UE5.8 Pivot / UE5.8-only Migration Start (2026-06-23)

User direction:

- Stop pursuing UE5.4 - UE5.8 cross-version compatibility.
- Make UE5.8 the only active target.
- Update documents and project wording first, then migrate code to FXRA58.
- Preserve UE5.4 history as archive, not current execution guidance.

Documentation updates started:

- `README.md` rewritten for UE5.8-only mainline.
- `FX_RetargetAssistant_技术方案.md` rewritten as UE5.8-only technical plan.
- `FX_RetargetAssistant_MVP0_SCOPE.md` rewritten as MVP0 archive.
- `FX_RetargetAssistant_LEGACY_PLAN_NOTE.md` rewritten as legacy archive.
- `FX_RetargetAssistant_UE54_MVP_Closure_Checklist.md` rewritten as UE5.4 archive.
- `FX_RetargetAssistant_BUILD_NOTES.md` rewritten as UE5.8 build plan plus UE5.4 archive notes.
- `FX_RetargetAssistant_MVP1_ALPHA_CLOSURE_CHECKLIST.md` rewritten as UE5.8 migration checklist.
- `FX_RetargetAssistant/MVP0_MANUAL_TEST.md` rewritten as manual test archive with UE5.8 validation pointer.

Code direction:

- Removed historical UE version conditional around `bRetainAdditiveFlags`; UE5.8 mainline uses the UE5.8 API directly.
- Deleted the empty `FX_RetargetAssistantVersionBridge.h` stub. Do not expand VersionBridge unless the commercial direction explicitly returns to cross-version support.
- Updated smoke-test messaging so the UE5.4 commandlet limitation is no longer treated as a UE5.8 design premise.

Next work:

- Copy plugin into `F:\Unreal Projects\FXRA58\Plugins\FX_RetargetAssistant`.
- Remove generated artifacts in the target plugin directory.
- Regenerate/build with UE5.8.
- Fix UE5.8 API compile errors directly.
- Revalidate the UE5.8 test matrix.

Migration result:

- Plugin copied into `F:\Unreal Projects\FXRA58\Plugins\FX_RetargetAssistant`.
- Target plugin generated artifacts were excluded/removed during migration.
- `FXRA58Editor` built successfully with UE5.8.
- Observed toolchain:
  - UE5.8 bundled .NET 10 runtime
  - Visual Studio 14.44.35228 toolchain
  - Windows 10.0.26100.0 SDK
- First build had UBA memory-pressure retries but completed successfully.
- Incremental rebuild after UI title update also passed.
- UI title changed to `FX Retarget Assistant - MVP1 Alpha / UE5.8`.
- `ResearchOnUENewTools` UE5.8 editor process was detected and left untouched.

## Phase 28 - UE5.8 Bring-up Automated Audit Pass 1 (2026-06-23)

Status: partial bring-up validation. Compile and commandlet warning-path checks passed; Editor UI and visual retarget cases remain pending.

Completed:

- Documentation audit on `ue58-main`:
  - README contains no old cross-version/current-target wording.
  - Current technical plan is UE5.8-only.
  - Current Closure Checklist is UE5.8 bring-up-focused.
  - Old terms such as UE5.4, MSVC 14.38, and commandlet limitations remain only in archive/history contexts.
- FXRA58 asset check:
  - Present:
    - `/Game/Characters/Mannequins/Meshes/SKM_Manny_Simple`
    - `/Game/Characters/Mannequins/Meshes/SKM_Quinn_Simple`
    - `/Game/Characters/Mannequins/Meshes/SK_Mannequin`
    - Mannequin animation assets under `/Game/Characters/Mannequins/Anims/`
  - Missing:
    - Mixamo test character and animations under `/Game/FXRA_Imported/MixamoShared/`
    - `/Game/Characters/Mannequins/Rigs/IK_Mannequin`
    - `/Game/Characters/Mannequins/Rigs/RTG_Mannequin`
- Updated missing-test-asset handling:
  - Panel `LoadMixamoTestSet` logs Warning instead of Error when optional test assets are missing.
  - SmokeTest commandlet logs the exact warning:
    `[Warning] Required test assets are missing in FXRA58. Please import or migrate test assets.`
  - SmokeTest exits 0 with warnings when optional validation assets are absent.
- Synced source to `F:\Unreal Projects\FXRA58\Plugins\FX_RetargetAssistant`.
- Rebuilt `FXRA58Editor` successfully with UE5.8.
- Ran `FX_RetargetAssistantSmokeTest` in UE5.8:
  - Result: 0 errors, warnings for missing validation assets.

Pending:

- Editor UI visual confirmation.
- Auto Create validation in FXRA58 using available Manny/Quinn assets.
- Mixamo -> UE Manny visual retarget after Mixamo assets are imported/migrated.
- UE Mannequin -> Mixamo visual retarget after Mixamo assets are imported/migrated.
- Retarget & Export full export validation.
- User Retargeter safety timestamp check.
- Duplicate output naming and Report.json full field validation.

## Phase 0 - Baseline Restore And UE5.4 Toolchain Check

- Date: 2026-06-23
- Project under test: `F:\Unreal Projects\FXRA54`
- Engine: `F:\Epic Games\UE_5.4`
- Repository root: `G:\UE5重定向插件开发\插件`
- Active plugin source: `G:\UE5重定向插件开发\插件\FX_RetargetAssistant`
- MVP boundary: existing IK Retargeter batch export only. No automatic IK Rig creation, no automatic IK Retargeter creation, no bone resolver in main path.
- Toolchain state: MSVC 14.38 x64 components are now installed, including `lib\x64\delayimp.lib`.
- Code cleanup done:
  - Removed the invalid `__has_feature(x)=0` Build.cs workaround.
  - Added UE version guard around `FIKRetargetBatchOperationContext::bRetainAdditiveFlags`, because UE 5.4 does not expose that field.

Next step: sync plugin into `FXRA54/Plugins/FX_RetargetAssistant` and compile with UE5.4 using `-CompilerVersion=14.38.33130`.

## Phase 1 - UE5.4 Compile Baseline

- Date: 2026-06-23
- Synced plugin source from `G:\UE5重定向插件开发\插件\FX_RetargetAssistant` into `F:\Unreal Projects\FXRA54\Plugins\FX_RetargetAssistant`.
- Enabled plugin in `F:\Unreal Projects\FXRA54\FXRA54.uproject`.
- Build command:
  - `F:\Epic Games\UE_5.4\Engine\Build\BatchFiles\Build.bat FXRA54Editor Win64 Development -Project="F:\Unreal Projects\FXRA54\FXRA54.uproject" -WaitMutex -NoHotReloadFromIDE -CompilerVersion=14.38.33130`
- Result: compile and link succeeded for `FXRA54Editor` and `UnrealEditor-FX_RetargetAssistant.dll`.
- Remaining warning:
  - `UPackage::SavePackage` uses deprecated overload in `FX_RetargetAssistantBatchExporter.cpp`. It compiles in UE 5.4, but should be modernized before wider UE 5.5/5.6 validation.
- MVP boundary still intact:
  - Main path uses existing IK Retargeter.
  - No automatic IK Rig creation.
  - No automatic IK Retargeter creation.
  - No bone-name resolver in execution path.

Next step: launch/load `FXRA54` editor to verify plugin module startup and menu/tab registration.

## Phase 2 - UE5.4 Editor Load Verification

- Date: 2026-06-23
- Launch command:
  - `F:\Epic Games\UE_5.4\Engine\Binaries\Win64\UnrealEditor.exe "F:\Unreal Projects\FXRA54\FXRA54.uproject" -NullRHI -NoSplash -Unattended -ExecCmds="Quit" -LogCmds="LogModuleManager Verbose"`
- Result: editor launched and exited with code 0.
- Latest log: `F:\Unreal Projects\FXRA54\Saved\Logs\FXRA54.log`
- Plugin load evidence:
  - `LogPluginManager: Mounting Project plugin FX_RetargetAssistant`
  - `LogModuleManager: Load Module 'FX_RetargetAssistant' DLL ... UnrealEditor-FX_RetargetAssistant.dll`
- No `FX_RetargetAssistant` startup error found.

Next step: remove the UE 5.4 `UPackage::SavePackage` deprecation warning and rebuild.

## Phase 3 - Clean Sync And Warning-Free UE5.4 Build

- Date: 2026-06-23
- Closed stale `FXRA54` UnrealEditor process that was locking `UnrealEditor-FX_RetargetAssistant.dll`.
- Cleaned the bad nested project plugin copy at `F:\Unreal Projects\FXRA54\Plugins\FX_RetargetAssistant\FX_RetargetAssistant`.
- Re-synced plugin source cleanly:
  - From `G:\UE5重定向插件开发\插件\FX_RetargetAssistant`
  - To `F:\Unreal Projects\FXRA54\Plugins\FX_RetargetAssistant`
- Modernized asset saving in `FX_RetargetAssistantBatchExporter.cpp`:
  - Uses `FSavePackageArgs`.
  - Removes UE5.4 `UPackage::SavePackage` deprecation warning.
- Clean build command:
  - `F:\Epic Games\UE_5.4\Engine\Build\BatchFiles\Build.bat FXRA54Editor Win64 Development -Project="F:\Unreal Projects\FXRA54\FXRA54.uproject" -WaitMutex -NoHotReloadFromIDE -CompilerVersion=14.38.33130`
- Result: build succeeded.
- Plugin output:
  - `F:\Unreal Projects\FXRA54\Plugins\FX_RetargetAssistant\Intermediate\Build\Win64\x64\UnrealEditor\Development\FX_RetargetAssistant\UnrealEditor-FX_RetargetAssistant.lib`
  - `F:\Unreal Projects\FXRA54\Plugins\FX_RetargetAssistant\Intermediate\Build\Win64\x64\UnrealEditor\Development\FX_RetargetAssistant\UnrealEditor-FX_RetargetAssistant.dll`
- Current MVP 0 status:
  - Compile baseline is good on UE5.4 with MSVC 14.38.
  - Plugin code remains scoped to existing IK Retargeter batch export.

Next step: run editor load verification again from the clean plugin layout, then proceed to manual in-editor MVP workflow validation with real Source Mesh, Target Mesh, IK Retargeter, and selected AnimSequence assets.

## Phase 4 - Clean Editor Load Verification

- Date: 2026-06-23
- Verification command used `UnrealEditor-Cmd.exe` with `FXRA54.uproject`, `-NullRHI`, and verbose module logging.
- The command process did not auto-exit, so only the `FXRA54` UnrealEditor-Cmd process was stopped manually.
- Latest log evidence:
  - `LogPluginManager: Mounting Project plugin FX_RetargetAssistant`
  - `LogModuleManager: Load Module 'FX_RetargetAssistant' DLL '../../../../../Unreal Projects/FXRA54/Plugins/FX_RetargetAssistant/Binaries/Win64/UnrealEditor-FX_RetargetAssistant.dll'`
- Result: clean project plugin layout loads the module successfully.

Next step: inspect `FXRA54` content for real MVP workflow inputs: Source Skeletal Mesh, Target Skeletal Mesh, existing IK Retargeter, and one or more AnimSequence assets.

## Phase 5 - MVP Test Asset Discovery And UI Selection Polish

- Date: 2026-06-23
- `FXRA54` contains usable MVP test assets:
  - Source candidate: `/Game/Characters/Mannequins/Meshes/SKM_Manny`
  - Target candidate: `/Game/Characters/Mannequins/Meshes/SKM_Quinn`
  - Existing Retargeter candidate: `/Game/Characters/Mannequins/Rigs/RTG_Mannequin`
  - Source animation candidates:
    - `/Game/Characters/Mannequins/Animations/Manny/MM_Walk_Fwd`
    - `/Game/Characters/Mannequins/Animations/Manny/MM_Run_Fwd`
    - `/Game/Characters/Mannequins/Animations/Manny/MM_Idle`
- UI polish added:
  - `SObjectPropertyEntryBox` fields now expose `ObjectPath_Lambda` so the selected Source Mesh, Target Mesh, and IK Retargeter remain visible after selection.

Next step: rebuild after UI polish and open the editor for manual MVP flow validation.

## Phase 6 - Rebuild After UI Polish And Manual Test Plan

- Date: 2026-06-23
- Re-synced plugin source into `F:\Unreal Projects\FXRA54\Plugins\FX_RetargetAssistant`.
- Rebuilt with UE5.4 and MSVC 14.38.
- Build result: succeeded.
- Added manual MVP0 validation guide:
  - `G:\UE5重定向插件开发\插件\FX_RetargetAssistant\MVP0_MANUAL_TEST.md`
- Manual test asset set documented:
  - Source Mesh: `/Game/Characters/Mannequins/Meshes/SKM_Manny`
  - Target Mesh: `/Game/Characters/Mannequins/Meshes/SKM_Quinn`
  - Existing IK Retargeter: `/Game/Characters/Mannequins/Rigs/RTG_Mannequin`
  - AnimSequence samples: `MM_Walk_Fwd`, `MM_Run_Fwd`, `MM_Idle`
- Current state:
  - MVP 0 compiles in UE5.4.
  - Plugin module loads.
  - Manual in-editor batch export validation is the next functional gate.

Next step: open UE editor and run the manual flow, or use these steps for user-side validation.

## Phase 7 - Project Browser Category Display Fix

- Date: 2026-06-23
- User-reported visual issue: `FXRA54`, `FXRA55`, and `FXRA56` project tiles showed the category label `其他` in the Unreal project browser.
- Root cause: all three `.uproject` files had an empty `Category` field.
- Updated:
  - `F:\Unreal Projects\FXRA54\FXRA54.uproject`
  - `F:\Unreal Projects\FXRA55\FXRA55.uproject`
  - `F:\Unreal Projects\FXRA56\FXRA56.uproject`
- New descriptor values:
  - `"Category": "Games"`
  - `"Description": "FX Retarget Assistant test project"`
- Expected visual result:
  - Project tiles should no longer be categorized as `其他` after the project browser refreshes.

Visual confirmation needed: reopen or refresh Unreal project browser and confirm the `其他` label is gone from FXRA54/55/56.

## Phase 8 - UI animation selection and test asset import (2026-06-23)

Status: completed in FXRA54 / UE 5.4.

Implemented/verified:
- Updated FX_RetargetAssistant panel UI to expose an explicit AnimSequence picker row with Add button.
- Reworked animation controls into: Add Selected From Content Browser, Clear Animations, Preflight, Batch Retarget.
- Selected AnimSequences are now accumulated instead of replaced, and displayed in a read-only multi-line list.
- Synced the plugin into F:\Unreal Projects\FXRA54\Plugins\FX_RetargetAssistant and compiled successfully with UE 5.4 + MSVC 14.38.
- Imported user-provided FBX test assets into FXRA54 under /Game/FXRA_Imported.

Imported assets verified on disk:
- /Game/FXRA_Imported/Mixamo/Center_Block_Anim
- /Game/FXRA_Imported/Mixamo/Hip_Hop_Dancing_Anim
- /Game/FXRA_Imported/Mixamo/Swing_Dancing_Anim
- /Game/FXRA_Imported/Mixamo/Center_Block, Hip_Hop_Dancing, Swing_Dancing plus their Skeleton assets
- /Game/FXRA_Imported/UE5Manny/SKM_Manny_Simple plus Skeleton
- /Game/FXRA_Imported/MetaHuman/SKM_MHC_Default_BodyMesh plus Skeleton

Notes:
- Import commandlet succeeded with 0 errors and 5 FBX smoothing-group warnings. These warnings are acceptable for this test pass.
- The Content Browser object-picker browse/sync icon is native SObjectPropertyEntryBox behavior and can be unclear; the MVP UI now provides explicit Add/Clear controls for animations so users are not blocked by that icon.
- Fixed import automation script stored at G:\UE5重定向插件开发\插件\Tools\import_fxra_test_assets.py. It uses conservative UE5.4-compatible FBX options.

## Phase 9 - Manny Test Set Shortcut and Headless Preflight (2026-06-23)

Status: completed in FXRA54 / UE 5.4.

Implemented:
- Added a `Load UE5 Manny Test Set` button to the editor panel.
- The button auto-fills:
  - Source Mesh: `/Game/Characters/Mannequins/Meshes/SKM_Manny`
  - Target Mesh: `/Game/Characters/Mannequins/Meshes/SKM_Quinn`
  - IK Retargeter: `/Game/Characters/Mannequins/Rigs/RTG_Mannequin`
  - AnimSequences: `MM_Walk_Fwd`, `MM_Run_Fwd`, `MM_Idle`
- Added `UFX_RetargetAssistantSmokeTestCommandlet` as a headless preflight check for development verification.

Verification:
- Synced plugin source into `F:\Unreal Projects\FXRA54\Plugins\FX_RetargetAssistant`.
- Rebuilt with UE 5.4 + MSVC 14.38 successfully.
- Ran commandlet:
  - `UnrealEditor-Cmd.exe FXRA54.uproject -run=FX_RetargetAssistantSmokeTest -unattended -nosplash`
- Result: commandlet exited 0.
- Log evidence:
  - `[RETARGETER_SELECTED] Using existing IK Retargeter: /Game/Characters/Mannequins/Rigs/RTG_Mannequin.RTG_Mannequin`
  - `[READY] 3 animation(s) ready for batch retarget.`
  - `Headless preflight passed for 3 animation(s).`

Important UE 5.4 limitation found:
- Directly running `UIKRetargetBatchOperation::RunRetarget` inside a commandlet crashes because UE5.4 always syncs the Content Browser and posts Slate notifications at the end of the operation.
- UE5.4 `FIKRetargetBatchOperationContext` has no public flag to disable this behavior.
- Therefore, headless automated validation can cover preflight/input readiness, but actual batch retarget export must be validated in the Editor UI path for MVP0.

Visual confirmation needed:
- Open FXRA54, open FX Retarget Assistant panel, confirm the new `Load UE5 Manny Test Set` button is visible.
- Click it, then click `Preflight`; expected log: `[READY] 3 animation(s) ready for batch retarget.`
- Click `Batch Retarget`; expected output path is whatever the panel currently shows under `/Game/FX_RetargetAssistant/Exports/...`.

## Phase 10 - Mixamo To UE Default Test Set (2026-06-23)

Status: completed in FXRA54 / UE 5.4.

User direction:
- Default test flow should be Mixamo animation retargeted onto a UE character, because this is a common user need and easier to visually inspect than Manny-to-Quinn.

Implemented:
- Reimported Mixamo FBX animations into a shared source skeleton location:
  - `/Game/FXRA_Imported/MixamoShared/Center_Block_Anim`
  - `/Game/FXRA_Imported/MixamoShared/Hip_Hop_Dancing_Anim`
  - `/Game/FXRA_Imported/MixamoShared/Swing_Dancing_Anim`
- Added `FFX_RetargetAssistantSetupManager::PrepareMixamoToUE5MannyTestSet`.
- The setup manager creates/loads test setup assets:
  - `/Game/FX_RetargetAssistant/TestSet/IK_Mixamo_FXRA`
  - `/Game/FX_RetargetAssistant/TestSet/RTG_Mixamo_to_UE5Manny_FXRA`
- Source mesh for default test: `/Game/FXRA_Imported/MixamoShared/Center_Block`
- Target mesh for default test: `/Game/Characters/Mannequins/Meshes/SKM_Manny`
- Target IK Rig reused from template: `/Game/Characters/Mannequins/Rigs/IK_Mannequin`
- Mixamo source IK Rig chains are created using common Mixamo bones with support for `Hips` and `mixamorig:` naming variants.
- Retargeter source/target IK rigs and preview meshes are configured through `UIKRetargeterController`, then chains are automapped with exact names.
- UI button changed from `Load UE5 Manny Test Set` to `Load Mixamo -> UE Test Set`.
- SmokeTest commandlet now validates the Mixamo default test set instead of Manny/Quinn.

Verification:
- Reimport command for MixamoShared completed with 0 errors and expected FBX smoothing group warnings.
- Plugin rebuilt successfully in UE5.4 with MSVC 14.38.
- SmokeTest commandlet exited 0.
- Log evidence:
  - `Created Mixamo IK Rig: /Game/FX_RetargetAssistant/TestSet/IK_Mixamo_FXRA.IK_Mixamo_FXRA`
  - `Created Mixamo retargeter: /Game/FX_RetargetAssistant/TestSet/RTG_Mixamo_to_UE5Manny_FXRA.RTG_Mixamo_to_UE5Manny_FXRA`
  - `Prepared Mixamo -> UE5 Manny test set with 3 animation(s).`
  - `[READY] 3 animation(s) ready for batch retarget.`
- Disk verification found:
  - `F:\Unreal Projects\FXRA54\Content\FX_RetargetAssistant\TestSet\IK_Mixamo_FXRA.uasset`
  - `F:\Unreal Projects\FXRA54\Content\FX_RetargetAssistant\TestSet\RTG_Mixamo_to_UE5Manny_FXRA.uasset`
  - three `*_Anim.uasset` files in `F:\Unreal Projects\FXRA54\Content\FXRA_Imported\MixamoShared`

Known limitation still applies:
- UE5.4 `UIKRetargetBatchOperation::RunRetarget` cannot be safely executed in commandlet mode because it always touches Content Browser/Slate at completion.
- Actual export must be visually validated through the editor panel.

Visual confirmation needed:
- Open FXRA54 and FX Retarget Assistant panel.
- Click `Load Mixamo -> UE Test Set`.
- Click `Preflight`, expected `[READY] 3 animation(s) ready for batch retarget.`
- Click `Batch Retarget`.
- Inspect exported AnimSequences under the panel output folder, then open one to preview on UE Manny.

## Phase 11 - Auto Align Retarget Pose For Mixamo Default Retargeter (2026-06-23)

Status: completed in FXRA54 / UE 5.4.

User finding:
- Mixamo animations exported successfully, but the UE character arms drooped because the generated retargeter did not auto-align retarget poses.
- Manually clicking Auto Align in the Retargeter fixed the animation.

Implemented:
- Updated `FFX_RetargetAssistantSetupManager` so Mixamo test retargeter configuration now:
  - Sets Source IK Rig and Target IK Rig.
  - Sets Source and Target preview meshes.
  - Auto maps chains by exact chain names.
  - Cleans the retargeter asset.
  - Resets the current Target retarget pose for all bones.
  - Calls `UIKRetargeterController::AutoAlignAllBones(ERetargetSourceOrTarget::Target)`.
  - Saves the retargeter asset.
- Existing `/Game/FX_RetargetAssistant/TestSet/RTG_Mixamo_to_UE5Manny_FXRA` assets are now updated and auto-aligned when the Mixamo test set is loaded, instead of being returned unchanged.
- This matches UE5.4's own procedural retarget asset flow in `SRetargetAnimAssetsWindow.cpp`, which auto-aligns the Target pose.

Verification:
- Closed FXRA54 editor process to release plugin DLL.
- Synced source to `F:\Unreal Projects\FXRA54\Plugins\FX_RetargetAssistant`.
- Rebuilt successfully with UE5.4 + MSVC 14.38.
- Ran SmokeTest commandlet successfully.
- Log evidence:
  - `Using existing Mixamo IK Rig: /Game/FX_RetargetAssistant/TestSet/IK_Mixamo_FXRA.IK_Mixamo_FXRA`
  - `Auto-aligned target retarget pose for Mixamo -> UE5 Manny.`
  - `Using updated Mixamo retargeter: /Game/FX_RetargetAssistant/TestSet/RTG_Mixamo_to_UE5Manny_FXRA.RTG_Mixamo_to_UE5Manny_FXRA`
  - `[READY] 3 animation(s) ready for batch retarget.`

Visual confirmation needed:
- Open the Retargeter `/Game/FX_RetargetAssistant/TestSet/RTG_Mixamo_to_UE5Manny_FXRA`.
- Target retarget pose should already be auto-aligned; arms should no longer droop after export.
- Re-run `Load Mixamo -> UE Test Set` and `Batch Retarget`, then preview exported animations on UE Manny.

Next step:
- Improve export-result discoverability and report feedback so users can immediately find the generated AnimSequences and verify batch output.

## Phase 12 - Output Discoverability Improvements (2026-06-23)

Status: completed in FXRA54 / UE 5.4.

Implemented:
- Added `Show Output Folder` button to the FX Retarget Assistant panel.
- The button scans the current output package path and syncs the Content Browser to assets inside that folder.
- After a successful batch retarget, the panel now automatically syncs the Content Browser to the output folder.
- The panel logs the output folder path and asset count after syncing.

Verification:
- Synced source to `F:\Unreal Projects\FXRA54\Plugins\FX_RetargetAssistant`.
- Rebuilt successfully with UE5.4 + MSVC 14.38.

Visual confirmation needed:
- Open FXRA54 and the FX Retarget Assistant panel.
- Confirm the new `Show Output Folder` button is visible.
- Run Mixamo -> UE export; after success, Content Browser should jump to the generated output assets automatically.
- Clicking `Show Output Folder` should repeat that sync.

## Phase 13 - UE5.4 MVP Closure Boundary Locked (2026-06-23)

Status: UE5.4 MVP Alpha / Closure in progress. Do not start UE5.5/5.6 adaptation yet.

User direction accepted:
- `Load Mixamo -> UE Test Set` must remain a developer demo/test flow only.
- MVP 0 main flow remains: user provides an existing IK Retargeter; plugin batch exports selected AnimSequences.
- Main flow must not create IK Rigs, create IK Retargeters, auto-map chains, auto-align poses, save, or otherwise modify user-selected Retargeters.
- Auto Align is valid only for plugin-owned test Retargeter `/Game/FX_RetargetAssistant/TestSet/RTG_Mixamo_to_UE5Manny_FXRA`.
- Any future user Retargeter repair must be explicit and require confirmation.

Actions completed:
- Renamed UI button from `Load Mixamo -> UE Test Set` to `Load Mixamo Demo/Test Set` to reduce main-flow confusion.
- Added warning log when demo/test set is loaded:
  - `Demo/Test Set only: this creates or updates FX_RetargetAssistant test assets. The main Batch Retarget flow will not modify a user-selected IK Retargeter.`
- Created UE5.4 closure checklist document:
  - `G:\UE5重定向插件开发\插件\FX_RetargetAssistant_UE54_MVP_Closure_Checklist.md`
- Code audit result:
  - Main `ExecuteBatchRetarget()` does not call `SetupManager`.
  - `BatchExporter` loads and passes the selected IK Retargeter into UE's batch operation but does not call `UIKRetargeterController`, auto-align, chain mapping, or save on the Retargeter.
  - Auto-align/configure/save logic exists only in `FFX_RetargetAssistantSetupManager`, used by the demo/test set flow.
- Synced plugin source to FXRA54 and rebuilt successfully with UE5.4 + MSVC 14.38.

Closure checklist highlights:
- Pending manual main-flow pass without demo/test button.
- Pending mismatch/single-failure/duplicate-output checks.
- Pending editor restart persistence check.
- Pending BuildPlugin packaging and clean UE5.4 enablement check.

Next step:
- Execute UE5.4 MVP Closure Checklist before any UE5.5 adaptation.

## Phase 14 - UE5.4 Closure Automation Pass 1 (2026-06-23)

Status: partially completed UE5.4 MVP Closure checks.

Completed this pass:
- Changed plugin descriptor from Beta to Alpha semantics:
  - `VersionName`: `0.1.0-alpha`
  - `IsBetaVersion`: `false`
- Synced updated descriptor to `F:\Unreal Projects\FXRA54\Plugins\FX_RetargetAssistant`.
- Ran `RunUAT BuildPlugin` from original Chinese G: path. Result: failed during PCH generation. Evidence suggests UE5.4 BuildPlugin path handling is unstable with the Chinese staging path.
- Re-ran `RunUAT BuildPlugin` from pure-English staging path:
  - Source: `C:\FXRA_UE54_Build\Source\FX_RetargetAssistant`
  - Package: `C:\FXRA_UE54_Build\Package\FX_RetargetAssistant`
  - Result: BUILD SUCCESSFUL.
- Installed packaged plugin into clean UE5.4 project:
  - `C:\FXRA_UE54_Build\CleanProject\Plugins\FX_RetargetAssistant`
- Clean project log evidence:
  - `LogPluginManager: Mounting Project plugin FX_RetargetAssistant`
  - `Executing Class /Script/FX_RetargetAssistant.FX_RetargetAssistantSmokeTestCommandlet`
  - Commandlet returned missing Mixamo source mesh as expected because clean project has no demo content.
- Verified latest `Report.json` fields from previous export:
  - `source`, `output`, `status`, `message`, `summary` all present.
- Verified exported AnimSequences survive a fresh UE commandlet session:
  - Loaded `Center_Block_Anim_RTG`, `Hip_Hop_Dancing_Anim_RTG`, and `Swing_Dancing_Anim_RTG` from `Batch_20260623_1341` successfully.

Updated closure checklist:
- `G:\UE5重定向插件开发\插件\FX_RetargetAssistant_UE54_MVP_Closure_Checklist.md`

Remaining UE5.4 closure work before UE5.5:
- Manual main-flow pass without `Load Mixamo Demo/Test Set`.
- Mismatch and duplicate-output behavior checks.
- Visual preview after editor restart.
- Show Output Folder visual confirmation.

## Phase 15 - FXRA54 Startup Crash Triage / DX11 Workaround (2026-06-23)

Status: resolved for closure testing by switching FXRA54 to DX11.

Issue:
- User reported FXRA54 crashed immediately on open with D3D12 callstack:
  - `Assertion failed: Distance >= 0`
  - `D3D12Adapter.cpp [Line: 1861]`
- This callstack is in UE's D3D12 RHI startup path, not FX_RetargetAssistant plugin code.

Actions:
- Checked `F:\Unreal Projects\FXRA54\Config\DefaultEngine.ini`; project was using `DefaultGraphicsRHI_DX12`.
- Launched with `-NullRHI`; process stayed alive until manually stopped, indicating project/plugin load was not the immediate crash source.
- Changed FXRA54 config to:
  - `DefaultGraphicsRHI=DefaultGraphicsRHI_DX11`
- Launched editor with `-dx11`; process stayed alive.
- Log evidence:
  - `LogRHI: Using Forced RHI: D3D11`
  - `LogRHI: RHI D3D11 with Feature Level SM5 is supported and will be used.`

Notes:
- This is an environment/project startup workaround for UE5.4 closure testing.
- Plugin logic was not changed.
- Closure document updated with the D3D12 crash note and DX11 workaround.

## Phase 16 - AnimSequence Picker UX Fix (2026-06-23)

Status: completed in FXRA54 / UE 5.4.

Issue observed from user screenshot:
- The AnimSequence picker showed `Center_Block_Anim`, but `Selected AnimSequence: 0` remained unchanged.
- Preflight then failed with `NO_ANIMATIONS` because users had to click `Add` after choosing an asset, which was unclear.
- `Add Selected From Content Browser` button text was too long and got clipped in the current panel width.

Implemented:
- AnimSequence picker now auto-adds the selected AnimSequence to the SelectedAnimations list via `OnObjectChanged`.
- It logs: `Added AnimSequence from picker: ... Total=N`.
- Duplicate prevention still goes through `AddAnimationAssetData`.
- Shortened Content Browser button text to `Add Selected`.
- Updated empty-list helper text so it no longer implies the picker requires an additional Add click.

Verification:
- Closed FXRA54 editor to release DLL.
- Synced source to `F:\Unreal Projects\FXRA54\Plugins\FX_RetargetAssistant`.
- Rebuilt successfully with UE5.4 + MSVC 14.38.

Expected visual result:
- Picking `Center_Block_Anim` immediately changes `Selected AnimSequence` from 0 to 1 and shows the asset path in the selected list.
- Preflight no longer fails with `NO_ANIMATIONS` after using the picker.

## Phase 17 - Hide Object Picker Auxiliary Buttons (2026-06-23)

Status: completed in FXRA54 / UE 5.4.

User request:
- Remove the circled auxiliary icon buttons beside asset picker fields.

Implemented:
- Added `.DisplayUseSelected(false)` and `.DisplayBrowse(false)` to the `SObjectPropertyEntryBox` widgets for:
  - Source Skeletal Mesh
  - Target Skeletal Mesh
  - IK Retargeter
  - AnimSequence
- The fields now keep the object picker/dropdown but hide the extra use-selected/browse icons.

Verification:
- Synced source to `F:\Unreal Projects\FXRA54\Plugins\FX_RetargetAssistant`.
- Rebuilt successfully with UE5.4 + MSVC 14.38.

Expected visual result:
- The circled icon buttons are gone from the panel.

## Phase 18 - UE5.4 Alpha Closure Documentation and Packaging Evidence (2026-06-23)

Status: UE5.4 MVP Alpha / manual main flow verified / closure candidate.

Scope lock:
- MVP0 functionality is frozen before UE5.5/UE5.6 adaptation.
- Main flow remains manual: user provides Source Mesh, Target Mesh, existing IK Retargeter, AnimSequence(s), and Output Folder; plugin only batch exports AnimSequence assets and writes Report.json.
- Demo/Test Set remains a developer/demo flow only and must not become the MVP0 main flow.
- Auto Align is limited to plugin-owned TestSet Retargeter `/Game/FX_RetargetAssistant/TestSet/RTG_Mixamo_to_UE5Manny_FXRA`.
- User-selected Retargeters are read-only in the main flow: no Auto Align, no chain mapping, no SavePackage, no Retarget Pose edits.

Documentation updated/created:
- `FX_RetargetAssistant_MVP0_SCOPE.md` is the current execution scope.
- `FX_RetargetAssistant_LEGACY_PLAN_NOTE.md` archives the older one-click IK Rig / Retargeter / chain mapping idea as future/non-MVP scope.
- `FX_RetargetAssistant_BUILD_NOTES.md` records packaging notes, including Chinese-path BuildPlugin failure and English-path success.
- `FX_RetargetAssistant_UE54_MVP_Closure_Checklist.md` tracks final closure items.

Evidence recorded:
- User manually verified the Editor UI main flow succeeds with manually selected assets.
- Latest file-level Retargeter evidence: export output was written after the selected Retargeter `.uasset` timestamps, while Retargeter file timestamps remained unchanged.
- Report.json contains `source`, `output`, `status`, `message`, and `summary`.
- Exported AnimSequences were loaded successfully in a fresh UE commandlet session.
- `RunUAT BuildPlugin` succeeded from pure-English staging path `C:\FXRA_UE54_Build\Source\FX_RetargetAssistant` to `C:\FXRA_UE54_Build\Package\FX_RetargetAssistant`.
- Packaged plugin was installed into clean/manual UE5.4 validation projects and commandlet smoke tests passed.

Remaining before UE5.5:
- Final packaged-plugin Editor UI click-through in `C:\FXRA_UE54_Build\ManualUIProject\FXRA54.uproject`.
- Manual visual checks for Show Output Folder and exported AnimSequence preview after restart.
- Explicit mismatch and duplicate-output checks if not already visually confirmed.

Environment note:
- FXRA54 D3D12 startup crash is tracked as an environment/RHI issue. DX11 is acceptable for current closure testing, but default-RHI smoke test remains required before public release.

## Phase 19 - MVP 1 Alpha UI Start / Output Folder Picker Fix (2026-06-23)

Status: completed in FXRA54 / UE 5.4 build; visual confirmation pending.

User issue:
- The packaged/previous UI still looked like MVP0.
- Output Folder could not be selected conveniently.
- User provided MVP1 direction: Guided Auto Setup Workflow with Source/Target Character Mesh first, Source Skeleton-filtered animations, Retarget Setup step, Export step, Log/Report step.

Implemented in this phase:
- Updated panel title/scope to `FX Retarget Assistant - MVP 1 Alpha` and guided workflow wording.
- Updated visible field labels toward staged flow:
  - Step 1 - Source Character Mesh
  - Step 1 - Target Character Mesh
  - Step 2 - Anim Clip
  - Step 3 - IK Retargeter
  - Step 4 - Output Folder
- Source Character Mesh picker now clears selected animations when changed, so stale animations from a previous Source Skeleton do not remain silently selected.
- Source and Target mesh rows now show read-only Skeleton paths under the mesh picker.
- Anim Clip picker is disabled until Source Character Mesh exists.
- Anim Clip picker uses Source Skeleton filtering through `OnShouldFilterAsset`.
- Add/Add Selected now reject AnimSequences whose Skeleton does not match Source Mesh Skeleton and logs a Warning.
- Output Folder now has a `Browse...` Content Browser path picker and `Use Selected Folder` button, while keeping the editable path field.
- The main button row now shows `Auto Create IK Rig + Retargeter`; it is a safe MVP1 UI placeholder for this phase and does not create or modify assets yet.

Important boundary:
- Retarget & Export still uses the existing batch exporter and does not auto-create setup or auto-repair mapping.
- Auto Create Setup currently only logs the planned setup path; asset creation is intentionally deferred to the next phase.

Verification:
- Synced source to `F:\Unreal Projects\FXRA54\Plugins\FX_RetargetAssistant`.
- Rebuilt FXRA54Editor successfully with UE5.4 and MSVC 14.38.
- Launched FXRA54 with `-dx11` for visual testing.

## Phase 20 - MVP 1 UI Order and Retargeter Open Button (2026-06-23)

Status: completed in FXRA54 / UE 5.4 build; visual confirmation pending.

User feedback:
- The MVP1 UI was usable, but Anim Clip was labeled Step 2 while visually placed after Step 4.
- IK Retargeter picker needed a direct jump/open button so users can manually inspect or edit the Retargeter.

Implemented:
- Reordered the top GridPanel rows so `Step 2 - Anim Clip` appears immediately after Step 1 Source/Target Character Mesh rows.
- Moved `Step 3 - IK Retargeter`, `Step 4 - Output Folder`, and Suffix below Anim Clip.
- Wrapped the IK Retargeter picker in a horizontal row and added an `Open` button.
- `Open` is enabled only when a Retargeter is selected.
- `Open` uses `UAssetEditorSubsystem::OpenEditorForAsset` to open the selected IK Retargeter in the native UE editor.

Boundary:
- No export logic changed.
- No automatic IK Rig / Retargeter creation was added in this phase.
- No automatic mapping repair or Retargeter mutation was added.

Verification:
- Synced source to `F:\Unreal Projects\FXRA54\Plugins\FX_RetargetAssistant`.
- Rebuilt FXRA54Editor successfully with UE5.4 and MSVC 14.38.
- Relaunched FXRA54 with `-dx11` for visual testing.

## Phase 21 - MVP 1 Auto Create Generated Setup (2026-06-23)

Status: implemented and compiled in FXRA54 / UE 5.4; user visual/function test pending.

Implemented:
- Added generic `FFRA_AutoRetargetSetup` and `FFX_RetargetAssistantSetupManager::PrepareAutoRetargetSetup`.
- `Auto Create IK Rig + Retargeter` now creates or loads plugin-owned setup assets under:
  `/Game/FX_RetargetAssistant/Setups/<SourceName>_to_<TargetName>/`
  - `IK_<SourceName>`
  - `IK_<TargetName>`
  - `RTG_<SourceName>_to_<TargetName>`
- UI `Auto Create IK Rig + Retargeter` button now calls the setup manager and automatically selects the generated Retargeter in the IK Retargeter field.
- Generated IK Rigs use heuristic retarget roots and main biped retarget chains.
- Supported first-pass chain/root candidates include UE Manny/Quinn style names and Mixamo/mixamorig variants:
  - Retarget root priority: pelvis / Hips / hips / hip / root
  - Main chains: Root, Pelvis, Spine, Neck, Head, LeftClavicle, LeftArm, RightClavicle, RightArm, LeftLeg, RightLeg
- Generated Retargeter is configured with Source/Target IK Rigs, preview meshes, exact chain automap, CleanAsset, and target Auto Align.

Important boundary:
- The creation/repair/save behavior applies only to plugin-generated assets under `/Game/FX_RetargetAssistant/Setups/...`.
- `Retarget & Export` still does not auto-create setup and does not auto-repair mapping.
- User-selected/manual Retargeters are not modified by export.

Verification:
- Synced source to `F:\Unreal Projects\FXRA54\Plugins\FX_RetargetAssistant`.
- Rebuilt FXRA54Editor successfully with UE5.4 and MSVC 14.38.
- Relaunched FXRA54 with `-dx11` for user testing.

Expected user test:
- Select Source Character Mesh and Target Character Mesh.
- Click `Auto Create IK Rig + Retargeter`.
- Verify generated assets appear under `/Game/FX_RetargetAssistant/Setups/<Source>_to_<Target>/`.
- Verify IK Retargeter field is filled with generated `RTG_<Source>_to_<Target>`.
- Optional: click `Open` next to IK Retargeter and inspect chain mapping / pose.

## Phase 22 - Pelvis Chain Default Unmapped Fix (2026-06-23)

Status: implemented and compiled in FXRA54 / UE 5.4; user visual retarget result check pending.

User feedback:
- Generated Retargeter mapped `Pelvis` to `pelvis` by default.
- This caused severe shaking in the retargeted result.
- Manually changing `Pelvis` mapping to `None` fixed the result.

Implemented:
- After exact chain automap and CleanAsset, generated Retargeter configuration now calls:
  `Controller->SetSourceChain(NAME_None, FName(TEXT("Pelvis")))`
- This makes the target `Pelvis` chain default to `None` for plugin-generated Retargeters.
- The same safety adjustment also applies when updating existing plugin-generated Retargeters; clicking `Auto Create IK Rig + Retargeter` again refreshes and saves the RTG with Pelvis unmapped.
- Log message now states generated Retargeter is configured with exact chain mapping, `Pelvis` unmapped, and target Auto Align.

Rationale:
- The Retarget Root already handles root/pelvis-level motion transfer.
- Mapping a separate one-bone Pelvis chain can double-apply/over-constrain pelvis motion and cause jitter/shaking for Mixamo -> UE Manny style cases.

Verification:
- Synced source to `F:\Unreal Projects\FXRA54\Plugins\FX_RetargetAssistant`.
- Rebuilt FXRA54Editor successfully with UE5.4 and MSVC 14.38.
- Relaunched FXRA54 with `-dx11`.

Expected user test:
- Select Source/Target meshes and click `Auto Create IK Rig + Retargeter` again.
- Open the generated Retargeter.
- Confirm `Pelvis` target chain Source Chain is `None`.
- Retargeted result should no longer show the severe pelvis/body shaking caused by Pelvis -> pelvis mapping.

## Phase 23 - MVP1 Alpha Closure Guardrails and Report Upgrade (2026-06-23)

Status: implemented and compiled in FXRA54 / UE 5.4; closure matrix pending visual/user checks.

Implemented:
- Step 3 label changed from `IK Retargeter` to `Retarget Setup`.
- Added read-only `Setup Status` values:
  - Not Created
  - Ready
  - Needs Review
  - Missing Chains
  - Invalid
- Auto Create IK Rig + Retargeter now defaults to reusing existing generated setup assets instead of reconfiguring/saving on every click.
- Added `Recreate Generated Setup` button for explicit reconfiguration/save of generated assets.
- Added `Auto Repair IK Mapping` button with safety boundary:
  - Retargeters under `/Game/FX_RetargetAssistant/Setups/` can be repaired/saved.
  - Retargeters outside that path are not modified; UI logs a warning instructing the user to copy/generate under FX_RetargetAssistant/Setups first.
- Explicit Recreate clears existing generated IK Rig retarget chains before re-adding chains to avoid duplicate/stale chains.
- Pelvis=None is now treated as Mixamo -> UE preset behavior, not a global rule for all generated setups.
- Report.json now records:
  - setupMode
  - sourceMesh / targetMesh
  - sourceSkeleton / targetSkeleton
  - generatedSourceIKRig
  - generatedTargetIKRig
  - generatedRetargeter
  - chainMappingSummary
  - warnings
- Added MVP1 closure checklist document:
  `G:\UE5重定向插件开发\插件\FX_RetargetAssistant_MVP1_ALPHA_CLOSURE_CHECKLIST.md`

Boundaries preserved:
- Retarget & Export remains pure export.
- Retarget & Export does not auto-create setup.
- Retarget & Export does not auto-repair IK Mapping.
- Retarget & Export does not modify user-selected Retargeters.

Verification:
- Synced source to `F:\Unreal Projects\FXRA54\Plugins\FX_RetargetAssistant`.
- Rebuilt FXRA54Editor successfully with UE5.4 and MSVC 14.38.
- Relaunched FXRA54 with `-dx11` for visual/functional closure testing.

## Phase 24 - Clear Animations UX Fix (2026-06-23)

Status: implemented and compiled in FXRA54 / UE 5.4; user confirmation pending.

User feedback:
- `Clear Animations` appeared to have no effect.

Root cause:
- The button cleared `SelectedAnimations`, but it did not clear `AnimSequenceObject`, so the Anim Clip picker still displayed the previously selected animation. Clicking Add could immediately add it back, making the action feel ineffective.
- `Add` also logged success even if the animation was not actually added due to duplicate or skeleton filtering.

Implemented:
- `ClearAnimations()` now resets both `SelectedAnimations` and `AnimSequenceObject`.
- It refreshes the selected animation list text.
- It logs the number of cleared animation assets.
- `AddPickedAnimation()` now logs `Added` only when `AddAnimationAssetData()` returns true.

Verification:
- Synced source to `F:\Unreal Projects\FXRA54\Plugins\FX_RetargetAssistant`.
- Rebuilt FXRA54Editor successfully with UE5.4 and MSVC 14.38.
- Relaunched FXRA54 with `-dx11` for user validation.

Expected visual result:
- Clicking `Clear Animations` sets selected count to 0.
- The selected animation list becomes empty.
- The Anim Clip picker is cleared.

## Phase 25 - MVP1 Alpha Closure Tightening (2026-06-23)

Status: implemented, synced to FXRA54, compiled in UE5.4, and headless smoke tested; full visual/editor closure matrix still pending.

User direction:
- Current status remains `FX_RetargetAssistant MVP1 Alpha / Guided Auto Setup + Batch Export flow working in UE5.4 / Closure in progress`.
- Do not mark Beta.
- Do not add new features or new skeleton types.
- Do not begin UE5.5 / UE5.6 adaptation until MVP1 Alpha Closure passes.
- Do not affect the separate UE5.8 editor/project if it is running.

Implemented:
- Added a unified `RefreshSetupStatus()` flow and called it after:
  - Source Mesh changes
  - Target Mesh changes
  - Retargeter changes
  - Auto Create Setup
  - Recreate Generated Setup
  - Auto Repair IK Mapping via Recreate
  - Add Animation
  - Clear Animations
- Current Setup Status rules:
  - `Invalid`: Source/Target mesh missing, or setup creation failed.
  - `Not Created`: Source/Target are valid, but no current Retargeter is selected.
  - `Missing Chains`: setup warnings include skipped/missing chain messages.
  - `Needs Review`: usable but needs attention, including manual/user Retargeter, setup warnings, or no selected animations.
  - `Ready`: generated Retargeter selected, no active setup warnings, and at least one valid AnimSequence selected.
- `Report.json` now records:
  - `namingRule.conflictPolicy = "Create Unique Name"`
  - `namingRule.overwrite = false`
- Batch Retarget log now explicitly says output conflicts use Create Unique Name and existing assets are not overwritten.

Boundaries preserved:
- `Retarget & Export` remains pure export.
- It does not auto-create IK Rig / IK Retargeter.
- It does not auto-repair mapping.
- It does not modify user-selected Retargeters.
- Auto Repair continues to warn and do nothing for Retargeters outside `/Game/FX_RetargetAssistant/Setups/`.
- Recreate Generated Setup still routes through plugin-generated setup paths only.
- Pelvis=None remains Mixamo -> UE preset behavior, not a global mapping rule.

Verification:
- Synced source from `G:\UE5重定向插件开发\插件\FX_RetargetAssistant` to `F:\Unreal Projects\FXRA54\Plugins\FX_RetargetAssistant`.
- Built `FXRA54Editor` successfully with UE5.4 and MSVC 14.38.
- Ran `FX_RetargetAssistantSmokeTest` commandlet with UE5.4:
  - Mixamo -> UE5 Manny test setup prepared.
  - Preflight passed for 3 animation(s).
  - Commandlet skipped actual batch export by design because UE5.4 batch retarget export touches Content Browser / Slate.

Remaining Closure matrix:
- Mixamo -> Manny visual export.
- Mixamo -> Quinn visual export.
- Manny -> Quinn visual export.
- Manual Mode / Use Existing Retargeter.
- Auto Repair does not modify user Retargeter.
- Duplicate output naming creates unique assets.
- Restart persistence for generated IK Rig / Retargeter / AnimSequence.
- Source Mesh changed clears animations.
- Add Selected filters wrong Skeleton assets.
- Open Retargeter, manually adjust, then export.

## Phase 26 - Root Family Directional Policy (2026-06-23)

Status: implemented, synced to FXRA54, compiled in UE5.4, and smoke tested for Mixamo -> UE TestSet Preflight. Visual closure still needed for Mixamo -> UE Manny and UE Mannequin -> Mixamo.

User feedback:
- UE Mannequin animation retargeted to Mixamo caused floating / global rotation when Target Chain `Root` mapped to Source Chain `Root`.
- Manually setting Target Chain `Root` to Source Chain `None` fixed the result.
- This belongs to the same family as the earlier Mixamo -> UE fix where Target Chain `Pelvis` needed Source Chain `None` to avoid body shaking.

Implemented:
- Added skeleton family detection:
  - `UEMannequin`: `root`, `pelvis`, `spine_01`, `clavicle_l`, `upperarm_l`, `thigh_l`, etc.
  - `Mixamo`: `Hips`, `Spine`, `LeftArm`, `LeftForeArm`, `LeftUpLeg`, `LeftLeg`, `LeftFoot`, including `mixamorig:` / `mixamorig_` prefixes.
  - fallback: `GenericHumanoid` / `Unknown`.
- Generated IK Rig retarget roots now use directional family rules:
  - UEMannequin: `pelvis`
  - Mixamo: `Hips`
  - fallback: pelvis/Hips/hip/root priority.
- Generated Retargeter setup order is now:
  1. Set Source/Target IK Rig
  2. Set Source/Target preview mesh
  3. Exact Chain Automap
  4. CleanAsset
  5. Reset target retarget pose
  6. Auto Align target pose
  7. ApplyDirectionalRootFamilyOverrides
  8. Save generated Retargeter
- Auto Create now updates existing plugin-generated IK Rigs and Retargeters with current directional policy, while still only touching assets under `/Game/FX_RetargetAssistant/Setups/`.
- Recreate and Auto Repair also go through the same policy path.

Directional root-family policies:
- `UEMannequin -> Mixamo`
  - Source Retarget Root = `pelvis`
  - Target Retarget Root = `Hips`
  - Target Chain `Root` -> Source Chain `None`
  - Pelvis/Hips mapping remains exact.
  - Log: `Applied UE->Mixamo root-family policy: Target Chain Root mapped to None to avoid global root double transform.`
- `Mixamo -> UEMannequin`
  - Source Retarget Root = `Hips`
  - Target Retarget Root = `pelvis`
  - Target Chain `Pelvis` -> Source Chain `None`
  - Target Chain `Root` -> Source Chain `None` for unreliable Mixamo root.
  - Log: `Applied Mixamo->UE root-family policy: Target Chain Pelvis mapped to None to avoid pelvis double transform.`
- `UEMannequin -> UEMannequin`
  - Root -> Root
  - Pelvis -> Pelvis
  - No None override.
- `Mixamo -> Mixamo`
  - Retarget Root uses `Hips`.
  - Target Chain `Root` -> Source Chain `None`.
  - Hips/Pelvis mapping remains exact.

Report.json additions:
- `rootFamilyPolicy`
- `sourceSkeletonFamily`
- `targetSkeletonFamily`
- `sourceRetargetRoot`
- `targetRetargetRoot`
- `rootChainMapping`
- `pelvisChainMapping`
- `chainMappingSummary`

Safety boundary:
- User-selected/manual Retargeters outside `/Game/FX_RetargetAssistant/Setups/` are still read-only.
- Auto Repair still only warns for user assets and does not modify or save them.

Verification:
- Synced plugin source to `F:\Unreal Projects\FXRA54\Plugins\FX_RetargetAssistant`.
- Built `FXRA54Editor` successfully with UE5.4 and MSVC 14.38.
- Ran smoke test commandlet successfully:
  - Mixamo -> UE5 Manny TestSet prepared.
  - Preflight passed for 3 animation(s).

Required visual checks:
- Mixamo -> UE Manny: confirm Target Chain `Pelvis=None` and no shaking.
- UE Mannequin -> Mixamo: confirm Target Chain `Root=None` and no floating/global rotation.

## Phase 27 - UE5.8 Test Asset Preparation + Bring-up Pass 2 Partial (2026-06-24)

Status: UE5.8 test assets prepared and automated bring-up checks passed. Editor UI / visual retarget matrix is still pending and must not be marked as MVP1 functional validation passed yet.

Context:
- Active branch remains `ue58-main`.
- Active project remains `F:\Unreal Projects\FXRA58`.
- Separate UE5.8 project `F:\Unreal Projects\ResearchOnUENewTools` was not opened or modified.

Implemented / prepared:
- Migrated Mixamo test assets into FXRA58 without copying old UE5.4 generated setup assets.
- Final UE5.8 test asset locations:
  - `/Game/FXRA_Imported/MixamoShared/Center_Block`
  - `/Game/FXRA_Imported/MixamoShared/Center_Block_Anim`
  - `/Game/FXRA_Imported/MixamoShared/Hip_Hop_Dancing_Anim`
  - `/Game/FXRA_Imported/MixamoShared/Swing_Dancing_Anim`
  - dependency: `/Game/FXRA_Imported/Mixamo/Center_Block_Skeleton`
- Important asset migration note:
  - Initial FBX automation did not reliably produce required `_Anim` names in UE5.8.
  - Copying only `MixamoShared` first caused AnimSequence assets to load with `Skeleton == nullptr`.
  - Fix was to copy `Center_Block_Skeleton` dependency first, then re-copy `MixamoShared`, then resave with UE5.8.
- ResavePackages passed for `/Game/FXRA_Imported/Mixamo` and `/Game/FXRA_Imported/MixamoShared` with 0 errors and 0 warnings.

Code adjustment:
- `PrepareMixamoToUE5MannyTestSet` no longer depends on old UE5.4 default assets:
  - removed hard requirement for `/Game/Characters/Mannequins/Rigs/IK_Mannequin`
  - target mesh now prefers `/Game/Characters/Mannequins/Meshes/SKM_Manny_Simple`, with `/Game/Characters/Mannequins/Meshes/SKM_Manny` as fallback
  - test set now uses `PrepareAutoRetargetSetup` to generate UE5.8 setup under `/Game/FX_RetargetAssistant/Setups/`

Automated verification:
- Built `FXRA58Editor` successfully with UE5.8 and VS 14.44.
- Asset validation script confirmed:
  - `Center_Block` loads as SkeletalMesh.
  - all 3 Mixamo AnimSequence assets load.
  - `Center_Block` and all 3 AnimSequences share `/Game/FXRA_Imported/Mixamo/Center_Block_Skeleton`.
- `FX_RetargetAssistantSmokeTest` passed:
  - generated `/Game/FX_RetargetAssistant/Setups/Center_Block_to_SKM_Manny_Simple/`
  - generated `IK_Center_Block`
  - generated `IK_SKM_Manny_Simple`
  - generated `RTG_Center_Block_to_SKM_Manny_Simple`
  - applied Mixamo -> UE root-family policy:
    - Source Retarget Root = `Hips`
    - Target Retarget Root = `pelvis`
    - Target Chain `Pelvis=None`
    - Target Chain `Root=None`
  - Preflight ready for 3 animation(s).
- FXRA58 Editor was launched with `-d3d11 -NoLiveCoding -log` and reached project window title `FXRA58 - Unreal Editor`.

Still pending before Pass 2 can be committed:
- Manual/UI verification of the FX Retarget Assistant panel in UE5.8.
- Visual export checks:
  - Mixamo -> UE Manny
  - UE Mannequin -> Mixamo
  - Manny -> Quinn
- Manual Mode / user Retargeter safety.
- Duplicate output naming and Report.json validation from real export.
- Reopen persistence check for generated IK Rig / Retargeter / AnimSequence.

## Phase 28 - UE5.8 Retarget Ops Stack Fix (2026-06-24)

Status: implemented, compiled, smoke-tested, and FXRA58 Editor relaunched for visual verification.

User finding:
- UE5.8 generated Retargeter opened with an empty Op Stack.
- It also showed no effective Auto Align behavior.
- Retargeted animation output was empty.

Root cause:
- UE5.8 Retargeter runtime/export depends on the new `RetargetOps` stack.
- Our generated setup was still mainly configuring old/global chain mapping and target pose data.
- `Controller->AutoMapChains(...)` before creating default Ops did not create the required UE5.8 operation stack.

Fix:
- In both generated retargeter configuration paths, changed the order to:
  1. `SetIKRig(Source/Target)`
  2. `SetPreviewMesh(Source/Target)`
  3. `RemoveAllOps()`
  4. `AddDefaultOps()`
  5. `AutoMapChains(EAutoMapChainType::Exact, true)`
  6. `CleanAsset()`
  7. reset target retarget pose
  8. `AutoAlignAllBones(Target)`
  9. apply directional Root/Pelvis policy
  10. save generated Retargeter
- Uses `FScopedReinitializeIKRetargeter(..., ERetargetRefreshMode::ProcessorAndOpStack)`.
- Updated log text to explicitly mention `UE5.8 default Retarget Ops`.

Verification:
- Synced source to `F:\Unreal Projects\FXRA58\Plugins\FX_RetargetAssistant`.
- Built `FXRA58Editor` successfully.
- Ran `FX_RetargetAssistantSmokeTest` successfully:
  - reused generated IK Rigs
  - reconfigured generated Retargeter with UE5.8 default Retarget Ops
  - applied Mixamo -> UE root-family policy
  - Preflight passed for 3 animation(s)
  - 0 errors / 0 warnings
- Binary inspection of generated `RTG_Center_Block_to_SKM_Manny_Simple.uasset` confirmed serialized op types:
  - `IKRetargetPelvisMotionOp`
  - `IKRetargetFKChainsOp`
  - `IKRetargetRunIKRigOp`
  - `IKRetargetRootMotionOp`
  - `IKRetargetCurveRemapOp`
- Relaunched FXRA58 Editor with fixed plugin.

Important manual step:
- Retargeters generated before this fix, such as `RTG_Hip_Hop_Dancing_to_SKM_Quinn_Simple`, must be recreated once via `Recreate Generated Setup` or regenerated by Auto Create. Old assets may still have empty Op Stack until reconfigured.

## Phase 29 - UE5.8 Retarget Ops Stack Closure Guardrails (2026-06-24)

Status: implemented and ready for commit after build/smoke verification.

User review requirement:
- The UE5.8 Retarget Ops Stack visual fix passed.
- Before commit, Auto Create must not overwrite a valid existing generated Retargeter because users may have manually fine-tuned it in Open Retargeter.
- Missing Retarget Ops Stack must be detected before export and written into Report.json.

Implementation:
- Added `FFX_RetargetAssistantRetargeterUtils` to inspect UE5.8 Retarget Ops Stack.
- Auto Create now reuses existing generated IK Rig assets without saving them.
- Auto Create now reuses existing generated Retargeters without `RemoveAllOps`, `AddDefaultOps`, Auto Align, or Save when their Retarget Ops Stack is valid.
- Auto Create may repair a missing Retarget Ops Stack only for plugin-generated Retargeters under `/Game/FX_RetargetAssistant/Setups/`.
- Recreate Generated Setup remains the explicit rebuild path and may clear/rebuild/save generated setup assets.
- User Retargeters outside `/Game/FX_RetargetAssistant/Setups/` remain read-only.

Preflight / status:
- Preflight now checks `UIKRetargeter::GetRetargetOps()`.
- A missing Retarget Ops Stack is an error and blocks export to avoid empty animations.
- For plugin-generated Retargeters, the error instructs users to use `Recreate Generated Setup`.
- For user Retargeters, the error instructs users to inspect/fix the asset in Unreal's native Retargeter editor.
- Setup Status treats a selected Retargeter with no valid Ops Stack as `Invalid`.

Report.json:
- Added `retargetOpsStackValid`.
- Added `retargetOpsStackCount`.
- Added `retargetOpsStackOpTypeNames`.

Technical conclusion:
- UE5.8 generated IK Retargeter setup is invalid without a Retarget Ops Stack.
- IK Rig references, Preview Mesh, Chain Mapping, Auto Align, and Root/Pelvis policy are not sufficient by themselves.
