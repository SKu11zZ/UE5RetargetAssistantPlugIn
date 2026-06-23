# FX_RetargetAssistant Legacy Plan Note

`FX_RetargetAssistant_技术方案.md` is preserved as historical planning context.
It is not the current execution contract for MVP0.

Current MVP0 execution and validation must use:

- `FX_RetargetAssistant_MVP0_SCOPE.md`
- `FX_RetargetAssistant_UE54_MVP_Closure_Checklist.md`

For MVP0, the main flow is frozen:

1. User manually provides Source Skeletal Mesh.
2. User manually provides Target Skeletal Mesh.
3. User manually provides an existing IK Retargeter.
4. User manually provides one or more AnimSequence assets.
5. User provides Output Folder and suffix.
6. FX_RetargetAssistant batch exports target AnimSequence assets and writes Report.json.

The following older ideas are archived and must not enter MVP0 main flow:

- One-click automatic user IK Rig creation.
- One-click automatic user IK Retargeter creation.
- Automatic user chain mapping.
- Automatic user retarget pose repair or Auto Align.
- Automatic saving or mutation of user-selected Retargeter assets.
- Bone-name resolver driven setup in the main path.

The Mixamo demo/test set is allowed only as a developer demonstration flow.
It may create or update plugin-owned test assets under:

- `/Game/FX_RetargetAssistant/TestSet`

It must not redefine MVP0 behavior.
