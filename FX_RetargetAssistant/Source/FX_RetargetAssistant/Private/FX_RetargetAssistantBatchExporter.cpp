#include "FX_RetargetAssistantBatchExporter.h"

#include "Animation/AnimSequence.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/SkeletalMesh.h"
#include "Misc/DateTime.h"
#include "Misc/EngineVersion.h"
#include "Misc/PackageName.h"
#include "RetargetEditor/IKRetargetBatchOperation.h"
#include "Retargeter/IKRetargeter.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"

namespace
{
    FString NormalizePackagePath(FString PackagePath)
    {
        PackagePath.RemoveFromEnd(TEXT("/"));
        return PackagePath;
    }

    bool SaveAsset(UObject* Asset, FString& OutError)
    {
        if (!Asset)
        {
            OutError = TEXT("Asset is null.");
            return false;
        }

        UPackage* Package = Asset->GetOutermost();
        if (!Package)
        {
            OutError = FString::Printf(TEXT("Asset has no package: %s"), *Asset->GetPathName());
            return false;
        }

        Package->SetDirtyFlag(true);
        FSavePackageArgs SaveArgs;
        SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
        SaveArgs.SaveFlags = SAVE_None;

        const FString Filename = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());
        if (!UPackage::SavePackage(Package, Asset, *Filename, SaveArgs))
        {
            OutError = FString::Printf(TEXT("Failed to save package: %s"), *Package->GetName());
            return false;
        }

        return true;
    }

    UAnimSequence* FindGeneratedAnimation(const FString& OutputPackagePath, const FString& ExpectedName, const TSet<FString>& ExistingObjectPaths, bool bOverwriteExisting)
    {
        FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
        TArray<FString> PathsToScan;
        PathsToScan.Add(OutputPackagePath);
        AssetRegistryModule.Get().ScanPathsSynchronous(PathsToScan, true);

        TArray<FAssetData> Assets;
        AssetRegistryModule.Get().GetAssetsByPath(FName(*OutputPackagePath), Assets, false);

        for (const FAssetData& AssetData : Assets)
        {
            const FString ObjectPath = AssetData.GetSoftObjectPath().ToString();
            const FString AssetName = AssetData.AssetName.ToString();
            const bool bIsAnimSequence = AssetData.GetClass() && AssetData.GetClass()->IsChildOf(UAnimSequence::StaticClass());
            if (!bIsAnimSequence)
            {
                continue;
            }
            if (!bOverwriteExisting && ExistingObjectPaths.Contains(ObjectPath))
            {
                continue;
            }
            if (AssetName == ExpectedName || AssetName.StartsWith(ExpectedName))
            {
                return Cast<UAnimSequence>(AssetData.GetAsset());
            }
        }

        return nullptr;
    }
}

