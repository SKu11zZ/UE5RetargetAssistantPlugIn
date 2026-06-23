# FX_RetargetAssistant MVP0 Scope

Status: UE5.4 MVP Alpha / Closure candidate.

This document is the current execution scope for MVP0.

## Product Boundary

FX_RetargetAssistant MVP0 is an Unreal Editor batch retarget export assistant.

The MVP0 main flow does not create or repair the retargeting setup. It uses a setup the user already prepared in Unreal.

## Main Flow

The user manually provides:

1. Source Skeletal Mesh
2. Target Skeletal Mesh
3. Existing IK Retargeter
4. One or more source AnimSequence assets
5. Output Folder
6. Output suffix

The plugin performs:

1. Preflight validation
2. Batch duplicate-and-retarget through UE's existing IK Retargeter batch operation
3. Per-animation success or failure logging
4. Animation Sequence saving
5. Report.json generation
6. Optional Content Browser sync to the output folder

## Explicit Non-Goals For MVP0

The main flow must not:

- Create user IK Rig assets
- Create user IK Retargeter assets
- Auto-map user retarget chains
- Auto-align or repair a user-selected Retargeter pose
- Save or modify a user-selected Retargeter asset
- Run a bone-name resolver in the main path
- Perform multi-source / multi-target matrix retargeting
- Provide runtime retargeting

Any future repair or setup feature must be a separate explicit action with clear user confirmation.

## Demo/Test Set Boundary

`Load Mixamo Demo/Test Set` is not the MVP0 main flow.

It is allowed only as a developer demonstration fixture. It may create or update plugin-owned assets under:

- `/Game/FX_RetargetAssistant/TestSet/IK_Mixamo_FXRA`
- `/Game/FX_RetargetAssistant/TestSet/RTG_Mixamo_to_UE5Manny_FXRA`

Auto Align is allowed only for that plugin-owned test Retargeter.

## Same-Name Output Strategy

MVP0 default policy:

- Do not overwrite existing output assets.
- Use suffix `_RTG` by default.
- If an output asset name already exists, UE's duplicate/retarget operation should generate a unique name, such as:
  - `Walk_RTG`
  - `Walk_RTG_001`
  - `Walk_RTG_002`
- Report.json records the actual generated output object path.

MVP0 does not expose an overwrite toggle.

## Current Closure Evidence

- UE5.4 project compile passes.
- Manual main flow has been user-confirmed working.
- Report.json records source, output, status, message, and summary.
- Packaged plugin builds from an English path.
- Packaged plugin can be mounted and commandlet-discovered in a clean UE5.4 project.

Remaining closure items are tracked in:

- `FX_RetargetAssistant_UE54_MVP_Closure_Checklist.md`
