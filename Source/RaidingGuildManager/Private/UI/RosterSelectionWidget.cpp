// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/RosterSelectionWidget.h"
#include "Roster/RosterManager.h"
#include "Roster/RosterDataTypes.h" // For FRaiderInfoUIData (though only used in return type, good for clarity)
#include "Kismet/GameplayStatics.h"
#include "Engine/GameInstance.h" // For GetGameInstance()

void URosterSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		RosterManagerInstance = GameInstance->GetSubsystem<URosterManager>();
		check(RosterManagerInstance); // Ensure the subsystem is available
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("RosterSelectionWidget: Could not get GameInstance in NativeConstruct!"));
	}
}

const TArray<FRaiderInfoUIData>& URosterSelectionWidget::GetDisplayRaiders() const
{
	if (RosterManagerInstance)
	{
		return RosterManagerInstance->GetPredefinedRaiders();
	}
	// Return an empty array if the manager is not available (should not happen if check passes)
	static const TArray<FRaiderInfoUIData> EmptyRaiders;
	return EmptyRaiders;
}

void URosterSelectionWidget::ToggleRaiderSelection(int32 RaiderIndexInPredefinedList)
{
	if (RosterManagerInstance)
	{
		RosterManagerInstance->ToggleSelectionRaider(RaiderIndexInPredefinedList);
		// Optionally, you might want to broadcast an event here for the Blueprint widget to update its visuals
		// OnRaiderSelectionChanged.Broadcast(RaiderIndexInPredefinedList, RosterManagerInstance->IsRaiderSelected(RaiderIndexInPredefinedList));
	}
}

bool URosterSelectionWidget::IsRaiderSelected(int32 RaiderIndexInPredefinedList) const
{
	if (RosterManagerInstance)
	{
		return RosterManagerInstance->IsRaiderSelected(RaiderIndexInPredefinedList);
	}
	return false;
}

void URosterSelectionWidget::LaunchRaid()
{
	UE_LOG(LogTemp, Log, TEXT("LaunchRaid called - attempting to open level Test_RaidArena"));
	UGameplayStatics::OpenLevel(this, FName("Test_RaidArena"));
}
