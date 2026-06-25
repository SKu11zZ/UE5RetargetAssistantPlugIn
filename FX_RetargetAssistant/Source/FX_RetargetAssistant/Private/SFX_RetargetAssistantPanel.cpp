#include "SFX_RetargetAssistantPanel.h"

#include "Animation/AnimSequence.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "ContentBrowserModule.h"
#include "Editor.h"
#include "Engine/SkeletalMesh.h"
#include "FX_RetargetAssistantAssetScanner.h"
#include "FX_RetargetAssistantBatchExporter.h"
#include "FX_RetargetAssistantPathManager.h"
#include "FX_RetargetAssistantPreflightValidator.h"
#include "FX_RetargetAssistantReportWriter.h"
#include "FX_RetargetAssistantRetargeterUtils.h"
#include "FX_RetargetAssistantSetupManager.h"
#include "IContentBrowserSingleton.h"
#include "Retargeter/IKRetargeter.h"
#include "Animation/Skeleton.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "PropertyCustomizationHelpers.h"
#include "UObject/SoftObjectPath.h"

#define LOCTEXT_NAMESPACE "FX_RetargetAssistantPanel"

void SFX_RetargetAssistantPanel::Construct(const FArguments& InArgs)
{
    const FSlateFontInfo LabelFont = FCoreStyle::GetDefaultFontStyle("Regular", 9);
    const FSlateFontInfo SubLabelFont = FCoreStyle::GetDefaultFontStyle("Regular", 8);
    const FSlateFontInfo ButtonFont = FCoreStyle::GetDefaultFontStyle("Bold", 9);
    const FSlateFontInfo ButtonSubFont = FCoreStyle::GetDefaultFontStyle("Regular", 8);

    auto MakeBilingualLabel = [LabelFont, SubLabelFont](const FText& English, const FText& Chinese)
    {
        return SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(STextBlock)
                .Text(English)
                .Font(LabelFont)
            ]
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0, 1, 0, 0)
            [
                SNew(STextBlock)
                .Text(Chinese)
                .Font(SubLabelFont)
                .ColorAndOpacity(FSlateColor(FLinearColor(0.62f, 0.62f, 0.62f, 1.0f)))
            ];
    };

    auto MakeBilingualButton = [ButtonFont, ButtonSubFont](const FText& English, const FText& Chinese)
    {
        return SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .AutoHeight()
            .HAlign(HAlign_Center)
            [
                SNew(STextBlock)
                .Text(English)
                .Font(ButtonFont)
                .Justification(ETextJustify::Center)
            ]
            + SVerticalBox::Slot()
            .AutoHeight()
            .HAlign(HAlign_Center)
            .Padding(0, 1, 0, 0)
            [
                SNew(STextBlock)
                .Text(Chinese)
                .Font(ButtonSubFont)
                .Justification(ETextJustify::Center)
                .ColorAndOpacity(FSlateColor(FLinearColor(0.78f, 0.78f, 0.78f, 1.0f)))
            ];
    };

    ChildSlot
    [
        SNew(SBorder)
        .Padding(12)
        [
            SNew(SVerticalBox)

            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0, 0, 0, 8)
            [
                SNew(STextBlock)
                .Text(LOCTEXT("Title", "FX Retarget Assistant - MVP1 Alpha / UE5.8"))
                .Font(FCoreStyle::GetDefaultFontStyle("Bold", 14))
            ]

            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0, 0, 0, 8)
            [
                SNew(SVerticalBox)
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(STextBlock)
                    .Text(LOCTEXT("Scope", "Guided workflow: choose Source/Target Character Meshes, add Source Skeleton-matching animations, then use an existing or generated IK Retargeter to export."))
                ]
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(0, 2, 0, 0)
                [
                    SNew(STextBlock)
                    .Text(LOCTEXT("ScopeCN", "引导流程：选择源/目标角色，添加匹配源骨架的动画，然后使用已有或生成的 IK Retargeter 导出。"))
                    .Font(SubLabelFont)
                    .ColorAndOpacity(FSlateColor(FLinearColor(0.62f, 0.62f, 0.62f, 1.0f)))
                ]
            ]

            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SGridPanel)
                + SGridPanel::Slot(0, 0).Padding(0, 3)
                [
                    MakeBilingualLabel(LOCTEXT("SourceMesh", "Step 1 - Source Character Mesh"), LOCTEXT("SourceMeshCN", "第 1 步 - 源角色网格"))
                ]
                + SGridPanel::Slot(1, 0).Padding(8, 3)
                [
                    SNew(SVerticalBox)
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        SNew(SObjectPropertyEntryBox)
                        .AllowedClass(USkeletalMesh::StaticClass())
                        .ToolTipText(LOCTEXT("SourceMeshTooltip", "Choose the character mesh that owns the source animations. Anim Clip and Add Selected will only accept AnimSequences using this mesh's Skeleton."))
                        .DisplayUseSelected(false)
                        .DisplayBrowse(false)
                        .ObjectPath_Lambda([this]()
                        {
                            return SourceMeshObject.IsValid() ? SourceMeshObject->GetPathName() : FString();
                        })
                        .OnObjectChanged(this, &SFX_RetargetAssistantPanel::OnSourceMeshChanged)
                    ]
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(0, 3, 0, 0)
                    [
                        SNew(STextBlock)
                        .Text_Lambda([this]() { return FText::FromString(FString::Printf(TEXT("Source Skeleton / 源骨架: %s"), *GetSourceSkeletonPath())); })
                    ]
                ]

                + SGridPanel::Slot(0, 1).Padding(0, 3)
                [
                    MakeBilingualLabel(LOCTEXT("TargetMesh", "Step 1 - Target Character Mesh"), LOCTEXT("TargetMeshCN", "第 1 步 - 目标角色网格"))
                ]
                + SGridPanel::Slot(1, 1).Padding(8, 3)
                [
                    SNew(SVerticalBox)
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        SNew(SObjectPropertyEntryBox)
                        .AllowedClass(USkeletalMesh::StaticClass())
                        .ToolTipText(LOCTEXT("TargetMeshTooltip", "Choose the character mesh that will receive the exported animations. Auto Create uses this mesh to build the target IK Rig."))
                        .DisplayUseSelected(false)
                        .DisplayBrowse(false)
                        .ObjectPath_Lambda([this]()
                        {
                            return TargetMeshObject.IsValid() ? TargetMeshObject->GetPathName() : FString();
                        })
                        .OnObjectChanged_Lambda([this](const FAssetData& AssetData)
                        {
                            TargetMeshObject = AssetData.GetAsset();
                            RefreshSetupStatus();
                        })
                    ]
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(0, 3, 0, 0)
                    [
                        SNew(STextBlock)
                        .Text_Lambda([this]() { return FText::FromString(FString::Printf(TEXT("Target Skeleton / 目标骨架: %s"), *GetTargetSkeletonPath())); })
                    ]
                ]

                + SGridPanel::Slot(0, 2).Padding(0, 3)
                [
                    MakeBilingualLabel(LOCTEXT("AnimSequence", "Step 2 - Anim Clip"), LOCTEXT("AnimSequenceCN", "第 2 步 - 动画片段"))
                ]
                + SGridPanel::Slot(1, 2).Padding(8, 3)
                [
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot()
                    .FillWidth(1.0f)
                    [
                        SNew(SObjectPropertyEntryBox)
                        .AllowedClass(UAnimSequence::StaticClass())
                        .ToolTipText(LOCTEXT("AnimClipTooltip", "Pick an AnimSequence that uses the Source Character Mesh skeleton. This picker is disabled until a Source Character Mesh is selected."))
                        .IsEnabled_Lambda([this]() { return Cast<USkeletalMesh>(SourceMeshObject.Get()) != nullptr; })
                        .OnShouldFilterAsset(this, &SFX_RetargetAssistantPanel::ShouldFilterAnimSequence)
                        .DisplayUseSelected(false)
                        .DisplayBrowse(false)
                        .ObjectPath_Lambda([this]()
                        {
                            return AnimSequenceObject.IsValid() ? AnimSequenceObject->GetPathName() : FString();
                        })
                        .OnObjectChanged_Lambda([this](const FAssetData& AssetData)
                        {
                            AnimSequenceObject = AssetData.GetAsset();
                            AddAnimationAssetData(AssetData);
                        })
                    ]
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding(6, 0, 0, 0)
                    [
                        SNew(SButton)
                        .Content()
                        [
                            MakeBilingualButton(LOCTEXT("AddPickedAnimation", "Add"), LOCTEXT("AddPickedAnimationCN", "添加"))
                        ]
                        .ToolTipText(LOCTEXT("AddPickedAnimationTooltip", "Add the picked AnimSequence to the batch list if it matches the Source Skeleton."))
                        .OnClicked(this, &SFX_RetargetAssistantPanel::AddPickedAnimation)
                    ]
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding(6, 0, 0, 0)
                    [
                        SNew(SButton)
                        .Content()
                        [
                            MakeBilingualButton(LOCTEXT("ClearAnimations", "Clear Animations"), LOCTEXT("ClearAnimationsCN", "清空动画"))
                        ]
                        .ToolTipText(LOCTEXT("ClearAnimationsTooltip", "Remove all AnimSequence assets from the current batch list."))
                        .OnClicked(this, &SFX_RetargetAssistantPanel::ClearAnimations)
                    ]
                ]

                + SGridPanel::Slot(0, 3).Padding(0, 3)
                [
                    MakeBilingualLabel(LOCTEXT("RetargetSetup", "Step 3 - Retarget Setup"), LOCTEXT("RetargetSetupCN", "第 3 步 - 重定向设置"))
                ]
                + SGridPanel::Slot(1, 3).Padding(8, 3)
                [
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot()
                    .FillWidth(1.0f)
                    [
                        SNew(SObjectPropertyEntryBox)
                        .AllowedClass(UIKRetargeter::StaticClass())
                        .ToolTipText(LOCTEXT("RetargetSetupTooltip", "Use a generated setup from Auto Create, or manually choose an existing IK Retargeter. User Retargeters outside /Game/FX_RetargetAssistant/Setups/ are treated as read-only."))
                        .DisplayUseSelected(false)
                        .DisplayBrowse(false)
                        .ObjectPath_Lambda([this]()
                        {
                            return RetargeterObject.IsValid() ? RetargeterObject->GetPathName() : FString();
                        })
                        .OnObjectChanged_Lambda([this](const FAssetData& AssetData)
                        {
                            RetargeterObject = AssetData.GetAsset();
                            RefreshSetupStatus();
                        })
                    ]
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding(6, 0, 0, 0)
                    [
                        SNew(SButton)
                        .Content()
                        [
                            MakeBilingualButton(LOCTEXT("OpenRetargeter", "Open"), LOCTEXT("OpenRetargeterCN", "打开"))
                        ]
                        .ToolTipText(LOCTEXT("OpenRetargeterTooltip", "Open the selected IK Retargeter in Unreal's native Retargeter editor for review or manual adjustment."))
                        .IsEnabled_Lambda([this]() { return RetargeterObject.IsValid(); })
                        .OnClicked(this, &SFX_RetargetAssistantPanel::OpenRetargeter)
                    ]
                ]

                + SGridPanel::Slot(0, 4).Padding(0, 3)
                [
                    MakeBilingualLabel(LOCTEXT("RetargetSetupTools", "Setup Tools"), LOCTEXT("RetargetSetupToolsCN", "设置工具"))
                ]
                + SGridPanel::Slot(1, 4).Padding(8, 3)
                [
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot()
                    .FillWidth(1.0f)
                    [
                        SNew(SButton)
                        .ButtonColorAndOpacity(FLinearColor(0.10f, 0.34f, 0.78f, 1.0f))
                        .Content()
                        [
                            MakeBilingualButton(LOCTEXT("AutoCreateSetup", "Auto Create IK Rig + RTG"), LOCTEXT("AutoCreateSetupCN", "自动创建 IK Rig + 重定向器"))
                        ]
                        .ToolTipText(LOCTEXT("AutoCreateSetupTooltip", "Create or reuse plugin-generated IK Rig and IK Retargeter assets under /Game/FX_RetargetAssistant/Setups/. Existing valid generated Retargeters are reused without overwriting manual edits."))
                        .OnClicked(this, &SFX_RetargetAssistantPanel::AutoCreateSetup)
                    ]
                    + SHorizontalBox::Slot()
                    .FillWidth(1.0f)
                    .Padding(6, 0, 0, 0)
                    [
                        SNew(SButton)
                        .Content()
                        [
                            MakeBilingualButton(LOCTEXT("RecreateSetup", "Recreate Generated Setup"), LOCTEXT("RecreateSetupCN", "重建生成设置"))
                        ]
                        .ToolTipText(LOCTEXT("RecreateSetupTooltip", "Explicitly rebuild and save plugin-generated setup assets. Only use this when you want to discard generated setup changes. User Retargeters outside the setup folder are not modified."))
                        .OnClicked(this, &SFX_RetargetAssistantPanel::RecreateGeneratedSetup)
                    ]
                    + SHorizontalBox::Slot()
                    .FillWidth(1.0f)
                    .Padding(6, 0, 0, 0)
                    [
                        SNew(SButton)
                        .Content()
                        [
                            MakeBilingualButton(LOCTEXT("AutoRepairMapping", "Auto Repair IK Mapping"), LOCTEXT("AutoRepairMappingCN", "自动修复 IK 映射"))
                        ]
                        .ToolTipText(LOCTEXT("AutoRepairMappingTooltip", "Repair only plugin-generated Retargeters under /Game/FX_RetargetAssistant/Setups/. User Retargeters are left untouched and will receive a warning."))
                        .OnClicked(this, &SFX_RetargetAssistantPanel::AutoRepairIKMapping)
                    ]
                ]

                + SGridPanel::Slot(0, 5).Padding(0, 3)
                [
                    MakeBilingualLabel(LOCTEXT("OutputFolder", "Step 4 - Output Folder"), LOCTEXT("OutputFolderCN", "第 4 步 - 输出文件夹"))
                ]
                + SGridPanel::Slot(1, 5).Padding(8, 3)
                [
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot()
                    .FillWidth(1.0f)
                    [
                        SAssignNew(OutputFolderTextBox, SEditableTextBox)
                        .ToolTipText(LOCTEXT("OutputFolderTooltip", "Content Browser package path for exported AnimSequence assets and Report.json, for example /Game/FX_RetargetAssistant/Exports/Batch_..."))
                        .Text(FText::FromString(FFX_RetargetAssistantPathManager::MakeDefaultBatchOutputPath()))
                    ]
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding(6, 0, 0, 0)
                    [
                        SNew(SComboButton)
                        .ToolTipText(LOCTEXT("BrowseOutputTooltip", "Pick an output folder from the Content Browser path tree."))
                        .ButtonContent()
                        [
                            MakeBilingualButton(LOCTEXT("BrowseOutput", "Browse..."), LOCTEXT("BrowseOutputCN", "浏览"))
                        ]
                        .MenuContent()
                        [
                            MakeOutputFolderPicker()
                        ]
                    ]
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding(6, 0, 0, 0)
                    [
                        SNew(SButton)
                        .Content()
                        [
                            MakeBilingualButton(LOCTEXT("UseSelectedFolder", "Use Selected Folder"), LOCTEXT("UseSelectedFolderCN", "使用所选文件夹"))
                        ]
                        .ToolTipText(LOCTEXT("UseSelectedFolderTooltip", "Use the folder currently selected in the Content Browser as the output folder."))
                        .OnClicked(this, &SFX_RetargetAssistantPanel::UseSelectedOutputFolder)
                    ]
                ]

                + SGridPanel::Slot(0, 6).Padding(0, 3)
                [
                    MakeBilingualLabel(LOCTEXT("Suffix", "Suffix"), LOCTEXT("SuffixCN", "后缀"))
                ]
                + SGridPanel::Slot(1, 6).Padding(8, 3)
                [
                    SAssignNew(SuffixTextBox, SEditableTextBox)
                    .ToolTipText(LOCTEXT("SuffixTooltip", "Suffix appended to exported AnimSequence asset names. Existing assets are not overwritten; unique names use _001, _002, and so on."))
                    .Text(FText::FromString(TEXT("_RTG")))
                ]

                + SGridPanel::Slot(0, 7).Padding(0, 3)
                [
                    MakeBilingualLabel(LOCTEXT("SetupStatusLabel", "Setup Status"), LOCTEXT("SetupStatusLabelCN", "设置状态"))
                ]
                + SGridPanel::Slot(1, 7).Padding(8, 3)
                [
                    SNew(STextBlock).Text_Lambda([this]() { return GetSetupStatusText(); })
                ]
            ]

            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0, 10, 0, 6)
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .FillWidth(1.0f)
                [
                    SNew(SButton)
                    .Content()
                    [
                        MakeBilingualButton(LOCTEXT("UseSelected", "Add Selected"), LOCTEXT("UseSelectedCN", "添加所选动画"))
                    ]
                    .ToolTipText(LOCTEXT("AddSelectedTooltip", "Add selected AnimSequence assets from the Content Browser. Only animations matching the Source Skeleton are accepted."))
                    .OnClicked(this, &SFX_RetargetAssistantPanel::UseSelectedAnimations)
                ]
                + SHorizontalBox::Slot()
                .FillWidth(1.0f)
                .Padding(8, 0, 0, 0)
                [
                    SNew(SButton)
                    .Content()
                    [
                        MakeBilingualButton(LOCTEXT("Preflight", "Preflight"), LOCTEXT("PreflightCN", "预检查"))
                    ]
                    .ToolTipText(LOCTEXT("PreflightTooltip", "Validate meshes, animations, output folder, selected Retargeter, and UE5.8 Retarget Ops Stack before export."))
                    .OnClicked(this, &SFX_RetargetAssistantPanel::RunPreflight)
                ]
                + SHorizontalBox::Slot()
                .FillWidth(1.0f)
                .Padding(8, 0, 0, 0)
                [
                    SNew(SButton)
                    .ButtonColorAndOpacity(FLinearColor(0.12f, 0.55f, 0.23f, 1.0f))
                    .Content()
                    [
                        MakeBilingualButton(LOCTEXT("Execute", "Retarget && Export"), LOCTEXT("ExecuteCN", "重定向并导出"))
                    ]
                    .ToolTipText(LOCTEXT("ExecuteTooltip", "Export selected AnimSequences using the current IK Retargeter. This action does not create setup assets, auto-repair mapping, Auto Align, or modify user Retargeters."))
                    .OnClicked(this, &SFX_RetargetAssistantPanel::ExecuteBatchRetarget)
                ]
            ]

            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0, 0, 0, 4)
            [
                SNew(STextBlock)
                .Text_Lambda([this]()
                {
                    return FText::FromString(FString::Printf(TEXT("Selected AnimSequence / 已选动画: %d"), SelectedAnimations.Num()));
                })
            ]

            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0, 0, 0, 8)
            [
                SAssignNew(AnimationListTextBox, SMultiLineEditableTextBox)
                .IsReadOnly(true)
                .AutoWrapText(true)
                .Text(LOCTEXT("NoAnimations", "No AnimSequence selected. Pick one above, or select assets in Content Browser and click Add Selected."))
            ]

            + SVerticalBox::Slot()
            .FillHeight(1.0f)
            [
                SAssignNew(LogTextBox, SMultiLineEditableTextBox)
                .IsReadOnly(true)
                .AutoWrapText(true)
            ]

            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0, 6, 0, 0)
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .FillWidth(1.0f)
                [
                    SNew(STextBlock)
                    .Text(LOCTEXT("OutputHint", "After export, use Show Output Folder to locate generated animations and Report.json."))
                    .Font(SubLabelFont)
                    .ColorAndOpacity(FSlateColor(FLinearColor(0.62f, 0.62f, 0.62f, 1.0f)))
                ]
                + SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(8, 0, 0, 0)
                [
                    SNew(SButton)
                    .Content()
                    [
                        MakeBilingualButton(LOCTEXT("ShowOutputFolder", "Show Output Folder"), LOCTEXT("ShowOutputFolderCN", "显示输出文件夹"))
                    ]
                    .ToolTipText(LOCTEXT("ShowOutputFolderTooltip", "Sync the Content Browser to the output folder after export."))
                    .OnClicked(this, &SFX_RetargetAssistantPanel::ShowOutputFolder)
                ]
            ]
        ]
    ];

    AppendLog(EFRA_LogSeverity::Info, TEXT("MVP1 Alpha / UE5.8 UI ready. Choose Source and Target Character Meshes first; animations are filtered by Source Skeleton."));
}

