// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameModes/RaidArenaGameMode.h"
#include "Components/RaidPartyStaminaComponent.h"
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
#include "Engine/Engine.h"           // For GEngine

ARaidArenaGameMode::ARaidArenaGameMode()
{
	// Set default pawn class to null, as we are spawning characters manually.
	DefaultPawnClass = nullptr;
	// PlayerControllerClass = AYourPlayerController::StaticClass(); // Set if you have a custom one
	// HUDClass = AYourHUD::StaticClass(); // Set if you have a custom HUD for this mode

	RaidPartyStaminaComponent = CreateDefaultSubobject<URaidPartyStaminaComponent>(TEXT("RaidPartyStaminaComp"));
	bIsInCombat = false;
	OutOfCombatTimer = 0.0f;
	bIsInBreakPeriod = false;
}

void ARaidArenaGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsInCombat)
	{
		if (RaidPartyStaminaComponent)
		{
			RaidPartyStaminaComponent->RaidPartyStamina.CurrentPullStamina -= 1.0f * DeltaTime;
			if (RaidPartyStaminaComponent->RaidPartyStamina.CurrentPullStamina < 0.0f)
			{
				RaidPartyStaminaComponent->RaidPartyStamina.CurrentPullStamina = 0.0f;
			}
			UE_LOG(LogTemp, Warning, TEXT("Current Pull Stamina: %f"), RaidPartyStaminaComponent->RaidPartyStamina.CurrentPullStamina);
		}
		OutOfCombatTimer = 0.0f; // Reset timer during combat
	}
	else // Not in combat
	{
		OutOfCombatTimer += DeltaTime;
		if (OutOfCombatTimer > 30.0f && RaidPartyStaminaComponent)
		{
			if (RaidPartyStaminaComponent->RaidPartyStamina.CurrentPullStamina < RaidPartyStaminaComponent->RaidPartyStamina.MaxPullStamina)
			{
				RaidPartyStaminaComponent->RaidPartyStamina.CurrentPullStamina = RaidPartyStaminaComponent->RaidPartyStamina.MaxPullStamina;
				UE_LOG(LogTemp, Log, TEXT("Pull Stamina Regenerated to Max: %f"), RaidPartyStaminaComponent->RaidPartyStamina.CurrentPullStamina);
				OutOfCombatTimer = 0.0f; // Reset timer after regeneration to prevent continuous logging
			}
		}
	}

	if (bIsInBreakPeriod && RaidPartyStaminaComponent)
	{
		if (RaidPartyStaminaComponent->RaidPartyStamina.CurrentRaidStamina < RaidPartyStaminaComponent->RaidPartyStamina.MaxRaidStamina)
		{
			RaidPartyStaminaComponent->RaidPartyStamina.CurrentRaidStamina += 5.0f * DeltaTime;
			if (RaidPartyStaminaComponent->RaidPartyStamina.CurrentRaidStamina > RaidPartyStaminaComponent->RaidPartyStamina.MaxRaidStamina)
			{
				RaidPartyStaminaComponent->RaidPartyStamina.CurrentRaidStamina = RaidPartyStaminaComponent->RaidPartyStamina.MaxRaidStamina;
			}
			UE_LOG(LogTemp, Verbose, TEXT("Current Raid Stamina: %f"), RaidPartyStaminaComponent->RaidPartyStamina.CurrentRaidStamina);
		}
	}

	// On-screen debug messages
	if (GEngine && RaidPartyStaminaComponent)
	{
		// Assuming RaidPartyStamina is a direct USTRUCT member in URaidPartyStaminaComponent
		const FRaidPartyStamina& Stamina = RaidPartyStaminaComponent->RaidPartyStamina;
		float Modifier = GetStaminaPerformanceModifier();

		FString DebugMsg = FString::Printf(TEXT("Pull Stamina: %.1f / %.1f | Raid Stamina: %.1f / %.1f [Mod: %.2f] Combat: %s | Break: %s"),
											 Stamina.CurrentPullStamina, Stamina.MaxPullStamina,
											 Stamina.CurrentRaidStamina, Stamina.MaxRaidStamina,
											 Modifier,
											 bIsInCombat ? TEXT("TRUE") : TEXT("FALSE"),
											 bIsInBreakPeriod ? TEXT("TRUE") : TEXT("FALSE"));
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, DebugMsg);
	}
}

void ARaidArenaGameMode::StartCombat()
{
	bIsInCombat = true;
	UE_LOG(LogTemp, Log, TEXT("Combat Started"));
	// If a break period was active, it should end when combat starts.
	if (bIsInBreakPeriod)
	{
		EndBreakPeriod();
		UE_LOG(LogTemp, Log, TEXT("Break period ended due to combat starting."));
	}
}

void ARaidArenaGameMode::EndCombat()
{
	bIsInCombat = false;
	UE_LOG(LogTemp, Log, TEXT("Combat Ended"));
}

void ARaidArenaGameMode::StartBreakPeriod()
{
	if (bIsInCombat)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot start break period while in combat!"));
		return;
	}
	bIsInBreakPeriod = true;
	UE_LOG(LogTemp, Log, TEXT("Break Period Started"));
}

void ARaidArenaGameMode::EndBreakPeriod()
{
	bIsInBreakPeriod = false;
	UE_LOG(LogTemp, Log, TEXT("Break Period Ended"));
}

float ARaidArenaGameMode::GetStaminaPerformanceModifier() const
{
	if (!RaidPartyStaminaComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetStaminaPerformanceModifier: RaidPartyStaminaComponent is null. Returning default modifier 1.0f."));
		return 1.0f;
	}

	// Accessing FRaidPartyStamina directly from the component
	const FRaidPartyStamina& Stamina = RaidPartyStaminaComponent->RaidPartyStamina;

	float Modifier = 1.0f;

	// Check Pull Stamina
	if (Stamina.MaxPullStamina > 0.0f) // Avoid division by zero
	{
		if ((Stamina.CurrentPullStamina / Stamina.MaxPullStamina) < 0.2f)
		{
			Modifier = 0.8f;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GetStaminaPerformanceModifier: MaxPullStamina is 0 or less. Pull stamina check skipped."));
	}

	// Check Raid Stamina
	if (Stamina.MaxRaidStamina > 0.0f) // Avoid division by zero
	{
		if ((Stamina.CurrentRaidStamina / Stamina.MaxRaidStamina) < 0.1f)
		{
			Modifier = FMath::Min(Modifier, 0.7f); // Apply the harsher penalty
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GetStaminaPerformanceModifier: MaxRaidStamina is 0 or less. Raid stamina check skipped."));
	}

	return Modifier;
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
