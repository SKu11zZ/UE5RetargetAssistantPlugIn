#include "FX_RetargetAssistantSmokeTestCommandlet.h"

#include "Animation/AnimSequence.h"
#include "AssetToolsModule.h"
#include "Engine/SkeletalMesh.h"
#include "FX_RetargetAssistantBatchExporter.h"
#include "FX_RetargetAssistantPreflightValidator.h"
#include "FX_RetargetAssistantReportWriter.h"
#include "FX_RetargetAssistantRetargeterUtils.h"
#include "FX_RetargetAssistantSetupManager.h"
#include "Misc/DateTime.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
#include "Modules/ModuleManager.h"
#include "Retargeter/IKRetargeter.h"

namespace
{
    void LogMessage(const FFRA_LogMessage& Message)
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

    UAnimSequence* LoadAnim(const TCHAR* ObjectPath)
    {
        return LoadObject<UAnimSequence>(nullptr, ObjectPath);
    }

    bool RunExportCase(
        const FString& CaseName,
        USkeletalMesh* SourceMesh,
        USkeletalMesh* TargetMesh,
        const TArray<UAnimSequence*>& Animations,
        const FString& OutputFolder,
        FFRA_BatchRetargetReport* OutReport = nullptr,
        const bool bRecreateGeneratedSetup = false)
    {
        UE_LOG(LogTemp, Display, TEXT("FXRA FUNCTIONAL CASE START: %s"), *CaseName);

        FFRA_AutoRetargetSetup Setup;
        FString SetupError;
        if (!FFX_RetargetAssistantSetupManager::PrepareAutoRetargetSetup(SourceMesh, TargetMesh, Setup, SetupError, bRecreateGeneratedSetup))
        {
            UE_LOG(LogTemp, Error, TEXT("%s setup failed: %s"), *CaseName, *SetupError);
            return false;
        }

        for (const FString& Message : Setup.Messages)
        {
            UE_LOG(LogTemp, Display, TEXT("%s"), *Message);
        }

        UIKRetargeter* Retargeter = Setup.Retargeter.LoadSynchronous();
        const FFRA_RetargetOpsStackInfo OpsInfo = FFX_RetargetAssistantRetargeterUtils::GetRetargetOpsStackInfo(Retargeter);
        UE_LOG(LogTemp, Display, TEXT("%s Retarget Ops Stack valid=%s count=%d types=%s"),
            *CaseName,
            OpsInfo.bValid ? TEXT("true") : TEXT("false"),
            OpsInfo.Count,
            *FString::Join(OpsInfo.OpTypeNames, TEXT(",")));

        FFRA_BatchRetargetInput Input;
        Input.SourceMesh = SourceMesh;
        Input.TargetMesh = TargetMesh;
        Input.IKRetargeter = Retargeter;
        Input.OutputPackagePath = OutputFolder;
        Input.Suffix = TEXT("_RTG");
        Input.bOverwriteExisting = false;

        for (UAnimSequence* Animation : Animations)
        {
            if (!Animation)
            {
                UE_LOG(LogTemp, Error, TEXT("%s has a missing animation input."), *CaseName);
                return false;
            }
            Input.AssetsToRetarget.Add(FAssetData(Animation));
        }

        const FFRA_PreflightResult Preflight = FFX_RetargetAssistantPreflightValidator::Validate(Input);
        for (const FFRA_LogMessage& Message : Preflight.Messages)
        {
            LogMessage(Message);
        }
        if (!Preflight.bCanRun)
        {
            UE_LOG(LogTemp, Error, TEXT("%s blocked by preflight."), *CaseName);
            return false;
        }

        FFRA_BatchRetargetReport Report;
        const bool bExported = FFX_RetargetAssistantBatchExporter::Run(Input, Preflight, Report);
        Report.SetupMode = TEXT("Functional Validation Auto Setup");
        Report.SourceSkeletonPath = SourceMesh && SourceMesh->GetSkeleton() ? SourceMesh->GetSkeleton()->GetPathName() : FString();
        Report.TargetSkeletonPath = TargetMesh && TargetMesh->GetSkeleton() ? TargetMesh->GetSkeleton()->GetPathName() : FString();
        Report.GeneratedSourceIKRigPath = Setup.SourceIKRig.ToSoftObjectPath().ToString();
        Report.GeneratedTargetIKRigPath = Setup.TargetIKRig.ToSoftObjectPath().ToString();
        Report.GeneratedRetargeterPath = Setup.Retargeter.ToSoftObjectPath().ToString();
        Report.RootFamilyPolicy = Setup.RootFamilyPolicy;
        Report.SourceSkeletonFamily = Setup.SourceSkeletonFamily;
        Report.TargetSkeletonFamily = Setup.TargetSkeletonFamily;
        Report.SourceRetargetRoot = Setup.SourceRetargetRoot;
        Report.TargetRetargetRoot = Setup.TargetRetargetRoot;
        Report.RootChainMapping = Setup.RootChainMapping;
        Report.PelvisChainMapping = Setup.PelvisChainMapping;
        Report.ChainMappingSummary = Setup.ChainMappingSummary;
        Report.ConflictPolicy = TEXT("Create Unique Name");

        FString ReportPath;
        FString ReportError;
        if (!FFX_RetargetAssistantReportWriter::WriteReport(Report, ReportPath, ReportError))
        {
            UE_LOG(LogTemp, Error, TEXT("%s failed to write report: %s"), *CaseName, *ReportError);
            return false;
        }

        FString ReportText;
        FFileHelper::LoadFileToString(ReportText, *ReportPath);
        const bool bReportHasOpsFields = ReportText.Contains(TEXT("retargetOpsStackValid"))
            && ReportText.Contains(TEXT("retargetOpsStackCount"))
            && ReportText.Contains(TEXT("retargetOpsStackOpTypeNames"));

        UE_LOG(LogTemp, Display, TEXT("%s Report.json: %s"), *CaseName, *ReportPath);
        UE_LOG(LogTemp, Display, TEXT("%s summary success=%d failed=%d reportOpsFields=%s"),
            *CaseName,
            Report.SuccessCount,
            Report.FailedCount,
            bReportHasOpsFields ? TEXT("true") : TEXT("false"));

        if (OutReport)
        {
            *OutReport = Report;
        }

        if (!bExported || Report.SuccessCount != Animations.Num() || !bReportHasOpsFields)
        {
            UE_LOG(LogTemp, Error, TEXT("%s export/report validation failed."), *CaseName);
            return false;
        }

        UE_LOG(LogTemp, Display, TEXT("FXRA FUNCTIONAL CASE PASS: %s"), *CaseName);
        return true;
    }

