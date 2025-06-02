// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI/UtilityAI/Tank/Considerations/LowThreatConsideration_Tank.h"
#include "AIController.h"
#include "Characters/TankRaider.h"
#include "Characters/BossCharacter.h" // ABossCharacter needs GetThreatForRaider and GetHighestThreat methods

ULowThreatConsideration_Tank::ULowThreatConsideration_Tank()
{
	Name = TEXT("LowThreat_Tank");
	// ThreatThresholdFactor = 0.8f; // Example: score high if tank's threat is less than 80% of highest
}

float ULowThreatConsideration_Tank::ScoreConsideration(AController* Controller, APawn* Pawn)
{
	Super::ScoreConsideration(Controller, Pawn);

	if (!Controller || !Pawn)
	{
		return 0.0f;
	}

	ATankRaider* Tank = Cast<ATankRaider>(Pawn);
	if (!Tank)
	{
		return 0.0f;
	}

	AAIController* AIController = Cast<AAIController>(Controller);
	if (!AIController)
	{
		return 0.0f;
	}

	ABossCharacter* Boss = Cast<ABossCharacter>(AIController->GetFocusActor());
	if (!Boss)
	{
		return 0.0f; // No boss, no threat context
	}

	// Placeholder logic: These functions need to be implemented in ABossCharacter
	// float TankThreat = Boss->GetThreatForRaider(Tank);
	// float HighestThreat = Boss->GetHighestThreat();

	// if (HighestThreat == 0.0f) // Avoid division by zero if no threat generated yet
	// {
	// 	return 1.0f; // If no one has threat, tank should try to generate some (e.g. for Taunt)
	// }

	// if (TankThreat < HighestThreat * ThreatThresholdFactor)
	// {
	// 	return 1.0f; // Tank's threat is low compared to the highest
	// }

	// For now, returning a neutral score as the actual threat logic is not yet in ABossCharacter
	// This consideration will become functional once ABossCharacter has threat mechanics.
	UE_LOG(LogTemp, Verbose, TEXT("LowThreatConsideration_Tank: Placeholder scoring. Boss threat functions needed."));
	return 0.5f; // Neutral score until threat system is in place
}
