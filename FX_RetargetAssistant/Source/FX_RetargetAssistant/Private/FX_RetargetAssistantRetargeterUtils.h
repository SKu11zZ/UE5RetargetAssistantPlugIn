#pragma once

#include "CoreMinimal.h"

class UIKRetargeter;

struct FFRA_RetargetOpsStackInfo
{
    bool bValid = false;
    int32 Count = 0;
    TArray<FString> OpTypeNames;
};

class FFX_RetargetAssistantRetargeterUtils
{
public:
    static constexpr const TCHAR* GeneratedSetupRoot = TEXT("/Game/FX_RetargetAssistant/Setups/");

    static FFRA_RetargetOpsStackInfo GetRetargetOpsStackInfo(const UIKRetargeter* Retargeter);
    static bool IsGeneratedSetupRetargeter(const UIKRetargeter* Retargeter);
    static bool IsGeneratedSetupPath(const FString& ObjectPath);
};
