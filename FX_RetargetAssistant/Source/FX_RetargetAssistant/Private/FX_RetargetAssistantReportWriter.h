#pragma once

#include "FX_RetargetAssistantTypes.h"

class FFX_RetargetAssistantReportWriter
{
public:
    static bool WriteReport(const FFRA_BatchRetargetReport& Report, FString& OutDiskPath, FString& OutError);
};
