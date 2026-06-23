#pragma once

#include "CoreMinimal.h"

class UAnimSequence;

class FFX_RetargetAssistantAssetScanner
{
public:
    static TArray<FAssetData> GetSelectedAnimSequences();
};