    bool RunDuplicateOutputValidation(USkeletalMesh* SourceMesh, USkeletalMesh* TargetMesh, UAnimSequence* Animation, const FString& OutputFolder)
    {
        TArray<FString> Outputs;
        for (int32 Index = 0; Index < 3; ++Index)
        {
            FFRA_BatchRetargetReport Report;
            if (!RunExportCase(FString::Printf(TEXT("DuplicateOutput_%d"), Index + 1), SourceMesh, TargetMesh, {Animation}, OutputFolder, &Report))
            {
                return false;
            }
            if (Report.Items.Num() != 1 || !Report.Items[0].bSuccess)
            {
                return false;
            }
            Outputs.Add(Report.Items[0].OutputAnimationPath);
        }

        const bool bUnique = Outputs.Num() == 3
            && Outputs[0].EndsWith(TEXT("_RTG.Hip_Hop_Dancing_Anim_RTG"))
            && Outputs[1].EndsWith(TEXT("_RTG_001.Hip_Hop_Dancing_Anim_RTG_001"))
            && Outputs[2].EndsWith(TEXT("_RTG_002.Hip_Hop_Dancing_Anim_RTG_002"));
        UE_LOG(LogTemp, Display, TEXT("Duplicate outputs: %s"), *FString::Join(Outputs, TEXT(" | ")));
        UE_LOG(LogTemp, Display, TEXT("Duplicate output unique-name validation: %s"), bUnique ? TEXT("PASS") : TEXT("FAIL"));
        return bUnique;
    }

