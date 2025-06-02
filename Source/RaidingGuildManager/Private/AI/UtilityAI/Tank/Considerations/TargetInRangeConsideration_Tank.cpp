// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI/UtilityAI/Tank/Considerations/TargetInRangeConsideration_Tank.h"
#include "AIController.h"
#include "Characters/TankRaider.h"
#include "Characters/BossCharacter.h"

UTargetInRangeConsideration_Tank::UTargetInRangeConsideration_Tank()
{
	Name = TEXT("TargetInRange_Tank");
	AttackRange = 300.0f; // Default attack range
}

float UTargetInRangeConsideration_Tank::ScoreConsideration(AController* Controller, APawn* Pawn)
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
		return 0.0f; // No target, so not in range
	}

	float Distance = FVector::Dist(Tank->GetActorLocation(), Boss->GetActorLocation());

	if (Distance <= AttackRange)
	{
		return 1.0f;
	}

	// Normalize score if needed, e.g., (1.0f - FMath::Clamp(Distance / (AttackRange * 2.0f), 0.0f, 1.0f))
	// For now, simple binary: in range or not.
	return 0.0f;
}
