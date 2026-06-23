#include "FX_RetargetAssistantSetupManager.h"

#include "Animation/AnimSequence.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/SkeletalMesh.h"
#include "Misc/PackageName.h"
#include "RetargetEditor/IKRetargeterController.h"
#include "Retargeter/IKRetargeter.h"
#include "Rig/IKRigDefinition.h"
#include "RigEditor/IKRigController.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"

namespace
{
    constexpr const TCHAR* TestSetFolder = TEXT("/Game/FX_RetargetAssistant/TestSet");
    constexpr const TCHAR* MixamoIKRigPath = TEXT("/Game/FX_RetargetAssistant/TestSet/IK_Mixamo_FXRA.IK_Mixamo_FXRA");
    constexpr const TCHAR* MixamoIKRigPackage = TEXT("/Game/FX_RetargetAssistant/TestSet/IK_Mixamo_FXRA");
    constexpr const TCHAR* MixamoRetargeterPath = TEXT("/Game/FX_RetargetAssistant/TestSet/RTG_Mixamo_to_UE5Manny_FXRA.RTG_Mixamo_to_UE5Manny_FXRA");
    constexpr const TCHAR* MixamoRetargeterPackage = TEXT("/Game/FX_RetargetAssistant/TestSet/RTG_Mixamo_to_UE5Manny_FXRA");

    bool HasBone(const USkeletalMesh* Mesh, const FName BoneName)
    {
        return Mesh && Mesh->GetRefSkeleton().FindBoneIndex(BoneName) != INDEX_NONE;
    }

    FName FirstExistingBone(const USkeletalMesh* Mesh, std::initializer_list<const TCHAR*> BoneNames)
    {
        for (const TCHAR* BoneName : BoneNames)
        {
            const FName Candidate(BoneName);
            if (HasBone(Mesh, Candidate))
            {
                return Candidate;
            }
        }
        return NAME_None;
    }

    bool SaveAsset(UObject* Asset, FString& OutError)
    {
        if (!Asset)
        {
            OutError = TEXT("Asset is null.");
            return false;
        }

        UPackage* Package = Asset->GetOutermost();
        if (!Package)
        {
            OutError = FString::Printf(TEXT("Asset has no package: %s"), *Asset->GetPathName());
            return false;
        }

        Package->SetDirtyFlag(true);
        FSavePackageArgs SaveArgs;
        SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
        SaveArgs.SaveFlags = SAVE_None;

        const FString Filename = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());
        if (!UPackage::SavePackage(Package, Asset, *Filename, SaveArgs))
        {
            OutError = FString::Printf(TEXT("Failed to save package: %s"), *Package->GetName());
            return false;
        }

