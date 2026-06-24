#pragma once

#include "AssetRegistry/AssetData.h"
#include "CoreMinimal.h"
#include "UObject/SoftObjectPtr.h"

class UAnimSequence;
class UIKRetargeter;
class USkeletalMesh;

enum class EFRA_LogSeverity : uint8
{
    Info,
    Warning,
    Error
};

struct FFRA_LogMessage
{
    EFRA_LogSeverity Severity = EFRA_LogSeverity::Info;
    FString Code;
    FString Message;
};

struct FFRA_BatchRetargetInput
{
    TSoftObjectPtr<USkeletalMesh> SourceMesh;
    TSoftObjectPtr<USkeletalMesh> TargetMesh;
    TSoftObjectPtr<UIKRetargeter> IKRetargeter;
    TArray<FAssetData> AssetsToRetarget;
    FString OutputPackagePath;
    FString Suffix = TEXT("_RTG");
    bool bOverwriteExisting = false;
};

struct FFRA_PreflightResult
{
    TArray<FFRA_LogMessage> Messages;
    bool bCanRun = false;
    bool bRetargetOpsStackValid = false;
    int32 RetargetOpsStackCount = 0;
    TArray<FString> RetargetOpsStackOpTypeNames;

    bool HasErrors() const
    {
        for (const FFRA_LogMessage& Message : Messages)
        {
            if (Message.Severity == EFRA_LogSeverity::Error)
            {
                return true;
            }
        }
        return false;
    }
};

struct FFRA_BatchRetargetItemResult
{
    FString SourceAnimationPath;
    FString OutputAnimationPath;
    bool bSuccess = false;
    FString Message;
};

struct FFRA_BatchRetargetReport
{
    FString BatchId;
    FString EngineVersion;
    FString Timestamp;
    FString SourceMeshPath;
    FString TargetMeshPath;
    FString RetargeterPath;
    FString SetupMode;
    FString SourceSkeletonPath;
    FString TargetSkeletonPath;
    FString GeneratedSourceIKRigPath;
    FString GeneratedTargetIKRigPath;
    FString GeneratedRetargeterPath;
    FString RootFamilyPolicy;
    FString SourceSkeletonFamily;
    FString TargetSkeletonFamily;
    FString SourceRetargetRoot;
    FString TargetRetargetRoot;
    FString RootChainMapping;
    FString PelvisChainMapping;
    FString ChainMappingSummary;
    bool bRetargetOpsStackValid = false;
    int32 RetargetOpsStackCount = 0;
    TArray<FString> RetargetOpsStackOpTypeNames;
    TArray<FString> Warnings;
    FString OutputFolder;
    FString Suffix;
    FString ConflictPolicy = TEXT("Create Unique Name");
    bool bOverwriteExisting = false;
    TArray<FFRA_LogMessage> PreflightMessages;
    TArray<FFRA_BatchRetargetItemResult> Items;
    int32 TotalCount = 0;
    int32 SuccessCount = 0;
    int32 FailedCount = 0;
};