FReply SFX_RetargetAssistantPanel::UseSelectedAnimations()
{
    const TArray<FAssetData> BrowserAnimations = FFX_RetargetAssistantAssetScanner::GetSelectedAnimSequences();
    for (const FAssetData& AssetData : BrowserAnimations)
    {
        AddAnimationAssetData(AssetData);
    }
    AppendLog(EFRA_LogSeverity::Info, FString::Printf(TEXT("Added %d AnimSequence asset(s) from Content Browser. Total=%d."), BrowserAnimations.Num(), SelectedAnimations.Num()));
    return FReply::Handled();
}

FReply SFX_RetargetAssistantPanel::AddPickedAnimation()
{
    UAnimSequence* AnimSequence = Cast<UAnimSequence>(AnimSequenceObject.Get());
    if (!AnimSequence)
    {
        AppendLog(EFRA_LogSeverity::Warning, TEXT("No AnimSequence picked."));
        return FReply::Handled();
    }

    const bool bAdded = AddAnimationAssetData(FAssetData(AnimSequence));
    if (bAdded)
    {
        AppendLog(EFRA_LogSeverity::Info, FString::Printf(TEXT("Added AnimSequence: %s. Total=%d."), *AnimSequence->GetPathName(), SelectedAnimations.Num()));
    }
    return FReply::Handled();
}

