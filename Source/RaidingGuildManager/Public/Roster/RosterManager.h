// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RosterDataTypes.h" // For FRaiderInfoUIData
#include "RosterManager.generated.h"

class URaidPlanData; // Forward declaration

/**
 * Manages the available raiders and the currently selected roster for a raid.
 * Also holds the active raid plan to be used in the game.
 */
UCLASS()
class RAIDINGGUILDMANAGER_API URosterManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	URosterManager(); // Constructor for initialization
	//~ Begin USubsystem Interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//~ End USubsystem Interface

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Roster")
	TArray<FRaiderInfoUIData> PredefinedRaiders;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Roster")
	TArray<int32> SelectedRaiderIndices; // Stores indices from PredefinedRaiders

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Raid Plan")
	URaidPlanData* ActiveRaidPlan;

	UFUNCTION(BlueprintCallable, Category = "Raid Plan")
	void SetActiveRaidPlan(URaidPlanData* InPlanData);

	UFUNCTION(BlueprintPure, Category = "Raid Plan")
	URaidPlanData* GetActiveRaidPlan() const;

	UFUNCTION(BlueprintCallable, Category = "Roster")
	void SelectRaider(int32 RaiderIndex);

	UFUNCTION(BlueprintCallable, Category = "Roster")
	void DeselectRaider(int32 RaiderIndex);

	UFUNCTION(BlueprintCallable, Category = "Roster")
	void ToggleSelectionRaider(int32 RaiderIndex);

	UFUNCTION(BlueprintPure, Category = "Roster")
	const TArray<FRaiderInfoUIData>& GetPredefinedRaiders() const { return PredefinedRaiders; }

	UFUNCTION(BlueprintPure, Category = "Roster")
	const TArray<int32>& GetSelectedRaiderIndices() const { return SelectedRaiderIndices; }

	UFUNCTION(BlueprintPure, Category = "Roster")
	bool IsRaiderSelected(int32 RaiderIndex) const;

	UFUNCTION(BlueprintCallable, Category = "Roster")
	void ClearSelectedRaiders();
};
