#pragma once

#include "FX_RetargetAssistantTypes.h"

class FFX_RetargetAssistantBatchExporter
{
public:
    static bool Run(const FFRA_BatchRetargetInput& Input, const FFRA_PreflightResult& Preflight, FFRA_BatchRetargetReport& OutReport);
};