FReply SFX_RetargetAssistantPanel::LoadMixamoTestSet()
{
    FFRA_MixamoTestSet TestSet;
    FString Error;
    if (!FFX_RetargetAssistantSetupManager::PrepareMixamoToUE5MannyTestSet(TestSet, Error))
    {
        AppendLog(EFRA_LogSeverity::Warning, TEXT("Required test assets are missing in FXRA58. Please import or migrate test assets."));
        AppendLog(EFRA_LogSeverity::Warning, Error);
        return FReply::Handled();
    }

    SourceMeshObject = TestSet.SourceMesh.LoadSynchronous();
    TargetMeshObject = TestSet.TargetMesh.LoadSynchronous();
    RetargeterObject = TestSet.Retargeter.LoadSynchronous();
    SelectedAnimations.Reset();

    for (const TSoftObjectPtr<UAnimSequence>& Animation : TestSet.Animations)
    {
        if (UAnimSequence* AnimSequence = Animation.LoadSynchronous())
        {
            AddAnimationAssetData(FAssetData(AnimSequence));
        }
    }

    if (OutputFolderTextBox.IsValid())
    {
        OutputFolderTextBox->SetText(FText::FromString(FFX_RetargetAssistantPathManager::MakeDefaultBatchOutputPath()));
    }

    for (const FString& Message : TestSet.Messages)
    {
        AppendLog(EFRA_LogSeverity::Info, Message);
    }
    AppendLog(EFRA_LogSeverity::Warning, TEXT("Demo/Test Set only: this creates or updates FX_RetargetAssistant test assets. The main Batch Retarget flow will not modify a user-selected IK Retargeter."));
    AppendLog(EFRA_LogSeverity::Info, FString::Printf(TEXT("Loaded Mixamo -> UE test set. AnimSequence Total=%d."), SelectedAnimations.Num()));
    return FReply::Handled();
}

