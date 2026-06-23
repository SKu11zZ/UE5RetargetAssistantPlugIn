#include "FX_RetargetAssistantSmokeTestCommandlet.h"

#include "Animation/AnimSequence.h"
#include "Engine/SkeletalMesh.h"
#include "FX_RetargetAssistantBatchExporter.h"
#include "FX_RetargetAssistantPreflightValidator.h"
#include "FX_RetargetAssistantReportWriter.h"
#include "FX_RetargetAssistantSetupManager.h"
#include "Misc/DateTime.h"
#include "Retargeter/IKRetargeter.h"

UFX_RetargetAssistantSmokeTestCommandlet::UFX_RetargetAssistantSmokeTestCommandlet()
{
    IsClient = false;
    IsEditor = true;
    IsServer = false;
    LogToConsole = true;
}

int32 UFX_RetargetAssistantSmokeTestCommandlet::Main(const FString& Params)
{
    UE_LOG(LogTemp, Display, TEXT("FX Retarget Assistant smoke test started."));

    FFRA_MixamoTestSet TestSet;
    FString SetupError;
    if (!FFX_RetargetAssistantSetupManager::PrepareMixamoToUE5MannyTestSet(TestSet, SetupError))
    {
        UE_LOG(LogTemp, Error, TEXT("%s"), *SetupError);
        return 1;
    }

    for (const FString& Message : TestSet.Messages)
    {
        UE_LOG(LogTemp, Display, TEXT("%s"), *Message);
    }

    FFRA_BatchRetargetInput Input;
    Input.SourceMesh = TestSet.SourceMesh;
    Input.TargetMesh = TestSet.TargetMesh;
    Input.IKRetargeter = TestSet.Retargeter;
    Input.OutputPackagePath = FString::Printf(TEXT("/Game/FX_RetargetAssistant/Exports/Smoke_%s"), *FDateTime::Now().ToString(TEXT("%Y%m%d_%H%M%S")));
    Input.Suffix = TEXT("_FXRA_SMOKE");
    Input.bOverwriteExisting = false;

    for (const TSoftObjectPtr<UAnimSequence>& Animation : TestSet.Animations)
    {
        UAnimSequence* AnimSequence = Animation.LoadSynchronous();
        if (!AnimSequence)
        {
            UE_LOG(LogTemp, Error, TEXT("Missing Mixamo test AnimSequence."));
            return 1;
        }
        Input.AssetsToRetarget.Add(FAssetData(AnimSequence));
    }

    const FFRA_PreflightResult Preflight = FFX_RetargetAssistantPreflightValidator::Validate(Input);
    for (const FFRA_LogMessage& Message : Preflight.Messages)
    {
        if (Message.Severity == EFRA_LogSeverity::Error)
        {
            UE_LOG(LogTemp, Error, TEXT("[%s] %s"), *Message.Code, *Message.Message);
        }
        else if (Message.Severity == EFRA_LogSeverity::Warning)
        {
            UE_LOG(LogTemp, Warning, TEXT("[%s] %s"), *Message.Code, *Message.Message);
        }
        else
        {
            UE_LOG(LogTemp, Display, TEXT("[%s] %s"), *Message.Code, *Message.Message);
        }
    }

    if (!Preflight.bCanRun)
    {
        UE_LOG(LogTemp, Error, TEXT("Smoke test blocked by preflight."));
        return 1;
    }

    UE_LOG(LogTemp, Display, TEXT("Headless preflight passed for %d animation(s)."), Input.AssetsToRetarget.Num());
    UE_LOG(LogTemp, Display, TEXT("Skipping batch retarget in commandlet mode because UE5.4 UIKRetargetBatchOperation always syncs Content Browser and posts Slate notifications."));
    UE_LOG(LogTemp, Display, TEXT("Use the FX Retarget Assistant editor panel to run the actual batch export."));
    return 0;
}
