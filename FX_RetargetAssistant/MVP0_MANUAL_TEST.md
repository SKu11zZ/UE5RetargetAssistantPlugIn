# FX_RetargetAssistant Manual Test Archive

Status: archived MVP0 manual test.

MVP0 validated the early "Use Existing Retargeter / Batch Export" flow in UE5.4. It is not the current active workflow.

Current active target:

- Unreal Engine 5.8 only
- Project: `F:\Unreal Projects\FXRA58\FXRA58.uproject`
- Current phase: MVP1 Alpha UE5.8 Migration / Guided Auto Setup + Batch Export

## Current UE5.8 Manual Test Entry

Use the UE5.8 migration checklist at repository root:

`FX_RetargetAssistant_MVP1_ALPHA_CLOSURE_CHECKLIST.md`

The current manual validation focus is:

1. Mixamo -> UE Manny: `Pelvis=None`, no shaking.
2. UE Mannequin -> Mixamo: `Root=None`, no floating/global rotation.
3. Manny -> Quinn: Root/Pelvis exact, no None special rule.
4. Manual Mode: user-selected Retargeter is not modified.
5. Auto Repair: only repairs plugin-generated assets under `/Game/FX_RetargetAssistant/Setups/`.
6. Recreate Generated Setup: does not touch user assets.
7. Duplicate export naming creates `_001`, `_002`, no overwrite.
8. Open Retargeter -> manual adjust -> export.
9. Reopen UE5.8 project and verify generated assets reload.
