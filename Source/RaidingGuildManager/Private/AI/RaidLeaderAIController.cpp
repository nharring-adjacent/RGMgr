// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI/RaidLeaderAIController.h"
#include "Planning/RaidPlanData.h"
#include "AI/RaiderAIController.h" // Already included in header, but good practice for .cpp
#include "EngineUtils.h" // For TActorIterator
#include "GameFramework/Pawn.h" // For APawn

// Helper function to find RaiderAIController by Pawn name (CharacterID)
ARaiderAIController* ARaidLeaderAIController::FindRaiderControllerByID(const FString& CharacterID)
{
	if (GetWorld())
	{
		for (TActorIterator<ARaiderAIController> It(GetWorld()); It; ++It)
		{
			ARaiderAIController* RaiderController = *It;
			if (RaiderController && RaiderController->GetPawn())
			{
				if (RaiderController->GetPawn()->GetName() == CharacterID)
				{
					return RaiderController;
				}
			}
		}
	}
	return nullptr;
}

ARaidLeaderAIController::ARaidLeaderAIController()
{
	// Constructor implementation
}

void ARaidLeaderAIController::SetCurrentRaidPlan(URaidPlanData* NewPlan)
{
	CurrentRaidPlan = NewPlan;
	if (CurrentRaidPlan)
	{
		UE_LOG(LogTemp, Log, TEXT("Raid plan '%s' set on RaidLeaderAIController."), *CurrentRaidPlan->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("A null Raid plan was set on RaidLeaderAIController."));
	}
}

void ARaidLeaderAIController::DistributePlanToSquad()
{
	if (!CurrentRaidPlan)
	{
		UE_LOG(LogTemp, Error, TEXT("ARaidLeaderAIController::DistributePlanToSquad: CurrentRaidPlan is null."));
		return;
	}

	const TMap<FString, FRaidPlanPath>& AllNamedPaths = CurrentRaidPlan->GetAllNamedPaths();
	if (AllNamedPaths.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("ARaidLeaderAIController::DistributePlanToSquad: No named paths in the current plan."));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("ARaidLeaderAIController::DistributePlanToSquad: Distributing %d paths."), AllNamedPaths.Num());

	for (const auto& Pair : AllNamedPaths)
	{
		const FString& CharacterOrRoleID = Pair.Key;
		const FRaidPlanPath& PathData = Pair.Value;

		ARaiderAIController* RaiderController = FindRaiderControllerByID(CharacterOrRoleID);

		if (RaiderController)
		{
			if (PathData.Points.Num() > 0)
			{
				RaiderController->SetPathToFollow(PathData.Points);
				UE_LOG(LogTemp, Log, TEXT("Assigned path with %d points to Raider: %s"), PathData.Points.Num(), *CharacterOrRoleID);
				// Optionally populate our ManagedRaiders map
				ManagedRaiders.FindOrAdd(CharacterOrRoleID, RaiderController);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Path for %s has no points. Not assigning to RaiderController."), *CharacterOrRoleID);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Could not find RaiderAIController for CharacterID: %s"), *CharacterOrRoleID);
		}
	}
}