FReply SFX_RetargetAssistantPanel::OpenRetargeter()
{
    UObject* Retargeter = RetargeterObject.Get();
    if (!Retargeter || !GEditor)
    {
        AppendLog(EFRA_LogSeverity::Warning, TEXT("No IK Retargeter selected."));
        return FReply::Handled();
    }

    if (UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>())
    {
        AssetEditorSubsystem->OpenEditorForAsset(Retargeter);
        AppendLog(EFRA_LogSeverity::Info, FString::Printf(TEXT("Opened IK Retargeter: %s"), *Retargeter->GetPathName()));
    }

    return FReply::Handled();
}
FReply SFX_RetargetAssistantPanel::AutoCreateSetup()
{
    USkeletalMesh* SourceMesh = Cast<USkeletalMesh>(SourceMeshObject.Get());
    USkeletalMesh* TargetMesh = Cast<USkeletalMesh>(TargetMeshObject.Get());
    if (!SourceMesh || !TargetMesh)
    {
        SetupStatus = TEXT("Invalid");
        AppendLog(EFRA_LogSeverity::Warning, TEXT("Choose both Source Character Mesh and Target Character Mesh before creating setup."));
        return FReply::Handled();
    }

    FFRA_AutoRetargetSetup Setup;
    FString Error;
    if (!FFX_RetargetAssistantSetupManager::PrepareAutoRetargetSetup(SourceMesh, TargetMesh, Setup, Error, false))
    {
        SetupStatus = TEXT("Invalid");
        AppendLog(EFRA_LogSeverity::Error, Error);
        return FReply::Handled();
    }

    RetargeterObject = Setup.Retargeter.LoadSynchronous();
    GeneratedSourceIKRigPath = Setup.SourceIKRig.ToSoftObjectPath().ToString();
    GeneratedTargetIKRigPath = Setup.TargetIKRig.ToSoftObjectPath().ToString();
    GeneratedRetargeterPath = Setup.Retargeter.ToSoftObjectPath().ToString();
    LastRootFamilyPolicy = Setup.RootFamilyPolicy;
    LastSourceSkeletonFamily = Setup.SourceSkeletonFamily;
    LastTargetSkeletonFamily = Setup.TargetSkeletonFamily;
    LastSourceRetargetRoot = Setup.SourceRetargetRoot;
    LastTargetRetargetRoot = Setup.TargetRetargetRoot;
    LastRootChainMapping = Setup.RootChainMapping;
    LastPelvisChainMapping = Setup.PelvisChainMapping;
    LastChainMappingSummary = Setup.ChainMappingSummary;
    LastSetupWarnings.Reset();

    bool bMissingChains = false;
    for (const FString& Message : Setup.Messages)
    {
        const bool bIsWarning = Message.Contains(TEXT("Skipped chain")) || Message.Contains(TEXT("missing Retarget Ops Stack"));
        AppendLog(bIsWarning ? EFRA_LogSeverity::Warning : EFRA_LogSeverity::Info, Message);
        if (bIsWarning)
        {
            bMissingChains = true;
            LastSetupWarnings.Add(Message);
        }
        else if (Message.Contains(TEXT("Target Chain Pelvis intentionally"))
            || Message.Contains(TEXT("Target Chain Root mapped to None"))
            || Message.Contains(TEXT("Target Chain Pelvis mapped to None")))
        {
            LastSetupWarnings.Add(Message);
        }
    }

    RefreshSetupStatus();
    if (RetargeterObject.IsValid())
    {
        AppendLog(EFRA_LogSeverity::Info, FString::Printf(TEXT("Generated IK Retargeter selected: %s"), *RetargeterObject->GetPathName()));
    }

    return FReply::Handled();
}

