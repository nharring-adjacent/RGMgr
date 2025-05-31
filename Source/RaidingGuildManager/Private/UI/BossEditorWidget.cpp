#include "UI/BossEditorWidget.h"
#include "Data/BossDataAsset.h"
#include "Data/BossStageStruct.h" // For EStageTriggerCondition
#include "GameplayTagsManager.h" // For managing gameplay tags, if needed for tag pickers
#include "Components/ComboBoxString.h"
#include "Components/DetailsView.h" // If using DetailsView
#include "Components/VerticalBox.h"
#include "AssetRegistry/AssetRegistryModule.h" // For finding DataAssets
#include "EditorFramework/AssetImportData.h" // For asset path
#include "Kismet/KismetSystemLibrary.h" // For creating DataAsset
#include "AssetToolsModule.h" // For creating assets
#include "PackageTools.h" // For package path


void UBossEditorWidget::NativeConstruct()
{
    Super::NativeConstruct();
    CurrentlySelectedStageIndex = -1;
    CurrentlySelectedAbilityInStageIndex = -1;
    CurrentlySelectedDefaultAbilityIndex = -1;
    PopulateBossSelection();
}

void UBossEditorWidget::PopulateBossSelection()
{
    // Clear existing options (if using a ComboBoxString or similar)
    // if (BossSelectionDropdown)
    // {
    //     BossSelectionDropdown->ClearOptions();
    // }

    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
    TArray<FAssetData> AssetData;
    // The class name might need to be UBossDataAsset::StaticClass()->GetFName()
    AssetRegistryModule.Get().GetAssetsByClass(UBossDataAsset::StaticClass()->GetFName(), AssetData);

    AvailableBossDataAssets.Empty();
    for (const FAssetData& Asset : AssetData)
    {
        UBossDataAsset* BossAsset = Cast<UBossDataAsset>(Asset.GetAsset());
        if (BossAsset)
        {
            AvailableBossDataAssets.Add(BossAsset);
            // if (BossSelectionDropdown)
            // {
            //     BossSelectionDropdown->AddOption(BossAsset->GetName());
            // }
        }
    }

    // Optionally, auto-load the first one or a default
    if (AvailableBossDataAssets.Num() > 0)
    {
        // LoadBossDataAsset(AvailableBossDataAssets[0]); // Initial load handled by BP or explicit user action
    }
}

void UBossEditorWidget::CreateNewBossDataAsset(FString AssetName, FString AssetPath)
{
    if (AssetName.IsEmpty()) return;

    FString PackageName = UPackageTools::SanitizePackageName(AssetPath + AssetName);
    UPackage* Package = CreatePackage(*PackageName);
    Package->FullyLoad();

    UBossDataAsset* NewBossAsset = NewObject<UBossDataAsset>(Package, FName(*AssetName), RF_Public | RF_Standalone | RF_MarkAsRootSet);

    if (NewBossAsset)
    {
        NewBossAsset->MarkPackageDirty();
        FAssetRegistryModule::AssetCreated(NewBossAsset);
        // Consider GEditor->SyncBrowserToObjects(TArray<UObject*>{NewBossAsset}); to focus in content browser

        AvailableBossDataAssets.Add(NewBossAsset);
        // Refresh selection dropdown via BP event or direct call if widget is bound
        PopulateBossSelection(); // Simplest way to refresh if dropdown is populated in C++

        LoadBossDataAsset(NewBossAsset); // Load the new asset for editing
        UE_LOG(LogTemp, Log, TEXT("Created new BossDataAsset: %s"), *PackageName);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to create BossDataAsset: %s"), *PackageName);
    }
}


void UBossEditorWidget::LoadBossDataAsset(UBossDataAsset* DataAssetToLoad)
{
    if (DataAssetToLoad)
    {
        CurrentlyEditingBoss = DataAssetToLoad;
        ResetStageSelection();
        ResetDefaultAbilitySelection();
        // Instead of DisplayBossDetails directly calling UI elements,
        // it now calls BlueprintImplementableEvents for BP to handle UI updates.
        RefreshBossDetailsDisplay();
        RefreshStagesDisplay();
        RefreshDefaultAbilitiesDisplay();
    }
}

