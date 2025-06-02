// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI/UtilityAI/Tank/AttackAction_Tank.h"
#include "Characters/TankRaider.h"
#include "Characters/BossCharacter.h" // Assuming this is the correct path
#include "AIController.h" // Required for AAIController

UAttackAction_Tank::UAttackAction_Tank()
{
	ActionName = TEXT("Tank Attack");
}

void UAttackAction_Tank::Execute(AController* Controller, APawn* Pawn)
{
	Super::Execute(Controller, Pawn);

	ATankRaider* Raider = Cast<ATankRaider>(Pawn);
	if (!Raider)
	{
		UE_LOG(LogTemp, Warning, TEXT("AttackAction_Tank: Pawn is not a TankRaider"));
		return;
	}

	// For now, assume the AIController has a way to get the current target.
	// This is a common pattern.
	AAIController* AIController = Cast<AAIController>(Controller);
	if (!AIController)
	{
		UE_LOG(LogTemp, Warning, TEXT("AttackAction_Tank: Controller is not an AAIController"));
		return;
	}

	ABossCharacter* Boss = Cast<ABossCharacter>(AIController->GetFocusActor()); // GetFocusActor() is one way to get the target
	if (!Boss)
	{
		// Fallback: Maybe the Raider has a target component or direct reference
		// This part would depend on how targeting is implemented elsewhere
		// For now, we'll log a warning if no boss is found via focus.
		UE_LOG(LogTemp, Warning, TEXT("AttackAction_Tank: Could not find BossCharacter target."));
		return;
	}

	// Placeholder function calls
	// These functions will need to be defined in ATankRaider.h and implemented in ATankRaider.cpp
	Raider->DealDamageToTarget(Boss, 10.0f);
	Raider->GenerateThreatForTarget(Boss, 20.0f);

	// UE_LOG(LogTemp, Log, TEXT("AttackAction_Tank: %s executed attack on %s"), *Raider->GetName(), *Boss->GetName());
	if (Controller && Pawn)
	{
		UE_LOG(LogTemp, Log, TEXT("AI %s executing action: %s on Pawn %s targeting %s"), *Controller->GetName(), *ActionName, *Pawn->GetName(), Boss ? *Boss->GetName() : TEXT("None"));
	}
}
