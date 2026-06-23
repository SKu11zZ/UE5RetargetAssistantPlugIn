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

## Archived UE5.4 Notes

The UE5.4 packaging and commandlet notes are historical only.

- Do not treat MSVC 14.38 as a current UE5.8 requirement.
- Do not treat the UE5.4 commandlet Content Browser / Slate limitation as a UE5.8 design premise.
- Do not continue UE5.4 / 5.5 / 5.6 / 5.7 BuildPlugin package work on the current mainline.

UE5.8 must be tested on its own toolchain and API behavior.
