#include "FX_RetargetAssistantModule.h"

#include "SFX_RetargetAssistantPanel.h"
#include "ToolMenus.h"
#include "Widgets/Docking/SDockTab.h"

#define LOCTEXT_NAMESPACE "FX_RetargetAssistant"

namespace
{
    const FName RetargetAssistantTabName(TEXT("FX_RetargetAssistant"));
}

void FFX_RetargetAssistantModule::StartupModule()
{
    FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
        RetargetAssistantTabName,
        FOnSpawnTab::CreateLambda([](const FSpawnTabArgs&)
        {
            return SNew(SDockTab)
                .TabRole(ETabRole::NomadTab)
                [
                    SNew(SFX_RetargetAssistantPanel)
                ];
        }))
        .SetDisplayName(LOCTEXT("TabTitle", "FX Retarget Assistant"))
        .SetTooltipText(LOCTEXT("TabTooltip", "Open FX Retarget Assistant"))
        .SetMenuType(ETabSpawnerMenuType::Hidden);

    UToolMenus::RegisterStartupCallback(
        FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FFX_RetargetAssistantModule::RegisterMenus));
}

void FFX_RetargetAssistantModule::ShutdownModule()
{
    if (UToolMenus::IsToolMenuUIEnabled())
    {
        UToolMenus::UnRegisterStartupCallback(this);
        UToolMenus::UnregisterOwner(this);
    }

    FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(RetargetAssistantTabName);
}

void FFX_RetargetAssistantModule::RegisterMenus()
{
    FToolMenuOwnerScoped OwnerScoped(this);

    UToolMenu* Menu = UToolMenus::Get()->ExtendMenu(TEXT("LevelEditor.MainMenu.Window"));
    FToolMenuSection& Section = Menu->FindOrAddSection(TEXT("WindowLayout"));
    Section.AddMenuEntry(
        TEXT("FX_RetargetAssistant_Open"),
        LOCTEXT("OpenMenuLabel", "FX Retarget Assistant"),
        LOCTEXT("OpenMenuTooltip", "Open the FX Retarget Assistant panel for UE5.8 guided IK Retargeter setup and batch animation export."),
        FSlateIcon(),
        FUIAction(FExecuteAction::CreateRaw(this, &FFX_RetargetAssistantModule::OpenAssistantTab)));
}

void FFX_RetargetAssistantModule::OpenAssistantTab()
{
    FGlobalTabmanager::Get()->TryInvokeTab(RetargetAssistantTabName);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FFX_RetargetAssistantModule, FX_RetargetAssistant)
