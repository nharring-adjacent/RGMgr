// Copyright Epic Games, Inc. All Rights Reserved.

#include "AI/BTTask_AttackTarget.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameplayTagContainer.h"
#include "Abilities/GA_BasicAttack.h" // To specify which ability to trigger
#include "AI/RaiderAIController.h" // For TargetActorKeyName, could be a shared header
#include "AI/BossAIController.h"   // For TargetActorKeyName

UBTTask_AttackTarget::UBTTask_AttackTarget()
{
	NodeName = "Attack Target";
	// Accept only Actors for the TargetActorKey
	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_AttackTarget, BlackboardKey), AActor::StaticClass());

	// Initialize with a default key name if desired, e.g., from a shared constants file
	// For this example, we'll assume it's set correctly in the Behavior Tree asset to "TargetActor"
	// BlackboardKey.SelectedKeyName = ARaiderAIController::TargetActorKeyName; // Or ABossAIController::TargetActorKeyName

	ActivatedAbilityHandle.Invalidate();
}

EBTNodeResult::Type UBTTask_AttackTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	APawn* ControlledPawn = AIController->GetPawn();
	if (!ControlledPawn)
	{
		return EBTNodeResult::Failed;
	}

	IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(ControlledPawn);
	if (!AbilitySystemInterface)
	{
		return EBTNodeResult::Failed;
	}

	CachedAbilitySystemComponent = AbilitySystemInterface->GetAbilitySystemComponent();
	if (!CachedAbilitySystemComponent)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}

	// Use the passed-in BlackboardKey selector instead of GetSelectedBlackboardKey(), which might be empty if not set.
	// This assumes TargetActorKey is set in the BT editor for this task node.
	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(BlackboardKey.SelectedKeyName));
	if (!TargetActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("BTTask_AttackTarget %s: TargetActor is not set in Blackboard (Key: %s) or is not an AActor."), *OwnerComp.GetAIOwner()->GetNameSafe(OwnerComp.GetAIOwner()->GetPawn()), *BlackboardKey.SelectedKeyName.ToString());
		return EBTNodeResult::Failed;
	}

	// Read Stamina Modifier from Blackboard
	float StaminaModifier = 1.0f;
	if (BlackboardComp->IsExist(ARaiderAIController::StaminaModifierKeyName))
	{
		StaminaModifier = BlackboardComp->GetValueAsFloat(ARaiderAIController::StaminaModifierKeyName);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BTTask_AttackTarget %s: StaminaModifierKeyName ('%s') not found on BB, defaulting to 1.0"), *OwnerComp.GetAIOwner()->GetNameSafe(OwnerComp.GetAIOwner()->GetPawn()), *ARaiderAIController::StaminaModifierKeyName.ToString());
	}

	UE_LOG(LogTemp, Log, TEXT("BTTask_AttackTarget %s: Attempting to attack %s with StaminaModifier: %f."), *OwnerComp.GetAIOwner()->GetNameSafe(OwnerComp.GetAIOwner()->GetPawn()), *GetNameSafe(TargetActor), StaminaModifier);

	IAbilitySystemInterface* TargetAbilitySystemInterface = Cast<IAbilitySystemInterface>(TargetActor);
	if(!TargetAbilitySystemInterface)
	{
		UE_LOG(LogTemp, Warning, TEXT("UBTTask_AttackTarget: TargetActor %s does not implement IAbilitySystemInterface."), *TargetActor->GetName());
		return EBTNodeResult::Failed;
	}
	UAbilitySystemComponent* TargetASC = TargetAbilitySystemInterface->GetAbilitySystemComponent();
	if(!TargetASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("UBTTask_AttackTarget: TargetActor %s has no AbilitySystemComponent."), *TargetActor->GetName());
		return EBTNodeResult::Failed;
	}


	// Find the Basic Attack ability by tag
	FGameplayTag AttackAbilityTag = FGameplayTag::RequestGameplayTag(FName("Ability.Attack.Basic"));
	TArray<FGameplayAbilitySpec*> MatchingAbilities;
	CachedAbilitySystemComponent->GetActivatableGameplayAbilitySpecsByAllMatchingTags(FGameplayTagContainer(AttackAbilityTag), MatchingAbilities);

	if (MatchingAbilities.Num() > 0)
	{
		FGameplayAbilitySpec* AbilitySpec = MatchingAbilities[0];
		if (AbilitySpec && AbilitySpec->Ability && CachedAbilitySystemComponent->CanActivateAbility(AbilitySpec->Handle))
		{
			// Create target data
			FGameplayAbilityTargetDataHandle TargetDataHandle;
			FGameplayAbilityTargetData_ActorArray* TargetData = new FGameplayAbilityTargetData_ActorArray();
			TargetData->TargetActorArray.Add(TargetActor);
			TargetDataHandle.Add(TargetData);

			// Trigger the ability with target data
			FGameplayEventData Payload;
			Payload.TargetData = TargetDataHandle;
			Payload.EventMagnitude = StaminaModifier; // Pass stamina modifier in payload
			UE_LOG(LogTemp, Log, TEXT("BTTask_AttackTarget: Passing StaminaModifier %f in payload to ability for %s."), StaminaModifier, *ControlledPawn->GetName());

			ActivatedAbilityHandle = AbilitySpec->Handle;

			// Hook into ability end
			OnAbilityEndedDelegateHandle = CachedAbilitySystemComponent->OnAbilityEnded.AddUObject(this, &UBTTask_AttackTarget::OnAbilityEnded);

			bool bActivated = CachedAbilitySystemComponent->TryActivateAbility(ActivatedAbilityHandle, true, &Payload);

			if (bActivated)
			{
				// If ability is instanced and has a long duration, it might return InProgress.
				// For an instant attack, it should finish quickly.
				// If it's latent or has a duration, this task should return InProgress.
				// For this basic attack, we assume it's mostly instant.
				// The OnAbilityEnded callback will call FinishLatentTask.
				return EBTNodeResult::InProgress;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("UBTTask_AttackTarget: Failed to activate basic attack ability for %s against %s."), *ControlledPawn->GetName(), *TargetActor->GetName());
				CachedAbilitySystemComponent->OnAbilityEnded.Remove(OnAbilityEndedDelegateHandle);
				ActivatedAbilityHandle.Invalidate();
				return EBTNodeResult::Failed;
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UBTTask_AttackTarget: Basic attack ability found but cannot be activated or is invalid for %s."), *ControlledPawn->GetName());
			return EBTNodeResult::Failed;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UBTTask_AttackTarget: No basic attack ability (Tag: Ability.Attack.Basic) found for %s."), *ControlledPawn->GetName());
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::Failed; // Should not reach here
}

void UBTTask_AttackTarget::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
    Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
	if (CachedAbilitySystemComponent && OnAbilityEndedDelegateHandle.IsValid())
	{
		CachedAbilitySystemComponent->OnAbilityEnded.Remove(OnAbilityEndedDelegateHandle);
	}
	ActivatedAbilityHandle.Invalidate();
	CachedAbilitySystemComponent = nullptr; // Clear cached component
}


void UBTTask_AttackTarget::OnAbilityEnded(const FGameplayAbilitySpecHandle& Handle, bool bWasCancelled)
{
    if (Handle == ActivatedAbilityHandle)
    {
        UBehaviorTreeComponent* OwnerComp = Cast<UBehaviorTreeComponent>(GetOuter());
        if (OwnerComp)
        {
            if (bWasCancelled)
            {
                FinishLatentTask(*OwnerComp, EBTNodeResult::Failed);
            }
            else
            {
                FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
            }
        }
		if (CachedAbilitySystemComponent && OnAbilityEndedDelegateHandle.IsValid())
		{
			CachedAbilitySystemComponent->OnAbilityEnded.Remove(OnAbilityEndedDelegateHandle);
		}
		ActivatedAbilityHandle.Invalidate();
    }
}
