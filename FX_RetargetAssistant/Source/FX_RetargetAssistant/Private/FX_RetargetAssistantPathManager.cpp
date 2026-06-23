#include "FX_RetargetAssistantPathManager.h"

#include "Misc/DateTime.h"
#include "Misc/PackageName.h"
#include "HAL/FileManager.h"

FString FFX_RetargetAssistantPathManager::MakeDefaultBatchOutputPath()
{
    const FString Timestamp = FDateTime::Now().ToString(TEXT("%Y%m%d_%H%M"));
    return FString::Printf(TEXT("/Game/FX_RetargetAssistant/Exports/Batch_%s/Animations"), *Timestamp);
}

bool FFX_RetargetAssistantPathManager::EnsurePackagePathExists(const FString& PackagePath, FString& OutError)
{
    FString DiskPath;
    if (!TryConvertPackagePathToDiskPath(PackagePath, DiskPath))
    {
        OutError = FString::Printf(TEXT("Invalid package path: %s"), *PackagePath);
        return false;
    }

    if (!IFileManager::Get().MakeDirectory(*DiskPath, true))
    {
        OutError = FString::Printf(TEXT("Failed to create directory for package path: %s"), *PackagePath);
        return false;
    }

    return true;
}

bool FFX_RetargetAssistantPathManager::TryConvertPackagePathToDiskPath(const FString& PackagePath, FString& OutDiskPath)
{
    FString NormalizedPath = PackagePath;
    NormalizedPath.RemoveFromEnd(TEXT("/"));
    if (!FPackageName::IsValidLongPackageName(NormalizedPath, false))
    {
        return false;
    }

    return FPackageName::TryConvertLongPackageNameToFilename(NormalizedPath, OutDiskPath);
}
