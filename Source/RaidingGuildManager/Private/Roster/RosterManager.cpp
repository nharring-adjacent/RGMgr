// Copyright Epic Games, Inc. All Rights Reserved.

#include "Roster/RosterManager.h"
#include "Planning/RaidPlanData.h" // Include for URaidPlanData

URosterManager::URosterManager()
	: ActiveRaidPlan(nullptr)
{
}

void URosterManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	ActiveRaidPlan = nullptr; // Ensure it's null on init

	// Populate PredefinedRaiders
	PredefinedRaiders.Emplace(TEXT("Valerius"), TEXT("Tank"), 0);
	PredefinedRaiders.Emplace(TEXT("Lyra"), TEXT("DPS"), 1);
	PredefinedRaiders.Emplace(TEXT("Elara"), TEXT("Healer"), 2);
	PredefinedRaiders.Emplace(TEXT("Marcus"), TEXT("DPS"), 3);
	PredefinedRaiders.Emplace(TEXT("Seraphina"), TEXT("Support"), 4);

	UE_LOG(LogTemp, Log, TEXT("RosterManager Initialized with %d predefined raiders."), PredefinedRaiders.Num());
}

void URosterManager::Deinitialize()
{
	PredefinedRaiders.Empty();
	SelectedRaiderIndices.Empty();
	Super::Deinitialize();
}

void URosterManager::SelectRaider(int32 RaiderIndex)
{
	if (PredefinedRaiders.IsValidIndex(RaiderIndex) && !SelectedRaiderIndices.Contains(RaiderIndex))
	{
		SelectedRaiderIndices.Add(RaiderIndex);
		UE_LOG(LogTemp, Log, TEXT("Raider selected: %s (Index: %d)"), *PredefinedRaiders[RaiderIndex].RaiderName, RaiderIndex);
	}
}

void URosterManager::DeselectRaider(int32 RaiderIndex)
{
	if (PredefinedRaiders.IsValidIndex(RaiderIndex))
	{
		int32 RemovedCount = SelectedRaiderIndices.Remove(RaiderIndex);
		if (RemovedCount > 0)
		{
			UE_LOG(LogTemp, Log, TEXT("Raider deselected: %s (Index: %d)"), *PredefinedRaiders[RaiderIndex].RaiderName, RaiderIndex);
		}
	}
}

void URosterManager::ToggleSelectionRaider(int32 RaiderIndex)
{
	if (!PredefinedRaiders.IsValidIndex(RaiderIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("ToggleSelectionRaider: Invalid RaiderIndex %d"), RaiderIndex);
		return;
	}

	if (SelectedRaiderIndices.Contains(RaiderIndex))
	{
		DeselectRaider(RaiderIndex);
	}
	else
	{
		SelectRaider(RaiderIndex);
	}
}

bool URosterManager::IsRaiderSelected(int32 RaiderIndex) const
{
	if (!PredefinedRaiders.IsValidIndex(RaiderIndex))
	{
		return false;
	}
	return SelectedRaiderIndices.Contains(RaiderIndex);
}

void URosterManager::ClearSelectedRaiders()
{
	SelectedRaiderIndices.Empty();
	UE_LOG(LogTemp, Log, TEXT("Selected raiders cleared."));
}

void URosterManager::SetActiveRaidPlan(URaidPlanData* InPlanData)
{
	ActiveRaidPlan = InPlanData;
	if (ActiveRaidPlan)
	{
		UE_LOG(LogTemp, Log, TEXT("RosterManager: Active raid plan set with %d points."), ActiveRaidPlan->GetPathPoints().Num());
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("RosterManager: Active raid plan cleared."));
	}
}

URaidPlanData* URosterManager::GetActiveRaidPlan() const
{
	return ActiveRaidPlan;
}
