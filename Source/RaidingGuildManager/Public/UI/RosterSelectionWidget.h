// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RosterSelectionWidget.generated.h"

class URosterManager; // Forward declaration
struct FRaiderInfoUIData; // Forward declaration

/**
 * Widget for selecting raiders from a predefined list.
 * Interacts with URosterManager to get raider data and manage selections.
 */
UCLASS()
class RAIDINGGUILDMANAGER_API URosterSelectionWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	//~ Begin UUserWidget Interface
	virtual void NativeConstruct() override;
	//~ End UUserWidget Interface

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Roster") // Transient, as it's a pointer to a subsystem
	URosterManager* RosterManagerInstance;

public:
	UFUNCTION(BlueprintPure, Category = "Roster")
	const TArray<FRaiderInfoUIData>& GetDisplayRaiders() const;

	UFUNCTION(BlueprintCallable, Category = "Roster")
	void ToggleRaiderSelection(int32 RaiderIndexInPredefinedList);

	UFUNCTION(BlueprintPure, Category = "Roster")
	bool IsRaiderSelected(int32 RaiderIndexInPredefinedList) const;

	UFUNCTION(BlueprintCallable, Category = "Roster")
	void LaunchRaid();
};
