// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI/BossAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Characters/RGMCharacterBase.h"
#include "Characters/BossCharacter.h" // Specifically for BossCharacter cast

// Define static const FName members
const FName ABossAIController::TargetActorKeyName = FName("TargetActor");
const FName ABossAIController::SelfActorKeyName = FName("SelfActor");

ABossAIController::ABossAIController()
{
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
}

void ABossAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (InPawn == nullptr)
	{
		return;
	}

	ABossCharacter* PossessedBossCharacter = Cast<ABossCharacter>(InPawn);
	if (PossessedBossCharacter)
	{
		// Initialize Attributes and Abilities
		if (PossessedBossCharacter->GetAbilitySystemComponent())
		{
			PossessedBossCharacter->InitializeAttributes();
			PossessedBossCharacter->GiveDefaultAbilities();
			UE_LOG(LogTemp, Log, TEXT("BossAIC %s: Initialized attributes and abilities for %s"), *GetNameSafe(this), *InPawn->GetName());
		}

		// Standard Behavior Tree and Blackboard setup
		if (BlackboardComponent && BehaviorTreeAsset && BehaviorTreeAsset->BlackboardAsset)
		{
			InitializeBlackboard(*BlackboardComponent, *BehaviorTreeAsset->BlackboardAsset);
			BlackboardComponent->SetValueAsObject(SelfActorKeyName, PossessedBossCharacter);
			// TargetActorKey would be set by a perception system or other logic.

			RunBehaviorTree(BehaviorTreeAsset);
			// BehaviorTreeComponent->StartTree(*BehaviorTreeAsset, EBTExecutionMode::Looped); // RunBehaviorTree already does this
			UE_LOG(LogTemp, Log, TEXT("BossAIController possessed %s and started behavior tree %s"), *InPawn->GetName(), *BehaviorTreeAsset->GetName());
		}
		else
		{
			if(!BehaviorTreeAsset) UE_LOG(LogTemp, Warning, TEXT("BossAIController has no BehaviorTreeAsset set for %s!"), *InPawn->GetName());
			if(BehaviorTreeAsset && !BehaviorTreeAsset->BlackboardAsset) UE_LOG(LogTemp, Warning, TEXT("BossAIController's BehaviorTreeAsset %s has no BlackboardAsset set for %s!"), *BehaviorTreeAsset->GetName(), *InPawn->GetName());
			if(!BlackboardComponent) UE_LOG(LogTemp, Warning, TEXT("BossAIController has no BlackboardComponent for %s!"), *InPawn->GetName());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BossAIController::OnPossess: InPawn is not an ABossCharacter. Pawn: %s"), *GetNameSafe(InPawn));
	}
}
