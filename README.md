# FX_RetargetAssistant / 重定向小助手

Current active target: **Unreal Engine 5.8 only**.

Current phase: **MVP1 Alpha UE5.8 Migration / Guided Auto Setup + Batch Export**.

The UE5.4 implementation is archived. It proved the workflow, but it is no longer the active development target or compatibility constraint.

## Positioning

FX_RetargetAssistant is a UE5.8 Editor Plugin. It does not replace Unreal Engine's native retargeting system. It provides a guided setup and batch export workflow on top of UE5.8 IK Rig, IK Retargeter, and Batch Retarget APIs.

One-line flow:

Select a Source character, Target character, and Source animations; the plugin creates or reuses IK Rig / IK Retargeter setup assets, applies UE5.8 native setup behavior plus FX_RetargetAssistant Root Family Directional Policy, then batch exports Target-ready AnimSequence assets.

## Main Workflow

1. Select Source Character Mesh.
2. Select Target Character Mesh.
3. Add Source Skeleton-matching AnimSequence assets.
4. Auto Create IK Rig + Retargeter.
5. Review or fine-tune the generated Retargeter in Unreal's native editor.
6. Optionally Auto Repair IK Mapping for plugin-generated setup assets.
7. Choose Output Folder, Suffix, and Conflict Policy.
8. Retarget & Export.
9. Review `Report.json` and synced output assets in Content Browser.

## Safety Boundaries

- `Retarget & Export` is pure export behavior.
- It does not create IK Rig assets.
- It does not create IK Retargeter assets.
- It does not repair mapping.
- It does not Auto Align.
- It does not modify user-selected Retargeters.
- Auto Repair and Recreate only modify plugin-generated setup assets under:
  `/Game/FX_RetargetAssistant/Setups/`
- Manual/user Retargeters outside that path are treated as read-only by default.
- `Auto Create` reuses an existing generated setup without overwriting it when its UE5.8 Retarget Ops Stack is valid.
- `Recreate Generated Setup` is the explicit destructive/rebuild path for plugin-generated setup assets.

## UE5.8 Retarget Ops Stack

UE5.8 generated IK Retargeters must have a valid Retarget Ops Stack. Creating IK Rigs, preview meshes, chain mapping, Auto Align, and Root Family Directional Policy is not enough by itself.

- Initial Create and Recreate create UE5.8 default Retarget Ops.
- Existing generated Retargeters with a valid Ops Stack are reused without Remove Ops / Add Default Ops / Save.
- Existing generated Retargeters missing an Ops Stack are treated as invalid setup and may be repaired only inside `/Game/FX_RetargetAssistant/Setups/`.
- User Retargeters outside the plugin setup path are never auto-modified; Preflight reports the missing Ops Stack and blocks export.

## Root Family Directional Policy

Root-family chains are not handled like ordinary Spine/Arm/Leg exact chain mapping.

- `UEMannequin -> Mixamo`: Target Chain `Root` maps to `None`.
- `Mixamo -> UEMannequin`: Target Chain `Pelvis` maps to `None`; Target Chain `Root` also maps to `None`.
- `UEMannequin -> UEMannequin`: Root and Pelvis remain exact.
- `Mixamo -> Mixamo`: Retarget Root uses `Hips`; Target Chain `Root` defaults to `None`.

These rules apply only to plugin-generated setup assets, or to copied assets explicitly repaired under the plugin setup path.

## Output Policy

Default conflict policy: `Create Unique Name`.

Existing assets are not overwritten by default. Duplicate outputs should become:

- `Walk_RTG`
- `Walk_RTG_001`
- `Walk_RTG_002`

`Report.json` records:

- `namingRule.conflictPolicy = "Create Unique Name"`
- `namingRule.overwrite = false`

## Development Paths

Active UE5.8 project:

`F:\Unreal Projects\FXRA58`

Active plugin destination:

`F:\Unreal Projects\FXRA58\Plugins\FX_RetargetAssistant`

Source repository plugin folder:

`G:\UE5重定向插件开发\插件\FX_RetargetAssistant`

## Ownership

This plugin is a personal project and asset owned by the project owner. See `LICENSE`.
