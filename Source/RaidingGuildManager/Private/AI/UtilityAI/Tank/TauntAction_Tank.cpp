// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI/UtilityAI/Tank/TauntAction_Tank.h"
#include "Characters/TankRaider.h"
#include "Characters/BossCharacter.h" // Assuming this is the correct path
#include "AIController.h" // Required for AAIController

UTauntAction_Tank::UTauntAction_Tank()
{
	ActionName = TEXT("Tank Taunt");
}

void UTauntAction_Tank::Execute(AController* Controller, APawn* Pawn)
{
	Super::Execute(Controller, Pawn);

	ATankRaider* Raider = Cast<ATankRaider>(Pawn);
	if (!Raider)
	{
		UE_LOG(LogTemp, Warning, TEXT("TauntAction_Tank: Pawn is not a TankRaider"));
		return;
	}

	AAIController* AIController = Cast<AAIController>(Controller);
	if (!AIController)
	{
		UE_LOG(LogTemp, Warning, TEXT("TauntAction_Tank: Controller is not an AAIController"));
		return;
	}

	ABossCharacter* Boss = Cast<ABossCharacter>(AIController->GetFocusActor());
	if (!Boss)
	{
		UE_LOG(LogTemp, Warning, TEXT("TauntAction_Tank: Could not find BossCharacter target."));
		return;
	}

	// Placeholder function call
	// This function will need to be defined in ATankRaider.h and implemented in ATankRaider.cpp
	Raider->GenerateThreatForTarget(Boss, 100.0f);

	// UE_LOG(LogTemp, Log, TEXT("TauntAction_Tank: %s executed taunt on %s"), *Raider->GetName(), *Boss->GetName());
	if (Controller && Pawn)
	{
		UE_LOG(LogTemp, Log, TEXT("AI %s executing action: %s on Pawn %s targeting %s"), *Controller->GetName(), *ActionName, *Pawn->GetName(), Boss ? *Boss->GetName() : TEXT("None"));
	}
}
