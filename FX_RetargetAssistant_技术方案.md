# FX_RetargetAssistant / 重定向小助手 技术方案

当前目标版本：**Unreal Engine 5.8 only**

当前阶段：**MVP1 Alpha UE5.8 Migration / Guided Auto Setup + Batch Export**

UE5.4 MVP1 Alpha 已归档。它验证了工作流，但不再作为当前开发目标或兼容性约束。

## 1. 项目定位

FX_RetargetAssistant 是 UE5.8 Editor Plugin。它不是重写 Unreal 的重定向系统，而是基于 UE5.8 原生 IK Rig / IK Retargeter / Batch Retarget API 的引导式自动 Setup 与批量导出工具。

一句话描述：

用户选择源角色、目标角色和源动画，插件自动创建或复用 IK Rig / IK Retargeter，应用 UE5.8 原生自动配置能力和插件内置 Root Family Directional Policy，然后批量导出目标角色可用的 AnimSequence。

## 2. 当前主流程

1. 选择 Source Character Mesh。
2. 选择 Target Character Mesh。
3. Anim Clip 区域只显示 Source Skeleton 对应的 AnimSequence。
4. 点击 Auto Create IK Rig + Retargeter。
5. 插件在 `/Game/FX_RetargetAssistant/Setups/` 下创建或复用生成资产。
6. 自动填入生成的 IK Retargeter。
7. 用户可点击 Open Retargeter 进入 UE 原生 Retargeter 编辑器精修。
8. 用户可点击 Auto Repair IK Mapping 修复插件生成的 Setup。
9. 选择 Output Folder / Suffix / Conflict Policy。
10. 点击 Retarget & Export 批量导出 AnimSequence。
11. 写入 `Report.json` 并同步 Content Browser 到输出目录。

## 3. 当前 UI 模块

UI 标题：

`FX Retarget Assistant - MVP1 Alpha / UE5.8`

### Step 1 - Characters

- Source Character Mesh
- Source Skeleton read-only
- Target Character Mesh
- Target Skeleton read-only

### Step 2 - Source Animations

- Anim Clip picker，按 Source Skeleton 过滤
- Add
- Add Selected
- Clear Animations
- Selected AnimSequence list

### Step 3 - Retarget Setup

- Current IK Retargeter
- Open
- Setup Status
- Auto Create IK Rig + Retargeter
- Recreate Generated Setup
- Auto Repair IK Mapping

### Step 4 - Export

- Output Folder
- Browse
- Use Selected Folder
- Suffix
- Conflict Policy: Create Unique Name
- Preflight
- Retarget & Export
- Show Output Folder

### Step 5 - Log / Report

- Info / Warning / Error 日志
- Report.json

## 4. Setup Status 规则

- `Invalid`: Source/Target 无效，Retargeter 无法加载，或 Source/Target IK Rig 缺失。
- `Not Created`: Source/Target 有效，但当前没有 Retargeter。
- `Missing Chains`: 主体核心链缺失，生成 Setup 报告 skipped/missing chain。
- `Needs Review`: 可用但有 warning，例如部分链缺失、fallback、Root Family Directional Policy 生效、当前使用用户手动 Retargeter、或动画列表为空。
- `Ready`: Retargeter、Source/Target IK Rig、核心链和动画列表均有效。

## 5. 自动创建 Setup

UE5.8 主线中，Auto Create IK Rig + Retargeter 应优先使用 UE5.8 官方 IK Rig / IK Retargeter 自动配置能力。插件自己的 Bone Resolver 和 Root Family Directional Policy 作为补充层，不要完全从零猜所有骨骼。

生成路径：

```text
/Game/FX_RetargetAssistant/Setups/<SourceName>_to_<TargetName>/
  IK_<SourceName>
  IK_<TargetName>
  RTG_<SourceName>_to_<TargetName>
```

默认策略：

- Auto Create 默认复用已有生成资产。
- Recreate Generated Setup 才显式清理并重建。
- 只允许清理和保存 `/Game/FX_RetargetAssistant/Setups/` 下的插件生成资产。
- 不修改用户项目其他路径下的 Retargeter。

## 6. Root Family Directional Policy

Skeleton Family：

- `UEMannequin`
- `Mixamo`
- `GenericHumanoid`
- `Unknown`

### UEMannequin -> Mixamo

- Source Retarget Root = `pelvis`
- Target Retarget Root = `Hips`
- Target Chain `Root` = `None`
- 目的：避免 UE root motion / root rotation 被错误传入 Mixamo 全局承载骨骼，导致浮空或整体旋转。

### Mixamo -> UEMannequin

