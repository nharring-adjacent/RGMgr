#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BossEditorWidget.generated.h"

class UBossDataAsset; // Forward declaration

UCLASS()
class RAIDINGGUILDMANAGER_API UBossEditorWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    // UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional)) // Optional if you prefer C++ only
    // class UComboBoxString* BossSelectionDropdown;

    // UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
    // class UDetailsView* BossDetailsView; // Or create individual UPROPERTY bindings for each field

    // UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
    // class UVerticalBox* StagesPanel; // Container for stage editor widgets

    // UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
    // class UVerticalBox* AbilitiesPanel; // Container for ability editor widgets

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Editor")
    TArray<UBossDataAsset*> AvailableBossDataAssets; // To be populated somehow (e.g. Asset Registry)

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss Editor")
    UBossDataAsset* CurrentlyEditingBoss;

    // Properties to keep track of selections
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss Editor|Selection")
    FBossStageStruct* CurrentlySelectedStage; // Pointer to a stage within CurrentlyEditingBoss->Stages

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss Editor|Selection")
    int32 CurrentlySelectedStageIndex;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss Editor|Selection")
    FBossAbilityStruct* CurrentlySelectedAbilityInStage; // Pointer to an ability within a stage

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss Editor|Selection")
    int32 CurrentlySelectedAbilityInStageIndex;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss Editor|Selection")
    FBossAbilityStruct* CurrentlySelectedDefaultAbility; // Pointer to a default ability

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss Editor|Selection")
    int32 CurrentlySelectedDefaultAbilityIndex;


    // Functions to be called from UI events (e.g., button clicks)
    // These would be bound to UMG widget delegates in Blueprint or C++

    UFUNCTION(BlueprintCallable, Category = "Boss Editor|Boss Actions")
    void HandleBossSelectionChanged(FString SelectedBossName); // Example if using ComboBoxString

    // Stage Management
    UFUNCTION(BlueprintCallable, Category = "Boss Editor|Stages")
    void AddNewStage();

    UFUNCTION(BlueprintCallable, Category = "Boss Editor|Stages")
    void RemoveSelectedStage();

    UFUNCTION(BlueprintCallable, Category = "Boss Editor|Stages")
    void SelectStage(int32 StageIndex);

    UFUNCTION(BlueprintCallable, Category = "Boss Editor|Stages")
    void UpdateSelectedStageName(const FText& NewName); // Example for a text field change

    UFUNCTION(BlueprintCallable, Category = "Boss Editor|Stages")
    void UpdateSelectedStageTriggerCondition(EStageTriggerCondition NewCondition);

    UFUNCTION(BlueprintCallable, Category = "Boss Editor|Stages")
    void UpdateSelectedStageTriggerHealth(float NewHealthPercentage);

    UFUNCTION(BlueprintCallable, Category = "Boss Editor|Stages")
    void UpdateSelectedStageTriggerTimer(float NewTimerSeconds);

    UFUNCTION(BlueprintCallable, Category = "Boss Editor|Stages")
    void UpdateSelectedStageEnvironmentTag(FGameplayTag NewTag);

    UFUNCTION(BlueprintCallable, Category = "Boss Editor|Stages")
    void UpdateSelectedStageRepeatable(bool bIsRepeatable);


    // Ability Management (for selected stage)
    UFUNCTION(BlueprintCallable, Category = "Boss Editor|Abilities")
    void AddNewAbilityToSelectedStage();

    UFUNCTION(BlueprintCallable, Category = "Boss Editor|Abilities")
    void RemoveSelectedAbilityFromStage();

    UFUNCTION(BlueprintCallable, Category = "Boss Editor|Abilities")
    void SelectAbilityInStage(int32 AbilityIndex);

    UFUNCTION(BlueprintCallable, Category = "Boss Editor|Abilities")
    void UpdateSelectedStageAbilityProperties(const FGameplayTag& AbilityTag, const FString& AbilityName, float Cooldown); // Simplified

    // Default Ability Management
    UFUNCTION(BlueprintCallable, Category = "Boss Editor|Default Abilities")
    void AddNewDefaultAbility();

    UFUNCTION(BlueprintCallable, Category = "Boss Editor|Default Abilities")
    void RemoveSelectedDefaultAbility();

    UFUNCTION(BlueprintCallable, Category = "Boss Editor|Default Abilities")
    void SelectDefaultAbility(int32 AbilityIndex);

    UFUNCTION(BlueprintCallable, Category = "Boss Editor|Default Abilities")
    void UpdateSelectedDefaultAbilityProperties(const FGameplayTag& AbilityTag, const FString& AbilityName, float Cooldown); // Simplified


    // UI Refresh/Population functions (called internally or after an action)
    // These would be responsible for telling the UMG Blueprint to update its visual elements
    UFUNCTION(BlueprintImplementableEvent, Category = "Boss Editor|UI Update")
    void RefreshBossDetailsDisplay(); // Tell BP to update boss name, health etc.

    UFUNCTION(BlueprintImplementableEvent, Category = "Boss Editor|UI Update")
    void RefreshStagesDisplay(); // Tell BP to rebuild the stages list

    UFUNCTION(BlueprintImplementableEvent, Category = "Boss Editor|UI Update")
    void RefreshSelectedStageDetailsDisplay(); // Tell BP to update selected stage info

    UFUNCTION(BlueprintImplementableEvent, Category = "Boss Editor|UI Update")
    void RefreshStageAbilitiesDisplay(); // Tell BP to rebuild abilities for selected stage

    UFUNCTION(BlueprintImplementableEvent, Category = "Boss Editor|UI Update")
    void RefreshDefaultAbilitiesDisplay(); // Tell BP to rebuild default abilities list

    virtual void NativeConstruct() override;

public:
    UFUNCTION(BlueprintCallable, Category = "Boss Editor")
    void CreateNewBossDataAsset(FString AssetName, FString AssetPath = "/Game/Data/Bosses/");

    UFUNCTION(BlueprintCallable, Category = "Boss Editor")
    void LoadBossDataAsset(UBossDataAsset* DataAssetToLoad);

    UFUNCTION(BlueprintCallable, Category = "Boss Editor")
    void SaveCurrentBossDataAsset();

    // Add more functions as needed for UI interaction
    // e.g., OnBossSelected, OnAddStage, OnRemoveStage, OnAddAbilityToStage, etc.

private:
    void PopulateBossSelection();
    void DisplayBossDetails(UBossDataAsset* BossAsset); // This might be less used now or renamed
    void ResetStageSelection();
    void ResetStageAbilitySelection();
    void ResetDefaultAbilitySelection();
    // Helper functions to refresh UI parts
};
