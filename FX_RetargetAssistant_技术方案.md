# FX_RetargetAssistant 重定向小助手 技术方案

版本：MVP 0 Implementation Contract v3.0  
日期：2026-06-23  
目标 UE 版本：最终支持 UE 5.4 - 5.8；MVP 0 先在一个主开发版本打穿闭环

## 1. 当前最终判断

`FX_RetargetAssistant / 重定向小助手` 的方向是正确的：它应该是 Unreal Editor 内的批量重定向助手，复用 UE 原生 `IK Rig / IK Retargeter` 工作流，而不是重写一套动画重定向系统。

但 MVP 0 必须进一步收紧。

MVP 0 的成功标准不是“看起来完整”，也不是“自动创建一整套重定向系统”，而是：

> 使用用户已有 IK Retargeter，稳定批量导出 Target Skeleton 对应的新 AnimSequence，并输出日志和 Report.json。

第一阶段严禁重新膨胀回“一键全自动重定向大师”。

## 2. MVP 0 核心目标

MVP 0 只验证一条最小闭环：

```text
Source Skeletal Mesh
+ Target Skeletal Mesh
+ 用户已有 IK Retargeter
+ 多个 Source AnimSequence
+ Output Folder
-> 批量导出 Target Skeleton 对应的新 Animation Sequence
-> 输出成功 / 失败日志
-> 生成 Report.json
```

这就是 MVP 0 的全部核心目标。

自动创建 IK Rig、自动创建 IK Retargeter、自动识别骨骼链、自动修复姿势，可以作为后续模块保留文件、接口或 TODO，但不能影响 MVP 0 的执行路径。

## 3. MVP 0 允许实现的功能

MVP 0 可以实现：

1. 创建 UE Editor Plugin。
2. 在 `Window / Tools` 或 Toolbar 中增加 `FX Retarget Assistant` 入口。
3. 打开一个简单 Slate 面板。
4. 支持用户选择 Source Skeletal Mesh。
5. 支持用户选择 Target Skeletal Mesh。
6. 支持用户选择已有 IK Retargeter。
7. 支持选择 Source AnimSequence。
8. 优先支持读取 Content Browser 当前选中的 AnimSequence。
9. 支持选择 Output Folder。
10. 支持命名后缀，默认 `_RTG`。
11. 支持 `Execute / Batch Retarget` 按钮。
12. 调用 UE 原生 IK Retargeter 批量导出接口。
13. 每个动画独立记录 `success / failed`。
14. 单个动画失败不应中断整个批次，除非 Retargeter 本身无效。
15. 在面板日志区输出 `Info / Warning / Error`。
16. 生成最小 `Report.json`。
17. 输出成功数量、失败数量、总数量。

## 4. MVP 0 禁止进入主流程的功能

以下功能不要进入 MVP 0 主流程：

1. 自动创建 IK Rig。
2. 自动创建 IK Retargeter。
3. 自动创建 Retarget Chains。
4. Bone Name Resolver。
5. 自动骨骼链识别。
6. 自动 Retarget Root 修正。
7. 自动 T Pose / A Pose 修正。
8. 自动手指链匹配。
9. 自动武器骨 / 道具 IK 处理。
10. 自动 Root Motion 修复。
11. 多 Source Skeleton 到多 Target Skeleton 的矩阵批处理。
12. Runtime 生成 Animation Sequence。
13. 复杂 UI。
14. 跨 UE 5.4 - 5.8 的完整 VersionBridge。

这些功能可以作为 Stub Only 存在：可以建文件、留接口、写 TODO，但不能接入 MVP 0 的主执行路径，也不能影响 MVP 0 的验收标准。

## 5. Preflight 边界

Preflight 是 MVP 0 的核心功能之一，但第一版不要做得过度智能。

MVP 0 的 Preflight 目标是：

> 确认能不能执行批量导出。

它不是替用户判断重定向质量。

### 5.1 阻塞级 Error

MVP 0 阻塞级 Error 只包括：

1. Source Skeletal Mesh 为空或无法加载。
2. Target Skeletal Mesh 为空或无法加载。
3. IK Retargeter 为空或无法加载。
4. Source AnimSequence 列表为空。
5. Output Folder 无效。
6. AnimSequence 无法加载。
7. AnimSequence 的 Skeleton 与 Source Mesh Skeleton 明显不一致。
8. IK Retargeter 缺少必要 Source / Target IKRig。
9. 批量导出接口调用失败。

### 5.2 Warning

MVP 0 Warning 包括：