FReply SFX_RetargetAssistantPanel::RecreateGeneratedSetup()
{
    USkeletalMesh* SourceMesh = Cast<USkeletalMesh>(SourceMeshObject.Get());
    USkeletalMesh* TargetMesh = Cast<USkeletalMesh>(TargetMeshObject.Get());
    if (!SourceMesh || !TargetMesh)
    {
        SetupStatus = TEXT("Invalid");
        AppendLog(EFRA_LogSeverity::Warning, TEXT("Choose both Source Character Mesh and Target Character Mesh before recreating setup."));
        return FReply::Handled();
    }

    FFRA_AutoRetargetSetup Setup;
    FString Error;
    if (!FFX_RetargetAssistantSetupManager::PrepareAutoRetargetSetup(SourceMesh, TargetMesh, Setup, Error, true))
    {
        SetupStatus = TEXT("Invalid");
        AppendLog(EFRA_LogSeverity::Error, Error);
        return FReply::Handled();
    }

    RetargeterObject = Setup.Retargeter.LoadSynchronous();
    GeneratedSourceIKRigPath = Setup.SourceIKRig.ToSoftObjectPath().ToString();
    GeneratedTargetIKRigPath = Setup.TargetIKRig.ToSoftObjectPath().ToString();
    GeneratedRetargeterPath = Setup.Retargeter.ToSoftObjectPath().ToString();
    LastRootFamilyPolicy = Setup.RootFamilyPolicy;
    LastSourceSkeletonFamily = Setup.SourceSkeletonFamily;
    LastTargetSkeletonFamily = Setup.TargetSkeletonFamily;
    LastSourceRetargetRoot = Setup.SourceRetargetRoot;
    LastTargetRetargetRoot = Setup.TargetRetargetRoot;
    LastRootChainMapping = Setup.RootChainMapping;
    LastPelvisChainMapping = Setup.PelvisChainMapping;
    LastChainMappingSummary = Setup.ChainMappingSummary;
    LastSetupWarnings.Reset();

    bool bMissingChains = false;
    for (const FString& Message : Setup.Messages)
    {
        const bool bIsWarning = Message.Contains(TEXT("Skipped chain")) || Message.Contains(TEXT("missing Retarget Ops Stack"));
        AppendLog(bIsWarning ? EFRA_LogSeverity::Warning : EFRA_LogSeverity::Info, Message);
        if (bIsWarning)
        {
            bMissingChains = true;
            LastSetupWarnings.Add(Message);
        }
        else if (Message.Contains(TEXT("Target Chain Pelvis intentionally"))
            || Message.Contains(TEXT("Target Chain Root mapped to None"))
            || Message.Contains(TEXT("Target Chain Pelvis mapped to None")))
        {
            LastSetupWarnings.Add(Message);
        }
    }

    RefreshSetupStatus();
    AppendLog(EFRA_LogSeverity::Info, TEXT("Recreate Generated Setup completed. Generated assets were explicitly reconfigured and saved."));
    return FReply::Handled();
}

