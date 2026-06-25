# FX_RetargetAssistant / 重定向小助手

中文 | [English](#english)

FX_RetargetAssistant（重定向小助手）是一个面向 Unreal Engine 5.8 的编辑器插件，用于辅助创建 IK Retargeter 设置，并批量导出重定向后的 AnimSequence。

当前状态：

```text
MVP1 Alpha / Beta-Gate Hygiene Pass Completed
```

这不是 Beta，也不是 Release Candidate。当前 MVP1 Alpha 流程已经通过 UE5.8 干净项目 packaged plugin 验证，并完成 Beta-Gate release-hygiene pass。

## 功能概览

FX_RetargetAssistant 不替代 Unreal Engine 原生 IK Rig / IK Retargeter 工具。它的目标是把常见动画重定向流程包成一个更安全、更清晰的引导式面板：

1. 选择 Source Character Mesh。
2. 选择 Target Character Mesh。
3. 添加匹配 Source Skeleton 的 AnimSequence。
4. 创建或复用插件生成的 IK Rig / IK Retargeter setup 资产。
5. 打开生成的 Retargeter，进行检查或手动微调。
6. 批量重定向并导出一个或多个动画。
7. 检查导出的 AnimSequence 和 `Report.json`。

当前 MVP1 重点验证路径：

- Mixamo -> UE Mannequin
- UE Mannequin -> Mixamo
- UE Mannequin -> UE Mannequin

## 安装

使用纯英文路径打包出来的插件：

```text
C:\FXRA_UE58_Build\FX_RetargetAssistant
```

把 packaged plugin 文件夹复制到你的 UE5.8 项目：

```text
YourProject/Plugins/FX_RetargetAssistant
```

然后重新编译或重新打开项目。该插件是 editor-only 插件，目前只支持 Unreal Engine 5.8。

## 打开面板

在 Unreal Editor 中：

```text
Window -> FX Retarget Assistant
```

面板标题应显示：

```text
FX Retarget Assistant - MVP1 Alpha / UE5.8
```

## 快速开始

1. 选择 `Step 1 - Source Character Mesh`。
2. 选择 `Step 1 - Target Character Mesh`。
3. 使用 `Step 2 - Anim Clip` 或 `Add Selected` 添加使用 Source Mesh skeleton 的动画。
4. 点击 `Auto Create IK Rig + Retargeter`。
5. 检查 `Setup Status`。
   - `Ready`：可以继续。
   - `Needs Review`：可用，但建议检查 warning 或打开 Retargeter。
   - `Missing Chains` / `Invalid`：导出前需要先修复 setup。
6. 点击 `Step 3 - Retarget Setup` 旁边的 `Open`，在 UE 原生 Retargeter 编辑器中检查生成的 Retargeter。
7. 选择 `Step 4 - Output Folder`。
8. 点击 `Preflight`。
9. 点击 `Retarget & Export`。
10. 点击 `Show Output Folder`，检查导出的 AnimSequence 和 `Report.json`。

## 主要按钮

- `Add Selected`：从 Content Browser 添加选中的 AnimSequence。Skeleton 不匹配的动画会被过滤或提示 warning。
- `Auto Create IK Rig + Retargeter`：在 `/Game/FX_RetargetAssistant/Setups/` 下创建或复用插件生成的 setup 资产。
- `Recreate Generated Setup`：显式重建插件生成的 IK Rig / IK Retargeter。用于生成 setup 过期或需要重建的情况。
- `Auto Repair IK Mapping`：只修复插件生成的 setup 资产。不会修改 setup 文件夹外的用户 Retargeter。
- `Clear Animations`：清空当前动画列表。
- `Show Output Folder`：在 Content Browser 中定位当前输出文件夹。
- `Preflight`：检查 mesh、Retargeter、动画、输出路径和 UE5.8 Retarget Ops Stack。
- `Retarget & Export`：导出选中的 AnimSequence。不会创建 setup 资产，也不会修改用户 Retargeter。

## 安全边界

- `Retarget & Export` 只负责导出。
- 它不会创建 IK Rig。
- 它不会创建 IK Retargeter。
- 它不会修复 mapping。
- 它不会 Auto Align。
- 它不会修改用户手动选择的 Retargeter。
- Auto Repair 和 Recreate 只允许修改插件生成路径下的 setup 资产：

```text
/Game/FX_RetargetAssistant/Setups/
```

- 该路径外的手动/用户 Retargeter 默认只读。
- `Auto Create` 在已有 generated setup 且 UE5.8 Retarget Ops Stack 有效时会复用资产，不覆盖用户可能做过的手动微调。
- `Recreate Generated Setup` 才是显式重建 generated setup 的路径。

## UE5.8 Retarget Ops Stack

UE5.8 中，生成式 IK Retargeter 必须拥有有效的 Retarget Ops Stack。只配置 IK Rig 引用、Preview Mesh、Chain Mapping、Auto Align 和 Root Family Directional Policy 还不够。

- Initial Create 和 Recreate 会创建 UE5.8 默认 Retarget Ops。
- 已存在且 Ops Stack 有效的 generated Retargeter 会被复用，不执行 Remove Ops / Add Default Ops / Save。
- 缺少 Ops Stack 的 generated Retargeter 会被视为 invalid setup，并且只允许在 `/Game/FX_RetargetAssistant/Setups/` 内修复。
- 用户 Retargeter 不会被自动修改。Preflight 会报告缺失 Ops Stack 并阻止导出。

## Root Family Directional Policy

Root-family chain 不能像 Spine、Arm、Leg 一样做普通 Exact Automap。

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
  - Retarget Root 使用 `Hips`
  - Target Chain `Root` 默认 `None`

这些规则只作用于插件生成的 setup 资产，以及用户显式修复的插件生成 setup 资产。

## 输出策略

默认冲突策略：`Create Unique Name`。

默认不覆盖已有资产。同名导出会生成：

- `Walk_RTG`
- `Walk_RTG_001`
- `Walk_RTG_002`

`Report.json` 会记录：

- `rootFamilyPolicy`
- `rootChainMapping`
- `pelvisChainMapping`
- `retargetOpsStackValid`
- `retargetOpsStackCount`
- `namingRule.conflictPolicy = "Create Unique Name"`
- `namingRule.overwrite = false`

## 已知限制

- 当前只支持 UE5.8。旧版本 UE 已归档，不是当前主线目标。
- MVP1 Alpha 已验证 Mixamo <-> UE Mannequin 风格流程，以及 UE Mannequin -> UE Mannequin。
- MetaHuman、CC4、手指链精修、高级 Root Motion 控制和更多骨架 preset 暂不在本阶段范围内。
- 插件不能保证每一种骨架都得到完美动作质量。重要动画仍建议打开生成的 Retargeter 做视觉检查。
- `/Game/FX_RetargetAssistant/Setups/` 外的用户 Retargeter 不会被自动修复或保存。
- 验证项目建议使用短项目名。UE 会提示项目名不要超过 20 个字符；建议使用 `FXRA58PkgVal`，不要使用 `FXRA58PackagedValidation` 这类长名字。

## 验证记录

当前 packaged validation 在一个干净 UE5.8 C++ 项目中完成，插件以 project plugin 形式安装。

最新干净验证项目：

```text
F:\Unreal Projects\FXRA58PkgVal
```

已验证：

- 插件可以在干净 UE5.8 项目中 mount。
- `Window -> FX Retarget Assistant` 可以打开面板。
- 面板标题显示 `FX Retarget Assistant - MVP1 Alpha / UE5.8`。
- Generated IK Rig / IK Retargeter 可以在干净项目中重新创建。
- UE5.8 Retarget Ops Stack 存在。
- Root Family Directional Policy 会写入 `Report.json`。
- 重复导出会生成 `_001` 和 `_002`。
- 重开后，生成的 IK Rig / RTG / AnimSequence 可以加载。
- `/Game/FX_RetargetAssistant/Setups/` 外的用户 Retargeter 在 Auto Repair 中保持只读。
- 提交前 `git diff --check` 通过。

## 开发备注

当前源码分支：

```text
ue58-main
```

当前 UE5.8 开发项目：

```text
F:\Unreal Projects\FXRA58
```

后续干净验证项目建议名称：

```text
FXRA58PkgVal
```

## 所有权

本插件是项目所有者的个人项目和个人资产。详见 `LICENSE`。

---

## English

FX_RetargetAssistant is a UE5.8 editor plugin for guided IK Retargeter setup and batch AnimSequence export.

Current status:

```text
MVP1 Alpha / Beta-Gate Hygiene Pass Completed
```

This is not Beta and not a Release Candidate. The MVP1 Alpha workflow has passed UE5.8 clean-project packaged validation and the Beta-Gate release-hygiene pass.

## What It Does

FX_RetargetAssistant does not replace Unreal Engine's native IK Rig or IK Retargeter tools. It wraps the common retargeting workflow with a safer, guided panel:

1. Choose a Source Character Mesh.
2. Choose a Target Character Mesh.
3. Add Source Skeleton-matching AnimSequence assets.
4. Create or reuse generated IK Rig / IK Retargeter setup assets.
5. Open the generated Retargeter for review or manual adjustment.
6. Retarget and export one or more animations.
7. Review output AnimSequence assets and `Report.json`.

The current MVP1 validation focus is:

- Mixamo -> UE Mannequin
- UE Mannequin -> Mixamo
- UE Mannequin -> UE Mannequin

## Install

Use the packaged plugin built from a pure-English path:

```text
C:\FXRA_UE58_Build\FX_RetargetAssistant
```

Copy the packaged plugin folder into your UE5.8 project:

```text
YourProject/Plugins/FX_RetargetAssistant
```

Then rebuild or reopen the project. The plugin is editor-only and currently requires Unreal Engine 5.8.

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
- `Recreate Generated Setup`: explicitly rebuilds plugin-generated IK Rig / IK Retargeter assets. Use this when a generated setup is stale.
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

Latest clean validation project:

```text
F:\Unreal Projects\FXRA58PkgVal
```

Validated:

- Plugin mounts in a clean UE5.8 project.
- `Window -> FX Retarget Assistant` opens the panel.
- Panel title shows `FX Retarget Assistant - MVP1 Alpha / UE5.8`.
- Generated IK Rig / IK Retargeter assets are created fresh in the clean project.
- UE5.8 Retarget Ops Stack exists.
- Root Family Directional Policy is recorded in `Report.json`.
- Duplicate exports generate `_001` and `_002`.
- Generated IK Rig / RTG / AnimSequence assets load after reopening.
- User Retargeters outside `/Game/FX_RetargetAssistant/Setups/` remain read-only during Auto Repair.
- `git diff --check` passes before commit.

## Development Notes

Active source branch:

```text
ue58-main
```

Current UE5.8 development project:

```text
F:\Unreal Projects\FXRA58
```

Recommended clean validation project name:

```text
FXRA58PkgVal
```

## Ownership

This plugin is a personal project and asset owned by the project owner. See `LICENSE`.
