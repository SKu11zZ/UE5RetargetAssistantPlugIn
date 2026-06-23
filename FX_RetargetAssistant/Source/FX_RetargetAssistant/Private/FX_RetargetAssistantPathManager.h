#pragma once

#include "CoreMinimal.h"

class FFX_RetargetAssistantPathManager
{
public:
    static FString MakeDefaultBatchOutputPath();
    static bool EnsurePackagePathExists(const FString& PackagePath, FString& OutError);
    static bool TryConvertPackagePathToDiskPath(const FString& PackagePath, FString& OutDiskPath);
};