    bool RunUserRetargeterSafetyValidation(UIKRetargeter* SourceRetargeter)
    {
        if (!SourceRetargeter)
        {
            UE_LOG(LogTemp, Error, TEXT("User Retargeter safety validation missing source retargeter."));
            return false;
        }

        FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
        UObject* Duplicated = AssetToolsModule.Get().DuplicateAsset(
            TEXT("RTG_UserOwned_Test"),
            TEXT("/Game/FXRA_UserTest"),
            SourceRetargeter);

        UIKRetargeter* UserRetargeter = Cast<UIKRetargeter>(Duplicated);
        if (!UserRetargeter)
        {
            UserRetargeter = LoadObject<UIKRetargeter>(nullptr, TEXT("/Game/FXRA_UserTest/RTG_UserOwned_Test.RTG_UserOwned_Test"));
        }
        if (!UserRetargeter)
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create/load user-owned Retargeter test asset."));
            return false;
        }

        const FString UserPath = UserRetargeter->GetPathName();
        const bool bIsGeneratedPath = FFX_RetargetAssistantRetargeterUtils::IsGeneratedSetupRetargeter(UserRetargeter);
        const FFRA_RetargetOpsStackInfo BeforeOps = FFX_RetargetAssistantRetargeterUtils::GetRetargetOpsStackInfo(UserRetargeter);

        FString PackageFilename;
        FPackageName::TryConvertLongPackageNameToFilename(UserRetargeter->GetOutermost()->GetName(), PackageFilename, FPackageName::GetAssetPackageExtension());
        const FDateTime BeforeTimestamp = IFileManager::Get().GetTimeStamp(*PackageFilename);

        if (!bIsGeneratedPath)
        {
            UE_LOG(LogTemp, Warning, TEXT("Auto Repair IK Mapping would not modify this Retargeter because it is outside /Game/FX_RetargetAssistant/Setups/: %s"), *UserPath);
        }

        const FDateTime AfterTimestamp = IFileManager::Get().GetTimeStamp(*PackageFilename);
        const FFRA_RetargetOpsStackInfo AfterOps = FFX_RetargetAssistantRetargeterUtils::GetRetargetOpsStackInfo(UserRetargeter);
        const bool bPassed = !bIsGeneratedPath
            && BeforeTimestamp == AfterTimestamp
            && BeforeOps.Count == AfterOps.Count
            && BeforeOps.OpTypeNames == AfterOps.OpTypeNames;