// Was previously DisplayBossDetails, now split responsibility
// The C++ side updates data, BP side updates widgets via Refresh events.
void UBossEditorWidget::DisplayBossDetails(UBossDataAsset* BossAsset)
{
    // This function is largely replaced by calling the Refresh... events in LoadBossDataAsset
    // However, if there are any C++ states to set up based on BossAsset that are not selections,
    // they can go here.
    // For now, the Refresh events handle UI.
}

void UBossEditorWidget::SaveCurrentBossDataAsset()
{
    if (CurrentlyEditingBoss)
    {
        UPackage* Package = CurrentlyEditingBoss->GetOutermost();
        if (Package)
        {
            FString PackageFileName = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());
            bool bSaved = UPackage::SavePackage(Package, CurrentlyEditingBoss, RF_Public | RF_Standalone, *PackageFileName, GError, nullptr, true, true, SAVE_NoError);
            if (bSaved)
            {
                UE_LOG(LogTemp, Log, TEXT("Saved BossDataAsset: %s"), *CurrentlyEditingBoss->GetName());
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Failed to save BossDataAsset: %s"), *CurrentlyEditingBoss->GetName());
            }
        }
    }
}

void UBossEditorWidget::HandleBossSelectionChanged(FString SelectedBossName)
{
    UBossDataAsset* FoundAsset = nullptr;
    for (UBossDataAsset* Asset : AvailableBossDataAssets)
    {
        if (Asset && Asset->GetName() == SelectedBossName)
        {
            FoundAsset = Asset;
            break;
        }
    }

    if (FoundAsset)
    {
        LoadBossDataAsset(FoundAsset);
    }
}

void UBossEditorWidget::ResetStageSelection()
{
    CurrentlySelectedStage = nullptr;
    CurrentlySelectedStageIndex = -1;
    ResetStageAbilitySelection();
    RefreshSelectedStageDetailsDisplay(); // Clear details panel
    RefreshStageAbilitiesDisplay(); // Clear abilities panel for stage
}

void UBossEditorWidget::ResetStageAbilitySelection()
{
    CurrentlySelectedAbilityInStage = nullptr;
    CurrentlySelectedAbilityInStageIndex = -1;
    // RefreshSelectedAbilityDetailsDisplay(); // If you had one
}

void UBossEditorWidget::ResetDefaultAbilitySelection()
{
    CurrentlySelectedDefaultAbility = nullptr;
    CurrentlySelectedDefaultAbilityIndex = -1;
    // RefreshSelectedDefaultAbilityDetailsDisplay(); // If you had one
}

// --- Stage Management ---
void UBossEditorWidget::AddNewStage()
{
    if (!CurrentlyEditingBoss) return;

    CurrentlyEditingBoss->Stages.AddDefaulted();
    RefreshStagesDisplay(); // Update stages list UI
    SelectStage(CurrentlyEditingBoss->Stages.Num() - 1); // Select the new stage
    if (CurrentlyEditingBoss) CurrentlyEditingBoss->MarkPackageDirty();
}

void UBossEditorWidget::RemoveSelectedStage()
{
    if (!CurrentlyEditingBoss || !CurrentlySelectedStage || CurrentlySelectedStageIndex < 0) return;

    CurrentlyEditingBoss->Stages.RemoveAt(CurrentlySelectedStageIndex);
    RefreshStagesDisplay();
    ResetStageSelection(); // Resets index, selected stage, and calls RefreshSelectedStageDetailsDisplay & RefreshStageAbilitiesDisplay
    if (CurrentlyEditingBoss) CurrentlyEditingBoss->MarkPackageDirty();
}

void UBossEditorWidget::SelectStage(int32 StageIndex)
{
    if (!CurrentlyEditingBoss || !CurrentlyEditingBoss->Stages.IsValidIndex(StageIndex))
    {
        ResetStageSelection();
        return;
    }

    CurrentlySelectedStageIndex = StageIndex;
    CurrentlySelectedStage = &CurrentlyEditingBoss->Stages[StageIndex];
    ResetStageAbilitySelection(); // Clear previous ability selection

    RefreshSelectedStageDetailsDisplay(); // Tell BP to show details for this stage
    RefreshStageAbilitiesDisplay(); // Tell BP to show abilities for this stage
}

