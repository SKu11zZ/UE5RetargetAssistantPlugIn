#include "FX_RetargetAssistantRetargeterUtils.h"

#include "Retargeter/IKRetargeter.h"

FFRA_RetargetOpsStackInfo FFX_RetargetAssistantRetargeterUtils::GetRetargetOpsStackInfo(const UIKRetargeter* Retargeter)
{
    FFRA_RetargetOpsStackInfo Info;
    if (!Retargeter)
    {
        return Info;
    }

    const TArray<FInstancedStruct>& RetargetOps = Retargeter->GetRetargetOps();
    Info.Count = RetargetOps.Num();
    Info.bValid = Info.Count > 0;

    for (const FInstancedStruct& OpStruct : RetargetOps)
    {
        const UScriptStruct* ScriptStruct = OpStruct.GetScriptStruct();
        Info.OpTypeNames.Add(ScriptStruct ? ScriptStruct->GetName() : TEXT("<invalid>"));
    }

    return Info;
}

bool FFX_RetargetAssistantRetargeterUtils::IsGeneratedSetupRetargeter(const UIKRetargeter* Retargeter)
{
    return Retargeter && IsGeneratedSetupPath(Retargeter->GetPathName());
}

bool FFX_RetargetAssistantRetargeterUtils::IsGeneratedSetupPath(const FString& ObjectPath)
{
    return ObjectPath.StartsWith(GeneratedSetupRoot);
}