bool FFX_RetargetAssistantBatchExporter::Run(const FFRA_BatchRetargetInput& Input, const FFRA_PreflightResult& Preflight, FFRA_BatchRetargetReport& OutReport)
{
    USkeletalMesh* SourceMesh = Input.SourceMesh.LoadSynchronous();
    USkeletalMesh* TargetMesh = Input.TargetMesh.LoadSynchronous();
    UIKRetargeter* Retargeter = Input.IKRetargeter.LoadSynchronous();
    const FString OutputPackagePath = NormalizePackagePath(Input.OutputPackagePath);

    OutReport.BatchId = FDateTime::Now().ToString(TEXT("%Y%m%d_%H%M%S"));
    OutReport.Timestamp = OutReport.BatchId;
    OutReport.EngineVersion = FEngineVersion::Current().ToString();
    OutReport.SourceMeshPath = SourceMesh ? SourceMesh->GetPathName() : FString();
    OutReport.TargetMeshPath = TargetMesh ? TargetMesh->GetPathName() : FString();
    OutReport.RetargeterPath = Retargeter ? Retargeter->GetPathName() : FString();
    OutReport.OutputFolder = OutputPackagePath;
    OutReport.Suffix = Input.Suffix;
    OutReport.bOverwriteExisting = Input.bOverwriteExisting;
    OutReport.PreflightMessages = Preflight.Messages;
    OutReport.TotalCount = Input.AssetsToRetarget.Num();

    if (!SourceMesh || !TargetMesh || !Retargeter)
    {
        OutReport.FailedCount = OutReport.TotalCount;
        return false;
    }

    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
    TArray<FString> PathsToScan;
    PathsToScan.Add(OutputPackagePath);
    AssetRegistryModule.Get().ScanPathsSynchronous(PathsToScan, true);

    TSet<FString> ExistingObjectPaths;
    TArray<FAssetData> ExistingAssets;
    AssetRegistryModule.Get().GetAssetsByPath(FName(*OutputPackagePath), ExistingAssets, false);
    for (const FAssetData& ExistingAsset : ExistingAssets)
    {
        ExistingObjectPaths.Add(ExistingAsset.GetSoftObjectPath().ToString());
    }

    for (const FAssetData& AssetData : Input.AssetsToRetarget)
    {
        FFRA_BatchRetargetItemResult Item;
        Item.SourceAnimationPath = AssetData.GetSoftObjectPath().ToString();

        UAnimSequence* SourceAnimSequence = Cast<UAnimSequence>(AssetData.GetAsset());
        if (!SourceAnimSequence)
        {
            Item.Message = TEXT("AnimSequence could not be loaded.");
            OutReport.Items.Add(Item);
            ++OutReport.FailedCount;
            continue;
        }

        const FString OutputAssetName = SourceAnimSequence->GetName() + Input.Suffix;
        Item.OutputAnimationPath = FString::Printf(TEXT("%s/%s.%s"), *OutputPackagePath, *OutputAssetName, *OutputAssetName);

        FIKRetargetBatchOperationContext Context;
        Context.AssetsToRetarget.Add(SourceAnimSequence);
        Context.SourceMesh = SourceMesh;
        Context.TargetMesh = TargetMesh;
        Context.IKRetargetAsset = Retargeter;
        Context.NameRule.FolderPath = OutputPackagePath;
        Context.NameRule.ReplaceFrom = SourceAnimSequence->GetName();
        Context.NameRule.ReplaceTo = OutputAssetName;
        Context.bIncludeReferencedAssets = false;
        Context.bOverwriteExistingFiles = Input.bOverwriteExisting;
        Context.bExportOnlyAnimatedBones = true;
        Context.bRetainAdditiveFlags = true;

        UIKRetargetBatchOperation* BatchOperation = NewObject<UIKRetargetBatchOperation>();
        BatchOperation->AddToRoot();
        BatchOperation->RunRetarget(Context);
        BatchOperation->RemoveFromRoot();

        UAnimSequence* GeneratedAnimSequence = FindGeneratedAnimation(OutputPackagePath, OutputAssetName, ExistingObjectPaths, Input.bOverwriteExisting);
        if (!GeneratedAnimSequence)
        {
            Item.Message = TEXT("UE retarget operation completed but no generated AnimSequence was found.");
            OutReport.Items.Add(Item);
            ++OutReport.FailedCount;
            continue;
        }

        FString SaveError;
        if (!SaveAsset(GeneratedAnimSequence, SaveError))
        {
            Item.OutputAnimationPath = GeneratedAnimSequence->GetPathName();
            Item.Message = SaveError;
            OutReport.Items.Add(Item);
            ++OutReport.FailedCount;
            continue;
        }

        Item.bSuccess = true;
        Item.OutputAnimationPath = GeneratedAnimSequence->GetPathName();
        Item.Message = TEXT("");
        OutReport.Items.Add(Item);
        ++OutReport.SuccessCount;
    }

    OutReport.FailedCount = OutReport.TotalCount - OutReport.SuccessCount;
    return OutReport.SuccessCount > 0;
}
