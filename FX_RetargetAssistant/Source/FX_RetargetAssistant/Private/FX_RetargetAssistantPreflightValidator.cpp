#include "FX_RetargetAssistantPreflightValidator.h"

#include "Animation/AnimSequence.h"
#include "FX_RetargetAssistantPathManager.h"
#include "FX_RetargetAssistantRetargeterUtils.h"
#include "Retargeter/IKRetargeter.h"
#include "Engine/SkeletalMesh.h"

FFRA_PreflightResult FFX_RetargetAssistantPreflightValidator::Validate(const FFRA_BatchRetargetInput& Input)
{
    FFRA_PreflightResult Result;

    USkeletalMesh* SourceMesh = Input.SourceMesh.LoadSynchronous();
    USkeletalMesh* TargetMesh = Input.TargetMesh.LoadSynchronous();
    UIKRetargeter* Retargeter = Input.IKRetargeter.LoadSynchronous();

    if (!SourceMesh)
    {
        Add(Result, EFRA_LogSeverity::Error, TEXT("SOURCE_MESH_MISSING"), TEXT("Source Skeletal Mesh is empty or cannot be loaded."));
    }
    if (!TargetMesh)
    {
        Add(Result, EFRA_LogSeverity::Error, TEXT("TARGET_MESH_MISSING"), TEXT("Target Skeletal Mesh is empty or cannot be loaded."));
    }
    if (!Retargeter)
    {
        Add(Result, EFRA_LogSeverity::Error, TEXT("RETARGETER_MISSING"), TEXT("IK Retargeter is empty or cannot be loaded."));
    }
    if (Input.AssetsToRetarget.IsEmpty())
    {
        Add(Result, EFRA_LogSeverity::Error, TEXT("NO_ANIMATIONS"), TEXT("No Source AnimSequence assets were selected."));
    }

    FString PathError;
    if (Input.OutputPackagePath.IsEmpty() || !FFX_RetargetAssistantPathManager::EnsurePackagePathExists(Input.OutputPackagePath, PathError))
    {
        Add(Result, EFRA_LogSeverity::Error, TEXT("OUTPUT_FOLDER_INVALID"), PathError.IsEmpty() ? TEXT("Output Folder is invalid.") : PathError);
    }

    if (SourceMesh && SourceMesh->GetSkeleton())
    {
        for (const FAssetData& AssetData : Input.AssetsToRetarget)
        {
            UAnimSequence* AnimSequence = Cast<UAnimSequence>(AssetData.GetAsset());
            if (!AnimSequence)
            {
                Add(Result, EFRA_LogSeverity::Error, TEXT("ANIMATION_LOAD_FAILED"), FString::Printf(TEXT("AnimSequence cannot be loaded: %s"), *AssetData.GetSoftObjectPath().ToString()));
                continue;
            }

            if (AnimSequence->GetSkeleton() != SourceMesh->GetSkeleton())
            {
                Add(Result, EFRA_LogSeverity::Error, TEXT("ANIMATION_SKELETON_MISMATCH"), FString::Printf(TEXT("AnimSequence skeleton does not match Source Mesh skeleton: %s"), *AnimSequence->GetPathName()));
            }
        }
    }

    if (Retargeter)
    {
        Add(Result, EFRA_LogSeverity::Info, TEXT("RETARGETER_SELECTED"), FString::Printf(TEXT("Using existing IK Retargeter: %s"), *Retargeter->GetPathName()));

        const FFRA_RetargetOpsStackInfo OpsInfo = FFX_RetargetAssistantRetargeterUtils::GetRetargetOpsStackInfo(Retargeter);
        Result.bRetargetOpsStackValid = OpsInfo.bValid;
        Result.RetargetOpsStackCount = OpsInfo.Count;
        Result.RetargetOpsStackOpTypeNames = OpsInfo.OpTypeNames;

        if (!OpsInfo.bValid)
        {
            const bool bGeneratedRetargeter = FFX_RetargetAssistantRetargeterUtils::IsGeneratedSetupRetargeter(Retargeter);
            Add(Result,
                EFRA_LogSeverity::Error,
                TEXT("RETARGET_OPS_STACK_MISSING"),
                bGeneratedRetargeter
                    ? TEXT("IK Retargeter has no valid UE5.8 Retarget Ops Stack. Use Recreate Generated Setup before exporting.")
                    : TEXT("IK Retargeter has no valid UE5.8 Retarget Ops Stack. Open it in the native UE Retargeter editor and add/check default ops; FX Retarget Assistant will not modify user Retargeters."));
        }
        else
        {
            Add(Result, EFRA_LogSeverity::Info, TEXT("RETARGET_OPS_STACK_READY"), FString::Printf(TEXT("Retarget Ops Stack valid. Count=%d."), OpsInfo.Count));
        }
    }

    if (Input.AssetsToRetarget.Num() > 100)
    {
        Add(Result, EFRA_LogSeverity::Warning, TEXT("LARGE_BATCH"), FString::Printf(TEXT("Large batch size (%d) may take time and memory."), Input.AssetsToRetarget.Num()));
    }

    Result.bCanRun = !Result.HasErrors();
    if (Result.bCanRun)
    {
        Add(Result, EFRA_LogSeverity::Info, TEXT("READY"), FString::Printf(TEXT("%d animation(s) ready for batch retarget."), Input.AssetsToRetarget.Num()));
    }

    return Result;
}

void FFX_RetargetAssistantPreflightValidator::Add(FFRA_PreflightResult& Result, EFRA_LogSeverity Severity, const FString& Code, const FString& Message)
{
    FFRA_LogMessage LogMessage;
    LogMessage.Severity = Severity;
    LogMessage.Code = Code;
    LogMessage.Message = Message;
    Result.Messages.Add(LogMessage);
}
