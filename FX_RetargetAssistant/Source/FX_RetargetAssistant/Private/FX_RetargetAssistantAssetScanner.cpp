#include "FX_RetargetAssistantAssetScanner.h"

#include "Animation/AnimSequence.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "Modules/ModuleManager.h"

TArray<FAssetData> FFX_RetargetAssistantAssetScanner::GetSelectedAnimSequences()
{
    TArray<FAssetData> SelectedAssets;
    FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
    ContentBrowserModule.Get().GetSelectedAssets(SelectedAssets);

    TArray<FAssetData> AnimSequences;
    for (const FAssetData& AssetData : SelectedAssets)
    {
        if (AssetData.GetClass() && AssetData.GetClass()->IsChildOf(UAnimSequence::StaticClass()))
        {
            AnimSequences.Add(AssetData);
        }
    }
    return AnimSequences;
}