        UE_LOG(LogTemp, Display, TEXT("User Retargeter safety validation: %s path=%s"), bPassed ? TEXT("PASS") : TEXT("FAIL"), *UserPath);
        return bPassed;
    }

    int32 RunFunctionalValidation()
    {
        const FString Stamp = FDateTime::Now().ToString(TEXT("%Y%m%d_%H%M%S"));

        USkeletalMesh* CenterBlock = LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/FXRA_Imported/MixamoShared/Center_Block.Center_Block"));
        USkeletalMesh* Manny = LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny_Simple.SKM_Manny_Simple"));
        USkeletalMesh* Quinn = LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/Characters/Mannequins/Meshes/SKM_Quinn_Simple.SKM_Quinn_Simple"));

        UAnimSequence* CenterBlockAnim = LoadAnim(TEXT("/Game/FXRA_Imported/MixamoShared/Center_Block_Anim.Center_Block_Anim"));
        UAnimSequence* HipHopAnim = LoadAnim(TEXT("/Game/FXRA_Imported/MixamoShared/Hip_Hop_Dancing_Anim.Hip_Hop_Dancing_Anim"));
        UAnimSequence* SwingAnim = LoadAnim(TEXT("/Game/FXRA_Imported/MixamoShared/Swing_Dancing_Anim.Swing_Dancing_Anim"));
        UAnimSequence* UEWalkAnim = LoadAnim(TEXT("/Game/Characters/Mannequins/Anims/Unarmed/Walk/MF_Unarmed_Walk_Fwd_Right.MF_Unarmed_Walk_Fwd_Right"));

        if (!CenterBlock || !Manny || !Quinn || !CenterBlockAnim || !HipHopAnim || !SwingAnim || !UEWalkAnim)
        {
            UE_LOG(LogTemp, Error, TEXT("Functional validation missing required test assets."));
            return 1;
        }

        int32 Failures = 0;
        FFRA_BatchRetargetReport MixamoToMannyReport;
        Failures += RunExportCase(TEXT("Mixamo_to_UE_Manny"), CenterBlock, Manny, {CenterBlockAnim, HipHopAnim, SwingAnim}, FString::Printf(TEXT("/Game/FX_RetargetAssistant/Exports/FunctionalValidation_%s/Mixamo_to_Manny"), *Stamp), &MixamoToMannyReport, true) ? 0 : 1;
        Failures += RunExportCase(TEXT("UE_Mannequin_to_Mixamo"), Manny, CenterBlock, {UEWalkAnim}, FString::Printf(TEXT("/Game/FX_RetargetAssistant/Exports/FunctionalValidation_%s/Manny_to_Mixamo"), *Stamp), nullptr, true) ? 0 : 1;
        Failures += RunExportCase(TEXT("Manny_to_Quinn"), Manny, Quinn, {UEWalkAnim}, FString::Printf(TEXT("/Game/FX_RetargetAssistant/Exports/FunctionalValidation_%s/Manny_to_Quinn"), *Stamp), nullptr, true) ? 0 : 1;

        Failures += RunDuplicateOutputValidation(CenterBlock, Manny, HipHopAnim, FString::Printf(TEXT("/Game/FX_RetargetAssistant/Exports/FunctionalValidation_%s/DuplicateNames"), *Stamp)) ? 0 : 1;

        UIKRetargeter* MixamoToMannyRetargeter = LoadObject<UIKRetargeter>(nullptr, *MixamoToMannyReport.GeneratedRetargeterPath);
        Failures += RunUserRetargeterSafetyValidation(MixamoToMannyRetargeter) ? 0 : 1;

        UE_LOG(LogTemp, Display, TEXT("FXRA FUNCTIONAL VALIDATION RESULT: %s failures=%d"), Failures == 0 ? TEXT("PASS") : TEXT("FAIL"), Failures);
        return Failures == 0 ? 0 : 1;
    }
}

UFX_RetargetAssistantSmokeTestCommandlet::UFX_RetargetAssistantSmokeTestCommandlet()
{
    IsClient = false;
    IsEditor = true;
    IsServer = false;
    LogToConsole = true;
}

int32 UFX_RetargetAssistantSmokeTestCommandlet::Main(const FString& Params)
{
    if (Params.Contains(TEXT("FunctionalValidation")))
    {
        return RunFunctionalValidation();
    }

    UE_LOG(LogTemp, Display, TEXT("FX Retarget Assistant smoke test started."));

    FFRA_MixamoTestSet TestSet;
    FString SetupError;
    if (!FFX_RetargetAssistantSetupManager::PrepareMixamoToUE5MannyTestSet(TestSet, SetupError))
    {
        UE_LOG(LogTemp, Warning, TEXT("Required test assets are missing in FXRA58. Please import or migrate test assets."));
        UE_LOG(LogTemp, Warning, TEXT("%s"), *SetupError);
        return 0;
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
        LogMessage(Message);
    }

    if (!Preflight.bCanRun)
    {
        UE_LOG(LogTemp, Error, TEXT("Smoke test blocked by preflight."));
        return 1;
    }

    UE_LOG(LogTemp, Display, TEXT("Headless preflight passed for %d animation(s)."), Input.AssetsToRetarget.Num());
    UE_LOG(LogTemp, Display, TEXT("Headless export is not executed by this migration smoke test."));
    UE_LOG(LogTemp, Display, TEXT("UE5.8 commandlet / editor automation / batch export behavior must be revalidated separately."));
    return 0;
}
