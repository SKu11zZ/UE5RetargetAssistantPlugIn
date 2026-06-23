# FX_RetargetAssistant MVP0 Archive

Status: archived historical validation phase.

MVP0 was the early "Use Existing Retargeter / Batch Export" validation pass. It proved that selected AnimSequence assets could be batch exported through an existing IK Retargeter, but it is no longer the active product scope.

Current active target:

- Unreal Engine 5.8 only
- MVP1 Alpha UE5.8 Migration
- Guided Auto Setup + Batch Export

Current default goal:

The user selects Source Character Mesh, Target Character Mesh, and Source Skeleton-matching animations. FX_RetargetAssistant creates or reuses generated IK Rig / IK Retargeter setup assets, applies Root Family Directional Policy, and batch exports target-ready AnimSequence assets.

Manual Mode remains supported:

- A user may still provide an existing IK Retargeter.
- `Retarget & Export` uses it as-is.
- The plugin must not modify user-provided Retargeters outside `/Game/FX_RetargetAssistant/Setups/`.

Archived MVP0 boundaries remain useful as safety rules, not as the main flow:

- Export should not silently modify Retargeters.
- Existing assets should not be overwritten by default.
- Reports should record source, output, status, message, and summary.

UE5.4 notes in older conversations are historical only. They are not UE5.8 design constraints.