FReply SFX_RetargetAssistantPanel::AutoRepairIKMapping()
{
    if (!RetargeterObject.IsValid())
    {
        SetupStatus = TEXT("Not Created");
        AppendLog(EFRA_LogSeverity::Warning, TEXT("No IK Retargeter selected."));
        return FReply::Handled();
    }

    if (!IsSelectedRetargeterGenerated())
    {
        SetupStatus = TEXT("Needs Review");
        AppendLog(EFRA_LogSeverity::Warning, TEXT("Auto Repair IK Mapping did not modify this Retargeter because it is outside /Game/FX_RetargetAssistant/Setups/. Copy it into FX_RetargetAssistant/Setups or use Recreate Generated Setup."));
        return FReply::Handled();
    }

    AppendLog(EFRA_LogSeverity::Info, TEXT("Auto Repair IK Mapping is repairing plugin-generated setup assets only."));
    return RecreateGeneratedSetup();
}
FReply SFX_RetargetAssistantPanel::ClearAnimations()
{
    const int32 PreviousCount = SelectedAnimations.Num();
    SelectedAnimations.Reset();
    AnimSequenceObject.Reset();
    RefreshAnimationListText();
    RefreshSetupStatus();
    AppendLog(EFRA_LogSeverity::Info, FString::Printf(TEXT("Cleared %d selected AnimSequence asset(s)."), PreviousCount));
    return FReply::Handled();
}

FReply SFX_RetargetAssistantPanel::ShowOutputFolder()
{
    if (!SyncOutputFolderToContentBrowser())
    {
        AppendLog(EFRA_LogSeverity::Warning, TEXT("Output folder has no assets yet, or the path is invalid."));
    }
    return FReply::Handled();
}

FReply SFX_RetargetAssistantPanel::RunPreflight()
{
    LastPreflight = FFX_RetargetAssistantPreflightValidator::Validate(MakeInput());
    for (const FFRA_LogMessage& Message : LastPreflight.Messages)
    {
        AppendLog(Message.Severity, FString::Printf(TEXT("[%s] %s"), *Message.Code, *Message.Message));
    }
    return FReply::Handled();
}

FReply SFX_RetargetAssistantPanel::ExecuteBatchRetarget()
{
    FFRA_BatchRetargetInput Input = MakeInput();
    LastPreflight = FFX_RetargetAssistantPreflightValidator::Validate(Input);
    for (const FFRA_LogMessage& Message : LastPreflight.Messages)
    {
        AppendLog(Message.Severity, FString::Printf(TEXT("[%s] %s"), *Message.Code, *Message.Message));
    }

    if (!LastPreflight.bCanRun)
    {
        AppendLog(EFRA_LogSeverity::Error, TEXT("Batch Retarget blocked by Preflight errors."));
        return FReply::Handled();
    }

    FFRA_BatchRetargetReport Report;
    FFX_RetargetAssistantBatchExporter::Run(Input, LastPreflight, Report);
    Report.SetupMode = IsSelectedRetargeterGenerated() ? TEXT("Auto Create Setup") : TEXT("Use Existing Retargeter / Manual");
    Report.SourceSkeletonPath = GetSourceSkeletonPath();
    Report.TargetSkeletonPath = GetTargetSkeletonPath();
    Report.GeneratedSourceIKRigPath = GeneratedSourceIKRigPath;
    Report.GeneratedTargetIKRigPath = GeneratedTargetIKRigPath;
    Report.GeneratedRetargeterPath = GeneratedRetargeterPath;
    Report.RootFamilyPolicy = IsSelectedRetargeterGenerated() ? LastRootFamilyPolicy : TEXT("Manual/user-provided Retargeter");
    Report.SourceSkeletonFamily = IsSelectedRetargeterGenerated() ? LastSourceSkeletonFamily : TEXT("Unknown");
    Report.TargetSkeletonFamily = IsSelectedRetargeterGenerated() ? LastTargetSkeletonFamily : TEXT("Unknown");
    Report.SourceRetargetRoot = IsSelectedRetargeterGenerated() ? LastSourceRetargetRoot : TEXT("Unknown");
    Report.TargetRetargetRoot = IsSelectedRetargeterGenerated() ? LastTargetRetargetRoot : TEXT("Unknown");
    Report.RootChainMapping = IsSelectedRetargeterGenerated() ? LastRootChainMapping : TEXT("User Retargeter used as-is");
    Report.PelvisChainMapping = IsSelectedRetargeterGenerated() ? LastPelvisChainMapping : TEXT("User Retargeter used as-is");
    Report.ChainMappingSummary = IsSelectedRetargeterGenerated() ? LastChainMappingSummary : TEXT("Manual/user-provided Retargeter used as-is; no mapping changes were made by Retarget & Export.");
    Report.ConflictPolicy = TEXT("Create Unique Name");
    Report.Warnings = LastSetupWarnings;
    

    for (const FFRA_BatchRetargetItemResult& Item : Report.Items)
    {
        AppendLog(Item.bSuccess ? EFRA_LogSeverity::Info : EFRA_LogSeverity::Error,
            FString::Printf(TEXT("%s -> %s %s"),
                *Item.SourceAnimationPath,
                Item.bSuccess ? *Item.OutputAnimationPath : TEXT("<failed>"),
                *Item.Message));
    }

    FString ReportDiskPath;
    FString ReportError;
    if (FFX_RetargetAssistantReportWriter::WriteReport(Report, ReportDiskPath, ReportError))
    {
        AppendLog(EFRA_LogSeverity::Info, FString::Printf(TEXT("Report.json written: %s"), *ReportDiskPath));
    }
    else
    {
        AppendLog(EFRA_LogSeverity::Warning, ReportError);
    }

    if (Report.SuccessCount > 0)
    {
        SyncOutputFolderToContentBrowser();
    }

    AppendLog(EFRA_LogSeverity::Info, TEXT("Output conflict policy: Create Unique Name; existing assets are not overwritten."));
    AppendLog(EFRA_LogSeverity::Info, FString::Printf(TEXT("Batch finished. Total=%d Success=%d Failed=%d"), Report.TotalCount, Report.SuccessCount, Report.FailedCount));
    return FReply::Handled();
}

