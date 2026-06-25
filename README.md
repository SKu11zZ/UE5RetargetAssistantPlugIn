# FX_RetargetAssistant / 重定向小助手

FX_RetargetAssistant is a UE5.8 editor plugin for guided IK Retargeter setup and batch AnimSequence export.

Current status:

`MVP1 Alpha / UE5.8 Packaged Validation Passed`

This is not Beta and not a Release Candidate. The MVP1 Alpha workflow has passed clean-project packaged validation on UE5.8.

## What It Does

FX_RetargetAssistant does not replace Unreal Engine's native IK Rig or IK Retargeter tools. It wraps the common retargeting workflow with a safer, guided panel:

1. Choose a Source Character Mesh.
2. Choose a Target Character Mesh.
3. Add Source Skeleton-matching AnimSequence assets.
4. Create or reuse generated IK Rig / IK Retargeter setup assets.
5. Open the generated Retargeter for review or manual adjustment.
6. Retarget and export one or more animations.
7. Review output AnimSequence assets and `Report.json`.

The plugin is focused on the common MVP1 use case: Mixamo <-> UE Mannequin style retargeting plus UE Mannequin -> UE Mannequin validation.

## Install

Use the packaged plugin built from a pure-English path:

```text
C:\FXRA_UE58_Build\FX_RetargetAssistant
```

Copy the packaged plugin folder into your UE5.8 project:

```text
YourProject/Plugins/FX_RetargetAssistant
```

Then rebuild or reopen the project. The plugin is editor-only and requires Unreal Engine 5.8.

## Open The Panel

In the Unreal Editor:

```text
Window -> FX Retarget Assistant
```

The panel title should read:

```text
FX Retarget Assistant - MVP1 Alpha / UE5.8
```

## Quick Start

1. Select `Step 1 - Source Character Mesh`.
2. Select `Step 1 - Target Character Mesh`.
3. Use `Step 2 - Anim Clip` or `Add Selected` to add animations that use the Source Mesh skeleton.
4. Click `Auto Create IK Rig + Retargeter`.
5. Check `Setup Status`.
   - `Ready`: safe to continue.
   - `Needs Review`: usable, but inspect warnings or open the Retargeter.
   - `Missing Chains` / `Invalid`: fix setup before exporting.
6. Click `Open` beside `Step 3 - Retarget Setup` to inspect the generated Retargeter in Unreal's native editor.
7. Choose `Step 4 - Output Folder`.
8. Click `Preflight`.
9. Click `Retarget & Export`.
10. Click `Show Output Folder` and inspect the exported AnimSequence assets and `Report.json`.

## Main Buttons

- `Add Selected`: adds selected AnimSequence assets from the Content Browser. Non-matching skeletons are filtered or warned.
- `Auto Create IK Rig + Retargeter`: creates or reuses generated setup assets under `/Game/FX_RetargetAssistant/Setups/`.
- `Recreate Generated Setup`: explicitly rebuilds plugin-generated IK Rig / Retargeter assets. Use this when a generated setup is stale.
- `Auto Repair IK Mapping`: repairs only plugin-generated setup assets. User Retargeters outside the setup folder are not modified.
- `Clear Animations`: clears the selected animation list.
- `Show Output Folder`: syncs Content Browser to the current output folder.
- `Preflight`: checks selected meshes, Retargeter, animations, output path, and UE5.8 Retarget Ops Stack.
- `Retarget & Export`: exports selected AnimSequence assets. It does not create setup assets or modify user Retargeters.

## Safety Boundaries

- `Retarget & Export` is pure export behavior.
- It does not create IK Rig assets.
- It does not create IK Retargeter assets.
- It does not repair mapping.
- It does not Auto Align.
- It does not modify user-selected Retargeters.
- Auto Repair and Recreate only modify plugin-generated setup assets under:

```text
/Game/FX_RetargetAssistant/Setups/
```

- Manual/user Retargeters outside that path are read-only by default.
- `Auto Create` reuses an existing generated setup without overwriting it when the UE5.8 Retarget Ops Stack is valid.
- `Recreate Generated Setup` is the explicit rebuild path for plugin-generated setup assets.

