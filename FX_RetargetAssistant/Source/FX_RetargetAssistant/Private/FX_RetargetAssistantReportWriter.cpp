#include "FX_RetargetAssistantReportWriter.h"

#include "FX_RetargetAssistantPathManager.h"
#include "HAL/FileManager.h"
#include "JsonObjectConverter.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonSerializer.h"

namespace
{
    FString SeverityToString(EFRA_LogSeverity Severity)
    {
        switch (Severity)
        {
        case EFRA_LogSeverity::Warning:
            return TEXT("warning");
        case EFRA_LogSeverity::Error:
            return TEXT("error");
        default:
            return TEXT("info");
        }
    }

    TSharedRef<FJsonObject> MakeMessageJson(const FFRA_LogMessage& Message)
    {
        TSharedRef<FJsonObject> Json = MakeShared<FJsonObject>();
        Json->SetStringField(TEXT("severity"), SeverityToString(Message.Severity));
        Json->SetStringField(TEXT("code"), Message.Code);
        Json->SetStringField(TEXT("message"), Message.Message);
        return Json;
    }
}

bool FFX_RetargetAssistantReportWriter::WriteReport(const FFRA_BatchRetargetReport& Report, FString& OutDiskPath, FString& OutError)
{
    FString DiskFolder;
    if (!FFX_RetargetAssistantPathManager::TryConvertPackagePathToDiskPath(Report.OutputFolder, DiskFolder))
    {
        OutError = FString::Printf(TEXT("Cannot convert output package path to disk path: %s"), *Report.OutputFolder);
        return false;
    }

    IFileManager::Get().MakeDirectory(*DiskFolder, true);
    OutDiskPath = FPaths::Combine(DiskFolder, TEXT("Report.json"));

    TSharedRef<FJsonObject> Root = MakeShared<FJsonObject>();
    Root->SetStringField(TEXT("plugin"), TEXT("FX_RetargetAssistant"));
    Root->SetStringField(TEXT("engineVersion"), Report.EngineVersion);
    Root->SetStringField(TEXT("timestamp"), Report.Timestamp);
    Root->SetStringField(TEXT("setupMode"), Report.SetupMode);
    Root->SetStringField(TEXT("sourceMesh"), Report.SourceMeshPath);
    Root->SetStringField(TEXT("targetMesh"), Report.TargetMeshPath);
    Root->SetStringField(TEXT("sourceSkeleton"), Report.SourceSkeletonPath);
    Root->SetStringField(TEXT("targetSkeleton"), Report.TargetSkeletonPath);
    Root->SetStringField(TEXT("ikRetargeter"), Report.RetargeterPath);
    Root->SetStringField(TEXT("generatedSourceIKRig"), Report.GeneratedSourceIKRigPath);
    Root->SetStringField(TEXT("generatedTargetIKRig"), Report.GeneratedTargetIKRigPath);
    Root->SetStringField(TEXT("generatedRetargeter"), Report.GeneratedRetargeterPath);
    Root->SetStringField(TEXT("chainMappingSummary"), Report.ChainMappingSummary);
    Root->SetStringField(TEXT("outputFolder"), Report.OutputFolder);

    TSharedRef<FJsonObject> NamingRule = MakeShared<FJsonObject>();
    NamingRule->SetStringField(TEXT("suffix"), Report.Suffix);
    NamingRule->SetStringField(TEXT("conflictPolicy"), Report.ConflictPolicy);
    NamingRule->SetBoolField(TEXT("overwrite"), Report.bOverwriteExisting);
    Root->SetObjectField(TEXT("namingRule"), NamingRule);

    TSharedRef<FJsonObject> Preflight = MakeShared<FJsonObject>();
    TArray<TSharedPtr<FJsonValue>> Errors;
    TArray<TSharedPtr<FJsonValue>> Warnings;
    TArray<TSharedPtr<FJsonValue>> Infos;
    for (const FFRA_LogMessage& Message : Report.PreflightMessages)
    {
        TSharedPtr<FJsonValueObject> Value = MakeShared<FJsonValueObject>(MakeMessageJson(Message));
        if (Message.Severity == EFRA_LogSeverity::Error)
        {
            Errors.Add(Value);
        }
        else if (Message.Severity == EFRA_LogSeverity::Warning)
        {
            Warnings.Add(Value);
        }
        else
        {
            Infos.Add(Value);
        }
    }
    Preflight->SetArrayField(TEXT("errors"), Errors);
    Preflight->SetArrayField(TEXT("warnings"), Warnings);
    Preflight->SetArrayField(TEXT("infos"), Infos);
    Root->SetObjectField(TEXT("preflight"), Preflight);

    TArray<TSharedPtr<FJsonValue>> ReportWarnings;
    for (const FString& Warning : Report.Warnings)
    {
        ReportWarnings.Add(MakeShared<FJsonValueString>(Warning));
    }
    Root->SetArrayField(TEXT("warnings"), ReportWarnings);

    TArray<TSharedPtr<FJsonValue>> Animations;
    for (const FFRA_BatchRetargetItemResult& Item : Report.Items)
    {
        TSharedRef<FJsonObject> ItemJson = MakeShared<FJsonObject>();
        ItemJson->SetStringField(TEXT("source"), Item.SourceAnimationPath);
        ItemJson->SetStringField(TEXT("output"), Item.OutputAnimationPath);
        ItemJson->SetStringField(TEXT("status"), Item.bSuccess ? TEXT("success") : TEXT("failed"));
        ItemJson->SetStringField(TEXT("message"), Item.Message);
        Animations.Add(MakeShared<FJsonValueObject>(ItemJson));
    }
    Root->SetArrayField(TEXT("animations"), Animations);

    TSharedRef<FJsonObject> Summary = MakeShared<FJsonObject>();
    Summary->SetNumberField(TEXT("total"), Report.TotalCount);
    Summary->SetNumberField(TEXT("success"), Report.SuccessCount);
    Summary->SetNumberField(TEXT("failed"), Report.FailedCount);
    Root->SetObjectField(TEXT("summary"), Summary);

    FString JsonText;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonText);
    if (!FJsonSerializer::Serialize(Root, Writer))
    {
        OutError = TEXT("Failed to serialize Report.json.");
        return false;
    }

    if (!FFileHelper::SaveStringToFile(JsonText, *OutDiskPath))
    {
        OutError = FString::Printf(TEXT("Failed to write Report.json: %s"), *OutDiskPath);
        return false;
    }

    return true;
}

