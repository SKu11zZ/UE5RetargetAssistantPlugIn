# FX_RetargetAssistant MVP 0 Manual Test

Project:

```text
F:\Unreal Projects\FXRA54\FXRA54.uproject
```

Engine:

```text
F:\Epic Games\UE_5.4
```

## Build

```powershell
& 'F:\Epic Games\UE_5.4\Engine\Build\BatchFiles\Build.bat' FXRA54Editor Win64 Development -Project='F:\Unreal Projects\FXRA54\FXRA54.uproject' -WaitMutex -NoHotReloadFromIDE '-CompilerVersion=14.38.33130'
```

Expected result:

```text
Result: build succeeded.
```

## Test Assets

Use these existing Third Person template assets:

```text
Source Skeletal Mesh:
/Game/Characters/Mannequins/Meshes/SKM_Manny

Target Skeletal Mesh:
/Game/Characters/Mannequins/Meshes/SKM_Quinn

IK Retargeter:
/Game/Characters/Mannequins/Rigs/RTG_Mannequin

Source AnimSequence candidates:
/Game/Characters/Mannequins/Animations/Manny/MM_Walk_Fwd
/Game/Characters/Mannequins/Animations/Manny/MM_Run_Fwd
/Game/Characters/Mannequins/Animations/Manny/MM_Idle
```

## Manual Flow

1. Open `FXRA54.uproject` in UE 5.4.
2. Open `Window -> FX Retarget Assistant`.
3. Set `Source Skeletal Mesh` to `SKM_Manny`.
4. Set `Target Skeletal Mesh` to `SKM_Quinn`.
5. Set `IK Retargeter` to `RTG_Mannequin`.
6. In Content Browser, select one or more Manny AnimSequence assets, for example `MM_Walk_Fwd`, `MM_Run_Fwd`, `MM_Idle`.
7. Click `Use Selected AnimSequence`.
8. Confirm the selected count appears in the panel.
9. Keep Output Folder as the default:

```text
/Game/FX_RetargetAssistant/Exports/Batch_YYYYMMDD_HHMM/Animations
```

10. Keep suffix as:

```text
_RTG
```

11. Click `Preflight`.
12. Expected: no blocking Error.
13. Click `Batch Retarget`.
14. Expected:
    - New target AnimSequence assets are created under the output folder.
    - Each generated asset has suffix `_RTG`.
    - `Report.json` is written next to the output folder on disk.
    - Log panel reports total, success, and failed counts.

## MVP 0 Boundary

This manual test must not require:

- Automatic IK Rig creation.
- Automatic IK Retargeter creation.
- Bone Name Resolver.
- Automatic Retarget Root correction.
- Automatic T Pose / A Pose correction.
- Folder recursive scan.
- Multi-source or multi-target matrix processing.

