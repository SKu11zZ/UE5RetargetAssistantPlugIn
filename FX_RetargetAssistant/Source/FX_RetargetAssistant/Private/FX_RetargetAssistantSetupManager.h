#pragma once

#include "CoreMinimal.h"
#include "UObject/SoftObjectPtr.h"

class UAnimSequence;
class UIKRetargeter;
class USkeletalMesh;

struct FFRA_AutoRetargetSetup
{
    TSoftObjectPtr<USkeletalMesh> SourceMesh;
    TSoftObjectPtr<USkeletalMesh> TargetMesh;
    TSoftObjectPtr<class UIKRigDefinition> SourceIKRig;
    TSoftObjectPtr<class UIKRigDefinition> TargetIKRig;
    TSoftObjectPtr<UIKRetargeter> Retargeter;
    FString SetupFolder;
    TArray<FString> Messages;
};

struct FFRA_MixamoTestSet
{
    TSoftObjectPtr<USkeletalMesh> SourceMesh;
    TSoftObjectPtr<USkeletalMesh> TargetMesh;
    TSoftObjectPtr<UIKRetargeter> Retargeter;
    TArray<TSoftObjectPtr<UAnimSequence>> Animations;
    TArray<FString> Messages;
};

class FFX_RetargetAssistantSetupManager
{
public:
    static bool PrepareAutoRetargetSetup(USkeletalMesh* SourceMesh, USkeletalMesh* TargetMesh, FFRA_AutoRetargetSetup& OutSetup, FString& OutError, bool bRecreateExisting = false);
    static bool PrepareMixamoToUE5MannyTestSet(FFRA_MixamoTestSet& OutTestSet, FString& OutError);
};


