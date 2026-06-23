#pragma once

#include "FX_RetargetAssistantTypes.h"

class FFX_RetargetAssistantPreflightValidator
{
public:
    static FFRA_PreflightResult Validate(const FFRA_BatchRetargetInput& Input);

private:
    static void Add(FFRA_PreflightResult& Result, EFRA_LogSeverity Severity, const FString& Code, const FString& Message);
};