void SFX_RetargetAssistantPanel::AppendLog(EFRA_LogSeverity Severity, const FString& Message)
{
    const TCHAR* Prefix = TEXT("Info");
    if (Severity == EFRA_LogSeverity::Warning)
    {
        Prefix = TEXT("Warning");
    }
    else if (Severity == EFRA_LogSeverity::Error)
    {
        Prefix = TEXT("Error");
    }

    LogLines.Add(FString::Printf(TEXT("[%s] %s"), Prefix, *Message));
    SetLogText();
}

void SFX_RetargetAssistantPanel::SetLogText()
{
    if (LogTextBox.IsValid())
    {
        LogTextBox->SetText(FText::FromString(FString::Join(LogLines, TEXT("\n"))));
    }
}

bool SFX_RetargetAssistantPanel::SyncOutputFolderToContentBrowser()
{
    if (!OutputFolderTextBox.IsValid())
    {
        return false;
    }

    FString OutputPackagePath = OutputFolderTextBox->GetText().ToString();
    OutputPackagePath.RemoveFromEnd(TEXT("/"));
    if (OutputPackagePath.IsEmpty() || !OutputPackagePath.StartsWith(TEXT("/Game/")))
    {
        return false;
    }

    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
    TArray<FString> PathsToScan;
    PathsToScan.Add(OutputPackagePath);
    AssetRegistryModule.Get().ScanPathsSynchronous(PathsToScan, true);

    TArray<FAssetData> Assets;
    AssetRegistryModule.Get().GetAssetsByPath(FName(*OutputPackagePath), Assets, false);
    if (Assets.IsEmpty())
    {
        return false;
    }

    FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
    ContentBrowserModule.Get().SyncBrowserToAssets(Assets);
    AppendLog(EFRA_LogSeverity::Info, FString::Printf(TEXT("Synced Content Browser to output folder: %s (%d asset(s))."), *OutputPackagePath, Assets.Num()));
    return true;
}

void SFX_RetargetAssistantPanel::RefreshAnimationListText()
{
    if (!AnimationListTextBox.IsValid())
    {
        return;
    }

    if (SelectedAnimations.IsEmpty())
    {
        AnimationListTextBox->SetText(LOCTEXT("NoAnimationsList", "No AnimSequence selected. Pick one above, or select assets in Content Browser and click Add Selected."));
        return;
    }

    TArray<FString> Lines;
    for (int32 Index = 0; Index < SelectedAnimations.Num(); ++Index)
    {
        Lines.Add(FString::Printf(TEXT("%d. %s"), Index + 1, *SelectedAnimations[Index].GetSoftObjectPath().ToString()));
    }
    AnimationListTextBox->SetText(FText::FromString(FString::Join(Lines, TEXT("\n"))));
}

bool SFX_RetargetAssistantPanel::AddAnimationAssetData(const FAssetData& AssetData)
{
    if (!AssetData.IsValid() || !AssetData.GetClass() || !AssetData.GetClass()->IsChildOf(UAnimSequence::StaticClass()))
    {
        return false;
    }

    FString Reason;
    if (!DoesAnimationMatchSourceSkeleton(AssetData, &Reason))
    {
        AppendLog(EFRA_LogSeverity::Warning, Reason);
        return false;
    }

    const FSoftObjectPath NewPath = AssetData.GetSoftObjectPath();
    for (const FAssetData& Existing : SelectedAnimations)
    {
        if (Existing.GetSoftObjectPath() == NewPath)
        {
            RefreshAnimationListText();
            return false;
        }
    }

    SelectedAnimations.Add(AssetData);
    RefreshAnimationListText();
    RefreshSetupStatus();
    return true;
}

void SFX_RetargetAssistantPanel::OnSourceMeshChanged(const FAssetData& AssetData)
{
    SourceMeshObject = AssetData.GetAsset();
    AnimSequenceObject.Reset();
    if (!SelectedAnimations.IsEmpty())
    {
        SelectedAnimations.Reset();
        RefreshAnimationListText();
        RefreshSetupStatus();
        AppendLog(EFRA_LogSeverity::Warning, TEXT("Source Character Mesh changed. Cleared selected animations; choose animations that match the new Source Skeleton."));
    }
    RefreshSetupStatus();
    AppendLog(EFRA_LogSeverity::Info, FString::Printf(TEXT("Source Character Mesh: %s"), SourceMeshObject.IsValid() ? *SourceMeshObject->GetPathName() : TEXT("None")));
}

void SFX_RetargetAssistantPanel::OnOutputPathSelected(const FString& Path)
{
    if (OutputFolderTextBox.IsValid())
    {
        OutputFolderTextBox->SetText(FText::FromString(Path));
    }
    AppendLog(EFRA_LogSeverity::Info, FString::Printf(TEXT("Output Folder set to: %s"), *Path));
}

FReply SFX_RetargetAssistantPanel::UseSelectedOutputFolder()
{
    FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
    TArray<FString> SelectedFolders;
    ContentBrowserModule.Get().GetSelectedFolders(SelectedFolders);
    if (SelectedFolders.IsEmpty())
    {
        AppendLog(EFRA_LogSeverity::Warning, TEXT("No Content Browser folder selected."));
        return FReply::Handled();
    }

    OnOutputPathSelected(SelectedFolders[0]);
    return FReply::Handled();
}