- Source Retarget Root = `Hips`
- Target Retarget Root = `pelvis`
- Target Chain `Pelvis` = `None`
- Target Chain `Root` = `None`
- 目的：避免 Mixamo Hips/Pelvis 类全局运动重复驱动 UE pelvis，导致晃动。

### UEMannequin -> UEMannequin

- Root -> Root
- Pelvis -> Pelvis
- 不应用 None 特殊规则。

### Mixamo -> Mixamo

- Retarget Root 使用 `Hips`
- Root 默认 `None`
- Hips/Pelvis 按稳定规则映射。

这些规则只作用于插件生成的 setup，或用户显式允许修复的复制资产。不要修改用户手动选择的 Retargeter。

## 7. Auto Repair IK Mapping

Auto Repair IK Mapping 只允许直接修改 `/Game/FX_RetargetAssistant/Setups/` 下的插件生成 Retargeter。

如果当前 Retargeter 不在插件 Setups 路径下：

- 不修改
- 不保存
- 只输出 Warning
- 未来可以提供 Duplicate and Repair，但不是当前必须项

## 8. Retarget & Export 边界

Retarget & Export 永远保持纯导出行为：

- 不自动创建 IK Rig
- 不自动创建 IK Retargeter
- 不自动修 Mapping
- 不 Auto Align
- 不修改用户 Retargeter
- 只使用当前 UI 中有效的 IK Retargeter 和 Selected AnimSequence 批量导出

## 9. 命名冲突策略

默认：

```text
Conflict Policy = Create Unique Name
overwrite = false
```

示例：

```text
Walk_RTG
Walk_RTG_001
Walk_RTG_002
```

不要默认覆盖用户已有资产。

`Report.json` 必须记录：

```json
{
  "namingRule": {
    "conflictPolicy": "Create Unique Name",
    "overwrite": false
  }
}
```

## 10. Report.json UE5.8 字段

至少包含：

- plugin
- engineVersion
- setupMode
- sourceMesh
- targetMesh
- sourceSkeleton
- targetSkeleton
- ikRetargeter
- generatedSourceIKRig
- generatedTargetIKRig
- generatedRetargeter
- sourceSkeletonFamily
- targetSkeletonFamily
- rootFamilyPolicy
- sourceRetargetRoot
- targetRetargetRoot
- rootChainMapping
- pelvisChainMapping
- chainMappingSummary
- outputFolder
- namingRule
- preflight errors / warnings / infos
- animations source / output / status / message
- summary total / success / failed
- warnings

## 11. UE5.8 迁移策略

先不新增功能，先做迁移验证：

1. 编译插件。
2. 修复 UE5.8 API 编译错误。
3. 删除或收缩低版本条件编译。
4. 优先使用 UE5.8 官方 IK Rig / IK Retargeter / Batch Retarget API。
5. 重新验证 Auto Create IK Rig + Retargeter。
6. 重新验证 Open Retargeter。
7. 重新验证 Auto Repair IK Mapping。
8. 重新验证 Retarget & Export。
9. 重新验证 Report.json。
10. 重新验证 Content Browser 同步输出目录。

## 12. UE5.8 第一轮测试矩阵

1. Mixamo -> UE Manny：确认 `Pelvis=None` 生效，不晃。
2. UE Mannequin -> Mixamo：确认 `Root=None` 生效，不浮空、不整体旋转。
3. Manny -> Quinn：UE -> UE 保持 Root/Pelvis exact，不应用 None 特殊规则。
4. Manual Mode / Use Existing Retargeter：用户手动选择的 Retargeter 不被修改。
5. Auto Repair IK Mapping：只修 `/Game/FX_RetargetAssistant/Setups/` 下的插件生成 Retargeter，用户路径 Retargeter 只 Warning。
6. Recreate Generated Setup：只清理插件生成资产，不误删用户资产。
7. 重复导出同名动画：生成 `_001`、`_002`，不覆盖。
8. Open Retargeter 手动调整后再导出：用户精修后的插件生成 Retargeter 可用于导出。
9. 重开 UE5.8 项目：生成的 IK Rig、Retargeter、AnimSequence 都能重新加载。

## 13. Git / 分支 / 记录

- `main` 保留 UE5.4 MVP1 Alpha 历史记录。
- `ue58-main` 是当前 UE5.8 主线分支。
- 当前 Phase：UE5.8 Pivot / UE5.8-only migration。

## 14. 已冻结内容

以下不再作为当前主线投入：

- UE5.4 兼容
- UE5.5 兼容
- UE5.6 兼容
- UE5.7 兼容
- VersionBridge 扩展
- 多版本 BuildPlugin 包
- 5.4 commandlet workaround