## UE5.8 Retarget Ops Stack

UE5.8 generated IK Retargeters must have a valid Retarget Ops Stack. IK Rig references, preview meshes, chain mapping, Auto Align, and Root Family Directional Policy are not enough by themselves.

- Initial Create and Recreate create UE5.8 default Retarget Ops.
- Existing generated Retargeters with a valid Ops Stack are reused without Remove Ops / Add Default Ops / Save.
- Existing generated Retargeters missing an Ops Stack are treated as invalid setup and may be repaired only inside `/Game/FX_RetargetAssistant/Setups/`.
- User Retargeters outside the plugin setup path are never auto-modified. Preflight reports the missing Ops Stack and blocks export.

## Root Family Directional Policy

Root-family chains are not handled like ordinary Spine, Arm, or Leg chains.

- `UEMannequin -> Mixamo`
  - Source Retarget Root = `pelvis`
  - Target Retarget Root = `Hips`
  - Target Chain `Root` -> Source Chain `None`
  - Target Chain `Pelvis/Hips` -> Source Chain `None`
- `Mixamo -> UEMannequin`
  - Source Retarget Root = `Hips`
  - Target Retarget Root = `pelvis`
  - Target Chain `Root` -> Source Chain `None`
  - Target Chain `Pelvis` -> Source Chain `None`
- `UEMannequin -> UEMannequin`
  - Root -> Root
  - Pelvis -> Pelvis
- `Mixamo -> Mixamo`
  - Retarget Root uses `Hips`
  - Target Chain `Root` defaults to `None`

These rules apply to plugin-generated setup assets and to generated setup assets explicitly repaired under the plugin setup path.

## Output Policy

Default conflict policy: `Create Unique Name`.

Existing assets are not overwritten by default. Duplicate outputs become:

- `Walk_RTG`
- `Walk_RTG_001`
- `Walk_RTG_002`

`Report.json` records:

- `rootFamilyPolicy`
- `rootChainMapping`
- `pelvisChainMapping`
- `retargetOpsStackValid`
- `retargetOpsStackCount`
- `namingRule.conflictPolicy = "Create Unique Name"`
- `namingRule.overwrite = false`

## Known Limitations

- UE5.8 only. Older UE versions are archived and not current targets.
- MVP1 Alpha is validated for Mixamo <-> UE Mannequin style workflows and UE Mannequin -> UE Mannequin.
- MetaHuman, CC4, finger-chain polish, advanced Root Motion controls, and broad skeleton presets are intentionally out of scope for this stabilization phase.
- The plugin cannot guarantee perfect motion quality for every skeleton. Always open the generated Retargeter and visually inspect important clips.
- User-owned Retargeters outside `/Game/FX_RetargetAssistant/Setups/` are not auto-repaired or saved.
- Use short project names for validation projects. UE warns when project names exceed 20 characters; use `FXRA58PkgVal` instead of long names such as `FXRA58PackagedValidation`.

## Validation Record

The current packaged validation was run in a clean UE5.8 C++ project with the packaged plugin installed as a project plugin.

Validated:

- Plugin mounts in a clean UE5.8 project.
- `Window -> FX Retarget Assistant` opens the panel.
- Panel title shows `FX Retarget Assistant - MVP1 Alpha / UE5.8`.
- Generated IK Rig / IK Retargeter assets are created fresh in the clean project.
- UE5.8 Retarget Ops Stack exists.
- Root Family Directional Policy is recorded in `Report.json`.
- Duplicate exports generate `_001` and `_002`.
- Generated IK Rig / RTG / AnimSequence assets load after reopening.

## Development Notes

Active source branch:

```text
ue58-main
```

Current UE5.8 development project:

```text
F:\Unreal Projects\FXRA58
```

Current clean validation project name for future runs:

```text
FXRA58PkgVal
```

## Ownership

This plugin is a personal project and asset owned by the project owner. See `LICENSE`.
