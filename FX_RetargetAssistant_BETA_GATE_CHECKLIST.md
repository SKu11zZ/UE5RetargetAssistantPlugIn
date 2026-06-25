# FX_RetargetAssistant Beta-Gate Checklist

Current phase:

```text
MVP1 Alpha Beta-Gate / Release Hygiene Pass
```

Current state:

```text
FX_RetargetAssistant MVP1 Alpha / Beta-Gate Hygiene Pass Completed
```

This is not Beta and not a Release Candidate. This checklist records the release-hygiene gate for sharing the current UE5.8 MVP1 Alpha package with testers.

## Scope Lock

- UE5.8 only.
- No UE5.4 / 5.5 / 5.6 / 5.7 compatibility work in this phase.
- No MetaHuman expansion.
- No CC4 expansion.
- No finger-chain polish expansion.
- No advanced Root Motion feature expansion.
- No new skeleton-family presets beyond the currently validated Mixamo <-> UE Mannequin and UE -> UE paths.

## P0 Beta-Gate Checks

| Check | Status | Evidence |
| --- | --- | --- |
| Rebuild packaged plugin with UE5.8 | Passed | `RunUAT BuildPlugin` succeeded to `C:\FXRA_UE58_Build\FX_RetargetAssistant`. |
| Packaged plugin contains no project content assets | Passed | Package contains `Source`, `Binaries`, `Intermediate`, and `FX_RetargetAssistant.uplugin`; no `Content` folder. |
| Clean C++ validation project uses short name | Passed | Created `F:\Unreal Projects\FXRA58PkgVal`. |
| Clean project builds with packaged plugin | Passed | `FXRA58PkgValEditor Win64 Development` succeeded. |
| Plugin mounts in clean project | Passed | Clean project build and commandlet validation load the project plugin. |
| UI panel entry/title | Passed | Source registers `Window -> FX Retarget Assistant`; title string is `FX Retarget Assistant - MVP1 Alpha / UE5.8`. |
| FunctionalValidation | Passed | `FX_RetargetAssistantSmokeTest -FunctionalValidation` passed in `FXRA58PkgVal`. |
| Mixamo -> UE Manny | Passed | Auto setup/export passed; root-family policy recorded in `Report.json`. |
| UE Manny -> Mixamo | Passed | Auto setup/export passed; `Root=None` and `Pelvis/Hips=None` policy recorded. |
| Manny -> Quinn | Passed | Auto setup/export passed; UE -> UE Root/Pelvis exact mapping recorded. |
| Duplicate naming | Passed | Generated `Hip_Hop_Dancing_Anim_RTG`, `_001`, and `_002`; no overwrite. |
| Report.json fields | Passed | Engine version, setup mode, meshes, skeletons, generated setup assets, root-family policy, ops stack, naming rule, animations, summary, and warnings verified. |
| User Retargeter safety | Passed | External `/Game/FXRA_UserTest/RTG_UserOwned_Test` received Warning only; no auto-modify/save. |
| Reopen asset load check | Passed | Generated IK Rig, RTG, AnimSequence assets, duplicate outputs, and `Report.json` loaded in a fresh commandlet process. |
| UI bug bash | Passed | Button labels, tooltips, Preflight guidance, Show Output Folder wording, and Quick Start alignment reviewed. |
| Repository hygiene | Passed | No generated assets, exports, Binaries, Intermediate, Saved, or DDC committed. |
| `git diff --check` | Passed | No whitespace errors. |

## Safety Boundaries To Preserve

- `Retarget & Export` only exports selected AnimSequence assets.
- `Retarget & Export` does not create IK Rig assets.
- `Retarget & Export` does not create IK Retargeter assets.
- `Retarget & Export` does not Auto Repair mapping.
- `Retarget & Export` does not Auto Align.
- `Retarget & Export` does not modify or save user Retargeters.
- `Auto Repair IK Mapping` may only modify plugin-generated Retargeters under `/Game/FX_RetargetAssistant/Setups/`.
- User Retargeters outside `/Game/FX_RetargetAssistant/Setups/` are read-only.
- `Auto Create IK Rig + Retargeter` reuses valid generated RTGs without overwriting manual edits.
- `Recreate Generated Setup` is the explicit rebuild path.

## Beta Entry Still Requires

- Fresh user-facing test pass from a second machine or user environment.
- Manual UI pass in a real editor viewport after packaging.
- At least one fresh visual check for Mixamo -> UE Manny and UE Manny -> Mixamo using tester-owned assets.
- Confirm final distribution zip contents and installation instructions.
- Decide Beta version number and tag policy.

## Current Non-Beta Status

```text
FX_RetargetAssistant MVP1 Alpha / Beta-Gate Hygiene Pass Completed
```

Do not label this as Beta or Release Candidate yet.