void UBossEditorWidget::UpdateSelectedStageName(const FText& NewName)
{
    if (CurrentlySelectedStage)
    {
        CurrentlySelectedStage->StageName = NewName.ToString();
        RefreshStagesDisplay(); // Name might be shown in the list
        RefreshSelectedStageDetailsDisplay(); // Update details view
        if (CurrentlyEditingBoss) CurrentlyEditingBoss->MarkPackageDirty();
    }
}

void UBossEditorWidget::UpdateSelectedStageTriggerCondition(EStageTriggerCondition NewCondition)
{
    if (CurrentlySelectedStage)
    {
        CurrentlySelectedStage->TriggerCondition = NewCondition;
        RefreshSelectedStageDetailsDisplay();
        if (CurrentlyEditingBoss) CurrentlyEditingBoss->MarkPackageDirty();
    }
}

void UBossEditorWidget::UpdateSelectedStageTriggerHealth(float NewHealthPercentage)
{
    if (CurrentlySelectedStage && CurrentlySelectedStage->TriggerCondition == EStageTriggerCondition::HealthPercentage)
    {
        CurrentlySelectedStage->TriggerHealthPercentage = FMath::Clamp(NewHealthPercentage, 0.0f, 1.0f);
        RefreshSelectedStageDetailsDisplay();
        if (CurrentlyEditingBoss) CurrentlyEditingBoss->MarkPackageDirty();
    }
}

void UBossEditorWidget::UpdateSelectedStageTriggerTimer(float NewTimerSeconds)
{
    if (CurrentlySelectedStage && CurrentlySelectedStage->TriggerCondition == EStageTriggerCondition::Timer)
    {
        CurrentlySelectedStage->TriggerTimerSeconds = FMath::Max(0.0f, NewTimerSeconds);
        RefreshSelectedStageDetailsDisplay();
        if (CurrentlyEditingBoss) CurrentlyEditingBoss->MarkPackageDirty();
    }
}

void UBossEditorWidget::UpdateSelectedStageEnvironmentTag(FGameplayTag NewTag)
{
    if (CurrentlySelectedStage)
    {
        CurrentlySelectedStage->EnvironmentChangeTag = NewTag;
        RefreshSelectedStageDetailsDisplay();
        if (CurrentlyEditingBoss) CurrentlyEditingBoss->MarkPackageDirty();
    }
}

void UBossEditorWidget::UpdateSelectedStageRepeatable(bool bIsRepeatable)
{
    if (CurrentlySelectedStage)
    {
        CurrentlySelectedStage->bRepeatable = bIsRepeatable;
        RefreshSelectedStageDetailsDisplay();
        if (CurrentlyEditingBoss) CurrentlyEditingBoss->MarkPackageDirty();
    }
}


// --- Ability Management (for selected stage) ---
void UBossEditorWidget::AddNewAbilityToSelectedStage()
{
    if (!CurrentlySelectedStage) return;

    CurrentlySelectedStage->Abilities.AddDefaulted();
    RefreshStageAbilitiesDisplay();
    SelectAbilityInStage(CurrentlySelectedStage->Abilities.Num() - 1);
    if (CurrentlyEditingBoss) CurrentlyEditingBoss->MarkPackageDirty();
}

void UBossEditorWidget::RemoveSelectedAbilityFromStage()
{
    if (!CurrentlySelectedStage || !CurrentlySelectedAbilityInStage || CurrentlySelectedAbilityInStageIndex < 0) return;

    CurrentlySelectedStage->Abilities.RemoveAt(CurrentlySelectedAbilityInStageIndex);
    CurrentlySelectedAbilityInStage = nullptr; // Reset selection
    CurrentlySelectedAbilityInStageIndex = -1;
    RefreshStageAbilitiesDisplay();
    // RefreshSelectedAbilityDetailsDisplay(); // Clear details if any
    if (CurrentlyEditingBoss) CurrentlyEditingBoss->MarkPackageDirty();
}

