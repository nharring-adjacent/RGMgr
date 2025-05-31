// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameModes/RaidArenaGameMode.h"
#include "Characters/RaiderCharacter.h"
#include "Characters/BossCharacter.h"
#include "Roster/RosterManager.h"
#include "Roster/RosterDataTypes.h" // For FRaiderInfoUIData
#include "Planning/RaidPlanData.h" // For URaidPlanData
#include "AI/RaiderAIController.h" // For ARaiderAIController cast
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h" // If you were to use PlayerStarts for spawning
#include "Kismet/GameplayStatics.h"   // For UGameplayStatics::GetAllActorsOfClass (alternative spawn point finding)
#include "Engine/GameInstance.h"     // For GetGameInstance()

ARaidArenaGameMode::ARaidArenaGameMode()
{
	// Set default pawn class to null, as we are spawning characters manually.
	DefaultPawnClass = nullptr;
	// PlayerControllerClass = AYourPlayerController::StaticClass(); // Set if you have a custom one
	// HUDClass = AYourHUD::StaticClass(); // Set if you have a custom HUD for this mode
}

void ARaidArenaGameMode::StartPlay()
{
	Super::StartPlay();

	UE_LOG(LogTemp, Log, TEXT("ARaidArenaGameMode::StartPlay() called."));

	UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("ARaidArenaGameMode: Could not get GameInstance!"));
		return;
	}

	URosterManager* RosterMgr = GameInstance->GetSubsystem<URosterManager>();
	if (!RosterMgr)
	{
		UE_LOG(LogTemp, Error, TEXT("ARaidArenaGameMode: RosterManager not found!"));
		return;
	}

	URaidPlanData* ActivePlan = RosterMgr->GetActiveRaidPlan();
	const TArray<FRaidPlanPointData>* PlanPointsPtr = nullptr; // Use a pointer to avoid copying array

	if (ActivePlan && ActivePlan->GetPathPoints().Num() > 0)
	{
		PlanPointsPtr = &ActivePlan->GetPathPoints();
		UE_LOG(LogTemp, Log, TEXT("ARaidArenaGameMode: ActiveRaidPlan found with %d points."), PlanPointsPtr->Num());
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("ARaidArenaGameMode: No ActiveRaidPlan or plan is empty. Raiders will not follow a specific plan."));
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("ARaidArenaGameMode: GetWorld() returned null."));
		return;
	}

	// Define spawn locations
	// TODO: These should ideally be fetched from PlayerStart actors or other configurable points in the level.
	FVector BossSpawnLocation(0.0f, 0.0f, 100.0f);
	TArray<FVector> RaiderSpawnLocations;
	RaiderSpawnLocations.Add(FVector(0.0f, -300.0f, 100.0f));
	RaiderSpawnLocations.Add(FVector(-150.0f, -250.0f, 100.0f));
	RaiderSpawnLocations.Add(FVector(150.0f, -250.0f, 100.0f));
	RaiderSpawnLocations.Add(FVector(-300.0f, -200.0f, 100.0f));
	RaiderSpawnLocations.Add(FVector(300.0f, -200.0f, 100.0f));


	// Spawn Boss
	ABossCharacter* Boss = World->SpawnActor<ABossCharacter>(ABossCharacter::StaticClass(), BossSpawnLocation, FRotator::ZeroRotator);
	if (Boss)
	{
		UE_LOG(LogTemp, Log, TEXT("Spawned Boss: %s at %s"), *Boss->GetName(), *BossSpawnLocation.ToString());
		Boss->SpawnDefaultController(); // This will spawn ABossAIController as defined in ABossCharacter
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn Boss at %s"), *BossSpawnLocation.ToString());
	}

	// Spawn Selected Raiders
	const TArray<int32>& SelectedIndices = RosterMgr->GetSelectedRaiderIndices();
	const TArray<FRaiderInfoUIData>& PredefinedRaiders = RosterMgr->GetPredefinedRaiders();
	int32 SpawnedRaiderCount = 0;

	if (SelectedIndices.Num() > 0)
	{
		for (int32 RaiderIndex : SelectedIndices)
		{
			if (PredefinedRaiders.IsValidIndex(RaiderIndex) && RaiderSpawnLocations.IsValidIndex(SpawnedRaiderCount))
			{
				const FRaiderInfoUIData& RaiderData = PredefinedRaiders[RaiderIndex];
				FVector RaiderSpawnLocation = RaiderSpawnLocations[SpawnedRaiderCount];

				ARaiderCharacter* Raider = World->SpawnActor<ARaiderCharacter>(ARaiderCharacter::StaticClass(), RaiderSpawnLocation, FRotator::ZeroRotator);
				if (Raider)
				{
					UE_LOG(LogTemp, Log, TEXT("Spawned Raider: %s (Role: %s) at %s"), *RaiderData.RaiderName, *RaiderData.RaiderRole, *RaiderSpawnLocation.ToString());
					Raider->SpawnDefaultController(); // This will spawn ARaiderAIController

					ARaiderAIController* RaiderAIC = Cast<ARaiderAIController>(Raider->GetController());
					if (RaiderAIC)
					{
						if (PlanPointsPtr) // Check if we have valid plan points
						{
							RaiderAIC->SetPathToFollow(*PlanPointsPtr);
						}
						else
						{
							RaiderAIC->SetPathToFollow(TArray<FRaidPlanPointData>()); // Pass empty array of correct type
						}
					}
					// Future: Raider->InitializeFromData(RaiderData);
					SpawnedRaiderCount++;
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Failed to spawn Raider: %s at %s"), *RaiderData.RaiderName, *RaiderSpawnLocation.ToString());
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Invalid RaiderIndex %d or not enough spawn locations for selected raiders."), RaiderIndex);
			}
		}
	}
	else if (PredefinedRaiders.Num() > 0 && RaiderSpawnLocations.Num() > 0) // MVP: If no raiders selected, spawn the first predefined one
	{
		UE_LOG(LogTemp, Log, TEXT("No raiders selected by player. Spawning the first predefined raider as MVP fallback."));
		const FRaiderInfoUIData& RaiderData = PredefinedRaiders[0];
		FVector RaiderSpawnLocation = RaiderSpawnLocations[0];

		ARaiderCharacter* Raider = World->SpawnActor<ARaiderCharacter>(ARaiderCharacter::StaticClass(), RaiderSpawnLocation, FRotator::ZeroRotator);
		if (Raider)
		{
			UE_LOG(LogTemp, Log, TEXT("Spawned Fallback Raider: %s (Role: %s) at %s"), *RaiderData.RaiderName, *RaiderData.RaiderRole, *RaiderSpawnLocation.ToString());
			Raider->SpawnDefaultController();

			ARaiderAIController* RaiderAIC = Cast<ARaiderAIController>(Raider->GetController());
			if (RaiderAIC)
			{
				if (PlanPointsPtr)
				{
					RaiderAIC->SetPathToFollow(*PlanPointsPtr);
				}
				else
				{
					RaiderAIC->SetPathToFollow(TArray<FRaidPlanPointData>()); // Pass empty array of correct type
				}
			}
			SpawnedRaiderCount++;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to spawn Fallback Raider: %s at %s"), *RaiderData.RaiderName, *RaiderSpawnLocation.ToString());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No raiders selected and no predefined raiders available or no spawn locations defined."));
	}

	UE_LOG(LogTemp, Log, TEXT("ARaidArenaGameMode::StartPlay() finished. Spawned %d raiders."), SpawnedRaiderCount);
}
