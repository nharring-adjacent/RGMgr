// Copyright Epic Games, Inc. All Rights Reserved.

#include "Characters/TankRaider.h"
#include "AI/UtilityAI/UtilityAIComponent.h"
#include "AI/UtilityAI/Tank/AttackAction_Tank.h"
#include "AI/UtilityAI/Tank/TauntAction_Tank.h"
#include "AI/UtilityAI/Tank/Considerations/TargetInRangeConsideration_Tank.h"
#include "AI/UtilityAI/Tank/Considerations/LowThreatConsideration_Tank.h"
#include "AI/UtilityAI/Tank/Considerations/AbilityCooldownConsideration_Tank.h"
#include "Characters/BossCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "DrawDebugHelpers.h" // Added for debug drawing

ATankRaider::ATankRaider()
{
	CharacterClass = ECharacterClass::Tank;
	// UtilityAIComponent is initialized in ARaiderCharacter (parent class)
}

void ATankRaider::BeginPlay()
{
	Super::BeginPlay();

	if (UtilityAIComponent)
	{
		// Configure Attack Action
		UAttackAction_Tank* AttackAction = NewObject<UAttackAction_Tank>(this, TEXT("AttackAction_Tank"));
		if (AttackAction)
		{
			UTargetInRangeConsideration_Tank* AttackRangeConsideration = NewObject<UTargetInRangeConsideration_Tank>(AttackAction);
			// AttackRangeConsideration->AttackRange = 300.f; // Can be adjusted or set via BP
			AttackAction->Considerations.Add(AttackRangeConsideration);

			UAbilityCooldownConsideration_Tank* AttackCooldownConsideration = NewObject<UAbilityCooldownConsideration_Tank>(AttackAction);
			AttackCooldownConsideration->AbilityName = FName(AttackAction->ActionName); // Or a unique ID like "Tank_Attack"
			AttackAction->Considerations.Add(AttackCooldownConsideration);

			// Optional: LowThreatConsideration for attack - might encourage attacking if threat is not super high already or to build initial threat
			ULowThreatConsideration_Tank* AttackLowThreatConsideration = NewObject<ULowThreatConsideration_Tank>(AttackAction);
			// AttackLowThreatConsideration->ThreatThresholdFactor = 0.9f; // e.g. attack if my threat < 90% of highest.
			AttackAction->Considerations.Add(AttackLowThreatConsideration);


			UtilityAIComponent->AddAction(AttackAction);
		}

		// Configure Taunt Action
		UTauntAction_Tank* TauntAction = NewObject<UTauntAction_Tank>(this, TEXT("TauntAction_Tank"));
		if (TauntAction)
		{
			UTargetInRangeConsideration_Tank* TauntRangeConsideration = NewObject<UTargetInRangeConsideration_Tank>(TauntAction);
			// TauntRangeConsideration->AttackRange = 600.f; // Taunt might have a larger range
			TauntAction->Considerations.Add(TauntRangeConsideration);

			ULowThreatConsideration_Tank* TauntLowThreatConsideration = NewObject<ULowThreatConsideration_Tank>(TauntAction);
			// TauntLowThreatConsideration->ThreatThresholdFactor = 0.7f; // Score higher if tank's threat is significantly low
			TauntAction->Considerations.Add(TauntLowThreatConsideration);

			UAbilityCooldownConsideration_Tank* TauntCooldownConsideration = NewObject<UAbilityCooldownConsideration_Tank>(TauntAction);
			TauntCooldownConsideration->AbilityName = FName(TauntAction->ActionName); // Or "Tank_Taunt"
			TauntAction->Considerations.Add(TauntCooldownConsideration);

			UtilityAIComponent->AddAction(TauntAction);
		}
	}

	// Target the Boss
	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController)
	{
		ABossCharacter* BossActor = Cast<ABossCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), ABossCharacter::StaticClass()));
		if (BossActor)
		{
			AIController->SetFocus(BossActor);
			UE_LOG(LogTemp, Log, TEXT("TankRaider %s focusing Boss %s"), *GetName(), *BossActor->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("TankRaider %s could not find BossCharacter in the world to focus."), *GetName());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("TankRaider %s does not have an AAIController."), *GetName());
	}
}

void ATankRaider::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AAIController* AIC = Cast<AAIController>(GetController());
	if (AIC && AIC->GetFocusActor())
	{
		DrawDebugLine(GetWorld(), GetActorLocation(), AIC->GetFocusActor()->GetActorLocation(), FColor::Green, false, -1, 0, 5.f);
		DrawDebugString(GetWorld(), GetActorLocation() + FVector(0,0,50.f), FString::Printf(TEXT("Targeting: %s"), *AIC->GetFocusActor()->GetName()), nullptr, FColor::Green, 0.0f, false, 0.8f);
	}

	// For Chosen Action: We will rely on logs from the actions themselves as per plan.
	// If UtilityAIComponent had a LastExecutedActionName property, we could display it here:
	// if (UtilityAIComponent && !UtilityAIComponent->LastExecutedActionName.IsEmpty())
	// {
	// 	DrawDebugString(GetWorld(), GetActorLocation() + FVector(0,0,75.f), FString::Printf(TEXT("Action: %s"), *UtilityAIComponent->LastExecutedActionName), nullptr, FColor::White, 0.0f, false, 0.8f);
	// }
}