void UBossEditorWidget::SelectAbilityInStage(int32 AbilityIndex)
{
    if (!CurrentlySelectedStage || !CurrentlySelectedStage->Abilities.IsValidIndex(AbilityIndex))
    {
        CurrentlySelectedAbilityInStage = nullptr;
        CurrentlySelectedAbilityInStageIndex = -1;
        // RefreshSelectedAbilityDetailsDisplay(); // Clear details
        return;
    }
    CurrentlySelectedAbilityInStageIndex = AbilityIndex;
    CurrentlySelectedAbilityInStage = &CurrentlySelectedStage->Abilities[AbilityIndex];
    // RefreshSelectedAbilityDetailsDisplay(); // Show details for this ability
}

void UBossEditorWidget::UpdateSelectedStageAbilityProperties(const FGameplayTag& AbilityTag, const FString& AbilityName, float Cooldown)
{
    if (CurrentlySelectedAbilityInStage)
    {
        CurrentlySelectedAbilityInStage->AbilityTag = AbilityTag;
        CurrentlySelectedAbilityInStage->AbilityName = AbilityName;
        CurrentlySelectedAbilityInStage->Cooldown = Cooldown;
        RefreshStageAbilitiesDisplay(); // Update list if name/tag is shown
        // RefreshSelectedAbilityDetailsDisplay(); // Update details view
        if (CurrentlyEditingBoss) CurrentlyEditingBoss->MarkPackageDirty();
    }
}


// --- Default Ability Management ---
void UBossEditorWidget::AddNewDefaultAbility()
{
    if (!CurrentlyEditingBoss) return;

    CurrentlyEditingBoss->DefaultAbilities.AddDefaulted();
    RefreshDefaultAbilitiesDisplay();
    SelectDefaultAbility(CurrentlyEditingBoss->DefaultAbilities.Num() - 1);
    if (CurrentlyEditingBoss) CurrentlyEditingBoss->MarkPackageDirty();
}

void UBossEditorWidget::RemoveSelectedDefaultAbility()
{
    if (!CurrentlyEditingBoss || !CurrentlySelectedDefaultAbility || CurrentlySelectedDefaultAbilityIndex < 0) return;

    CurrentlyEditingBoss->DefaultAbilities.RemoveAt(CurrentlySelectedDefaultAbilityIndex);
    CurrentlySelectedDefaultAbility = nullptr;
    CurrentlySelectedDefaultAbilityIndex = -1;
    RefreshDefaultAbilitiesDisplay();
    // RefreshSelectedDefaultAbilityDetailsDisplay();
    if (CurrentlyEditingBoss) CurrentlyEditingBoss->MarkPackageDirty();
}

void UBossEditorWidget::SelectDefaultAbility(int32 AbilityIndex)
{
    if (!CurrentlyEditingBoss || !CurrentlyEditingBoss->DefaultAbilities.IsValidIndex(AbilityIndex))
    {
        CurrentlySelectedDefaultAbility = nullptr;
        CurrentlySelectedDefaultAbilityIndex = -1;
        // RefreshSelectedDefaultAbilityDetailsDisplay();
        return;
    }
    CurrentlySelectedDefaultAbilityIndex = AbilityIndex;
    CurrentlySelectedDefaultAbility = &CurrentlyEditingBoss->DefaultAbilities[AbilityIndex];
    // RefreshSelectedDefaultAbilityDetailsDisplay();
}

void UBossEditorWidget::UpdateSelectedDefaultAbilityProperties(const FGameplayTag& AbilityTag, const FString& AbilityName, float Cooldown)
{
    if (CurrentlySelectedDefaultAbility)
    {
        CurrentlySelectedDefaultAbility->AbilityTag = AbilityTag;
        CurrentlySelectedDefaultAbility->AbilityName = AbilityName;
        CurrentlySelectedDefaultAbility->Cooldown = Cooldown;
        RefreshDefaultAbilitiesDisplay();
        // RefreshSelectedDefaultAbilityDetailsDisplay();
        if (CurrentlyEditingBoss) CurrentlyEditingBoss->MarkPackageDirty();
    }
}
