# FX_RetargetAssistant Build Notes

Current active target: **Unreal Engine 5.8 only**.

Active project:

`F:\Unreal Projects\FXRA58`

Active engine:

`F:\Epic Games\UE_5.8`

Active branch:

`ue58-main`

## UE5.8 Build Plan

1. Copy the repository plugin folder into:
   `F:\Unreal Projects\FXRA58\Plugins\FX_RetargetAssistant`
2. Remove generated plugin artifacts in the target project:
   - `Binaries/`
   - `Intermediate/`
   - `Saved/`
   - `.vs/` if present
3. Regenerate project files using UE5.8.
4. Build `FXRA58Editor`.
5. Fix UE5.8 API compile errors directly against UE5.8 APIs.
6. Remove or minimize historical low-version conditional compilation.

## UE5.8 Build Result - First Migration Pass

Result: passed.

Command:

```powershell
& 'F:\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat' FXRA58Editor Win64 Development -Project='F:\Unreal Projects\FXRA58\FXRA58.uproject' -WaitMutex -NoHotReloadFromIDE
```

Toolchain observed:

- Unreal Engine 5.8
- Visual Studio 14.44.35228 toolchain
- Windows 10.0.26100.0 SDK

Notes:

- UBT invalidated the makefile and processed project build data.
- UBA retried several compile actions because the machine was under memory pressure.
- Final result succeeded.
- `ResearchOnUENewTools` was left running and was not closed or modified.

## UE5.8 Bring-up Pass 1

Result: partial pass.

Build command still succeeds after the missing-test-asset warning-path update.

Smoke test command:

```powershell
& 'F:\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor-Cmd.exe' 'F:\Unreal Projects\FXRA58\FXRA58.uproject' -run=FX_RetargetAssistantSmokeTest -unattended -nop4 -nullrhi -log
```

Observed result:

- 0 errors.
- Expected warnings because FXRA58 currently lacks the optional Mixamo validation assets and old UE5.4-style `IK_Mannequin` path.
- The tool logs:
  `[Warning] Required test assets are missing in FXRA58. Please import or migrate test assets.`

FXRA58 currently has:

- `SKM_Manny_Simple`
- `SKM_Quinn_Simple`
- `SK_Mannequin`
- Mannequin animation assets under `/Game/Characters/Mannequins/Anims/`

FXRA58 currently lacks:

- `/Game/FXRA_Imported/MixamoShared/Center_Block`
- `/Game/Characters/Mannequins/Rigs/IK_Mannequin`
- `/Game/Characters/Mannequins/Rigs/RTG_Mannequin`

## Archived UE5.4 Notes

The UE5.4 packaging and commandlet notes are historical only.

- Do not treat MSVC 14.38 as a current UE5.8 requirement.
- Do not treat the UE5.4 commandlet Content Browser / Slate limitation as a UE5.8 design premise.
- Do not continue UE5.4 / 5.5 / 5.6 / 5.7 BuildPlugin package work on the current mainline.

UE5.8 must be tested on its own toolchain and API behavior.

## UE5.8 Packaged Plugin Build

Result: passed.

Use a pure-English package path for `RunUAT BuildPlugin`.

Command:

```powershell
& 'F:\Epic Games\UE_5.8\Engine\Build\BatchFiles\RunUAT.bat' BuildPlugin -Plugin='G:\UE5重定向插件开发\插件\FX_RetargetAssistant\FX_RetargetAssistant.uplugin' -Package='C:\FXRA_UE58_Build\FX_RetargetAssistant' -TargetPlatforms=Win64 -Rocket
```

Output:

```text
C:\FXRA_UE58_Build\FX_RetargetAssistant
```

Observed result:

- `BUILD SUCCESSFUL`
- Packaged plugin contains `Source`, `Binaries`, `Intermediate`, and `FX_RetargetAssistant.uplugin`.
- Packaged plugin does not contain a `Content` folder.
- Therefore the packaged plugin does not carry FXRA58 temporary setup/export/test content assets.

## UE5.8 Clean Project Validation

Result: passed.

Validation project used during this phase:

```text
F:\Unreal Projects\FXRA58PackagedValidation
```

Note: UE warns that project names must not be longer than 20 characters. Future clean validation projects should use:

```text
FXRA58PkgVal
```

Validated in the clean project:

- Packaged plugin mounts as a project plugin.
- `Window -> FX Retarget Assistant` opens the panel.
- Panel title is `FX Retarget Assistant - MVP1 Alpha / UE5.8`.
- Minimal Mixamo and UE Manny / Quinn assets can drive the workflow.
- Generated IK Rig / IK Retargeter assets are created fresh under `/Game/FX_RetargetAssistant/Setups/`.
- UE5.8 Retarget Ops Stack exists and is recorded in `Report.json`.
- Root Family Directional Policy is recorded in `Report.json`.
- Duplicate output naming creates `_001` and `_002`.
- Generated IK Rig / RTG / AnimSequence assets load in a fresh Unreal commandlet process.

Status after validation:

```text
FX_RetargetAssistant MVP1 Alpha / UE5.8 Packaged Validation Passed
```

This is not Beta and not a Release Candidate.
