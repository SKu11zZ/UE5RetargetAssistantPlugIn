# FX_RetargetAssistant / 重定向小助手

FX_RetargetAssistant is an Unreal Engine editor plugin for guided animation retargeting workflows.

Current project status:

- MVP1 Alpha
- Guided Auto Setup + Batch Export flow working in UE 5.4
- MVP1 Alpha Closure in progress
- UE 5.5 / 5.6 adaptation has not started yet

## MVP1 Alpha Scope

The plugin provides an editor panel for:

- Selecting Source and Target character skeletal meshes
- Adding Source Skeleton-matching AnimSequence assets
- Creating or reusing plugin-generated IK Rig / IK Retargeter setup assets
- Opening the IK Retargeter for manual review
- Batch retargeting selected animations to Animation Sequence assets
- Writing a `Report.json` with setup, chain mapping, warnings, outputs, and summary

## Safety Boundaries

- `Retarget & Export` is pure export behavior.
- It does not automatically create IK Rig / IK Retargeter assets.
- It does not automatically repair chain mappings.
- It does not modify user-selected Retargeters.
- Auto Repair and Recreate only modify plugin-generated setup assets under:
  `/Game/FX_RetargetAssistant/Setups/`
- Manual/user Retargeters outside that path are treated as read-only by default.

## Output Policy

The default output conflict strategy is `Create Unique Name`.

Existing assets are not overwritten by default. Duplicate outputs are expected to become names such as:

- `Walk_RTG`
- `Walk_RTG_001`
- `Walk_RTG_002`

## Development Notes

Primary UE 5.4 development project:

`F:\Unreal Projects\FXRA54`

Primary plugin folder:

`FX_RetargetAssistant/`

Useful project documents:

- `FX_RetargetAssistant_MVP_MEMORY.md`
- `FX_RetargetAssistant_MVP1_ALPHA_CLOSURE_CHECKLIST.md`
- `FX_RetargetAssistant_BUILD_NOTES.md`

## Ownership

This plugin is a personal project and asset owned by the project owner. See `LICENSE`.