        return true;
    }

    UIKRigDefinition* CreateOrLoadIKRig(USkeletalMesh* SourceMesh, FFRA_MixamoTestSet& OutTestSet, FString& OutError)
    {
        if (UIKRigDefinition* Existing = LoadObject<UIKRigDefinition>(nullptr, MixamoIKRigPath))
        {
            OutTestSet.Messages.Add(FString::Printf(TEXT("Using existing Mixamo IK Rig: %s"), *Existing->GetPathName()));
            return Existing;
        }

        UPackage* Package = CreatePackage(MixamoIKRigPackage);
        UIKRigDefinition* IKRig = NewObject<UIKRigDefinition>(Package, TEXT("IK_Mixamo_FXRA"), RF_Public | RF_Standalone | RF_Transactional);
        if (!IKRig)
        {
            OutError = TEXT("Failed to create Mixamo IK Rig asset.");
            return nullptr;
        }

        UIKRigController* Controller = UIKRigController::GetController(IKRig);
        if (!Controller || !Controller->SetSkeletalMesh(SourceMesh))
        {
            OutError = TEXT("Failed to initialize Mixamo IK Rig with source mesh.");
            return nullptr;
        }

        const FName Hips = FirstExistingBone(SourceMesh, {TEXT("Hips"), TEXT("mixamorig:Hips"), TEXT("mixamorig_Hips")});
        if (Hips == NAME_None)
        {
            OutError = TEXT("Mixamo source mesh does not contain Hips bone.");
            return nullptr;
        }

        Controller->SetRetargetRoot(Hips);

        auto AddChain = [Controller, SourceMesh, &OutTestSet](const TCHAR* ChainName, std::initializer_list<const TCHAR*> StartBones, std::initializer_list<const TCHAR*> EndBones)
        {
            const FName StartBone = FirstExistingBone(SourceMesh, StartBones);
            const FName EndBone = FirstExistingBone(SourceMesh, EndBones);
            if (StartBone == NAME_None || EndBone == NAME_None)
            {
                OutTestSet.Messages.Add(FString::Printf(TEXT("Skipped Mixamo chain %s because a required bone was not found."), ChainName));
                return;
            }
            Controller->AddRetargetChain(FName(ChainName), StartBone, EndBone, NAME_None);
        };

        AddChain(TEXT("Spine"), {TEXT("Spine"), TEXT("mixamorig:Spine"), TEXT("mixamorig_Spine")}, {TEXT("Spine2"), TEXT("mixamorig:Spine2"), TEXT("mixamorig_Spine2")});
        AddChain(TEXT("Head"), {TEXT("Neck"), TEXT("mixamorig:Neck"), TEXT("mixamorig_Neck")}, {TEXT("Head"), TEXT("mixamorig:Head"), TEXT("mixamorig_Head")});
        AddChain(TEXT("LeftArm"), {TEXT("LeftArm"), TEXT("mixamorig:LeftArm"), TEXT("mixamorig_LeftArm")}, {TEXT("LeftHand"), TEXT("mixamorig:LeftHand"), TEXT("mixamorig_LeftHand")});
        AddChain(TEXT("RightArm"), {TEXT("RightArm"), TEXT("mixamorig:RightArm"), TEXT("mixamorig_RightArm")}, {TEXT("RightHand"), TEXT("mixamorig:RightHand"), TEXT("mixamorig_RightHand")});
        AddChain(TEXT("LeftLeg"), {TEXT("LeftUpLeg"), TEXT("mixamorig:LeftUpLeg"), TEXT("mixamorig_LeftUpLeg")}, {TEXT("LeftToeBase"), TEXT("mixamorig:LeftToeBase"), TEXT("mixamorig_LeftToeBase"), TEXT("LeftFoot"), TEXT("mixamorig:LeftFoot"), TEXT("mixamorig_LeftFoot")});
        AddChain(TEXT("RightLeg"), {TEXT("RightUpLeg"), TEXT("mixamorig:RightUpLeg"), TEXT("mixamorig_RightUpLeg")}, {TEXT("RightToeBase"), TEXT("mixamorig:RightToeBase"), TEXT("mixamorig_RightToeBase"), TEXT("RightFoot"), TEXT("mixamorig:RightFoot"), TEXT("mixamorig_RightFoot")});

        Controller->SortRetargetChains();
        FAssetRegistryModule::AssetCreated(IKRig);

        if (!SaveAsset(IKRig, OutError))
        {
            return nullptr;
        }

        OutTestSet.Messages.Add(FString::Printf(TEXT("Created Mixamo IK Rig: %s"), *IKRig->GetPathName()));
        return IKRig;
    }

    bool ConfigureAndSaveRetargeter(UIKRetargeter* Retargeter, UIKRigDefinition* SourceIKRig, UIKRigDefinition* TargetIKRig, USkeletalMesh* SourceMesh, USkeletalMesh* TargetMesh, FFRA_MixamoTestSet& OutTestSet, FString& OutError)
    {
        if (!Retargeter)
        {
            OutError = TEXT("Mixamo retargeter asset is null.");
            return false;
        }

        const UIKRetargeterController* Controller = UIKRetargeterController::GetController(Retargeter);
        if (!Controller)
        {
            OutError = TEXT("Failed to create Mixamo retargeter controller.");
            return false;
        }

        {
            FScopedReinitializeIKRetargeter Reinitialize(Controller);
            Controller->SetIKRig(ERetargetSourceOrTarget::Source, SourceIKRig);
            Controller->SetIKRig(ERetargetSourceOrTarget::Target, TargetIKRig);
            Controller->SetPreviewMesh(ERetargetSourceOrTarget::Source, SourceMesh);
            Controller->SetPreviewMesh(ERetargetSourceOrTarget::Target, TargetMesh);
            Controller->AutoMapChains(EAutoMapChainType::Exact, true);
            Controller->CleanAsset();
            Controller->SetSourceChain(NAME_None, FName(TEXT("Pelvis")));

            const FName TargetRetargetPoseName = Controller->GetCurrentRetargetPoseName(ERetargetSourceOrTarget::Target);
            Controller->ResetRetargetPose(TargetRetargetPoseName, TArray<FName>(), ERetargetSourceOrTarget::Target);
            Controller->AutoAlignAllBones(ERetargetSourceOrTarget::Target);
        }

        FAssetRegistryModule::AssetCreated(Retargeter);

        if (!SaveAsset(Retargeter, OutError))
        {
            return false;
        }

        OutTestSet.Messages.Add(TEXT("Auto-aligned target retarget pose for Mixamo -> UE5 Manny."));
        return true;
    }

    UIKRetargeter* CreateOrLoadRetargeter(UIKRigDefinition* SourceIKRig, UIKRigDefinition* TargetIKRig, USkeletalMesh* SourceMesh, USkeletalMesh* TargetMesh, FFRA_MixamoTestSet& OutTestSet, FString& OutError)
    {
        if (UIKRetargeter* Existing = LoadObject<UIKRetargeter>(nullptr, MixamoRetargeterPath))
        {
            if (!ConfigureAndSaveRetargeter(Existing, SourceIKRig, TargetIKRig, SourceMesh, TargetMesh, OutTestSet, OutError))
            {
                return nullptr;
            }

            OutTestSet.Messages.Add(FString::Printf(TEXT("Using updated Mixamo retargeter: %s"), *Existing->GetPathName()));
            return Existing;
        }

        UPackage* Package = CreatePackage(MixamoRetargeterPackage);
        UIKRetargeter* Retargeter = NewObject<UIKRetargeter>(Package, TEXT("RTG_Mixamo_to_UE5Manny_FXRA"), RF_Public | RF_Standalone | RF_Transactional);
        if (!Retargeter)
        {
            OutError = TEXT("Failed to create Mixamo retargeter asset.");
            return nullptr;
        }

        if (!ConfigureAndSaveRetargeter(Retargeter, SourceIKRig, TargetIKRig, SourceMesh, TargetMesh, OutTestSet, OutError))
        {
            return nullptr;
        }

        OutTestSet.Messages.Add(FString::Printf(TEXT("Created Mixamo retargeter: %s"), *Retargeter->GetPathName()));
        return Retargeter;
    }

    FString SanitizeAssetToken(FString Name)
    {
        Name.TrimStartAndEndInline();
        const TCHAR* InvalidChars = TEXT(" .,:;\\/|?*\"'`~!@#$%^&()+={}[]<>");
        for (int32 Index = 0; InvalidChars[Index] != 0; ++Index)
        {
            Name.ReplaceCharInline(InvalidChars[Index], TEXT('_'));
        }
        while (Name.Contains(TEXT("__")))
        {
            Name.ReplaceInline(TEXT("__"), TEXT("_"));
        }
        Name.RemoveFromStart(TEXT("_"));
        Name.RemoveFromEnd(TEXT("_"));
        return Name.IsEmpty() ? TEXT("Mesh") : Name;
    }

    FString MakeObjectPath(const FString& PackagePath, const FString& AssetName)
    {
        return FString::Printf(TEXT("%s.%s"), *PackagePath, *AssetName);
    }

    FString NormalizeBoneToken(FString Name)
    {
        Name.ToLowerInline();
        Name.RemoveFromStart(TEXT("mixamorig:"));
        Name.RemoveFromStart(TEXT("mixamorig_"));
        Name.ReplaceInline(TEXT("-"), TEXT("_"));
        return Name;
    }

    FName FirstExistingBoneFlexible(const USkeletalMesh* Mesh, std::initializer_list<const TCHAR*> BoneNames)
    {
        if (!Mesh)
        {
            return NAME_None;
        }

        for (const TCHAR* BoneName : BoneNames)
        {
            const FName Exact(BoneName);
            if (HasBone(Mesh, Exact))
            {
                return Exact;
            }
        }

        const FReferenceSkeleton& RefSkeleton = Mesh->GetRefSkeleton();
        for (const TCHAR* BoneName : BoneNames)
        {
            const FString Wanted = NormalizeBoneToken(BoneName);
            for (int32 BoneIndex = 0; BoneIndex < RefSkeleton.GetNum(); ++BoneIndex)
            {
                const FName ExistingName = RefSkeleton.GetBoneName(BoneIndex);
                if (NormalizeBoneToken(ExistingName.ToString()) == Wanted)
                {
                    return ExistingName;
                }
            }
        }

        return NAME_None;
    }


    bool IsLikelyMixamoMesh(const USkeletalMesh* Mesh)
    {
        return FirstExistingBoneFlexible(Mesh, {TEXT("Hips"), TEXT("mixamorig:Hips"), TEXT("mixamorig_Hips")}) != NAME_None
            && FirstExistingBoneFlexible(Mesh, {TEXT("LeftArm"), TEXT("mixamorig:LeftArm"), TEXT("mixamorig_LeftArm")}) != NAME_None;
    }

    bool IsLikelyUEMannyFamilyMesh(const USkeletalMesh* Mesh)
    {
        return FirstExistingBoneFlexible(Mesh, {TEXT("pelvis")}) != NAME_None
            && FirstExistingBoneFlexible(Mesh, {TEXT("upperarm_l")}) != NAME_None
            && FirstExistingBoneFlexible(Mesh, {TEXT("thigh_l")}) != NAME_None;
    }

    bool ShouldApplyMixamoToUEPreset(const USkeletalMesh* SourceMesh, const USkeletalMesh* TargetMesh)
    {
        return IsLikelyMixamoMesh(SourceMesh) && IsLikelyUEMannyFamilyMesh(TargetMesh);
    }
    void AddGeneratedChain(UIKRigController* Controller, const USkeletalMesh* Mesh, TArray<FString>& Messages, const TCHAR* ChainName, std::initializer_list<const TCHAR*> StartBones, std::initializer_list<const TCHAR*> EndBones)
    {
        const FName StartBone = FirstExistingBoneFlexible(Mesh, StartBones);
        const FName EndBone = FirstExistingBoneFlexible(Mesh, EndBones);
        if (StartBone == NAME_None || EndBone == NAME_None)
        {
            Messages.Add(FString::Printf(TEXT("Skipped chain %s because a required bone was not found."), ChainName));
            return;
        }

        Controller->AddRetargetChain(FName(ChainName), StartBone, EndBone, NAME_None);
        Messages.Add(FString::Printf(TEXT("Added chain %s: %s -> %s"), ChainName, *StartBone.ToString(), *EndBone.ToString()));
    }

    UIKRigDefinition* CreateOrLoadGeneratedIKRig(USkeletalMesh* Mesh, const FString& PackagePath, const FString& AssetName, FFRA_AutoRetargetSetup& OutSetup, FString& OutError, bool bRecreateExisting)
    {
        if (!Mesh)
        {
            OutError = TEXT("Cannot create IK Rig without a Skeletal Mesh.");
            return nullptr;
        }

        if (UIKRigDefinition* Existing = LoadObject<UIKRigDefinition>(nullptr, *MakeObjectPath(PackagePath, AssetName)))
        {
            if (!bRecreateExisting)
            {
                OutSetup.Messages.Add(FString::Printf(TEXT("Reusing existing generated IK Rig: %s"), *Existing->GetPathName()));
                return Existing;
            }
            OutSetup.Messages.Add(FString::Printf(TEXT("Recreating generated IK Rig in existing asset: %s"), *Existing->GetPathName()));
        }

        UPackage* Package = CreatePackage(*PackagePath);
        UIKRigDefinition* IKRig = LoadObject<UIKRigDefinition>(nullptr, *MakeObjectPath(PackagePath, AssetName));
        if (!IKRig)
        {
            IKRig = NewObject<UIKRigDefinition>(Package, *AssetName, RF_Public | RF_Standalone | RF_Transactional);
        }
        if (!IKRig)
        {
            OutError = FString::Printf(TEXT("Failed to create IK Rig asset: %s"), *PackagePath);
            return nullptr;
        }

        UIKRigController* Controller = UIKRigController::GetController(IKRig);
        if (!Controller || !Controller->SetSkeletalMesh(Mesh))
        {
            OutError = FString::Printf(TEXT("Failed to initialize IK Rig with mesh: %s"), *Mesh->GetPathName());
            return nullptr;
        }

        if (bRecreateExisting)
        {
            TArray<FName> ExistingChainNames;
            for (const FBoneChain& ExistingChain : Controller->GetRetargetChains())
            {
                ExistingChainNames.Add(ExistingChain.ChainName);
            }
            for (const FName& ExistingChainName : ExistingChainNames)
            {
                Controller->RemoveRetargetChain(ExistingChainName);
            }
        }

        const FName RetargetRoot = FirstExistingBoneFlexible(Mesh, {TEXT("pelvis"), TEXT("Hips"), TEXT("hips"), TEXT("hip"), TEXT("root")});
        if (RetargetRoot != NAME_None)
        {
            Controller->SetRetargetRoot(RetargetRoot);
            OutSetup.Messages.Add(FString::Printf(TEXT("Set Retarget Root for %s: %s"), *AssetName, *RetargetRoot.ToString()));
        }
        else
        {
            OutSetup.Messages.Add(FString::Printf(TEXT("No obvious Retarget Root found for %s."), *AssetName));
        }

        AddGeneratedChain(Controller, Mesh, OutSetup.Messages, TEXT("Root"), {TEXT("root"), TEXT("pelvis"), TEXT("Hips")}, {TEXT("root"), TEXT("pelvis"), TEXT("Hips")});
        AddGeneratedChain(Controller, Mesh, OutSetup.Messages, TEXT("Pelvis"), {TEXT("pelvis"), TEXT("Hips"), TEXT("hips"), TEXT("hip")}, {TEXT("pelvis"), TEXT("Hips"), TEXT("hips"), TEXT("hip")});
        AddGeneratedChain(Controller, Mesh, OutSetup.Messages, TEXT("Spine"), {TEXT("spine_01"), TEXT("Spine"), TEXT("spine")}, {TEXT("spine_05"), TEXT("spine_04"), TEXT("spine_03"), TEXT("Spine2"), TEXT("Spine1"), TEXT("Spine")});
        AddGeneratedChain(Controller, Mesh, OutSetup.Messages, TEXT("Neck"), {TEXT("neck_01"), TEXT("Neck"), TEXT("neck")}, {TEXT("neck_01"), TEXT("Neck"), TEXT("neck")});
        AddGeneratedChain(Controller, Mesh, OutSetup.Messages, TEXT("Head"), {TEXT("head"), TEXT("Head")}, {TEXT("head"), TEXT("Head")});
        AddGeneratedChain(Controller, Mesh, OutSetup.Messages, TEXT("LeftClavicle"), {TEXT("clavicle_l"), TEXT("LeftShoulder")}, {TEXT("clavicle_l"), TEXT("LeftShoulder")});
        AddGeneratedChain(Controller, Mesh, OutSetup.Messages, TEXT("LeftArm"), {TEXT("upperarm_l"), TEXT("LeftArm")}, {TEXT("hand_l"), TEXT("LeftHand")});
        AddGeneratedChain(Controller, Mesh, OutSetup.Messages, TEXT("RightClavicle"), {TEXT("clavicle_r"), TEXT("RightShoulder")}, {TEXT("clavicle_r"), TEXT("RightShoulder")});
        AddGeneratedChain(Controller, Mesh, OutSetup.Messages, TEXT("RightArm"), {TEXT("upperarm_r"), TEXT("RightArm")}, {TEXT("hand_r"), TEXT("RightHand")});
        AddGeneratedChain(Controller, Mesh, OutSetup.Messages, TEXT("LeftLeg"), {TEXT("thigh_l"), TEXT("LeftUpLeg")}, {TEXT("ball_l"), TEXT("foot_l"), TEXT("LeftToeBase"), TEXT("LeftFoot")});
        AddGeneratedChain(Controller, Mesh, OutSetup.Messages, TEXT("RightLeg"), {TEXT("thigh_r"), TEXT("RightUpLeg")}, {TEXT("ball_r"), TEXT("foot_r"), TEXT("RightToeBase"), TEXT("RightFoot")});

        Controller->SortRetargetChains();
        FAssetRegistryModule::AssetCreated(IKRig);

        if (!SaveAsset(IKRig, OutError))
        {
            return nullptr;
        }

        OutSetup.Messages.Add(FString::Printf(TEXT("Created generated IK Rig: %s"), *IKRig->GetPathName()));
        return IKRig;
    }

    bool ConfigureGeneratedRetargeter(UIKRetargeter* Retargeter, UIKRigDefinition* SourceIKRig, UIKRigDefinition* TargetIKRig, USkeletalMesh* SourceMesh, USkeletalMesh* TargetMesh, FFRA_AutoRetargetSetup& OutSetup, FString& OutError)
    {
        if (!Retargeter || !SourceIKRig || !TargetIKRig || !SourceMesh || !TargetMesh)
        {
            OutError = TEXT("Generated Retargeter configuration has missing inputs.");
            return false;
        }

        const UIKRetargeterController* Controller = UIKRetargeterController::GetController(Retargeter);
        if (!Controller)
        {
            OutError = TEXT("Failed to create IK Retargeter controller.");
            return false;
        }

        {
            FScopedReinitializeIKRetargeter Reinitialize(Controller);
            Controller->SetIKRig(ERetargetSourceOrTarget::Source, SourceIKRig);
            Controller->SetIKRig(ERetargetSourceOrTarget::Target, TargetIKRig);
            Controller->SetPreviewMesh(ERetargetSourceOrTarget::Source, SourceMesh);
            Controller->SetPreviewMesh(ERetargetSourceOrTarget::Target, TargetMesh);
            Controller->AutoMapChains(EAutoMapChainType::Exact, true);
            Controller->CleanAsset();
            if (ShouldApplyMixamoToUEPreset(SourceMesh, TargetMesh))
            {
                Controller->SetSourceChain(NAME_None, FName(TEXT("Pelvis")));
                OutSetup.Messages.Add(TEXT("Target Chain Pelvis intentionally mapped to Source Chain None for Mixamo -> UE stability."));
            }

            const FName TargetPoseName = Controller->GetCurrentRetargetPoseName(ERetargetSourceOrTarget::Target);
            Controller->ResetRetargetPose(TargetPoseName, TArray<FName>(), ERetargetSourceOrTarget::Target);
            Controller->AutoAlignAllBones(ERetargetSourceOrTarget::Target);
        }

        FAssetRegistryModule::AssetCreated(Retargeter);
        if (!SaveAsset(Retargeter, OutError))
        {
            return false;
        }

        OutSetup.Messages.Add(TEXT("Configured generated IK Retargeter with exact chain mapping and target Auto Align."));
        return true;
    }

    UIKRetargeter* CreateOrLoadGeneratedRetargeter(UIKRigDefinition* SourceIKRig, UIKRigDefinition* TargetIKRig, USkeletalMesh* SourceMesh, USkeletalMesh* TargetMesh, const FString& PackagePath, const FString& AssetName, FFRA_AutoRetargetSetup& OutSetup, FString& OutError, bool bRecreateExisting)
    {
        if (UIKRetargeter* Existing = LoadObject<UIKRetargeter>(nullptr, *MakeObjectPath(PackagePath, AssetName)))
        {
            if (!bRecreateExisting)
            {
                OutSetup.Messages.Add(FString::Printf(TEXT("Reusing existing generated IK Retargeter: %s"), *Existing->GetPathName()));
                return Existing;
            }
            if (!ConfigureGeneratedRetargeter(Existing, SourceIKRig, TargetIKRig, SourceMesh, TargetMesh, OutSetup, OutError))
            {
                return nullptr;
            }
            OutSetup.Messages.Add(FString::Printf(TEXT("Recreated generated IK Retargeter: %s"), *Existing->GetPathName()));
            return Existing;
        }

        UPackage* Package = CreatePackage(*PackagePath);
        UIKRetargeter* Retargeter = NewObject<UIKRetargeter>(Package, *AssetName, RF_Public | RF_Standalone | RF_Transactional);
        if (!Retargeter)
        {
            OutError = FString::Printf(TEXT("Failed to create IK Retargeter asset: %s"), *PackagePath);
            return nullptr;
        }

        if (!ConfigureGeneratedRetargeter(Retargeter, SourceIKRig, TargetIKRig, SourceMesh, TargetMesh, OutSetup, OutError))
        {
            return nullptr;
        }

        OutSetup.Messages.Add(FString::Printf(TEXT("Created generated IK Retargeter: %s"), *Retargeter->GetPathName()));
        return Retargeter;
    }
}

