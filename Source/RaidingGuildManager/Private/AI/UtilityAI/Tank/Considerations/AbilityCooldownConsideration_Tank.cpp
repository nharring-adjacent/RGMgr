// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI/UtilityAI/Tank/Considerations/AbilityCooldownConsideration_Tank.h"
#include "Characters/TankRaider.h" // ATankRaider needs IsAbilityOnCooldown

UAbilityCooldownConsideration_Tank::UAbilityCooldownConsideration_Tank()
{
	Name = TEXT("AbilityCooldown_Tank");
	AbilityName = NAME_None;
}

float UAbilityCooldownConsideration_Tank::ScoreConsideration(AController* Controller, APawn* Pawn)
{
	Super::ScoreConsideration(Controller, Pawn);

	if (!Pawn || AbilityName == NAME_None)
	{
		return 0.0f;
	}

	ATankRaider* Tank = Cast<ATankRaider>(Pawn);
	if (!Tank)
	{
		return 0.0f;
	}

	// Placeholder: Assumes ATankRaider will have a method like IsAbilityOnCooldown
	// bool bIsOnCooldown = Tank->IsAbilityOnCooldown(AbilityName);
	// if (bIsOnCooldown)
	// {
	// 	return 0.0f; // Ability is on cooldown, score low
	// }

	// For now, returning a high score as the actual cooldown logic is not yet in ATankRaider
	// This consideration will become functional once ATankRaider has cooldown mechanics.
	UE_LOG(LogTemp, Verbose, TEXT("AbilityCooldownConsideration_Tank: Placeholder scoring for ability '%s'. ATankRaider::IsAbilityOnCooldown needed."), *AbilityName.ToString());
	return 1.0f; // Ability is ready, score high
}