1. 核心链可能不完整。
2. Retarget Root 疑似异常。
3. Source / Target 可能存在 T Pose / A Pose 差异。
4. Source Mesh / Target Mesh 与 Retargeter Preview Mesh 不是同一个资产。
5. 命名冲突。
6. 批量数量较大，可能存在保存或内存风险。
7. 输出目录已有同名资产。
8. Retargeter 配置可能不适合当前 Mesh。

Warning 不应该阻止用户执行。因为 MVP 0 使用的是用户已有 IK Retargeter，插件第一阶段不应该强行替用户判断质量。如果 UE 原生 Retargeter 本身能跑，插件就不应该因为过度预检把流程拦住。

## 6. Source / Target Mesh 与 Retargeter 一致性

不要要求用户选择的 Source Mesh / Target Mesh 必须与 Retargeter 内部 Preview Mesh 是完全同一个资产。

真实项目里经常会出现同一个 Skeleton 下有多个不同角色 Mesh 的情况。只要 Skeleton 兼容，就应该允许继续执行。

检查优先级：

1. Source AnimSequence Skeleton 是否与 Source Mesh Skeleton 一致。
2. Source Mesh Skeleton 是否与 Source IKRig Preview Mesh Skeleton 兼容。
3. Target Mesh Skeleton 是否与 Target IKRig Preview Mesh Skeleton 兼容。
4. 如果只是 Preview Mesh 资产不同，但 Skeleton 相同或兼容，则给 Warning，不阻塞。
5. 如果 Skeleton 明显不一致，则给 Error。

## 7. 动画输入方式

MVP 0 不要一开始就做复杂的文件夹递归扫描。

实现顺序：

1. 读取 Content Browser 当前选中的 AnimSequence。
2. 支持用户在面板中手动添加多个 AnimSequence。
3. 再支持选择文件夹并扫描该文件夹下的 AnimSequence。

这样可以尽快验证 BatchExporter，而不是把时间耗在资产扫描、递归过滤、保留目录结构这些周边功能上。

## 8. 目录结构原则

`/Game/FX_RetargetAssistant/` 只能作为推荐工作区，不能作为强制输入路径。

用户应该可以直接选择项目中任意位置的：

1. Source Skeletal Mesh
2. Target Skeletal Mesh
3. IK Retargeter
4. Source AnimSequence
5. Output Folder

推荐目录结构保留为：

```text
/Game/FX_RetargetAssistant/
  Setups/
  Exports/
```

`Setups/` 用于后续保存可复用配置，例如 IK Rig、IK Retargeter、插件配置资产、用户手动修正的链映射配置。

`Exports/` 用于保存每次批量导出的结果，例如：

```text
/Game/FX_RetargetAssistant/Exports/Batch_YYYYMMDD_HHMM/
  Animations/
  Report.json
```

不要每次导出都重复生成一套 IK Rig / Retargeter 放进 `Exports`。`Setups` 是长期配置，`Exports` 是一次性批处理结果，两者必须分开。

## 9. 模块实现优先级

### 9.1 MVP 0 Active 模块

1. `SFX_RetargetAssistantPanel`  
   负责简单 Slate 面板、按钮、日志显示。

2. `AssetScanner`  
   负责读取用户选择的 Mesh、Retargeter、AnimSequence，也可以读取 Content Browser 当前选择。

3. `PreflightValidator`  
   负责必要输入检查、Error / Warning / Info 聚合。

4. `BatchExporter`  
   负责调用 UE 原生批量重定向导出接口。这是 MVP 0 最核心模块。

5. `PathManager`  
   负责创建推荐工作区和输出目录。

6. `ReportWriter`  
   负责生成 `Report.json`。

### 9.2 MVP 0 Stub Only 模块

1. `SetupManager`
2. `IKRigBuilder`
3. `RetargeterBuilder`
4. `BoneNameResolver`
5. `VersionBridge`

Stub Only 的意思是可以建文件和接口，但不能接入主流程，也不能影响 MVP 0 的成功标准。

## 10. 实现顺序建议

不要先做复杂 UI，也不要先做自动路径和骨骼识别。

实现顺序：

1. 创建 Editor Plugin。
2. 添加菜单入口或 Toolbar 按钮。
3. 打开一个最简单的 Dockable Slate 面板。
4. 面板支持选择 IK Retargeter。
5. 面板支持读取 Content Browser 当前选中的 AnimSequence。
6. 面板支持选择 Output Folder。
7. 点击 Execute。
8. 调用 UE 原生批量 Retarget 导出接口。
9. 输出最小日志。
10. 验证是否能成功生成目标 Skeleton 对应 AnimSequence。
11. 再补 Source Mesh / Target Mesh 选择。
12. 再补 Preflight。
13. 再补 Report.json。
14. 再补推荐工作区目录。
15. 最后再考虑文件夹扫描和更完整 UI。