bool FFX_RetargetAssistantSetupManager::PrepareAutoRetargetSetup(USkeletalMesh* SourceMesh, USkeletalMesh* TargetMesh, FFRA_AutoRetargetSetup& OutSetup, FString& OutError, bool bRecreateExisting)
{
    OutSetup = FFRA_AutoRetargetSetup();

    if (!SourceMesh)
    {
        OutError = TEXT("Source Character Mesh is missing.");
        return false;
    }
    if (!TargetMesh)
    {
        OutError = TEXT("Target Character Mesh is missing.");
        return false;
    }

    const FString SourceName = SanitizeAssetToken(SourceMesh->GetName());
    const FString TargetName = SanitizeAssetToken(TargetMesh->GetName());
    const FString PairName = FString::Printf(TEXT("%s_to_%s"), *SourceName, *TargetName);
    const FString SetupFolder = FString::Printf(TEXT("/Game/FX_RetargetAssistant/Setups/%s"), *PairName);
    const FString SourceIKRigName = FString::Printf(TEXT("IK_%s"), *SourceName);
    const FString TargetIKRigName = FString::Printf(TEXT("IK_%s"), *TargetName);
    const FString RetargeterName = FString::Printf(TEXT("RTG_%s"), *PairName);

    UIKRigDefinition* SourceIKRig = CreateOrLoadGeneratedIKRig(SourceMesh, FString::Printf(TEXT("%s/%s"), *SetupFolder, *SourceIKRigName), SourceIKRigName, OutSetup, OutError, bRecreateExisting);
    if (!SourceIKRig)
    {
        return false;
    }

    UIKRigDefinition* TargetIKRig = CreateOrLoadGeneratedIKRig(TargetMesh, FString::Printf(TEXT("%s/%s"), *SetupFolder, *TargetIKRigName), TargetIKRigName, OutSetup, OutError, bRecreateExisting);
    if (!TargetIKRig)
    {
        return false;
    }

    UIKRetargeter* Retargeter = CreateOrLoadGeneratedRetargeter(SourceIKRig, TargetIKRig, SourceMesh, TargetMesh, FString::Printf(TEXT("%s/%s"), *SetupFolder, *RetargeterName), RetargeterName, OutSetup, OutError, bRecreateExisting);
    if (!Retargeter)
    {
        return false;
    }

    OutSetup.SourceMesh = SourceMesh;
    OutSetup.TargetMesh = TargetMesh;
    OutSetup.SourceIKRig = SourceIKRig;
    OutSetup.TargetIKRig = TargetIKRig;
    OutSetup.Retargeter = Retargeter;
    OutSetup.SetupFolder = SetupFolder;
    OutSetup.Messages.Add(FString::Printf(TEXT("Prepared generated setup: %s"), *SetupFolder));
    return true;
}
bool FFX_RetargetAssistantSetupManager::PrepareMixamoToUE5MannyTestSet(FFRA_MixamoTestSet& OutTestSet, FString& OutError)
{
    OutTestSet = FFRA_MixamoTestSet();

    USkeletalMesh* SourceMesh = LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/FXRA_Imported/MixamoShared/Center_Block.Center_Block"));
    USkeletalMesh* TargetMesh = LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny.SKM_Manny"));
    UIKRigDefinition* TargetIKRig = LoadObject<UIKRigDefinition>(nullptr, TEXT("/Game/Characters/Mannequins/Rigs/IK_Mannequin.IK_Mannequin"));

    if (!SourceMesh)
    {
        OutError = TEXT("Missing Mixamo source mesh: /Game/FXRA_Imported/MixamoShared/Center_Block");
        return false;
    }
    if (!TargetMesh)
    {
        OutError = TEXT("Missing UE target mesh: /Game/Characters/Mannequins/Meshes/SKM_Manny");
        return false;
    }
    if (!TargetIKRig)
    {
        OutError = TEXT("Missing UE target IK Rig: /Game/Characters/Mannequins/Rigs/IK_Mannequin");
        return false;
    }

    UIKRigDefinition* SourceIKRig = CreateOrLoadIKRig(SourceMesh, OutTestSet, OutError);
    if (!SourceIKRig)
    {
        return false;
    }

    UIKRetargeter* Retargeter = CreateOrLoadRetargeter(SourceIKRig, TargetIKRig, SourceMesh, TargetMesh, OutTestSet, OutError);
    if (!Retargeter)
    {
        return false;
    }

    const TCHAR* AnimationPaths[] = {
        TEXT("/Game/FXRA_Imported/MixamoShared/Center_Block_Anim.Center_Block_Anim"),
        TEXT("/Game/FXRA_Imported/MixamoShared/Hip_Hop_Dancing_Anim.Hip_Hop_Dancing_Anim"),
        TEXT("/Game/FXRA_Imported/MixamoShared/Swing_Dancing_Anim.Swing_Dancing_Anim")
    };

    OutTestSet.SourceMesh = SourceMesh;
    OutTestSet.TargetMesh = TargetMesh;
    OutTestSet.Retargeter = Retargeter;

    for (const TCHAR* AnimationPath : AnimationPaths)
    {
        UAnimSequence* AnimSequence = LoadObject<UAnimSequence>(nullptr, AnimationPath);
        if (!AnimSequence)
        {
            OutTestSet.Messages.Add(FString::Printf(TEXT("Missing Mixamo test AnimSequence: %s"), AnimationPath));
            continue;
        }
        OutTestSet.Animations.Add(AnimSequence);
    }

    if (OutTestSet.Animations.IsEmpty())
    {
        OutError = TEXT("No Mixamo shared AnimSequence assets could be loaded.");
        return false;
    }

    OutTestSet.Messages.Add(FString::Printf(TEXT("Prepared Mixamo -> UE5 Manny test set with %d animation(s)."), OutTestSet.Animations.Num()));
    return true;
}





