#pragma once

#include "CoreMinimal.h"
#include "FX_RetargetAssistantTypes.h"
#include "Widgets/SCompoundWidget.h"

class SEditableTextBox;
class SMultiLineEditableTextBox;
class SWidget;

class SFX_RetargetAssistantPanel : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SFX_RetargetAssistantPanel) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    FReply UseSelectedAnimations();
    FReply AddPickedAnimation();
    FReply LoadMixamoTestSet();
    FReply ClearAnimations();
    FReply ShowOutputFolder();
    FReply RunPreflight();
    FReply ExecuteBatchRetarget();
    FReply UseSelectedOutputFolder();
    FReply OpenRetargeter();
    FReply AutoCreateSetup();
    FReply RecreateGeneratedSetup();
    FReply AutoRepairIKMapping();

    void OnSourceMeshChanged(const FAssetData& AssetData);
    void OnOutputPathSelected(const FString& Path);
    FText GetSetupStatusText() const;
    void RefreshSetupStatus();
    bool IsSelectedRetargeterGenerated() const;
    FString GetSelectedRetargeterPath() const;

    void AppendLog(EFRA_LogSeverity Severity, const FString& Message);
    void SetLogText();
    bool SyncOutputFolderToContentBrowser();
    void RefreshAnimationListText();
    bool AddAnimationAssetData(const FAssetData& AssetData);
    bool DoesAnimationMatchSourceSkeleton(const FAssetData& AssetData, FString* OutReason = nullptr) const;
    bool ShouldFilterAnimSequence(const FAssetData& AssetData) const;
    FString GetSourceSkeletonPath() const;
    FString GetTargetSkeletonPath() const;
    TSharedRef<SWidget> MakeOutputFolderPicker();
    FFRA_BatchRetargetInput MakeInput() const;

    TWeakObjectPtr<UObject> SourceMeshObject;
    TWeakObjectPtr<UObject> TargetMeshObject;
    TWeakObjectPtr<UObject> RetargeterObject;
    TWeakObjectPtr<UObject> AnimSequenceObject;
    TArray<FAssetData> SelectedAnimations;
    FString SetupStatus = TEXT("Not Created");
    FString GeneratedSourceIKRigPath;
    FString GeneratedTargetIKRigPath;
    FString GeneratedRetargeterPath;
    FString LastRootFamilyPolicy;
    FString LastSourceSkeletonFamily;
    FString LastTargetSkeletonFamily;
    FString LastSourceRetargetRoot;
    FString LastTargetRetargetRoot;
    FString LastRootChainMapping;
    FString LastPelvisChainMapping;
    FString LastChainMappingSummary;
    TArray<FString> LastSetupWarnings;

    TSharedPtr<SEditableTextBox> OutputFolderTextBox;
    TSharedPtr<SEditableTextBox> SuffixTextBox;
    TSharedPtr<SMultiLineEditableTextBox> AnimationListTextBox;
    TSharedPtr<SMultiLineEditableTextBox> LogTextBox;
    TArray<FString> LogLines;
    FFRA_PreflightResult LastPreflight;
};



