#pragma once

#include "Commandlets/Commandlet.h"
#include "FX_RetargetAssistantSmokeTestCommandlet.generated.h"

UCLASS()
class UFX_RetargetAssistantSmokeTestCommandlet : public UCommandlet
{
    GENERATED_BODY()

public:
    UFX_RetargetAssistantSmokeTestCommandlet();

    virtual int32 Main(const FString& Params) override;
};