bool SFX_RetargetAssistantPanel::DoesAnimationMatchSourceSkeleton(const FAssetData& AssetData, FString* OutReason) const
{
    USkeletalMesh* SourceMesh = Cast<USkeletalMesh>(SourceMeshObject.Get());
    USkeleton* SourceSkeleton = SourceMesh ? SourceMesh->GetSkeleton() : nullptr;
    if (!SourceSkeleton)
    {
        if (OutReason)
        {
            *OutReason = TEXT("Choose a Source Character Mesh before adding animations.");
        }
        return false;
    }

    UAnimSequence* AnimSequence = Cast<UAnimSequence>(AssetData.GetAsset());
    if (!AnimSequence)
    {
        if (OutReason)
        {
            *OutReason = FString::Printf(TEXT("AnimSequence cannot be loaded: %s"), *AssetData.GetSoftObjectPath().ToString());
        }
        return false;
    }

    if (AnimSequence->GetSkeleton() != SourceSkeleton)
    {
        if (OutReason)
        {
            *OutReason = FString::Printf(TEXT("Skipped AnimSequence because its Skeleton does not match Source Skeleton: %s"), *AnimSequence->GetPathName());
        }
        return false;
    }

    return true;
}

bool SFX_RetargetAssistantPanel::ShouldFilterAnimSequence(const FAssetData& AssetData) const
{
    USkeletalMesh* SourceMesh = Cast<USkeletalMesh>(SourceMeshObject.Get());
    USkeleton* SourceSkeleton = SourceMesh ? SourceMesh->GetSkeleton() : nullptr;
    if (!SourceSkeleton)
    {
        return false;
    }

    UAnimSequence* AnimSequence = Cast<UAnimSequence>(AssetData.GetAsset());
    return !AnimSequence || AnimSequence->GetSkeleton() != SourceSkeleton;
}

FString SFX_RetargetAssistantPanel::GetSourceSkeletonPath() const
{
    USkeletalMesh* SourceMesh = Cast<USkeletalMesh>(SourceMeshObject.Get());
    USkeleton* Skeleton = SourceMesh ? SourceMesh->GetSkeleton() : nullptr;
    return Skeleton ? Skeleton->GetPathName() : FString();
}

FString SFX_RetargetAssistantPanel::GetTargetSkeletonPath() const
{
    USkeletalMesh* TargetMesh = Cast<USkeletalMesh>(TargetMeshObject.Get());
    USkeleton* Skeleton = TargetMesh ? TargetMesh->GetSkeleton() : nullptr;
    return Skeleton ? Skeleton->GetPathName() : FString();
}

TSharedRef<SWidget> SFX_RetargetAssistantPanel::MakeOutputFolderPicker()
{
    FPathPickerConfig PathPickerConfig;
    PathPickerConfig.DefaultPath = OutputFolderTextBox.IsValid() ? OutputFolderTextBox->GetText().ToString() : TEXT("/Game");
    PathPickerConfig.OnPathSelected = FOnPathSelected::CreateSP(this, &SFX_RetargetAssistantPanel::OnOutputPathSelected);
    PathPickerConfig.bAllowContextMenu = true;

    FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
    return SNew(SBox)
        .WidthOverride(420)
        .HeightOverride(520)
        [
            ContentBrowserModule.Get().CreatePathPicker(PathPickerConfig)
        ];
}
FString SFX_RetargetAssistantPanel::GetSelectedRetargeterPath() const
{
    return RetargeterObject.IsValid() ? RetargeterObject->GetPathName() : FString();
}

bool SFX_RetargetAssistantPanel::IsSelectedRetargeterGenerated() const
{
    return FFX_RetargetAssistantRetargeterUtils::IsGeneratedSetupPath(GetSelectedRetargeterPath());
}

FText SFX_RetargetAssistantPanel::GetSetupStatusText() const
{
    return FText::FromString(SetupStatus);
}
void SFX_RetargetAssistantPanel::RefreshSetupStatus()
{
    USkeletalMesh* SourceMesh = Cast<USkeletalMesh>(SourceMeshObject.Get());
    USkeletalMesh* TargetMesh = Cast<USkeletalMesh>(TargetMeshObject.Get());
    if (!SourceMesh || !TargetMesh)
    {
        SetupStatus = TEXT("Invalid");
        return;
    }

    if (!RetargeterObject.IsValid())
    {
        SetupStatus = TEXT("Not Created");
        return;
    }

    UIKRetargeter* Retargeter = Cast<UIKRetargeter>(RetargeterObject.Get());
    const FFRA_RetargetOpsStackInfo OpsInfo = FFX_RetargetAssistantRetargeterUtils::GetRetargetOpsStackInfo(Retargeter);
    if (!OpsInfo.bValid)
    {
        SetupStatus = TEXT("Invalid");
        return;
    }

    bool bHasMissingCoreChains = false;
    bool bHasWarnings = !LastSetupWarnings.IsEmpty();
    for (const FString& Warning : LastSetupWarnings)
    {
        if (Warning.Contains(TEXT("Skipped chain")))
        {
            bHasMissingCoreChains = true;
            break;
        }
    }

    if (bHasMissingCoreChains)
    {
        SetupStatus = TEXT("Missing Chains");
        return;
    }

    if (SelectedAnimations.IsEmpty())
    {
        SetupStatus = TEXT("Needs Review");
        return;
    }

    if (!IsSelectedRetargeterGenerated() || bHasWarnings)
    {
        SetupStatus = TEXT("Needs Review");
        return;
    }

    SetupStatus = TEXT("Ready");
}
FFRA_BatchRetargetInput SFX_RetargetAssistantPanel::MakeInput() const
{
    FFRA_BatchRetargetInput Input;
    Input.SourceMesh = Cast<USkeletalMesh>(SourceMeshObject.Get());
    Input.TargetMesh = Cast<USkeletalMesh>(TargetMeshObject.Get());
    Input.IKRetargeter = Cast<UIKRetargeter>(RetargeterObject.Get());
    Input.AssetsToRetarget = SelectedAnimations;
    Input.OutputPackagePath = OutputFolderTextBox.IsValid() ? OutputFolderTextBox->GetText().ToString() : FString();
    Input.Suffix = SuffixTextBox.IsValid() ? SuffixTextBox->GetText().ToString() : FString(TEXT("_RTG"));
    Input.bOverwriteExisting = false;
    return Input;
}

#undef LOCTEXT_NAMESPACE


