第一轮开发应该直冲 BatchExporter。这个项目的第一颗钉子是“能不能稳定调用 UE 原生批量重定向导出”，不是 UI，不是路径，不是自动骨骼识别。

## 11. Report.json 最小格式

MVP 0 的 `Report.json` 提前定格式，避免后续日志散掉。

```json
{
  "plugin": "FX_RetargetAssistant",
  "engineVersion": "5.x",
  "timestamp": "YYYYMMDD_HHMM",
  "sourceMesh": "/Game/...",
  "targetMesh": "/Game/...",
  "ikRetargeter": "/Game/...",
  "outputFolder": "/Game/...",
  "namingRule": {
    "suffix": "_RTG",
    "overwrite": false
  },
  "preflight": {
    "errors": [],
    "warnings": [],
    "infos": []
  },
  "animations": [
    {
      "source": "/Game/Anim/Walk",
      "output": "/Game/FX_RetargetAssistant/Exports/Batch_xxx/Animations/Walk_RTG",
      "status": "success",
      "message": ""
    },
    {
      "source": "/Game/Anim/Run",
      "output": "",
      "status": "failed",
      "message": "Reason here"
    }
  ],
  "summary": {
    "total": 0,
    "success": 0,
    "failed": 0
  }
}
```

`Report.json` 对后续调试、用户报错、Fab 上架后的售后都很重要。

## 12. UE 版本策略

UE 5.4 - 5.8 可以作为最终目标，但不要一开始就同时维护五个版本。

建议：

```text
开发目标：UE 5.4 - 5.8
首个实现版本：当前主开发版本，优先 UE 5.4 或实际正在使用的版本
第二验证版本：UE 5.8
最后补验证版本：UE 5.5 / UE 5.6 / UE 5.7
```

发布包最终可以分为：

```text
FX_RetargetAssistant_UE5.4
FX_RetargetAssistant_UE5.5
FX_RetargetAssistant_UE5.6
FX_RetargetAssistant_UE5.7
FX_RetargetAssistant_UE5.8
```

但 MVP 0 不需要完整 VersionBridge。先在一个版本打穿闭环，再处理跨版本差异。

## 13. 第一版公开承诺建议

第一版可以公开承诺：

1. Unreal Engine Editor Plugin。
2. 支持用户选择已有 IK Retargeter。
3. 支持一个 Source Skeletal Mesh 到一个 Target Skeletal Mesh。
4. 支持多个 Source AnimSequence 批量导出为 Target Skeleton 对应 Animation Sequence。
5. 支持输出目录选择。
6. 支持命名后缀 `_RTG`。
7. 支持 Preflight 预检。
8. 支持批量日志。
9. 支持 `Report.json`。
10. 支持单个动画失败后继续处理后续动画。

第一版不要公开承诺：

1. 自动创建 IK Rig。
2. 自动创建 IK Retargeter。
3. 任意未知骨架自动识别。
4. 自动 T Pose / A Pose 修正。
5. 手指 / 武器骨 / 道具 IK 完整质量保证。
6. Root Motion 完整修复。
7. Runtime 生成 Animation Sequence。
8. 多 Source Skeleton / 多 Target Skeleton 矩阵批处理。

## 14. 当前实现状态

已创建独立插件骨架：

```text
FX_RetargetAssistant/
  FX_RetargetAssistant.uplugin
  Source/
    FX_RetargetAssistant/
      FX_RetargetAssistant.Build.cs
      Public/
        FX_RetargetAssistantTypes.h
        FX_RetargetAssistantModule.h
      Private/
        FX_RetargetAssistantModule.cpp
        SFX_RetargetAssistantPanel.h/.cpp
        FX_RetargetAssistantAssetScanner.h/.cpp
        FX_RetargetAssistantPreflightValidator.h/.cpp
        FX_RetargetAssistantBatchExporter.h/.cpp
        FX_RetargetAssistantPathManager.h/.cpp
        FX_RetargetAssistantReportWriter.h/.cpp
        FX_RetargetAssistantSetupManager.h
        FX_RetargetAssistantIKRigBuilder.h
        FX_RetargetAssistantRetargeterBuilder.h
        FX_RetargetAssistantBoneNameResolver.h
        FX_RetargetAssistantVersionBridge.h
```

当前实现只接入 MVP 0 Active 模块。

Stub Only 模块已经留文件，但没有接入主流程。

## 15. MVP 0 验收标准

MVP 0 的验收标准只有一句话：

> 在 UE Editor 中，用户提供已有 IK Retargeter 和多个 AnimSequence，插件能够稳定批量导出 Target Skeleton 对应的新 AnimSequence，并输出日志和 Report.json。

